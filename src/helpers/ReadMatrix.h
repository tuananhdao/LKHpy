#ifndef READ_MATRIX_H
#define READ_MATRIX_H

#include <pybind11/numpy.h>
#include <pybind11/pybind11.h>
namespace py = pybind11;

extern "C" {
    #include "LKH.h"
    #include "Heap.h"
}

static void CheckSpecificationPart(void);
static char *Copy(char *S);
static void CreateNodes(void);
static int FixEdge(Node * Na, Node * Nb);
static void Read_DIMENSION(size_t dim);
static void Read_EDGE_WEIGHT_FORMAT(void);
static void Read_EDGE_WEIGHT_SECTION(py::array_t<int> array);
static void Read_EDGE_WEIGHT_TYPE(void);
static void Read_TYPE(void);
static int TwoDWeightType(void);
static int ThreeDWeightType(void);

void ReadMatrix(py::array_t<int> array);

#endif // READ_MATRIX_H