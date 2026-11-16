#ifndef TPCCALIB_STRIPEMATCHING_PARAMETERS_H
#define TPCCALIB_STRIPEMATCHING_PARAMETERS_H

// Isolation filter: a stripe must have enough neighbors inside this radius.
inline constexpr double isolation_radius_cm = 4.0;
inline constexpr int min_isolation_neighbors = 3;

// Iterative global-assignment controls.
inline constexpr int global_assignment_max_iterations = 10;
inline constexpr double global_assignment_convergence_fraction = 0.01;

// Broad hard gates for a measured/reference pair.
inline constexpr double global_assignment_max_abs_delta_r_cm = 3.0;
inline constexpr double global_assignment_max_abs_delta_phi_rad = 0.025;

// Field-consistency sigmas and weights used in assignment costs.
inline constexpr double global_assignment_field_sigma_delta_r_cm = 1.50;
inline constexpr double global_assignment_field_sigma_delta_phi_rad = 0.006;
inline constexpr double global_assignment_field_delta_r_weight = 1.0;
inline constexpr double global_assignment_field_delta_phi_weight = 1.0;

// Radial-row topology settings.
inline constexpr double topology_row_tolerance_cm = 0.75;
inline constexpr int topology_min_stripes_per_row = 1;
inline constexpr int topology_region_count = 3;
inline constexpr int topology_excluded_outer_rows_per_region = 0;
inline constexpr int topology_excluded_inner_rows_per_region = 0;

// Row mapping mode switches.
inline constexpr bool use_topology_row_mapping_dp = false;
inline constexpr bool use_topology_edge_row_branch_repair = true;
inline constexpr int topology_edge_row_branch_repair_edge_rows = 1;

// Dynamic-programming row-map costs.
inline constexpr double topology_row_mapping_sigma_delta_r_cm = 0.75;
inline constexpr double topology_row_mapping_index_weight = 4.0;
inline constexpr double topology_row_mapping_measured_skip_cost = 100.0;

// Branch shifts to probe when radial rows may be offset by one row.
inline constexpr int matching_branch_probe_min_shift = -1;
inline constexpr int matching_branch_probe_max_shift = 1;

// Branch scoring weights.
inline constexpr double matching_hypothesis_residual_weight = 1.0;
inline constexpr double matching_hypothesis_unmatched_weight = 0.0;
inline constexpr double matching_hypothesis_cleaning_weight = 0.0;

// Minimum branch quality before it is considered viable.
inline constexpr double matching_branch_probe_min_matched_fraction = 0.50;
inline constexpr double matching_branch_probe_min_cleaning_survival = 0.50;

// Hungarian assignment costs.
inline constexpr double global_assignment_unmatched_cost = 25.0;
inline constexpr double global_assignment_max_final_cost = 24.0;

// Local consistency cleaner for selected seed matches.
inline constexpr double seed_clean_radial_window_cm = 1.0;
inline constexpr double seed_clean_phi_window_rad = 1.5707963267948966;
inline constexpr int seed_clean_min_neighbors = 3;
inline constexpr double seed_clean_max_local_residual_cm = 0.5;

// Minimum matches and radial range for field fitting.
inline constexpr int min_ml_seed_neighbors = 3;
inline constexpr double fit_r_min_cm = 30.0;
inline constexpr double fit_r_max_cm = 73.0;

// Global field grid and regularization.
inline constexpr double global_field_kernel_nugget = 1e-3;
inline constexpr double global_field_regularization_lambda = 50;
inline constexpr double global_field_control_phi_spacing_rad = 0.25;
inline constexpr int global_field_control_r_bins = 22;

// Active-control interpolation settings.
inline constexpr bool use_active_control_interpolation = true;
inline constexpr double global_field_min_control_support = 2.5;
inline constexpr int global_field_min_active_controls = 3;
inline constexpr int global_field_active_interpolation_neighbors = 6;
inline constexpr double global_field_active_interpolation_power = 2.0;
inline constexpr double global_field_active_interpolation_r_scale_cm = 2.0;
inline constexpr double global_field_active_interpolation_rphi_scale_cm = 30.0;

// Robust fallback sigma limits for the field fit.
inline constexpr double fallback_sigma_prior_delta_r_cm = 0.75;
inline constexpr double fallback_sigma_prior_r_delta_phi_cm = 0.75;
inline constexpr double min_sigma_prior_delta_r_cm = 0.30;
inline constexpr double min_sigma_prior_r_delta_phi_cm = 0.30;
inline constexpr double max_sigma_prior_delta_r_cm = 1.50;
inline constexpr double max_sigma_prior_r_delta_phi_cm = 1.50;

// Reference-source switch.
inline bool useIdealStripesAsReference = false;

// Flood-fill stripe detection thresholds and shape limits.
inline constexpr double stripeFloodSeedMinContent = 1.5;
inline constexpr double stripeFloodGrowMinContent = 1.5;
inline constexpr int stripeFloodMaxPhiGapBins = 1;
inline constexpr int stripeFloodMaxRGapBins = 1;
inline constexpr int stripeFloodMaxPhiSpanBins = 8;
inline constexpr int stripeFloodMaxRSpanBins = 14;
inline constexpr int stripeFloodMinBins = 3;
inline constexpr double stripeFloodMinTotalContent = 10.0;
inline constexpr double duplicateStripeMaxDeltaRCm = 0.5;
inline constexpr double duplicateStripeMaxDeltaPhiRad = 0.03;

// Lamination masking switch and radial-row finder settings.
inline constexpr bool applyPreStripeLaminationMask = true;
inline constexpr double laminationOuterGapMinRCm = 30.0;
inline constexpr int maxLaminationGapCount = 22;
inline constexpr double radialRowPeakMinRCm = laminationOuterGapMinRCm;
inline constexpr double radialRowPeakMinSeparationCm = 1.0;
inline constexpr double radialRowPeakThresholdFrac = 0.05;
inline constexpr double radialRowBoundaryThresholdFrac = 0.001;
inline constexpr double radialRowBoundaryMaxHalfWidthCm = 0.75;
inline constexpr double radialGapBoundaryPaddingCm = 0.0;
inline constexpr double radialGapMaskPaddingCm = 0.5;
inline constexpr double radialGapMaskMaxPaddingFraction = 0.25;
inline constexpr double radialGapMaxLaminationWidthCm = 2.0;

// Minimum bin content used in the radial projection.
inline constexpr double radialRowProjectionMinContent = 100.0;

// Optional radial weighting inside lamination gaps.
inline constexpr bool weightLaminationGapByDistanceFromRows = true;
inline constexpr double laminationGapRadialWeightPower = 2.0;

// Phi-lamination finder and mask settings.
inline constexpr double laminationGapPhiMinContent = 10.0;
inline constexpr double laminationGlobalPhiSupportThreshold = 0.0;
inline constexpr double laminationGlobalPhiSuppressWindowRad = 0.25;
inline constexpr int laminationGlobalPhiIntegralHalfWindowBins = 2;
inline constexpr double laminationPhiMaskHalfWidthRad = 0.015;
inline constexpr bool wrapPhiForPreLaminationMask = false;


#endif
