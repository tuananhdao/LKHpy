[![Version](https://img.shields.io/pypi/v/LKHpy)](https://pypi.org/project/LKHpy/)
![Language](https://img.shields.io/badge/Language-python-blue)
![License](https://img.shields.io/badge/License-MIT-blue)
[![Pip Actions Status][actions-pip-badge]][actions-pip-link]
[![Wheels Actions Status][actions-wheels-badge]][actions-wheels-link]

[actions-pip-link]:        https://github.com/tuananhdao/LKHpy/actions?query=workflow%3A%22Pip
[actions-pip-badge]:       https://github.com/tuananhdao/LKHpy/workflows/Pip/badge.svg
[actions-wheels-link]:     https://github.com/tuananhdao/LKHpy/actions?query=workflow%3AWheels
[actions-wheels-badge]:    https://github.com/tuananhdao/LKHpy/workflows/Wheels/badge.svg

<img src="./static/banner.jpg" alt="LKHpy Banner" style='width: 100%; height: auto;'>

**LKHpy** is a Python library for solving travelling salesman problems (TSP) based on <a href="http://akira.ruc.dk/~keld/research/LKH-3/" target="_blank">[**LKH 3**]</a>. The library aims to minimize overheads and optimize the communication between the LKH library written in C and the Python interface. The Python bindings of existing C code is done via [pybind11](https://github.com/pybind/pybind11). The LKH folder containing the C code *remains unchanged* from the original source.

## Installation

Easy installation via pip. Remember to change `$USERNAME` and `$PAT` to yours.

```bash
pip install git+https://$USERNAME:$PAT@github.com/tuananhdao/LKHpy.git
```

Or if SSH has correct credentials,

```bash
pip install git+ssh://git@github.com/tuananhdao/LKHpy.git
```

> [!NOTE]
> Built versions are tested against the latest Ubuntu and macOS.
> LKH 3 does not support Windows (no one should use Windows anyway), and neither does LKHpy. As a workaround, use `WSL`.

## Usage

### Import
```python
import LKHpy as LK
```

### Using cost matrix (distance matrix)

See [PARAMETERS](LKH-3.0.12/DOC/LKH-3_PARAMETERS.pdf) for parameter lists. Some [custom parameters](#lkhpy-custom-parameters) are defined by LKHpy and must start with '#'.

```python
params = {
        '#SHOW_OUTPUT': False,
        'SPECIAL': '',
        'MTSP_OBJECTIVE': 'MINMAX',
        'MAX_CANDIDATES': 6,
        'MAX_TRIALS': 10000,
        'SALESMEN': 2,
        'RUNS': 1,
        'TOTAL_TIME_LIMIT': 7}
cost_matrix = [[0, 1, 2], [1, 0, 3], [2, 3, 0]] # int
solution = LK.cost_matrix(cost_matrix, params)
```

### Using coordinates
#### Euclid distance

```python
coordinates = [[1, 2], [2, 1], [3, 3], [1, 3]] # int
solution = LK.euclid(coordinates, params)
```
##### GEOM and GEO distance (latitude longitude)
```python
coordinates = [[0.1, 0.1], [0.2, 1.2], [1.3, 0.3]] # double
solution = LK.geo(coordinates, params)
```

```python
coordinates = [[0.1, 0.1], [0.2, 1.2], [1.3, 0.3]] # double
solution = LK.geom(coordinates, params)
```


### Using a `.par` file
See `examples/par_file`
```python
solution = LK.par_file('example.par')
```

### LKHpy custom parameters

| Parameter      | Meaning            | Default |
|----------------|--------------------|---------|
| `#SHOW_OUTPUT` | Show output by LKH | False   |