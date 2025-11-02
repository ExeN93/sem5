import matplotlib.pyplot as plt
import time
import pandas as pd
from main import bfs, conflicts_tuple, conflicts_vector, dfs


def run_experiment(n_max):
    with open('results.csv', 'w') as f:
        f.write('N,representation,algorithm,solution,open_count,closed_count,time\n')
        
        for n in range(4, n_max + 1):
            print(f"\nN={n}:")
            
            for rep_type in ['vector', 'tuple']:
                for algo_name, algo_func in [('BFS', bfs), ('DFS', dfs)]:
                    print(f"{algo_name} {rep_type}", end=' ')
                    
                    start = time.perf_counter()
                    solution, open_count, closed_count = algo_func(n, rep_type)
                    end = time.perf_counter()
                    
                    exec_time = end - start
                    
                    if solution:
                        f.write(f'{n},{rep_type},{algo_name},"{solution}",{open_count},{closed_count},{exec_time:.5f}\n')
                        print(f"{exec_time:.5f}s, Closed:{closed_count}, Open:{open_count}")
    
    print("\nResults saved!")

df = pd.read_csv('results.csv')

metrics = [
    ('open_count', 'Stany w Open', 'Open'),
    ('closed_count', 'Stany w Closed', 'Closed'),
    ('time', 'Czas wykonania (s)', 'Czas (s)')
]

fig, axes = plt.subplots(1, 3, figsize=(15, 4))

for ax, (col, title, ylabel) in zip(axes, metrics):
    for algo in ['BFS', 'DFS']:
        for rep in ['vector', 'tuple']:
            data = df[(df['algorithm'] == algo) & (df['representation'] == rep)]
            ax.plot(data['N'], data[col], marker='o', label=f'{algo} {rep}')
    ax.set_xlabel('N')
    ax.set_ylabel(ylabel)
    ax.set_title(title)
    ax.legend()
    ax.grid()

plt.tight_layout()
plt.show()

if __name__ == "__main__":
    board_vector = [1, 3, 0, 2]
    board_tuple = [(0, 1), (1, 3), (2, 0), (3, 2)]
    
    print(f"Vector {board_vector}{conflicts_vector(board_vector)}")
    print(f"Tuple {board_tuple}{conflicts_tuple(board_tuple)}")
    
    run_experiment(n_max=10)