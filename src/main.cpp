#include <pybind11/pybind11.h>
namespace py = pybind11;

extern "C" {
    #include "LKH.h"
    #include "Genetic.h"
    #include "BIT.h"
}
#define STRINGIFY(x) #x
#define MACRO_STRINGIFY(x) STRINGIFY(x)

#include "ParFile.cpp"

PYBIND11_MODULE(LkhPy, m) {
    m.doc() = R"pbdoc(
        Pybind11 example plugin
        -----------------------

        .. currentmodule:: LkhPy

        .. autosummary::
           :toctree: _generate

           ParFile
    )pbdoc";

    m.def("ParFile", &ParFile, R"pbdoc(
        Add two numbers

        Some other explanation about the add function.
    )pbdoc");

#ifdef VERSION_INFO
    m.attr("__version__") = MACRO_STRINGIFY(VERSION_INFO);
#else
    m.attr("__version__") = "dev";
#endif
}
