# =============================================================================================
# Project Setup
# ====================================================================
cmake_minimum_required(VERSION 3.16)
project(sEPD-Study)

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
    calo_io calo_reco fun4all calotrigger centrality ffamodules mbd epd
    eventplaneinfo zdcinfo globalvertex phool phparameter sphenixnpc cdbobjects
    jetbackground g4jets jetbase particleflow g4detectors_io
)
set(MY_LIBS sEPDValidation)

# --- Define a variable for common libraries for standalone macros ---
set(STANDALONE_MACRO_LIBS
    ${ROOT_LIBS}
    ROOT::Core      # For gSystem, etc.
    ROOT::RIO       # For TFile, TTree I/O
    ROOT::Hist      # For TH1, TFitResultPtr,
    ROOT::Graf      # For base graphics classes
    ROOT::HistPainter # For TPaletteAxis
    ROOT::Gpad      # For TCanvas, TPad,
    ROOT::Tree      # For TTree
    ${SPHENIX_LIBS}
)

# ====================================================================
# Define Executable Targets
# ====================================================================
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# --- Fun4All_sEPD ---
add_executable(Fun4All_sEPD macros/Fun4All_sEPD.C)
target_link_libraries(Fun4All_sEPD PRIVATE
    ROOT::Core # Needs explicit Core for gSystem
    ${ROOT_LIBS}
    ${SPHENIX_LIBS}
    ${MY_LIBS}
)

# --- Fun4All_CaloCheck ---
add_executable(Fun4All_CaloCheck macros/Fun4All_CaloCheck.C)
target_link_libraries(Fun4All_CaloCheck PRIVATE
    ROOT::Core # Needs explicit Core for gSystem
    ${ROOT_LIBS}
    ${SPHENIX_LIBS}
    ${MY_LIBS}
)

# --- Fun4All_Jet ---
add_executable(Fun4All_Jet macros/Fun4All_Jet.C)
target_link_libraries(Fun4All_Jet PRIVATE
    ROOT::Core # Needs explicit Core for gSystem
    ${ROOT_LIBS}
    ${SPHENIX_LIBS}
    ${MY_LIBS}
)

# --- Fun4All_sEPD_DataMC ---
add_executable(Fun4All_sEPD_DataMC macros/Fun4All_sEPD_DataMC.C)
target_link_libraries(Fun4All_sEPD_DataMC PRIVATE
    ROOT::Core # Needs explicit Core for gSystem
    ${ROOT_LIBS}
    ${SPHENIX_LIBS}
    ${MY_LIBS}
)

# --- checkCDB ---
add_executable(checkCDB macros/checkCDB.C)
target_link_libraries(checkCDB PRIVATE ${STANDALONE_MACRO_LIBS})

# --- checkTree ---
add_executable(checkTree macros/checkTree.C)
target_link_libraries(checkTree PRIVATE ${STANDALONE_MACRO_LIBS})

# --- displayHotMap ---
add_executable(displayHotMap macros/displayHotMap.C)
target_link_libraries(displayHotMap PRIVATE ${STANDALONE_MACRO_LIBS})

# --- Bad Tower Map Analysis ---
add_executable(badTowMapAna macros/BadTowerMapsAnalysis.C)
target_link_libraries(badTowMapAna PRIVATE ${STANDALONE_MACRO_LIBS})

# --- Q vector Correction ---
add_executable(Q_vec_corr macros/Q-vector-correction.C)
target_link_libraries(Q_vec_corr PRIVATE ${STANDALONE_MACRO_LIBS})

# --- Q vector Correction v2 ---
add_executable(Q-vec-corrv2 macros/Q-vector-correctionv2.C)
target_link_libraries(Q-vec-corrv2 PRIVATE ${STANDALONE_MACRO_LIBS})

# --- Jet Ana ---
add_executable(Jet-Ana macros/Jet-Ana.C)
target_link_libraries(Jet-Ana PRIVATE ${STANDALONE_MACRO_LIBS})

# --- display ---
add_executable(display macros/display.C)
target_link_libraries(display PRIVATE ${STANDALONE_MACRO_LIBS})

# --- displayv2 ---
add_executable(displayv2 macros/displayv2.C)
target_link_libraries(displayv2 PRIVATE ${STANDALONE_MACRO_LIBS})

# --- display-Jet-Ana ---
add_executable(display-Jet-Ana macros/display-Jet-Ana.C)
target_link_libraries(display-Jet-Ana PRIVATE ${STANDALONE_MACRO_LIBS})

# --- display-Jet-Anav3 ---
add_executable(display-Jet-Anav3 macros/display-Jet-Anav3.C)
target_link_libraries(display-Jet-Anav3 PRIVATE ${STANDALONE_MACRO_LIBS})

# --- display Q vector correction ---
add_executable(display-Q-vec macros/display-Q-vec.C)
target_link_libraries(display-Q-vec PRIVATE ${STANDALONE_MACRO_LIBS})

# ====================================================================
# Optional: Add a Cppcheck Target
# ====================================================================
find_program(CPPCHECK_EXECUTABLE cppcheck)
if(CPPCHECK_EXECUTABLE)
    add_custom_target(cppcheck
        COMMAND ${CPPCHECK_EXECUTABLE} --std=c++20 --enable=all --force
                --suppress=missingIncludeSystem --suppress=missingInclude
                "macros"
        COMMENT "Running Cppcheck static analysis..."
    )
endif()
