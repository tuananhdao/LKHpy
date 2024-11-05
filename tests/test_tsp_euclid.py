import pytest
import numpy as np
import LKHpy as LK
from calculate_cost import calculate_cost_from_cost_matrix, calculate_cost_from_node_coord

# test data: berlin52
test_data = [
    ([[0, 0], [0, 1], [1, 0]], [0, 1, 2, 0]),
    ([[565, 575],[25, 185],[345, 750],[945, 685],[845, 655],[880, 660],[25, 230],[525, 1000],[580, 1175],[650, 1130],[1605, 620],[1220, 580],[1465, 200],[1530, 5],[845, 680],[725, 370],[145, 665],[415, 635],[510, 875],[560, 365],[300, 465],[520, 585],[480, 415],[835, 625],[975, 580],[1215, 245],[1320, 315],[1250, 400],[660, 180],[410, 250],[420, 555],[575, 665],[1150, 1160],[700, 580],[685, 595],[685, 610],[770, 610],[795, 645],[720, 635],[760, 650],[475, 960],[95, 260],[875, 920],[700, 500],[555, 815],[830, 485],[1170, 65],[830, 610],[605, 625],[595, 360],[1340, 725],[1740, 245]],[0, 21, 30, 17, 2, 16, 20, 41, 6, 1, 29, 22, 19, 49, 28, 15, 45, 43, 33, 34, 35, 38, 39, 36, 37, 47, 23, 4, 14, 5, 3, 24, 11, 27, 26, 25, 46, 12, 13, 51, 10, 50, 32, 42, 9, 8, 7, 40, 18, 44, 31, 48, 0])
]

@pytest.mark.parametrize("coordinates, exact_tour", test_data)
def test_tsp_euclid(coordinates, exact_tour):
    params = {
        'MOVE_TYPE' : 5,
        'PATCHING_C' : 3,
        'PATCHING_A' : 2,
        'RUNS' : 1,
        'TOTAL_TIME_LIMIT' : 1
    }
    result = LK.euclid(coordinates, params)
    assert calculate_cost_from_node_coord(coordinates, result) == calculate_cost_from_node_coord(coordinates, exact_tour), f"Expected {exact_tour}, got {result}"