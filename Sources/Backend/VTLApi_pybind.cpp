#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/complex.h>
namespace py = pybind11;

#include "VocalTractLabApi.h"

PYBIND11_MODULE(vtl, m)
{
    // m.doc() = "Vocal Tract Lab Backend API Library Python Edition";
    py::class_<VocalTractLab>(m, "VocalTractLab")
        .def(py::init<const string>())
        .def("getTractParamInfo", &VocalTractLab::vtlGetTractParamInfo, "Get Vocal Tract Parameters Info")
        .def("getGlottisParamInfo", &VocalTractLab::vtlGetGlottisParamInfo, "Get Glottis Model Parameters Info")
        .def("close", &VocalTractLab::vtlClose, "Close VTL")
        .def("set_anatomy", &VocalTractLab::vtlSetAnatomyParams, "Set Anatomy", py::arg("anatomyParams"))
        .def("get_anatomy", &VocalTractLab::vtlGetAnatomyParams, "Get Anatomy")
        .def("synth_audio", &VocalTractLab::vtlSynthAudio, "Synthesize audio using given tract and glottis parameters.", 
            py::arg("tractParams"), py::arg("glottisParams"), py::arg("numFrames"), 
            py::arg("frameStep_samples"));
}

