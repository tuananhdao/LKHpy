[![Version](https://img.shields.io/pypi/v/LKHpy)](https://pypi.org/project/LKHpy/)
![Language](https://img.shields.io/badge/Language-python-blue)
![License](https://img.shields.io/badge/License-MIT-blue)
[![Pip Actions Status][actions-pip-badge]][actions-pip-link]

[actions-pip-link]:        https://github.com/tuananhdao/LKHpy/actions?query=workflow%3A%22Pip
[actions-pip-badge]:       https://github.com/tuananhdao/LKHpy/workflows/Pip/badge.svg

<img src="./static/banner.jpg" alt="LKHpy Banner" style='width: 100%; height: auto;'>

**LKHpy** is a Python library for solving travelling salesman problems (TSP) based on <a href="http://akira.ruc.dk/~keld/research/LKH-3/" target="_blank">[**LKH 3**]</a>. The library aims to minimize overheads and optimize the communication between the LKH library written in C and the Python interface. The Python bindings of existing C code is done via [pybind11](https://github.com/pybind/pybind11). The LKH folder containing the C code is *remained unchanged* from the original source.

## Installation

Easy installation via pip

```bash
python3 -m pip install LKHpy
```

> [!NOTE]  
> LKH 3 does not support Windows and neither does LKHpy. On Windows, use `WSL`.

## Usage

```python
import LKHpy as LK

cost_matrix = [[0, 1, 2], [1, 0, 3], [2, 3, 0]]
solution = LK.TSP(cost_matrix)
```

```python
import LKHpy as LK

cost_matrix = [[0, 1, 2], [1, 0, 3], [2, 3, 0]]
salesmen = 2
solution = LK.mTSP(cost_matrix, salesmen)
```

```python
import LKHpy as LK
solution = LK.par_file('example.par')
```