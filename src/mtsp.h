#ifndef MTSP_H
#define MTSP_H

#include <pybind11/pybind11.h>
namespace py = pybind11;
#include <pybind11/numpy.h>

extern "C" {
    #include "LKH.h"
}

py::array_t<int> mtsp(py::array_t<int>, int);

#endif // MTSP_H