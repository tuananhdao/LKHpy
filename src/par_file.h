#ifndef PAR_FILE_H
#define PAR_FILE_H

#include <pybind11/numpy.h>
#include <pybind11/pybind11.h>
namespace py = pybind11;

extern "C" {
    #include "LKH.h"
    #include "Genetic.h"
    #include "BIT.h"
}

py::array_t<int> par_file(char * parFileName);

#endif // PAR_FILE_H