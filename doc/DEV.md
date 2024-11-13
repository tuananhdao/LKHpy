## Requirements

Required packages: `python3`, `python3-config`, `cmake`, `pytest`.

## Compile python .egg-info
```sh
pip install .
```

## Run tests
```sh
pytest
```

### Debug test
Write `if __name__ == "__main__":` similar to `tests/test_coord_large.py`, setting `#SHOW_OUTPUT = True`.

Then
```bash
python3 test_coord_large.py
```

to see to C output.