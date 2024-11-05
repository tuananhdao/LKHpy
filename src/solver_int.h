#ifndef SOLVER_INT_H
#define SOLVER_INT_H

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
#include "helpers/ReadMatrix.h"
#include "helpers/ReadXY_int.h"

py::array_t<int> solve_int(py::str, py::array_t<int>, py::dict);

#endif // SOLVER_INT_H