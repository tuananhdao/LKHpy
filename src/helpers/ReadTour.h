#ifndef READ_TOUR_H
#define READ_TOUR_H

#include <pybind11/numpy.h>
#include <pybind11/pybind11.h>
namespace py = pybind11;

extern "C" {
    #include "LKH.h"
    #include "Heap.h"
}

static void CreateNodes(void);

void Read_TOUR_SECTION(FILE ** File);

#endif // READ_TOUR_H