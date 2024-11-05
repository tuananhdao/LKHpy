#ifndef READ_XY_INT_H
#define READ_XY_INT_H

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
static void Read_NODE_COORD_SECTION(py::array_t<int>);
static void Read_EDGE_WEIGHT_TYPE(std::string arrayType);
static void Read_TYPE(void);
static int TwoDWeightType(void);
static int ThreeDWeightType(void);
static void Convert2FullMatrix(void);

void ReadXY_int(py::str, py::array_t<int>);

#endif // READ_XY_INT_H