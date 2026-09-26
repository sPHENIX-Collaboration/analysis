cmake_minimum_required(VERSION 3.16)
project(corral LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
if(NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE RelWithDebInfo)
endif()

# Relies on ROOT's own CMake config (ROOTConfig.cmake) being discoverable --
# e.g. via `source thisroot.sh`, which sets CMAKE_PREFIX_PATH, same as this
# machine's environment already does.
find_package(ROOT REQUIRED)
include(${ROOT_USE_FILE})

# corral_m runs corral_loop.cxx over the outTree with CalcRm (event mixing).
# (An earlier convolution version, CalcRc, was removed 2026-09-26: convolution
# cannot work on datasets that need pair cuts.)
set(COMMON_SOURCES
	src/corral_main.cxx
	src/corral_utils.cxx
	src/corral_loop.cxx
	src/corral_finalize.cxx
)

# The old Makefile linked root-config's --evelibs set plus -lMinuit -lGuiHtml
# -lHistPainter -- template leftovers from a shared base Makefile. Nothing here
# includes or calls into TEve/TGeo/TGWindow/TMinuit/RDataFrame/threading/network
# APIs (checked via grep over every #include and call site in src/), so this is
# trimmed to what the source actually uses: Core services, histogramming/graphs,
# canvas+PostScript output (for the QA .ps/.pdf), tree I/O via TTreeReader, and
# TVector3/TLorentzVector (Physics) + TMath (MathCore).
set(CORRAL_ROOT_LIBS
	ROOT::Core ROOT::RIO ROOT::Hist ROOT::Graf ROOT::Graf3d ROOT::Gpad
	ROOT::Tree ROOT::TreePlayer ROOT::Postscript ROOT::Physics ROOT::MathCore
)

# The binary lands at the repo root (not a build/ subdir) so the run scripts'
# `./corral_m` keeps working unchanged.
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR})

add_executable(corral_m ${COMMON_SOURCES} src/CalcRm.cxx)
target_include_directories(corral_m PRIVATE src)
target_link_libraries(corral_m PRIVATE ${CORRAL_ROOT_LIBS})

# Default project directory (lists/<set>/, root/<set>/ read by Finalize) = this source tree;
# the environment variable CORRAL_DIR overrides it at run time (finalize_hists.h, CorralDir()).
target_compile_definitions(corral_m PRIVATE CORRAL_DIR_DEFAULT="${CMAKE_SOURCE_DIR}/")
