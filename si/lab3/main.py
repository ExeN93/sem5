import time
import csv
import matplotlib.pyplot as plt

def conflicts_tuple(board):
    n = len(board)
    for i in range(n):
        for j in range(i + 1, n):
            r1, c1 = board[i]
            r2, c2 = board[j]
            # ten sam wiersz, kolumna albo przekątna
            if r1 == r2 or c1 == c2 or abs(r1 - r2) == abs(c1 - c2):
                return True
    return False

def conflicts_vector(board):
    n = len(board)
    for i in range(n):
        for j in range(i + 1, n):
            # ten sam wiersz (kolumna w wektorze) albo przekątna
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
        
        # sprawdzanie czy stan końcowy
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
            # warunek nie dodawania stanów z konfliktami
            has_conflicts = conflicts_tuple(t) if rep_type == 'tuple' else conflicts_vector(t)
            if has_conflicts:
                continue

            t_hash = tuple(t)
            if t_hash not in closed and t_hash not in open_set:
                open_stack.append(t)
                open_set.add(t_hash)
        
        closed.add(s_hash)
    
    return None, 0, len(closed)

# Heurystyki H1- H1+

def _count_attacks_tuple(board):
    attacks = 0
    n = len(board)
    for i in range(n):
        r1, c1 = board[i]
        for j in range(i + 1, n):
            r2, c2 = board[j]
            if r1 == r2 or c1 == c2 or abs(r1 - r2) == abs(c1 - c2):
                attacks += 1
    return attacks

def _count_attacks_vector(board):
    attacks = 0
    n = len(board)
    for i in range(n):
        for j in range(i + 1, n):
            if board[i] == board[j] or abs(board[i] - board[j]) == abs(i - j):
                attacks += 1
    return attacks

def count_attacks(board, rep_type):
    if rep_type == 'tuple':
        return _count_attacks_tuple(board)
    else:
        return _count_attacks_vector(board)

def count_attacks_queens(board, n, rep_type):
    l = len(board)
    return count_attacks(board, rep_type) + (n - l)

DEBUG = False

def bestfs(n, rep_type, use_h1_plus=False):
    s0 = []
    open_list = [s0]
    open_set = {tuple(s0)}
    closed = set()

    while open_list:
        # sortujemy otwarte według heurystyki najmniejsza wartość na początku
        if use_h1_plus:
            open_list.sort(key=lambda state: count_attacks_queens(state, n, rep_type))
        else:
            open_list.sort(key=lambda state: count_attacks(state, rep_type))

        s = open_list.pop(0)
        s_hash = tuple(s)
        open_set.discard(s_hash)

        if use_h1_plus:
            h_val = count_attacks_queens(s, n, rep_type)
        else:
            h_val = count_attacks(s, rep_type)

        if DEBUG:
            print("Aktualny stan:", s)
            print("Wartość heurystyki:", h_val)
            print("---")

        # sprawdzamy czy mamy pełną planszę bez konfliktów
        if len(s) == n:
            has_conflicts = conflicts_tuple(s) if rep_type == 'tuple' else conflicts_vector(s)
            if not has_conflicts:
                return s, len(open_list), len(closed), h_val

        successors = generate_successors(s, n, rep_type)

        for t in successors:
            t_hash = tuple(t)
            if t_hash in closed or t_hash in open_set:
                continue

            open_list.append(t)
            open_set.add(t_hash)

        closed.add(s_hash)

    return None, len(open_list), len(closed), None

def _read_results(csv_path='results.csv'):
    """Wczytuje results.csv i normalizuje nazwy kolumn (obcina spacje).

    Zwraca listę słowników z kluczami:
    'N', 'representation', 'heuristic', 'solution',
    'open_count', 'closed_count', 'time'.
    """
    rows = []
    try:
        with open(csv_path, newline='') as f:
            reader = csv.DictReader(f)
            if reader.fieldnames is None:
                print("Plik", csv_path, "nie ma nagłówka – brak danych do wykresów.")
                return []

            # mapowanie "oczyszczona_nazwa" -> oryginalna_nazwa
            name_map = {name.strip(): name for name in reader.fieldnames}

            for row in reader:
                norm_row = {}
                for clean_name, orig_name in name_map.items():
                    norm_row[clean_name] = row.get(orig_name)
                rows.append(norm_row)
    except FileNotFoundError:
        print("Nie znaleziono pliku", csv_path)
    return rows


def plot_results(csv_path='results.csv'):
    rows = _read_results(csv_path)
    if not rows:
        return

    # konwersja typów z pomocą LLM (bo csv.DictReader zwraca wszystko jako stringi)
    for r in rows:
        try:
            r['N'] = int(r['N'])
        except (TypeError, ValueError):
            r['N'] = None
        for col in ('open_count', 'closed_count'):
            try:
                r[col] = int(float(r[col])) if r[col] is not None else None
            except (TypeError, ValueError):
                r[col] = None
        try:
            r['time'] = float(r['time']) if r['time'] is not None else None
        except (TypeError, ValueError):
            r['time'] = None

    metrics = [
        ('open_count', 'Rozmiar OPEN'),
        ('closed_count', 'Rozmiar CLOSED'),
        ('time', 'Czas [s]'),
    ]

    for metric, ylabel in metrics:
        series = {}
        for r in rows:
            if r['N'] is None or r[metric] is None:
                continue
            rep = (r.get('representation') or '').strip()
            heur = (r.get('heuristic') or '').strip()
            label = f"{rep}-{heur}" if heur else rep
            series.setdefault(label, []).append((r['N'], r[metric]))

        plt.figure()
        for label, points in series.items():
            points.sort(key=lambda x: x[0])
            xs = [p[0] for p in points]
            ys = [p[1] for p in points]
            plt.plot(xs, ys, marker='o', label=label)

        plt.xlabel('N (liczba hetmanów)')
        plt.ylabel(ylabel)
        plt.title(f'{ylabel} w zależności od N')
        plt.legend()
        plt.grid(True)
        plt.tight_layout()

    plt.show()

def main():
    n = 7
    representations = ['tuple', 'vector']

    with open('results.csv', 'a', newline='') as f:
        writer = csv.writer(f)

        if f.tell() == 0:
            writer.writerow(['N', 'representation', 'heuristic', 'solution',
                             'open_count', 'closed_count', 'time'])

        for rep in representations:
            print("--- N:",n,", representation: ", rep)

            start = time.perf_counter()
            sol, open_c, closed_c = bfs(n, rep)
            elapsed = time.perf_counter() - start
            print("BFS: solution =", bool(sol), "open =", open_c,
                  "closed =", closed_c, "time =", elapsed)
            writer.writerow([n, rep, 'none', 1 if sol else 0,
                             open_c, closed_c, elapsed])

            start = time.perf_counter()
            sol, open_c, closed_c = dfs(n, rep)
            elapsed = time.perf_counter() - start
            print("DFS: solution =", bool(sol), "open =", open_c,
                  "closed =", closed_c, "time =", elapsed)
            writer.writerow([n, rep, 'none', 1 if sol else 0,
                             open_c, closed_c, elapsed])

            # BestFS z H1-
            start = time.perf_counter()
            sol, open_c, closed_c, h_val = bestfs(n, rep, use_h1_plus=False)
            elapsed = time.perf_counter() - start
            print("BestFS H1-: solution =", bool(sol), "open =", open_c,
                  "closed =", closed_c, "time =", elapsed, "h(solution) =", h_val)
            writer.writerow([n, rep, 'H1-', 1 if sol else 0,
                             open_c, closed_c, elapsed])

            # BestFS z H1+
            start = time.perf_counter()
            sol, open_c, closed_c, h_val = bestfs(n, rep, use_h1_plus=True)
            elapsed = time.perf_counter() - start
            print("BestFS H1+: solution =", bool(sol), "open =", open_c,
                  "closed =", closed_c, "time =", elapsed, "h(solution) =", h_val)
            writer.writerow([n, rep, 'H1+', 1 if sol else 0,
                             open_c, closed_c, elapsed])
            
            plot_results('results.csv')

if __name__ == '__main__':
    main()
