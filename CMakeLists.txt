cmake_minimum_required(VERSION 3.15 FATAL_ERROR)

project(VocalTractLab 
        VERSION 0.2
        DESCRIPTION "A X System Version of VocalTractLab"
        LANGUAGES CXX
)

set(CMAKE_CXX_COMPILER "c++")
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(with_GUI false)

find_package(OpenAL REQUIRED)

if(with_GUI)
    find_package(OpenGL REQUIRED)
    find_package(GLUT REQUIRED)
    find_package(wxWidgets REQUIRED COMPONENTS base core aui xrc html xml adv gl net qa)
    set(CMAKE_CXX_FLAGS "-framework OpenAL -framework OpenGL -framework GLUT")
    add_definitions(-I/usr/local/lib/wx/include/osx_cocoa-unicode-3.1 -I/usr/local/include/wx-3.1 -DwxUSE_GLCANVAS -D_USE_MATH_DEFINES -D_CRT_SECURE_NO_WARNINGS 
-D UNICODE -D wxUSE_UNICODE -D_FILE_OFFSET_BITS=64 -DWXUSINGDLL -D__WXMAC__ -D__WXOSX__ -D__WXOSX_COCOA__ -D _CRT_SECURE_NO_DEPRECATE -D _CRT_NONSTDC_NO_DEPRECATE -D NDEBUG)
else()
    find_package(pybind11 REQUIRED)
endif(with_GUI)

set(PYTHON_INCLUDE_DIR "/usr/local/anaconda3/include/python3.7m" CACHE PATH "")

set(PROJECT_NAME VocalTractLab)

################################################################################
# Source groups
################################################################################
set(Backend
    "Sources/Backend/AnatomyParams.cpp"
    "Sources/Backend/AnatomyParams.h"
    "Sources/Backend/AudioFile.h"
    "Sources/Backend/Constants.h"
    "Sources/Backend/Dsp.cpp"
    "Sources/Backend/Dsp.h"
    "Sources/Backend/F0EstimatorYin.cpp"
    "Sources/Backend/F0EstimatorYin.h"
    "Sources/Backend/GeometricGlottis.cpp"
    "Sources/Backend/GeometricGlottis.h"
    "Sources/Backend/Geometry.cpp"
    "Sources/Backend/Geometry.h"
    "Sources/Backend/GesturalScore.cpp"
    "Sources/Backend/GesturalScore.h"
    "Sources/Backend/Glottis.cpp"
    "Sources/Backend/Glottis.h"
    "Sources/Backend/IirFilter.cpp"
    "Sources/Backend/IirFilter.h"
    "Sources/Backend/ImpulseExcitation.cpp"
    "Sources/Backend/ImpulseExcitation.h"
    "Sources/Backend/LfPulse.cpp"
    "Sources/Backend/LfPulse.h"
    "Sources/Backend/Matrix2x2.cpp"
    "Sources/Backend/Matrix2x2.h"
    "Sources/Backend/PoleZeroPlan.cpp"
    "Sources/Backend/PoleZeroPlan.h"
    "Sources/Backend/Sampa.cpp"
    "Sources/Backend/Sampa.h"
    "Sources/Backend/SegmentSequence.cpp"
    "Sources/Backend/SegmentSequence.h"
    "Sources/Backend/Signal.cpp"
    "Sources/Backend/Signal.h"
    "Sources/Backend/SoundLib.cpp"
    "Sources/Backend/SoundLib.h"
    "Sources/Backend/Splines.cpp"
    "Sources/Backend/Splines.h"
    "Sources/Backend/StaticPhone.cpp"
    "Sources/Backend/StaticPhone.h"
    "Sources/Backend/Surface.cpp"
    "Sources/Backend/Surface.h"
    "Sources/Backend/Synthesizer.cpp"
    "Sources/Backend/Synthesizer.h"
    "Sources/Backend/TdsModel.cpp"
    "Sources/Backend/TdsModel.h"
    "Sources/Backend/TimeFunction.cpp"
    "Sources/Backend/TimeFunction.h"
    "Sources/Backend/TlModel.cpp"
    "Sources/Backend/TlModel.h"
    "Sources/Backend/TriangularGlottis.cpp"
    "Sources/Backend/TriangularGlottis.h"
    "Sources/Backend/Tube.cpp"
    "Sources/Backend/Tube.h"
    "Sources/Backend/TubeSequence.h"
    "Sources/Backend/TwoMassModel.cpp"
    "Sources/Backend/TwoMassModel.h"
    "Sources/Backend/VocalTract.cpp"
    "Sources/Backend/VocalTract.h"
    "Sources/Backend/VocalTractLabApi.cpp"
    "Sources/Backend/VocalTractLabApi.h"
    "Sources/Backend/VoiceQualityEstimator.cpp"
    "Sources/Backend/VoiceQualityEstimator.h"
    "Sources/Backend/VowelLf.cpp"
    "Sources/Backend/VowelLf.h"
    "Sources/Backend/XmlHelper.cpp"
    "Sources/Backend/XmlHelper.h"
    "Sources/Backend/XmlNode.cpp"
    "Sources/Backend/XmlNode.h"
)
source_group("Backend" FILES ${Backend})

if(with_GUI)
    set(no_group_source_files
    "resource.h"
    "VocalTractLab2.ico"
    )
    source_group("" FILES ${no_group_source_files})

    set(Frontend
    "Sources/Frontend/AnalysisResultsDialog.cpp" "Sources/Frontend/AnalysisResultsDialog.h"
    "Sources/Frontend/AnalysisSettingsDialog.cpp" "Sources/Frontend/AnalysisSettingsDialog.h"
    "Sources/Frontend/AnatomyParamsDialog.cpp" "Sources/Frontend/AnatomyParamsDialog.h"
    "Sources/Frontend/AnnotationDialog.cpp" "Sources/Frontend/AnnotationDialog.h"
    "Sources/Frontend/Application.cpp" "Sources/Frontend/Application.h"
    "Sources/Frontend/AreaFunctionPicture.cpp" "Sources/Frontend/AreaFunctionPicture.h"
    "Sources/Frontend/BasicPicture.cpp" "Sources/Frontend/BasicPicture.h"
    "Sources/Frontend/ColorScale.cpp" "Sources/Frontend/ColorScale.h"
    "Sources/Frontend/CrossSectionPicture.cpp" "Sources/Frontend/CrossSectionPicture.h"
    "Sources/Frontend/Data.cpp" "Sources/Frontend/Data.h"
    "Sources/Frontend/EmaConfigDialog.cpp" "Sources/Frontend/EmaConfigDialog.h"
    "Sources/Frontend/FdsOptionsDialog.cpp" "Sources/Frontend/FdsOptionsDialog.h"
    "Sources/Frontend/FormantOptimizationDialog.cpp" "Sources/Frontend/FormantOptimizationDialog.h"
    "Sources/Frontend/GesturalScorePage.cpp" "Sources/Frontend/GesturalScorePage.h"
    "Sources/Frontend/GesturalScorePicture.cpp" "Sources/Frontend/GesturalScorePicture.h"
    "Sources/Frontend/GlottisDialog.cpp" "Sources/Frontend/GlottisDialog.h"
    "Sources/Frontend/GlottisPanel.cpp" "Sources/Frontend/GlottisPanel.h"
    "Sources/Frontend/GlottisPicture.cpp" "Sources/Frontend/GlottisPicture.h"
    "Sources/Frontend/Graph.cpp" "Sources/Frontend/Graph.h"
    "Sources/Frontend/IconsXpm.h"
    "Sources/Frontend/LfPulseDialog.cpp" "Sources/Frontend/LfPulseDialog.h"
    "Sources/Frontend/LfPulsePicture.cpp" "Sources/Frontend/LfPulsePicture.h"
    "Sources/Frontend/MainWindow.cpp" "Sources/Frontend/MainWindow.h"
    "Sources/Frontend/PhoneticParamsDialog.cpp" "Sources/Frontend/PhoneticParamsDialog.h"
    "Sources/Frontend/PoleZeroDialog.cpp" "Sources/Frontend/PoleZeroDialog.h"
    "Sources/Frontend/PoleZeroPlot.cpp" "Sources/Frontend/PoleZeroPlot.h"
    "Sources/Frontend/SignalComparisonPicture.cpp" "Sources/Frontend/SignalComparisonPicture.h"
    "Sources/Frontend/SignalPage.cpp" "Sources/Frontend/SignalPage.h"
    "Sources/Frontend/SignalPicture.cpp" "Sources/Frontend/SignalPicture.h"
    "Sources/Frontend/SilentMessageBox.cpp" "Sources/Frontend/SilentMessageBox.h"
    "Sources/Frontend/SimpleSpectrumPicture.cpp" "Sources/Frontend/SimpleSpectrumPicture.h"
    "Sources/Frontend/SpectrogramPicture.cpp" "Sources/Frontend/SpectrogramPicture.h"
    "Sources/Frontend/SpectrogramPlot.cpp" "Sources/Frontend/SpectrogramPlot.h"
    "Sources/Frontend/SpectrumOptionsDialog.cpp" "Sources/Frontend/SpectrumOptionsDialog.h"
    "Sources/Frontend/SpectrumPicture.cpp" "Sources/Frontend/SpectrumPicture.h"
    "Sources/Frontend/SynthesisThread.cpp" "Sources/Frontend/SynthesisThread.h"
    "Sources/Frontend/TdsOptionsDialog.cpp" "Sources/Frontend/TdsOptionsDialog.h"
    "Sources/Frontend/TdsPage.cpp" "Sources/Frontend/TdsPage.h"
    "Sources/Frontend/TdsSpatialSignalPicture.cpp" "Sources/Frontend/TdsSpatialSignalPicture.h"
    "Sources/Frontend/TdsTimeSignalPicture.cpp" "Sources/Frontend/TdsTimeSignalPicture.h"
    "Sources/Frontend/TdsTubePicture.cpp" "Sources/Frontend/TdsTubePicture.h"
    "Sources/Frontend/TimeAxisPicture.cpp" "Sources/Frontend/TimeAxisPicture.h"
    "Sources/Frontend/TransitionDialog.cpp" "Sources/Frontend/TransitionDialog.h"
    "Sources/Frontend/VocalTractDialog.cpp" "Sources/Frontend/VocalTractDialog.h"
    "Sources/Frontend/VocalTractPage.cpp" "Sources/Frontend/VocalTractPage.h"
    "Sources/Frontend/VocalTractPicture.cpp" "Sources/Frontend/VocalTractPicture.h"
    "Sources/Frontend/VocalTractShapesDialog.cpp" "Sources/Frontend/VocalTractShapesDialog.h"
    )
    source_group("Frontend" FILES ${Frontend})

    set(ALL_FILES
        ${no_group_source_files}
        ${Backend}
        ${Frontend}
    )
else()
    set(ALL_FILES
        ${Backend}
    )
endif(with_GUI)

################################################################################
# Target
################################################################################
add_compile_options(-w)

if(with_GUI)
    include_directories( ${GLUT_INCLUDE_DIRS} ${OPENGL_INCLUDE_DIRS} })
    include(${wxWidgets_USE_FILE})
    add_executable(${PROJECT_NAME} ${ALL_FILES})
    target_link_libraries( ${PROJECT_NAME} ${wxWidgets_LIBRARIES} ${OPENGL_LIBRARIES} ${OPENAL_LIBRARIES} ${GLUT_LIBRARY} )
else()
    add_library( ${PROJECT_NAME} SHARED ${ALL_FILES} )
    pybind11_add_module(vtl ${ALL_FILES} "./Sources/Backend/VTLApi_pybind.cpp")
    target_include_directories( ${PROJECT_NAME} PUBLIC ${OPENAL_INCLUDE_DIR} ${pybind11_INCLUDE_DIR} )
    target_link_libraries( ${PROJECT_NAME} ${OPENAL_LIBRARY} ${pybind11_LIBRARIES} )
endif(with_GUI)
