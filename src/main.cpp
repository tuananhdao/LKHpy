#include <pybind11/pybind11.h>
namespace py = pybind11;

extern "C" {
    #include "LKH.h"
    #include "Genetic.h"
    #include "BIT.h"
}
#define STRINGIFY(x) #x
#define MACRO_STRINGIFY(x) STRINGIFY(x)

#include "suppress_output.h"
#include "global_vars.h"
#include "par_file.h"
#include "cost_matrix.h"

PYBIND11_MODULE(LKHpy, m) {
    m.doc() = R"pbdoc(
        LKHpy: Python bindings for the LKH library
        -----------------------

        .. currentmodule:: LKHpy

        .. autosummary::
           :toctree: _generate

            par_file
            cost_matrix
    )pbdoc";

    m.def("par_file",
        [](char* parFileName) -> py::array_t<int> {
            OutputSuppressor suppressor;
            return par_file(parFileName);
        },
        R"pbdoc(
            Run KLH algorithm with a parameter file .par
        )pbdoc");

    m.def("cost_matrix",
        [](py::array_t<int> array, py::dict params) -> py::array_t<int> {
            if (params.contains("#SHOW_OUTPUT")) {
                SHOW_OUTPUT = params.attr("get")("#SHOW_OUTPUT").cast<bool>();
            }

            OutputSuppressor suppressor;

            return cost_matrix(array, params);
        },
        R"pbdoc(
            Run KLH algorithm given a cost matrix and parameters
        )pbdoc");

    m.attr("SHOW_OUTPUT") = &SHOW_OUTPUT; // TODO: this is not working

#ifdef VERSION_INFO
    m.attr("__version__") = MACRO_STRINGIFY(VERSION_INFO);
#else
    m.attr("__version__") = "dev";
#endif
}
