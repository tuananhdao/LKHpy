#ifndef TSP_H
#define TSP_H

#include <pybind11/pybind11.h>
namespace py = pybind11;
#include <pybind11/numpy.h>

extern "C" {
    #include "LKH.h"
}

py::array_t<int> tsp(py::array_t<int>);

#endif // TSP_H