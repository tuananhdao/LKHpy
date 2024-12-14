import pytest
import numpy as np
import LKHpy as LK
import json
from calculate_cost import calculate_minmax_cost_from_cost_matrix

SHOW_OUTPUT = False
target_cost = 600000000

def test_mtsp():
    with open('tests/data/nghi2000.txt', 'r') as json_file:
        cost_matrix = json.load(json_file)
    params = {
        '#SHOW_OUTPUT': SHOW_OUTPUT,
        'SPECIAL': '',
        'INITIAL_TOUR_ALGORITHM': 'NEAREST-NEIGHBOR',
        "MOVE_TYPE": "3 SPECIAL",
        "SUBSEQUENT_MOVE_TYPE": "5",
        'SEED': 1,
        'MTSP_OBJECTIVE': 'MINMAX',
        'MAX_CANDIDATES': 6,
        'MAX_TRIALS': 10000,
        'SALESMEN': 4,
        'RUNS': 1,
        'TOTAL_TIME_LIMIT': 20}
    result = LK.cost_matrix(cost_matrix, params)
    cost = calculate_minmax_cost_from_cost_matrix(cost_matrix, result)
    print(cost)
    assert cost < target_cost, f"Expected cost smaller than {target_cost}, got {cost}"

if __name__ == '__main__':
    SHOW_OUTPUT = True
    test_mtsp()