![Version](https://img.shields.io/badge/Version-0.0.1-blue)
![Language](https://img.shields.io/badge/Language-python-blue)
![License](https://img.shields.io/badge/License-MIT-blue)
[![Pip Actions Status][actions-pip-badge]][actions-pip-link]

[actions-pip-link]:        https://github.com/tuananhdao/lkhpy/actions?query=workflow%3A%22Pip
[actions-pip-badge]:       https://github.com/tuananhdao/lkhpy/workflows/Pip/badge.svg

# LkhPy
<img src="./static/banner.jpg" alt="LkhPy Banner" style='width: 100%; height: auto;'>

**LkhPy** is a Python library for solving travelling salesman problems (TSP) based on <a href="http://akira.ruc.dk/~keld/research/LKH-3/" target="_blank">[**LKH 3**]</a>. The library aims to minimize overheads and optimize the communication between the LKH library written in C and the Python interface. The Python bindings of existing C code is done via [pybind11](https://github.com/pybind/pybind11). The LKH folder containing the C code is *remained unchanged* from the original source.

## Installation

Easy installation via pip

```bash
python3 -m pip install LkhPy
```

## Usage

`LKH.py`
```python
import LkhPy as lp
solution = lp.FromPar('example.par')
```