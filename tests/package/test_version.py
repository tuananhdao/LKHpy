import re
import LKHpy as LK

def test_version():
    pattern = r'^[0-9]+\.[0-9]+\.[0-9]+$'
    match = re.match(pattern, LK.__version__)
    assert match is not None, f"Version '{LK.__version__}' does not match the pattern {pattern}"