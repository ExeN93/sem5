def conflicts_tuple(board):
    n = len(board)
    for i in range(n):
        for j in range(i + 1, n):
            r1, c1 = board[i]
            r2, c2 = board[j]
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

def generate_successors_tuple(board, n):
    current_queens = len(board)
    if current_queens >= n:
        return []
    
    successors = []
    row = current_queens
    for col in range(n):
        new_board = list(board)
        new_board.append((row, col))
        successors.append(new_board)
    
    return successors

def generate_successors_vector(board, n):
    current_queens = len(board)
    if current_queens >= n:
        return []
    
    successors = []
    for col in range(n):
        new_board = list(board)
        new_board.append(col)
        successors.append(new_board)
    
    return successors

def generate_successors(board, n, rep_type):
    if rep_type == 'tuple':
        return generate_successors_tuple(board, n)
    elif rep_type == 'vector':
        return generate_successors_vector(board, n)


def bfs(n, rep_type):
    s0 = []
    
    closed = set()
    open_queue = [s0]
    open_set = {tuple(s0)}
    
    while open_queue:
        s = open_queue.pop(0)
        s_hash = tuple(s)
        open_set.discard(s_hash)
        
        # sprawdź czy stan końcowy
        if len(s) == n:
            has_conflicts = conflicts_tuple(s) if rep_type == 'tuple' else conflicts_vector(s)
            if not has_conflicts:
                return s, len(open_queue), len(closed)
        
        successors = generate_successors(s, n, rep_type)
        
        for t in successors:
            # warunek nie dodawania stanów z konfliktami
            has_conflicts = conflicts_tuple(t) if rep_type == 'tuple' else conflicts_vector(t)
            if has_conflicts:
                continue
                
            t_hash = tuple(t)
            if t_hash not in closed and t_hash not in open_set:
                open_queue.append(t)
                open_set.add(t_hash)
        
        closed.add(s_hash)
    
    return None, 0, len(closed)

def dfs(n, rep_type):
    s0 = []
    
    closed = set()
    open_stack = [s0]
    open_set = {tuple(s0)}
    
    while open_stack:
        s = open_stack.pop()
        s_hash = tuple(s)
        open_set.discard(s_hash)

        # sprawdź czy stan końcowy
        if len(s) == n:
            has_conflicts = conflicts_tuple(s) if rep_type == 'tuple' else conflicts_vector(s)
            if not has_conflicts:
                return s, len(open_stack), len(closed)
        
        successors = generate_successors(s, n, rep_type)
        
        for t in successors:
            # zabezpieczenie przed dodawaniem stanów z konfliktami
            has_conflicts = conflicts_tuple(t) if rep_type == 'tuple' else conflicts_vector(t)
            if has_conflicts:
                continue

            t_hash = tuple(t)
            if t_hash not in closed and t_hash not in open_set:
                open_stack.append(t)
                open_set.add(t_hash)
        
        closed.add(s_hash)
    
    return None, 0, len(closed)