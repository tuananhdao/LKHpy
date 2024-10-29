# LkhPy
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