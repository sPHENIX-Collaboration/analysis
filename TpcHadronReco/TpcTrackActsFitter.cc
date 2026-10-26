#include "TpcTrackActsFitter.h"

#include "TpcTrackHelixFitter.h"
#include "TpcTrackKalmanFitter.h"

#include <trackbase/ActsGeometry.h>

#include <phfield/PHField.h>

#include <phool/PHCompositeNode.h>
#include <phool/getClass.h>

#include <Acts/Definitions/Algebra.hpp>
#include <Acts/Definitions/TrackParametrization.hpp>
#include <Acts/Definitions/Units.hpp>
#include <Acts/EventData/MultiTrajectory.hpp>
#include <Acts/EventData/ParticleHypothesis.hpp>
#include <Acts/EventData/SourceLink.hpp>
#include <Acts/EventData/TrackContainer.hpp>
#include <Acts/EventData/TrackParameters.hpp>
#include <Acts/EventData/VectorMultiTrajectory.hpp>
#include <Acts/EventData/VectorTrackContainer.hpp>
#include <Acts/Geometry/GeometryContext.hpp>
#include <Acts/Geometry/GeometryIdentifier.hpp>
#include <Acts/MagneticField/MagneticFieldContext.hpp>
#include <Acts/MagneticField/MagneticFieldProvider.hpp>
#include <Acts/Propagator/DirectNavigator.hpp>
#include <Acts/Propagator/EigenStepper.hpp>
#include <Acts/Propagator/Propagator.hpp>
#include <Acts/Surfaces/CylinderSurface.hpp>
#include <Acts/Surfaces/PerigeeSurface.hpp>
#include <Acts/Surfaces/Surface.hpp>
#include <Acts/TrackFitting/GainMatrixSmoother.hpp>
#include <Acts/TrackFitting/GainMatrixUpdater.hpp>
#include <Acts/TrackFitting/KalmanFitter.hpp>
#include <Acts/Utilities/CalibrationContext.hpp>
#include <Acts/Utilities/Logger.hpp>
#include <Acts/Utilities/Result.hpp>

#include <CLHEP/Units/SystemOfUnits.h>

#include <Eigen/Dense>

#include <algorithm>
#include <array>
#include <cmath>
#include <cstdint>
#include <iostream>
#include <limits>
#include <map>
#include <memory>
#include <numeric>
#include <string>
#include <utility>
#include <vector>

namespace tpchadronreco
{
  namespace
  {
    // ACTS view of the PHField used by TpcTrackKalmanFitter. Outside the map
    // PHField3DCartesian returns zero field, so propagation never fails there
    // (the ACTS interpolated map instead returns "out of bounds" errors).
    // Without a PHField, a uniform Bz = bfield_t is used.
    class PHFieldProvider final : public Acts::MagneticFieldProvider
    {
     public:
      struct Cache
      {
        explicit Cache(const Acts::MagneticFieldContext & /*mctx*/) {}
      };

      void set_field(const PHField *field, const double bz_tesla)
      {
        m_field = field;
        m_bz_tesla = bz_tesla;
      }

      Acts::MagneticFieldProvider::Cache makeCache(
          const Acts::MagneticFieldContext &mctx) const override
      {
        return Acts::MagneticFieldProvider::Cache(std::in_place_type<Cache>, mctx);
      }

      Acts::Result<Acts::Vector3> getField(
          const Acts::Vector3 &position,
          Acts::MagneticFieldProvider::Cache & /*cache*/) const override
      {
        Acts::Vector3 field_tesla(0.0, 0.0, m_bz_tesla);
        if (m_field)
        {
          const double point[4] = {position.x() / Acts::UnitConstants::mm * CLHEP::mm,
                                   position.y() / Acts::UnitConstants::mm * CLHEP::mm,
                                   position.z() / Acts::UnitConstants::mm * CLHEP::mm,
                                   0.0};
          double field[3] = {0.0, 0.0, 0.0};
          m_field->GetFieldValue(point, field);
          field_tesla = Acts::Vector3(field[0], field[1], field[2]) / CLHEP::tesla;
          if (!field_tesla.allFinite())
          {
            field_tesla.setZero();
          }
        }
        return Acts::Result<Acts::Vector3>::success(field_tesla * Acts::UnitConstants::T);
      }

     private:
      const PHField *m_field{nullptr};
      double m_bz_tesla{1.4};
    };

    constexpr double kPi = 3.14159265358979323846;
    constexpr double kCurvaturePerCm = 0.003;

    // Half length of the per-point measurement cylinders. Larger than the TPC
    // so the bounds never reject a measurement.
    constexpr double kCylinderHalfLengthMm = 3000.0;

    // Geometry ids for the per-point surfaces. They only need to be unique
    // within one fit; they never enter the sPHENIX tracking geometry.
    constexpr std::uint64_t kGeometryIdBase = std::uint64_t{0xfe} << 56U;

    // Rejection codes, matching TpcTrackKalmanFitter.
    constexpr unsigned char kUsed = 0U;
    constexpr unsigned char kRejectedChi2Hard = 2U;
    constexpr unsigned char kExcludedByConfig = 3U;

    template <class T>
    constexpr T square(const T &value)
    {
      return value * value;
    }

    double normalize_phi(const double phi)
    {
      return std::atan2(std::sin(phi), std::cos(phi));
    }

    double point_radius(const TpcTrackPoint &point)
    {
      return std::hypot(point.position.x, point.position.y);
    }

    struct FitSourceLink
    {
      Acts::GeometryIdentifier geometry_id;
      std::size_t index{0};

      friend bool operator==(const FitSourceLink &lhs, const FitSourceLink &rhs)
      {
        return lhs.geometry_id == rhs.geometry_id && lhs.index == rhs.index;
      }
    };

    // One measurement per input point, in fit order (increasing radius).
    struct FitMeasurement
    {
      Acts::ActsVector<2> parameters{Acts::ActsVector<2>::Zero()};      // (r*phi, z) [mm]
      Acts::ActsSquareMatrix<2> covariance{Acts::ActsSquareMatrix<2>::Zero()};  // [mm^2]
      bool use{true};
      double sigma_rphi_cm{0.0};
      double sigma_z_cm{0.0};
    };

    struct FitSurfaceAccessor
    {
      const std::vector<std::shared_ptr<const Acts::Surface>> *surfaces{nullptr};

      const Acts::Surface *operator()(const Acts::SourceLink &source_link) const
      {
        const FitSourceLink link = source_link.get<FitSourceLink>();
        return link.index < surfaces->size() ? (*surfaces)[link.index].get() : nullptr;
      }
    };

    struct FitCalibrator
    {
      const std::vector<FitMeasurement> *measurements{nullptr};

      void calibrate(const Acts::GeometryContext & /*gctx*/,
                     const Acts::CalibrationContext & /*cctx*/,
                     const Acts::SourceLink &source_link,
                     Acts::VectorMultiTrajectory::TrackStateProxy track_state) const
      {
        track_state.setUncalibratedSourceLink(Acts::SourceLink{source_link});
        const FitSourceLink link = source_link.get<FitSourceLink>();
        const FitMeasurement &measurement = (*measurements)[link.index];
        track_state.allocateCalibrated(measurement.parameters.eval(),
                                       measurement.covariance.eval());
        track_state.setProjectorSubspaceIndices(
            std::array<Acts::BoundIndices, 2>{Acts::eBoundLoc0, Acts::eBoundLoc1});
      }
    };

    // Predicted chi2 of a 2D (loc0, loc1) measurement.
    template <class parameters_t, class covariance_t>
    double predicted_chi2(const FitMeasurement &measurement,
                          const parameters_t &predicted,
                          const covariance_t &predicted_covariance)
    {
      const Eigen::Vector2d residual(
          measurement.parameters(0) - predicted(Acts::eBoundLoc0),
          measurement.parameters(1) - predicted(Acts::eBoundLoc1));
      Eigen::Matrix2d innovation;
      innovation << measurement.covariance(0, 0) + predicted_covariance(Acts::eBoundLoc0, Acts::eBoundLoc0),
          measurement.covariance(0, 1) + predicted_covariance(Acts::eBoundLoc0, Acts::eBoundLoc1),
          measurement.covariance(1, 0) + predicted_covariance(Acts::eBoundLoc1, Acts::eBoundLoc0),
          measurement.covariance(1, 1) + predicted_covariance(Acts::eBoundLoc1, Acts::eBoundLoc1);
      const double determinant = innovation.determinant();
      if (!(determinant > 0.0) || !std::isfinite(determinant))
      {
        return std::numeric_limits<double>::quiet_NaN();
      }
      return residual.dot(innovation.inverse() * residual);
    }

    // Marks excluded points and hard-chi2 failures as outliers. Outliers are
    // not used in the update but still get predicted/smoothed states.
    struct FitOutlierFinder
    {
      const std::vector<FitMeasurement> *measurements{nullptr};
      std::vector<unsigned char> *reasons{nullptr};
      std::vector<double> *raw_chi2{nullptr};
      double chi2_hard{0.0};

      bool operator()(Acts::MultiTrajectory<Acts::VectorMultiTrajectory>::ConstTrackStateProxy state) const
      {
        if (!state.hasCalibrated())
        {
          return false;
        }
        const FitSourceLink link = state.getUncalibratedSourceLink().template get<FitSourceLink>();
        if (link.index >= measurements->size())
        {
          return false;
        }

        const FitMeasurement &measurement = (*measurements)[link.index];
        double chi2 = std::numeric_limits<double>::quiet_NaN();
        if (state.hasPredicted())
        {
          chi2 = predicted_chi2(measurement, state.predicted(), state.predictedCovariance());
        }
        (*raw_chi2)[link.index] = chi2;

        unsigned char reason = kUsed;
        if (!measurement.use)
        {
          reason = kExcludedByConfig;
        }
        else if (chi2_hard > 0.0 && std::isfinite(chi2) && chi2 > chi2_hard)
        {
          reason = kRejectedChi2Hard;
        }
        (*reasons)[link.index] = reason;
        return reason != kUsed;
      }
    };

    struct FitState
    {
      bool valid{false};
      bool has_predicted{false};
      double chi2{std::numeric_limits<double>::quiet_NaN()};
      Acts::BoundVector predicted{Acts::BoundVector::Zero()};
      Acts::BoundSquareMatrix predicted_covariance{Acts::BoundSquareMatrix::Zero()};
      Acts::BoundVector filtered{Acts::BoundVector::Zero()};
      Acts::BoundSquareMatrix filtered_covariance{Acts::BoundSquareMatrix::Zero()};
      Acts::BoundVector smoothed{Acts::BoundVector::Zero()};
      Acts::BoundSquareMatrix smoothed_covariance{Acts::BoundSquareMatrix::Zero()};
    };

    // Converts bound parameters on a per-point cylinder (local frame rotated by
    // surface_phi) to the TpcTrackKalmanFitter state
    //   (x, y, z [cm], phi, q/pT, tan(lambda)),
    // where q/pT = -charge/pT along the direction of travel. With reverse=true
    // the state describes travel in the opposite direction, as used when the
    // input point sequence runs inward.
    void to_internal_state(const Acts::BoundVector &parameters,
                           const Acts::BoundSquareMatrix &covariance,
                           const double radius_mm,
                           const double surface_phi,
                           const bool reverse,
                           std::array<double, 6> &state,
                           std::array<double, 36> &state_covariance)
    {
      using Fitter = TpcTrackKalmanFitter;
      const double cm = Acts::UnitConstants::cm;
      const double position_phi = surface_phi + parameters[Acts::eBoundLoc0] / radius_mm;
      const double theta = parameters[Acts::eBoundTheta];
      const double sin_theta = std::sin(theta);
      const double cos_theta = std::cos(theta);
      const double q_over_p = parameters[Acts::eBoundQOverP];

      state[Fitter::X] = radius_mm * std::cos(position_phi) / cm;
      state[Fitter::Y] = radius_mm * std::sin(position_phi) / cm;
      state[Fitter::Z] = parameters[Acts::eBoundLoc1] / cm;
      state[Fitter::Phi] = parameters[Acts::eBoundPhi];
      state[Fitter::QOverPt] = -q_over_p / sin_theta;
      state[Fitter::TanLambda] = cos_theta / sin_theta;

      // Rows: internal state. Columns: ACTS bound parameters.
      Eigen::Matrix<double, 6, Acts::eBoundSize> jacobian =
          Eigen::Matrix<double, 6, Acts::eBoundSize>::Zero();
      jacobian(Fitter::X, Acts::eBoundLoc0) = -std::sin(position_phi) / cm;
      jacobian(Fitter::Y, Acts::eBoundLoc0) = std::cos(position_phi) / cm;
      jacobian(Fitter::Z, Acts::eBoundLoc1) = 1.0 / cm;
      jacobian(Fitter::Phi, Acts::eBoundPhi) = 1.0;
      jacobian(Fitter::QOverPt, Acts::eBoundTheta) = q_over_p * cos_theta / square(sin_theta);
      jacobian(Fitter::QOverPt, Acts::eBoundQOverP) = -1.0 / sin_theta;
      jacobian(Fitter::TanLambda, Acts::eBoundTheta) = -1.0 / square(sin_theta);

      if (reverse)
      {
        state[Fitter::Phi] += kPi;
        state[Fitter::QOverPt] *= -1.0;
        state[Fitter::TanLambda] *= -1.0;
        jacobian.row(Fitter::QOverPt) *= -1.0;
        jacobian.row(Fitter::TanLambda) *= -1.0;
      }
      state[Fitter::Phi] = normalize_phi(state[Fitter::Phi]);

      Eigen::Matrix<double, 6, 6> converted = jacobian * covariance * jacobian.transpose();
      converted = 0.5 * (converted + converted.transpose()).eval();
      for (int row = 0; row < 6; ++row)
      {
        for (int col = 0; col < 6; ++col)
        {
          state_covariance[static_cast<std::size_t>(row) * 6U + static_cast<std::size_t>(col)] =
              converted(row, col);
        }
      }
    }
  }  // namespace

  struct TpcTrackActsFitter::Impl
  {
    using Stepper = Acts::EigenStepper<>;
    using Propagator = Acts::Propagator<Stepper, Acts::DirectNavigator>;
    using Fitter = Acts::KalmanFitter<Propagator, Acts::VectorMultiTrajectory>;
    using TrackContainer = Acts::TrackContainer<Acts::VectorTrackContainer,
                                                Acts::VectorMultiTrajectory,
                                                std::shared_ptr>;

    explicit Impl(const ActsGeometry *geometry_in)
      : geometry(geometry_in)
      , field_provider(std::make_shared<PHFieldProvider>())
      , logger(Acts::getDefaultLogger("TpcTrackActsFitter", Acts::Logging::FATAL))
      , fitter(Propagator(Stepper(field_provider),
                          Acts::DirectNavigator{logger->cloneWithSuffix("DirectNavigator")},
                          logger->cloneWithSuffix("DirectPropagator")),
               logger->cloneWithSuffix("DirectFitter"))
    {
    }

    const ActsGeometry *geometry{nullptr};
    std::shared_ptr<PHFieldProvider> field_provider;
    std::unique_ptr<const Acts::Logger> logger;
    Fitter fitter;
    Acts::GainMatrixUpdater updater;
    Acts::GainMatrixSmoother smoother;

    // Diagnostics, printed by print_summary().
    std::uint64_t nfits{0};
    std::uint64_t nsuccess{0};
    std::uint64_t ncharge_disagree{0};
    std::uint64_t nmeasurements{0};
    std::uint64_t nexcluded{0};
    std::uint64_t nrejected_chi2{0};
    std::map<std::string, std::uint64_t> failures;
  };

  std::unique_ptr<TpcTrackActsFitter> TpcTrackActsFitter::create(PHCompositeNode *topNode)
  {
    auto *geometry = findNode::getClass<ActsGeometry>(topNode, "ActsGeometry");
    if (!geometry)
    {
      return nullptr;
    }
    return std::unique_ptr<TpcTrackActsFitter>(
        new TpcTrackActsFitter(std::make_unique<Impl>(geometry)));
  }

  TpcTrackActsFitter::TpcTrackActsFitter(std::unique_ptr<Impl> impl)
    : m_impl(std::move(impl))
  {
  }

  TpcTrackActsFitter::~TpcTrackActsFitter() = default;

  bool TpcTrackActsFitter::fit(const std::vector<TpcTrackPoint> &points,
                               const int charge,
                               const TpcKalmanConfig &config,
                               TpcKalmanResult &result,
                               const double mass_gev) const
  {
    ++m_impl->nfits;
    const bool success = fit_impl(points, charge, config, result, mass_gev);
    if (success)
    {
      ++m_impl->nsuccess;
    }
    else
    {
      // Keep the counter key free of per-track numbers.
      ++m_impl->failures[result.message.substr(0, result.message.find(" ("))];
    }
    return success;
  }

  void TpcTrackActsFitter::print_summary(const std::string &prefix) const
  {
    std::cout << prefix << ": ACTS fit summary: fits=" << m_impl->nfits
              << " success=" << m_impl->nsuccess
              << " charge_disagree=" << m_impl->ncharge_disagree
              << " measurements=" << m_impl->nmeasurements
              << " excluded=" << m_impl->nexcluded
              << " rejected_chi2_hard=" << m_impl->nrejected_chi2 << std::endl;
    for (const auto &[reason, count] : m_impl->failures)
    {
      std::cout << prefix << ":   failed " << count << " x \"" << reason << "\"" << std::endl;
    }
  }

  bool TpcTrackActsFitter::fit_impl(const std::vector<TpcTrackPoint> &points,
                                    const int charge,
                                    const TpcKalmanConfig &config,
                                    TpcKalmanResult &result,
                                    const double mass_gev) const
  {
    const double nan = std::numeric_limits<double>::quiet_NaN();
    const double cm = Acts::UnitConstants::cm;

    result = TpcKalmanResult{};
    result.charge = charge;
    result.bfield_t = config.bfield_t;
    result.magnetic_field = config.magnetic_field;
    result.analytic_uniform_propagation = config.analytic_uniform_propagation;
    result.mass_gev = mass_gev;

    const std::size_t npoints = points.size();
    if (npoints < 5)
    {
      result.message = "need at least five TPC points";
      return false;
    }

    // ACTS propagates along the physical direction: increasing radius.
    std::vector<std::size_t> order(npoints);
    std::iota(order.begin(), order.end(), 0U);
    std::stable_sort(order.begin(), order.end(),
                     [&points](const std::size_t lhs, const std::size_t rhs)
                     { return point_radius(points[lhs]) < point_radius(points[rhs]); });

    // Same global helix seed as TpcTrackKalmanFitter: only points used in the
    // update enter the seed.
    std::vector<TpcTrackPoint> seed_points;
    seed_points.reserve(npoints);
    for (const std::size_t index : order)
    {
      if (points[index].use_in_kalman)
      {
        seed_points.push_back(points[index]);
      }
    }
    if (seed_points.size() < 5 ||
        !TpcTrackHelixFitter::fit(seed_points, 0, config.bfield_t, result.seed) ||
        !(result.seed.radius > 0.0) || !std::isfinite(result.seed.radius))
    {
      result.message = "helix seed failed";
      return false;
    }
    const TpcTrackHelix &helix = result.seed;

    // Transverse arc length along the fit order, then tan(lambda) from a
    // straight-line fit of z versus arc length.
    std::vector<double> theta_fit(npoints, 0.0);
    for (std::size_t k = 0; k < npoints; ++k)
    {
      const auto &position = points[order[k]].position;
      double theta = std::atan2(position.y - helix.cy, position.x - helix.cx);
      if (k > 0)
      {
        while (theta - theta_fit[k - 1] > kPi)
        {
          theta -= 2.0 * kPi;
        }
        while (theta - theta_fit[k - 1] < -kPi)
        {
          theta += 2.0 * kPi;
        }
      }
      theta_fit[k] = theta;
    }

    double sum_n = 0.0;
    double sum_s = 0.0;
    double sum_z = 0.0;
    double sum_ss = 0.0;
    double sum_sz = 0.0;
    for (std::size_t k = 0; k < npoints; ++k)
    {
      const auto &point = points[order[k]];
      if (!point.use_in_kalman)
      {
        continue;
      }
      const double s = helix.radius * std::abs(theta_fit[k] - theta_fit.front());
      sum_n += 1.0;
      sum_s += s;
      sum_z += point.position.z;
      sum_ss += s * s;
      sum_sz += s * point.position.z;
    }
    const double slope_denominator = sum_n * sum_ss - sum_s * sum_s;
    if (!(slope_denominator > 1.0e-9))
    {
      result.message = "z-arc seed fit failed";
      return false;
    }
    const double seed_tanl = (sum_n * sum_sz - sum_s * sum_z) / slope_denominator;

    // Seed direction: tangent to the helix circle at the innermost point,
    // oriented towards the outer points.
    const TpcTrackPoint &first = points[order.front()];
    const TpcTrackPoint &reference = points[order[std::min<std::size_t>(5, npoints - 1)]];
    const double radial_x = first.position.x - helix.cx;
    const double radial_y = first.position.y - helix.cy;
    const double radial_norm = std::hypot(radial_x, radial_y);
    if (!(radial_norm > 0.0))
    {
      result.message = "degenerate seed circle";
      return false;
    }
    double tangent_x = -radial_y / radial_norm;  // counter-clockwise
    double tangent_y = radial_x / radial_norm;
    double turn_sign = 1.0;
    if (tangent_x * (reference.position.x - first.position.x) +
            tangent_y * (reference.position.y - first.position.y) <
        0.0)
    {
      tangent_x = -tangent_x;
      tangent_y = -tangent_y;
      turn_sign = -1.0;
    }

    const auto &tracking_geometry = m_impl->geometry->geometry();
    const Acts::GeometryContext &gctx = tracking_geometry.getGeoContext();
    const Acts::MagneticFieldContext &mctx = tracking_geometry.magFieldContext;
    const Acts::CalibrationContext &cctx = tracking_geometry.calibContext;

    // Charge from the turning sense and the local field: a positive charge
    // turns clockwise in a field along +z.
    m_impl->field_provider->set_field(config.magnetic_field, config.bfield_t);
    double bz_tesla = config.bfield_t;
    {
      auto field_cache = m_impl->field_provider->makeCache(mctx);
      const Acts::Vector3 field_position(first.position.x * cm, first.position.y * cm, first.position.z * cm);
      const auto field = m_impl->field_provider->getField(field_position, field_cache);
      if (field.ok() && std::abs((*field).z() / Acts::UnitConstants::T) > 1.0e-3)
      {
        bz_tesla = (*field).z() / Acts::UnitConstants::T;
      }
    }
    const double fit_charge = -turn_sign * (bz_tesla >= 0.0 ? 1.0 : -1.0);
    if (charge != 0 && fit_charge * charge < 0.0)
    {
      ++m_impl->ncharge_disagree;
    }

    const double seed_pt = std::max(kCurvaturePerCm * std::abs(bz_tesla) * helix.radius,
                                    std::max(config.min_pt_gev, 1.0e-3));
    const double direction_norm = std::sqrt(1.0 + seed_tanl * seed_tanl);
    const Acts::Vector3 seed_direction(tangent_x / direction_norm,
                                       tangent_y / direction_norm,
                                       seed_tanl / direction_norm);
    const double seed_momentum = seed_pt * direction_norm;
    const double sin_theta = 1.0 / direction_norm;

    // Start 1 cm before the innermost point so the first surface lies ahead.
    const Acts::Vector3 seed_position =
        Acts::Vector3(first.position.x * cm, first.position.y * cm, first.position.z * cm) -
        1.0 * cm * seed_direction;
    const Acts::Vector4 seed_position4(seed_position.x(), seed_position.y(), seed_position.z(), 0.0);
    auto perigee = Acts::Surface::makeShared<Acts::PerigeeSurface>(seed_position);

    // Prior covariance from the same configuration as TpcTrackKalmanFitter.
    double sigma_qop_t = std::max(config.initial_sigma_qop_t, 1.0e-12);
    if (config.use_relative_initial_sigma_qop_t)
    {
      sigma_qop_t = std::max(std::max(config.initial_sigma_qop_t_floor, 1.0e-12),
                             std::max(config.initial_sigma_qop_t_relative, 0.0) / seed_pt);
    }
    const double sigma_position_mm = std::max(config.initial_sigma_pos_cm, 1.0e-6) * cm;
    Acts::BoundSquareMatrix seed_covariance = Acts::BoundSquareMatrix::Zero();
    seed_covariance(Acts::eBoundLoc0, Acts::eBoundLoc0) = square(sigma_position_mm);
    seed_covariance(Acts::eBoundLoc1, Acts::eBoundLoc1) = square(sigma_position_mm);
    seed_covariance(Acts::eBoundPhi, Acts::eBoundPhi) = square(std::max(config.initial_sigma_phi, 1.0e-9));
    seed_covariance(Acts::eBoundTheta, Acts::eBoundTheta) =
        square(std::max(config.initial_sigma_tanl, 1.0e-9) * sin_theta * sin_theta);
    seed_covariance(Acts::eBoundQOverP, Acts::eBoundQOverP) = square(sigma_qop_t * sin_theta);
    seed_covariance(Acts::eBoundTime, Acts::eBoundTime) = square(1.0 * Acts::UnitConstants::ns);

    auto seed_parameters = Acts::BoundTrackParameters::create(
        gctx,
        perigee,
        seed_position4,
        seed_direction,
        fit_charge / seed_momentum,
        seed_covariance,
        Acts::ParticleHypothesis::pion());
    if (!seed_parameters.ok())
    {
      result.message = "ACTS seed parameters failed";
      return false;
    }

    // One cylinder per point. Its local frame is rotated to the point azimuth,
    // so the measured loc0 is zero and residuals never cross the phi branch cut.
    const double min_sigma_cm = std::max(config.min_measurement_sigma_cm, 1.0e-12);
    std::vector<std::shared_ptr<const Acts::Surface>> surfaces;
    std::vector<const Acts::Surface *> surface_sequence;
    std::vector<FitMeasurement> measurements;
    std::vector<Acts::SourceLink> source_links;
    std::vector<double> surface_radius_mm(npoints, 0.0);
    std::vector<double> surface_phi(npoints, 0.0);
    surfaces.reserve(npoints);
    surface_sequence.reserve(npoints);
    measurements.reserve(npoints);
    source_links.reserve(npoints);
    for (std::size_t k = 0; k < npoints; ++k)
    {
      const TpcTrackPoint &point = points[order[k]];
      const double radius_cm = point_radius(point);
      if (!(radius_cm > 0.0) || !std::isfinite(point.position.z))
      {
        result.message = "invalid TPC point";
        return false;
      }
      surface_radius_mm[k] = radius_cm * cm;
      surface_phi[k] = std::atan2(point.position.y, point.position.x);

      Acts::Transform3 transform = Acts::Transform3::Identity();
      transform.rotate(Acts::AngleAxis3(surface_phi[k], Acts::Vector3::UnitZ()));
      auto surface = Acts::Surface::makeShared<Acts::CylinderSurface>(
          transform, surface_radius_mm[k], kCylinderHalfLengthMm);
      surface->assignGeometryId(Acts::GeometryIdentifier(kGeometryIdBase + k + 1U));

      FitMeasurement measurement;
      measurement.use = point.use_in_kalman;
      measurement.sigma_rphi_cm =
          std::max(point.sigma_rphi_cm > 0.0 ? point.sigma_rphi_cm : config.meas_sigma_rphi_cm, min_sigma_cm);
      measurement.sigma_z_cm =
          std::max(point.sigma_z_cm > 0.0 ? point.sigma_z_cm : config.meas_sigma_z_cm, min_sigma_cm);
      measurement.parameters << 0.0, point.position.z * cm;
      measurement.covariance(0, 0) = square(measurement.sigma_rphi_cm * cm);
      measurement.covariance(1, 1) = square(measurement.sigma_z_cm * cm);
      measurements.push_back(measurement);

      source_links.emplace_back(FitSourceLink{surface->geometryId(), k});
      surface_sequence.push_back(surface.get());
      surfaces.push_back(surface);
    }

    std::vector<unsigned char> reasons(npoints, kUsed);
    std::vector<double> raw_chi2(npoints, nan);

    FitSurfaceAccessor surface_accessor{&surfaces};
    FitCalibrator calibrator{&measurements};
    FitOutlierFinder outlier_finder{
        &measurements, &reasons, &raw_chi2,
        config.robust_measurement_updates ? config.robust_chi2_hard : 0.0};

    Acts::KalmanFitterExtensions<Acts::VectorMultiTrajectory> extensions;
    // cppcheck-suppress constStatement
    extensions.updater.connect<&Acts::GainMatrixUpdater::operator()<Acts::VectorMultiTrajectory>>(&m_impl->updater);
    // cppcheck-suppress constStatement
    extensions.smoother.connect<&Acts::GainMatrixSmoother::operator()<Acts::VectorMultiTrajectory>>(&m_impl->smoother);
    extensions.calibrator.connect<&FitCalibrator::calibrate>(&calibrator);
    extensions.surfaceAccessor.connect<&FitSurfaceAccessor::operator()>(&surface_accessor);
    extensions.outlierFinder.connect<&FitOutlierFinder::operator()>(&outlier_finder);

    const Acts::PropagatorPlainOptions propagator_options(gctx, mctx);
    Acts::KalmanFitterOptions<Acts::VectorMultiTrajectory> fitter_options(
        gctx, mctx, cctx, extensions, propagator_options, perigee.get());
    fitter_options.multipleScattering = false;
    fitter_options.energyLoss = false;

    Impl::TrackContainer tracks(std::make_shared<Acts::VectorTrackContainer>(),
                                std::make_shared<Acts::VectorMultiTrajectory>());
    auto fit_result = m_impl->fitter.fit(source_links.begin(), source_links.end(),
                                         seed_parameters.value(), fitter_options,
                                         surface_sequence, tracks);
    if (!fit_result.ok())
    {
      result.message = "ACTS fit failed: " + fit_result.error().message();
      return false;
    }
    const auto track = fit_result.value();

    // Walk from the last state inwards. Outliers after the last used
    // measurement are not smoothed by ACTS; they keep the filtered state.
    std::vector<FitState> fitted(npoints);
    bool before_last_used = true;
    tracks.trackStateContainer().visitBackwards(
        track.tipIndex(),
        [&](const auto &state)
        {
          if (!state.hasCalibrated())
          {
            return true;
          }
          const FitSourceLink link = state.getUncalibratedSourceLink().template get<FitSourceLink>();
          if (link.index >= npoints)
          {
            return true;
          }

          FitState &out = fitted[link.index];
          if (state.hasPredicted())
          {
            out.has_predicted = true;
            out.predicted = state.predicted();
            out.predicted_covariance = state.predictedCovariance();
          }
          if (state.hasFiltered())
          {
            out.filtered = state.filtered();
            out.filtered_covariance = state.filteredCovariance();
          }
          else if (out.has_predicted)
          {
            out.filtered = out.predicted;
            out.filtered_covariance = out.predicted_covariance;
          }
          else
          {
            return true;
          }

          const bool used = reasons[link.index] == kUsed;
          if (state.hasSmoothed() && (used || !before_last_used))
          {
            out.smoothed = state.smoothed();
            out.smoothed_covariance = state.smoothedCovariance();
          }
          else
          {
            out.smoothed = out.filtered;
            out.smoothed_covariance = out.filtered_covariance;
          }
          if (used)
          {
            out.chi2 = state.chi2();
            before_last_used = false;
          }
          out.valid = true;
          return true;
        });

    const std::size_t nvalid = static_cast<std::size_t>(
        std::count_if(fitted.begin(), fitted.end(), [](const FitState &state)
                      { return state.valid; }));
    if (nvalid != npoints)
    {
      result.message = "ACTS fit missing states (" + std::to_string(nvalid) +
                       " of " + std::to_string(npoints) + " points)";
      return false;
    }

    // Output follows the input point order. If that order runs inward, the
    // states describe travel along the sequence, like TpcTrackKalmanFitter.
    const bool reverse = point_radius(points.front()) > point_radius(points.back());

    result.states_filtered.resize(npoints);
    result.covs_filtered.resize(npoints);
    result.states_smoothed.resize(npoints);
    result.covs_smoothed.resize(npoints);
    result.measurement_original_index.resize(npoints);
    result.measurement_chi2.assign(npoints, nan);
    result.measurement_chi2_raw.assign(npoints, nan);
    result.measurement_used.assign(npoints, 0U);
    result.measurement_rejection_reason.assign(npoints, kUsed);
    result.measurement_weight_scale.assign(npoints, 0.0);
    result.measurement_sigma_r_used.assign(npoints, nan);
    result.measurement_sigma_rphi_used.assign(npoints, nan);
    result.measurement_sigma_z_used.assign(npoints, nan);
    result.measurement_in_seed.assign(npoints, 0U);
    result.innovation_residual_r.assign(npoints, nan);
    result.innovation_residual_rphi.assign(npoints, nan);
    result.innovation_residual_z.assign(npoints, nan);
    result.prediction_sigma_r.assign(npoints, nan);
    result.prediction_sigma_rphi.assign(npoints, nan);
    result.prediction_sigma_z.assign(npoints, nan);
    result.innovation_sigma_r.assign(npoints, nan);
    result.innovation_sigma_rphi.assign(npoints, nan);
    result.innovation_sigma_z.assign(npoints, nan);
    result.innovation_rho_r_rphi.assign(npoints, nan);
    result.innovation_rho_r_z.assign(npoints, nan);
    result.innovation_rho_rphi_z.assign(npoints, nan);
    result.innovation_whitened_0.assign(npoints, nan);
    result.innovation_whitened_1.assign(npoints, nan);
    result.innovation_whitened_2.assign(npoints, nan);

    double chi2 = 0.0;
    std::size_t naccepted = 0;
    m_impl->nmeasurements += npoints;
    for (std::size_t k = 0; k < npoints; ++k)
    {
      m_impl->nexcluded += reasons[k] == kExcludedByConfig ? 1U : 0U;
      m_impl->nrejected_chi2 += reasons[k] == kRejectedChi2Hard ? 1U : 0U;
      const std::size_t index = order[k];
      const FitState &state = fitted[k];
      const FitMeasurement &measurement = measurements[k];
      const bool used = reasons[k] == kUsed;

      to_internal_state(state.filtered, state.filtered_covariance,
                        surface_radius_mm[k], surface_phi[k], reverse,
                        result.states_filtered[index], result.covs_filtered[index]);
      to_internal_state(state.smoothed, state.smoothed_covariance,
                        surface_radius_mm[k], surface_phi[k], reverse,
                        result.states_smoothed[index], result.covs_smoothed[index]);

      result.measurement_original_index[index] = points[index].original_index;
      result.measurement_chi2_raw[index] = raw_chi2[k];
      result.measurement_chi2[index] = used ? state.chi2 : raw_chi2[k];
      result.measurement_used[index] = used ? 1U : 0U;
      result.measurement_rejection_reason[index] = reasons[k];
      result.measurement_weight_scale[index] = used ? 1.0 : 0.0;
      result.measurement_sigma_rphi_used[index] = measurement.sigma_rphi_cm;
      result.measurement_sigma_z_used[index] = measurement.sigma_z_cm;
      result.measurement_in_seed[index] = points[index].use_in_kalman ? 1U : 0U;

      if (state.has_predicted)
      {
        const double residual_rphi = measurement.parameters(0) - state.predicted[Acts::eBoundLoc0];
        const double residual_z = measurement.parameters(1) - state.predicted[Acts::eBoundLoc1];
        const double prediction_var_rphi = state.predicted_covariance(Acts::eBoundLoc0, Acts::eBoundLoc0);
        const double prediction_var_z = state.predicted_covariance(Acts::eBoundLoc1, Acts::eBoundLoc1);
        const double innovation_var_rphi = prediction_var_rphi + measurement.covariance(0, 0);
        const double innovation_var_z = prediction_var_z + measurement.covariance(1, 1);
        const double innovation_cov_rphi_z =
            state.predicted_covariance(Acts::eBoundLoc0, Acts::eBoundLoc1) + measurement.covariance(0, 1);

        result.innovation_residual_rphi[index] = residual_rphi / cm;
        result.innovation_residual_z[index] = residual_z / cm;
        result.prediction_sigma_rphi[index] = std::sqrt(std::max(prediction_var_rphi, 0.0)) / cm;
        result.prediction_sigma_z[index] = std::sqrt(std::max(prediction_var_z, 0.0)) / cm;
        result.innovation_sigma_rphi[index] = std::sqrt(std::max(innovation_var_rphi, 0.0)) / cm;
        result.innovation_sigma_z[index] = std::sqrt(std::max(innovation_var_z, 0.0)) / cm;
        if (innovation_var_rphi > 0.0 && innovation_var_z > 0.0)
        {
          result.innovation_rho_rphi_z[index] =
              innovation_cov_rphi_z / std::sqrt(innovation_var_rphi * innovation_var_z);
          result.innovation_whitened_0[index] = residual_rphi / std::sqrt(innovation_var_rphi);
          result.innovation_whitened_1[index] = residual_z / std::sqrt(innovation_var_z);
        }
      }

      if (used)
      {
        ++naccepted;
        if (std::isfinite(state.chi2))
        {
          chi2 += state.chi2;
        }
      }
    }

    if (naccepted < 5)
    {
      result.message = "fewer than five ACTS measurements were accepted";
      return false;
    }

    // Transverse arc length along the input order, as in TpcTrackKalmanFitter.
    result.path_s.assign(npoints, 0.0);
    double previous_theta = 0.0;
    for (std::size_t index = 0; index < npoints; ++index)
    {
      const auto &position = points[index].position;
      double theta = std::atan2(position.y - helix.cy, position.x - helix.cx);
      if (index > 0)
      {
        while (theta - previous_theta > kPi)
        {
          theta -= 2.0 * kPi;
        }
        while (theta - previous_theta < -kPi)
        {
          theta += 2.0 * kPi;
        }
        result.path_s[index] = result.path_s[index - 1] + helix.radius * std::abs(theta - previous_theta);
      }
      previous_theta = theta;
    }

    result.naccepted = naccepted;
    result.nrejected = npoints - naccepted;
    result.chi2 = chi2;
    result.ndof = 2 * static_cast<int>(naccepted) - 5;
    result.success = true;
    result.message = "ok";
    return true;
  }
}  // namespace tpchadronreco
