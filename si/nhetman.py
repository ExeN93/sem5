import random
import time

def generate_board(N):
    board = []
    for row in range(N):
        col = random.randint(0, N - 1)
        board.append((row, col))
    return board

def conflicts(board):
    N = len(board)
    for i in range(N):
        r1, c1 = board[i]
        for j in range(i + 1, N):
            r2, c2 = board[j]
            if r1 == r2 or c1 == c2 or abs(r1 - r2) == abs(c1 - c2):
                return True
    return False


def experiment(N, trials):
    start_time = time.time()
    conflict_count = 0

    for _ in range(trials):
        board = generate_board(N)
        if conflicts(board):
            conflict_count += 1

    total_time = time.time() - start_time
    avg_time = total_time / trials
    conflict_rate = conflict_count / trials

    print(f"N = {N}")
    print(f"Conflict rate: {conflict_rate:.3f}")
    print(f"Średni czas jednej próby: {avg_time:.6f} s")


if __name__ == "__main__":
    experiment(N=7, trials=100000)
