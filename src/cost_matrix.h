#ifndef COST_MATRIX_H
#define COST_MATRIX_H

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

py::array_t<int> cost_matrix(py::array_t<int>, py::dict);

#endif // COST_MATRIX_H