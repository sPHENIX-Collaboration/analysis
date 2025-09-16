# =============================================================================================
# Project Setup
# ====================================================================
cmake_minimum_required(VERSION 3.16)
project(EMCal-Calibration)

# ====================================================================
# Compiler Settings
# ====================================================================
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
add_compile_options(
    -Wall -Wextra -pedantic -Wshadow -Wconversion -Wsign-conversion
    -Wnull-dereference -Wformat=2 -Wduplicated-cond -Wuseless-cast
    -Wold-style-cast -Wnoexcept -Woverloaded-virtual
)

# ====================================================================
# Find Dependencies
# ====================================================================
find_package(ROOT REQUIRED)
set(SPHENIX_INSTALL "$ENV{OFFLINE_MAIN}")
set(MY_INSTALL "$ENV{MYINSTALL}")

include_directories(
    "${MY_INSTALL}/include"
    SYSTEM ${ROOT_INCLUDE_DIRS}
    SYSTEM "${SPHENIX_INSTALL}/include"
    SYSTEM "${SPHENIX_INSTALL}/rootmacros"
)
link_directories(
    "${MY_INSTALL}/lib"
    "${SPHENIX_INSTALL}/lib"
)

# ====================================================================
# Define Library Linkages
# ====================================================================
set(SPHENIX_LIBS
    calo_io emcNoisyTowerFinder LiteCaloEvalTowSlope cdbobjects SubsysReco calibCaloEmc_pi0
    fun4all fun4allraw phool calo_reco ffamodules globalvertex mbd
)

# --- Define a variable for common libraries for standalone macros ---
set(STANDALONE_MACRO_LIBS
    ${ROOT_LIBS}
    ROOT::Core      # For gSystem, etc.
    ROOT::RIO       # For TFile, TTree I/O
    ROOT::Hist      # For TH1, TFitResultPtr,
    ROOT::Graf      # For base graphics classes
    ROOT::Gpad      # For TCanvas, TPad,
    ROOT::Tree      # For TTree
    ${SPHENIX_LIBS}
)

# ====================================================================
# Define Executable Targets
# ====================================================================
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# --- Fun4All_sEPD ---
add_executable(Fun4All_EMCal macros/Fun4All_EMCal.C)
target_link_libraries(Fun4All_EMCal PRIVATE
    ROOT::Core # Needs explicit Core for gSystem
    ROOT::RIO  # For TFile, TTree I/O
    ${ROOT_LIBS}
    ${SPHENIX_LIBS}
)

# --- display ---
add_executable(display macros/display.C)
target_link_libraries(display PRIVATE ${STANDALONE_MACRO_LIBS})

# --- display ---
add_executable(displayv2 macros/displayv2.C)
target_link_libraries(displayv2 PRIVATE ${STANDALONE_MACRO_LIBS})

# --- genDefaultCalib ---
add_executable(genDefaultCalib macros/genDefaultCalib.C)
target_link_libraries(genDefaultCalib PRIVATE ${STANDALONE_MACRO_LIBS})

# --- doTscFit ---
add_executable(doTscFit macros/doTscFit.C)
target_link_libraries(doTscFit PRIVATE ${STANDALONE_MACRO_LIBS})

# --- doFitAndCalibUpdate ---
add_executable(doFitAndCalibUpdate macros/doFitAndCalibUpdate.C)
target_link_libraries(doFitAndCalibUpdate PRIVATE ${STANDALONE_MACRO_LIBS})

# --- quickHotMapGen ---
add_executable(quickHotMapGen macros/quickHotMapGen.C)
target_link_libraries(quickHotMapGen PRIVATE ${STANDALONE_MACRO_LIBS})
