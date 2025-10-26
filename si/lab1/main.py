import random
import time

# nqueens problem
#   n_values = [4, 8, 16, 32, 64, 128]
#   rep_types = ['tuple', 'array', 'vector', 'perm']
#   1000 random boards for each n and rep_type

def conflicts_tuple(board):
    n = len(board)
    for i in range(n):
        for j in range(i + 1, n):
            r1, c1 = board[i]
            r2, c2 = board[j]
            if r1 == r2 or c1 == c2 or abs(r1 - r2) == abs(c1 - c2):
                return True
    return False

def conflicts_array(board):
    queens = []
    n = len(board)
    for i in range(n):
        for j in range(n):
            if board[i][j] == 1:
                queens.append((i, j))
    
    for i in range(len(queens)):
        for j in range(i + 1, len(queens)):
            r1, c1 = queens[i]
            r2, c2 = queens[j]
            if r1 == r2 or c1 == c2 or abs(r1 - r2) == abs(c1 - c2):
                return True
    return False

def conflicts_vector(board):
    n = len(board)
    for i in range(n):
        for j in range(i + 1, n):
            if board[i] == board[j] or abs(board[i] - board[j]) == abs(i - j):
                return True
    return False

def conflicts_perm(board):
    n = len(board)
    for i in range(n):
        for j in range(i + 1, n):
            if abs(board[i] - board[j]) == abs(i - j):
                return True
    return False

def conflicts(board):
    if type(board) == tuple:
        return conflicts_perm(board)
    if type(board) == list and len(board) > 0:
        if type(board[0]) == tuple:
            return conflicts_tuple(board)
        if type(board[0]) == list:
            return conflicts_array(board)
        if type(board[0]) == int:
            return conflicts_vector(board)

def generate_board(n, rep_type):
    if rep_type == 'tuple':
        positions = []
        for i in range(n):
            for j in range(n):
                positions.append((i, j))
        return random.sample(positions, n)
    
    if rep_type == 'array':
        board = []
        for i in range(n):
            row = []
            for j in range(n):
                row.append(0)
            board.append(row)
        positions = []
        for i in range(n):
            for j in range(n):
                positions.append((i, j))
        selected = random.sample(positions, n)
        for r, c in selected:
            board[r][c] = 1
        return board
    
    if rep_type == 'vector':
        board = []
        for i in range(n):
            board.append(random.randint(0, n - 1))
        return board
    
    if rep_type == 'perm':
        board = list(range(n))
        random.shuffle(board)
        return tuple(board)

def measure_time(n, rep_type, iterations=1000):
    total_time = 0
    conflicts_count = 0
    
    for i in range(iterations):
        board = generate_board(n, rep_type)
        start = time.perf_counter()
        has_conflict = conflicts(board)
        end = time.perf_counter()
        total_time += (end - start)
        if has_conflict:
            conflicts_count += 1
    
    avg_time = total_time / iterations
    conflict_rate = conflicts_count / iterations
    return avg_time, conflict_rate

def experiment():
    n_values = [4, 8, 16, 32, 64, 128]
    rep_types = ['tuple', 'array', 'vector', 'perm']
    
    f = open('results.csv', 'w', newline='')
    f.write('N,representation_type,execution_time,conflict_rate\n')
    
    for n in n_values:
        for rep in rep_types:
            avg_time, conflict_rate = measure_time(n, rep, 1000)
            f.write(f'{n},{rep},{avg_time:.6f},{conflict_rate:.2f}\n')
            
            
    f.close()

if __name__ == "__main__":
    # testowanie czy konfilkty działają poprawnie
    board_tuple = [(0, 2), (1, 0), (2, 3), (3, 1)]
    board_array = [[0, 0, 1, 0], [1, 0, 0, 0], [0, 0, 0, 1], [0, 1, 0, 0]]
    board_vector = [2, 0, 3, 1]
    board_perm = (2, 0, 3, 1)
    
    print("board_tuple:", conflicts(board_tuple))
    print("board_array:", conflicts(board_array))
    print("board_vector:", conflicts(board_vector))
    print("board_perm:", conflicts(board_perm))
    
    experiment()
    print("Results saved!")