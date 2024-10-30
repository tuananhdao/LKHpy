#include <pybind11/pybind11.h>
namespace py = pybind11;

extern "C" {
    #include "LKH.h"
    #include "Genetic.h"
    #include "BIT.h"
}
#define STRINGIFY(x) #x
#define MACRO_STRINGIFY(x) STRINGIFY(x)

#include "par_file.h"
#include "cost_matrix.h"

PYBIND11_MODULE(LKHpy, m) {
    m.doc() = R"pbdoc(
        Pybind11 example plugin
        -----------------------

        .. currentmodule:: LKHpy

        .. autosummary::
           :toctree: _generate

            par_file
            cost_matrix
    )pbdoc";

    m.def("par_file", &par_file, R"pbdoc(
        Run KLH algorithm with a parameter file .par
    )pbdoc");

    m.def ("cost_matrix", &cost_matrix, R"pbdoc(
        Run KLH algorithm given a cost matrix and parameters
    )pbdoc");

#ifdef VERSION_INFO
    m.attr("__version__") = MACRO_STRINGIFY(VERSION_INFO);
#else
    m.attr("__version__") = "dev";
#endif
}
