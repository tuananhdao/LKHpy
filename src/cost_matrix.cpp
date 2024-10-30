#include "cost_matrix.h"
#include "helpers/ReadParametersFromDictionary.h"

// Function to accept a 2D NumPy array
void cost_matrix(py::array_t<int> array, py::dict params) {
    // Ensure the input is a 2D array
    if (array.ndim() != 2) {
        throw std::runtime_error("Input should be a 2D NumPy array");
    }

    // Get the shape of the array
    auto shape = array.shape();
    size_t rows = shape[0];
    size_t cols = shape[1];

    // Get a pointer to the data
    int* data = static_cast<int*>(array.request().ptr);

    // Use the data directly without allocating more memory
    // Example: Print the array
    for (size_t i = 0; i < rows; ++i) {
        for (size_t j = 0; j < cols; ++j) {
            std::cout << data[i * cols + j] << " ";
        }
        std::cout << std::endl;
    }

    ReadParametersFromDictionary(params);
}