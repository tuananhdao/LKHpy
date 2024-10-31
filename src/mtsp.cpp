#include "mtsp.h"
#include "cost_matrix.h"

py::array_t<int> mtsp(py::array_t<int> array, int salesmen) {
    py::dict params;
    params[pybind11::str("RUNS")] = "1";
    params[pybind11::str("SALESMEN")] = salesmen;
    return cost_matrix(array, params);
}