#ifndef COST_MATRIX_H
#define COST_MATRIX_H

#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>
#include <pybind11/stl.h>

void cost_matrix(pybind11::array_t<int>, pybind11::dict);

#endif // COST_MATRIX_H