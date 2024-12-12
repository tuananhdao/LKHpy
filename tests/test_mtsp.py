import pytest
import numpy as np
import LKHpy as LK
from calculate_cost import calculate_cost_from_cost_matrix

SHOW_OUTPUT = False
test_data = [
    ([[0, 1, 2], [1, 0, 3], [2, 3, 0]], 2, [0, 1, 0, 0, 2, 0]),
    ([[0,8,50,31,12,48,36,2,5,39,10],[8,0,38,9,33,37,22,6,4,14,32],[50,38,0,11,55,1,23,46,41,17,52],[31,9,11,0,44,13,16,19,25,18,42],[12,33,55,44,0,54,53,30,28,45,7],[48,37,1,13,54,0,26,47,40,24,51],[36,22,23,16,53,26,0,29,35,34,49],[2,6,46,19,30,47,29,0,3,27,15],[5,4,41,25,28,40,35,3,0,20,21],[39,14,17,18,45,24,34,27,20,0,43],[10,32,52,42,7,51,49,15,21,43,0]], 2, [0, 4, 10, 0, 0, 7, 6, 3, 5, 2, 9, 1, 8, 0])
]

@pytest.mark.parametrize("cost_matrix, salesmen, exact_tour", test_data)
def test_mtsp(cost_matrix, salesmen, exact_tour):
    params = {
        '#SHOW_OUTPUT': SHOW_OUTPUT,
        'SPECIAL': '',
        'INITIAL_TOUR_ALGORITHM': 'NEAREST-NEIGHBOR',
        'SEED': 1,
        'MTSP_OBJECTIVE': 'MINMAX',
        'MAX_CANDIDATES': 6,
        'MAX_TRIALS': 10000,
        'SALESMEN': salesmen,
        'RUNS': 1,
        'TOTAL_TIME_LIMIT': 20
    }
    result = LK.cost_matrix(cost_matrix, params)
    # assert calculate_cost_from_cost_matrix(cost_matrix, result) == calculate_cost_from_cost_matrix(cost_matrix, exact_tour), f"Expected {exact_tour}, got {result}"

if __name__ == '__main__':
    SHOW_OUTPUT = True
    test_mtsp(test_data[1][0], test_data[1][1], test_data[1][2])