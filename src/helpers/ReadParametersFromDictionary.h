#ifndef READ_PARAMETERS_FROM_DICTIONARY_H
#define READ_PARAMETERS_FROM_DICTIONARY_H

#include <iostream>
#include <pybind11/stl.h>
#include <pybind11/pybind11.h>
namespace py = pybind11;

extern "C" {
    #include "LKH.h"
    #include "Genetic.h"
}

static char Delimiters[] = "= \n\t\r\f\v\xef\xbb\xbf";
static char *GetFileName(char *Line);
static char *ReadYesOrNo(int *V);
#undef max
static size_t max(size_t a, size_t b);

void ReadParametersFromDictionary(py::dict params);

#endif // READ_PARAMETERS_FROM_DICTIONARY_H