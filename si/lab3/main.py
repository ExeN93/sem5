import heapq
import time
import csv
import matplotlib.pyplot as plt

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

# Nowe funkcje heurystyczne
def count_attacks(board, rep_type):
    """Heurystyka H1-: liczba ataków między hetmanami"""
    n = len(board)
    attacks = 0
    
    if rep_type == 'tuple':
        for i in range(n):
            for j in range(i + 1, n):
                r1, c1 = board[i]
                r2, c2 = board[j]
                if r1 == r2 or c1 == c2 or abs(r1 - r2) == abs(c1 - c2):
                    attacks += 1
    else:  # vector
        for i in range(n):
            for j in range(i + 1, n):
                if board[i] == board[j] or abs(board[i] - board[j]) == abs(i - j):
                    attacks += 1
    
    return attacks

def count_attacks_queens(board, n_max, rep_type):
    """Heurystyka H1+: liczba ataków + (N - liczba hetmanów)"""
    attacks = count_attacks(board, rep_type)
    l = len(board)
    return attacks + (n_max - l)

def bestfs(n, rep_type, heuristic='h1-', verbose=False):
    """
    Best-First Search dla problemu n-hetmanów
    
    Args:
        n: liczba hetmanów
        rep_type: 'tuple' lub 'vector'
        heuristic: 'h1-' lub 'h1+'
        verbose: czy wyświetlać stany pośrednie
    """
    s0 = []
    
    closed = set()
    # Kolejka priorytetowa: (wartość_heurystyki, counter, stan)
    counter = 0  # dla zapewnienia stabilności sortowania
    open_heap = []
    open_set = {}
    
    # Oblicz heurystykę dla stanu początkowego
    if heuristic == 'h1-':
        h_value = count_attacks(s0, rep_type)
    else:  # h1+
        h_value = count_attacks_queens(s0, n, rep_type)
    
    heapq.heappush(open_heap, (h_value, counter, s0))
    open_set[tuple(s0)] = h_value
    counter += 1
    
    while open_heap:
        h_current, _, s = heapq.heappop(open_heap)
        s_hash = tuple(s)
        
        if s_hash in open_set:
            del open_set[s_hash]
        
        if verbose:
            print(f"Stan: {s}, Heurystyka: {h_current}")
        
        # Sprawdź czy stan końcowy
        if len(s) == n:
            has_conflicts = conflicts_tuple(s) if rep_type == 'tuple' else conflicts_vector(s)
            if not has_conflicts:
                return s, h_current, len(open_heap), len(closed)
        
        successors = generate_successors(s, n, rep_type)
        
        for t in successors:
            # Warunek nie dodawania stanów z konfliktami
            has_conflicts = conflicts_tuple(t) if rep_type == 'tuple' else conflicts_vector(t)
            if has_conflicts:
                continue
            
            t_hash = tuple(t)
            if t_hash not in closed and t_hash not in open_set:
                # Oblicz heurystykę dla następnika
                if heuristic == 'h1-':
                    h_value = count_attacks(t, rep_type)
                else:  # h1+
                    h_value = count_attacks_queens(t, n, rep_type)
                
                heapq.heappush(open_heap, (h_value, counter, t))
                open_set[t_hash] = h_value
                counter += 1
        
        closed.add(s_hash)
    
    return None, -1, 0, len(closed)

def bfs(n, rep_type):
    s0 = []
    
    closed = set()
    open_queue = [s0]
    open_set = {tuple(s0)}
    
    while open_queue:
        s = open_queue.pop(0)
        s_hash = tuple(s)
        open_set.discard(s_hash)
        
        if len(s) == n:
            has_conflicts = conflicts_tuple(s) if rep_type == 'tuple' else conflicts_vector(s)
            if not has_conflicts:
                return s, len(open_queue), len(closed)
        
        successors = generate_successors(s, n, rep_type)
        
        for t in successors:
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

        if len(s) == n:
            has_conflicts = conflicts_tuple(s) if rep_type == 'tuple' else conflicts_vector(s)
            if not has_conflicts:
                return s, len(open_stack), len(closed)
        
        successors = generate_successors(s, n, rep_type)
        
        for t in successors:
            has_conflicts = conflicts_tuple(t) if rep_type == 'tuple' else conflicts_vector(t)
            if has_conflicts:
                continue

            t_hash = tuple(t)
            if t_hash not in closed and t_hash not in open_set:
                open_stack.append(t)
                open_set.add(t_hash)
        
        closed.add(s_hash)
    
    return None, 0, len(closed)

if __name__ == "__main__":
    print("=== Program do problemu N-hetmanów ===")
    print("Uruchamianie eksperymentów...")
    
    n_start = int(input("Podaj N początkowe (np. 4): "))
    n_end = int(input("Podaj N końcowe (np. 10): "))
    
    # Otwórz plik CSV
    f = open('results.csv', 'w', newline='')
    f.write("N,representation,heuristic,solution,open_count,closed_count,time\n")
    
    # Pętle do testowania
    for n in range(n_start, n_end + 1):
        print(f"\n=== Testowanie dla N={n} ===")
        
        # Dla obu reprezentacji
        for rep in ['tuple', 'vector']:
            
            # BestFS z H1-
            print(f"  BestFS H1- {rep}...")
            start = time.time()
            sol, h_val, open_c, closed_c = bestfs(n, rep, 'h1-')
            czas = time.time() - start
            f.write(f"{n},{rep},h1-,{sol},{open_c},{closed_c},{czas}\n")
            
            # BestFS z H1+
            print(f"  BestFS H1+ {rep}...")
            start = time.time()
            sol, h_val, open_c, closed_c = bestfs(n, rep, 'h1+')
            czas = time.time() - start
            f.write(f"{n},{rep},h1+,{sol},{open_c},{closed_c},{czas}\n")
    
    f.close()
    print("\n=== Gotowe! Wyniki w pliku results.csv ===")
    
    # Pytanie czy zrobić wykresy
    wykresy = input("\nCzy wygenerować wykresy? (t/n): ")
    if wykresy.lower() == 't':
        print("Generowanie wykresów...")
        
        # Wczytaj dane z CSV
        n_values = []
        h1minus_tuple_open = []
        h1minus_tuple_closed = []
        h1minus_tuple_time = []
        h1minus_vector_open = []
        h1minus_vector_closed = []
        h1minus_vector_time = []
        h1plus_tuple_open = []
        h1plus_tuple_closed = []
        h1plus_tuple_time = []
        h1plus_vector_open = []
        h1plus_vector_closed = []
        h1plus_vector_time = []
        
        with open('results.csv', 'r') as f:
            lines = f.readlines()[1:]  # Pomijamy nagłówek
            for line in lines:
                parts = line.strip().split(',')
                n = int(parts[0])
                rep = parts[1]
                heur = parts[2]
                open_c = int(parts[4])
                closed_c = int(parts[5])
                t = float(parts[6])
                
                if heur == 'h1-' and rep == 'tuple':
                    if n not in n_values:
                        n_values.append(n)
                    h1minus_tuple_open.append(open_c)
                    h1minus_tuple_closed.append(closed_c)
                    h1minus_tuple_time.append(t)
                elif heur == 'h1-' and rep == 'vector':
                    h1minus_vector_open.append(open_c)
                    h1minus_vector_closed.append(closed_c)
                    h1minus_vector_time.append(t)
                elif heur == 'h1+' and rep == 'tuple':
                    h1plus_tuple_open.append(open_c)
                    h1plus_tuple_closed.append(closed_c)
                    h1plus_tuple_time.append(t)
                elif heur == 'h1+' and rep == 'vector':
                    h1plus_vector_open.append(open_c)
                    h1plus_vector_closed.append(closed_c)
                    h1plus_vector_time.append(t)
        
        # Wykres 1: Rozmiar Open
        plt.figure(figsize=(10, 6))
        plt.plot(n_values, h1minus_tuple_open, marker='o', label='H1- tuple')
        plt.plot(n_values, h1minus_vector_open, marker='s', label='H1- vector')
        plt.plot(n_values, h1plus_tuple_open, marker='^', label='H1+ tuple')
        plt.plot(n_values, h1plus_vector_open, marker='d', label='H1+ vector')
        plt.xlabel('Liczba hetmanów (N)')
        plt.ylabel('Rozmiar Open')
        plt.title('Rozmiar Open w zależności od N')
        plt.legend()
        plt.grid(True)
        plt.savefig('wykres_open.png')
        print("  Zapisano: wykres_open.png")
        
        # Wykres 2: Rozmiar Closed
        plt.figure(figsize=(10, 6))
        plt.plot(n_values, h1minus_tuple_closed, marker='o', label='H1- tuple')
        plt.plot(n_values, h1minus_vector_closed, marker='s', label='H1- vector')
        plt.plot(n_values, h1plus_tuple_closed, marker='^', label='H1+ tuple')
        plt.plot(n_values, h1plus_vector_closed, marker='d', label='H1+ vector')
        plt.xlabel('Liczba hetmanów (N)')
        plt.ylabel('Rozmiar Closed')
        plt.title('Rozmiar Closed w zależności od N')
        plt.legend()
        plt.grid(True)
        plt.savefig('wykres_closed.png')
        print("  Zapisano: wykres_closed.png")
        
        # Wykres 3: Czas wykonania
        plt.figure(figsize=(10, 6))
        plt.plot(n_values, h1minus_tuple_time, marker='o', label='H1- tuple')
        plt.plot(n_values, h1minus_vector_time, marker='s', label='H1- vector')
        plt.plot(n_values, h1plus_tuple_time, marker='^', label='H1+ tuple')
        plt.plot(n_values, h1plus_vector_time, marker='d', label='H1+ vector')
        plt.xlabel('Liczba hetmanów (N)')
        plt.ylabel('Czas (s)')
        plt.title('Czas wykonania w zależności od N')
        plt.legend()
        plt.grid(True)
        plt.savefig('wykres_czas.png')
        print("  Zapisano: wykres_czas.png")
        
        print("\n=== Wykresy wygenerowane! ===")