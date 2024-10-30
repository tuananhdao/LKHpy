#ifndef GET_OUTPUT_TOUR_H
#define GET_OUTPUT_TOUR_H

#include <pybind11/numpy.h>
#include <pybind11/pybind11.h>
namespace py = pybind11;

extern "C" {
    #include "LKH.h"
}

static int Best_CTSP_D_Direction(int *Tour);

py::array_t<int> GetOutputTour(int *Tour);

#endif // GET_OUTPUT_TOUR_H