def calculate_cost_from_cost_matrix(cost_matrix, tour):
    total_cost = 0
    for i in range(len(tour) - 1):
        total_cost += cost_matrix[tour[i]][tour[i + 1]]
    return total_cost

def calculate_minmax_cost_from_cost_matrix(cost_matrix, tour):
    max_cost = 0
    current_cost = 0
    for i in range(len(tour) - 1):
        if (tour[i] == 0 and tour[i + 1] == 0):
            max_cost = max(max_cost, current_cost)
            current_cost = 0
        else:
            current_cost += cost_matrix[tour[i]][tour[i + 1]]
    max_cost = max(max_cost, current_cost)
    return max_cost

def calculate_cost_from_node_coord(node_coord, tour):
    total_cost = 0
    for i in range(len(tour) - 1):
        total_cost += calculate_euc_2d_distance(node_coord[tour[i]], node_coord[tour[i + 1]])
    return total_cost

def calculate_euc_2d_distance(node1, node2):
    return ((node1[0] - node2[0])**2 + (node1[1] - node2[1])**2)**0.5