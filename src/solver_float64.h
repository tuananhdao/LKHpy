#ifndef SOLVER_FLOAT64_H
#define SOLVER_FLOAT64_H

#include <pybind11/pybind11.h>
namespace py = pybind11;
#include <pybind11/numpy.h>
#include <pybind11/stl.h>

extern "C" {
    #include "LKH.h"
    #include "Genetic.h"
    #include "BIT.h"
}

#include "helpers/ReadParametersFromDictionary.h"
#include "helpers/GetOutputTour.h"
#include "helpers/ReadXY_float64.h"

py::array_t<int> solve_float64(py::str, py::array_t<double>, py::dict);

#endif // COST_MATRIX_H