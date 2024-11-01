import time
import LKHpy as LK

def test_performance():
    start = time.time()
    solution = LK.par_file('data/whizzkids96.par')
    secondsTaken = time.time() - start
    assert secondsTaken < 10, f"Expected less than 10 seconds, got {secondsTaken}"