#include "tsp.h"
#include "cost_matrix.h"

py::array_t<int> tsp(py::array_t<int> array) {
    py::dict params;
    params[pybind11::str("RUNS")] = "1";
    return cost_matrix(array, params);
}