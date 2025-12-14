/*
taski trzeba zrobić i mutexy na locki
*/

#include <iostream>
#include <vector>
#include <fstream>
#include <chrono>
#include <thread>
#include <omp.h>

using namespace std;

const int WIDTH = 15;
const int HEIGHT = 15;

vector<vector<int>> maze = {
    {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {-1,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, -1},
    {-1, -1, -1, -1, -1,  0, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {-1, -1, -1, -1, -1,  0, -1, -1, -1, -1,  0, -1, -1, -1, -1},
    {-1, -1, -1, -1, -1,  0, -1, -1, -1, -1,  0, -1, -1, -1, -1},
    {-1,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, -1, -1},
    {-1, -1, -1, -1, -1,  0, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {-1, -1, -1, -1, -1,  0, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {-1, -1, -1, -1, -1,  0, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {-1, -1, -1, -1, -1,  0, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {-1, -1, -1, -1, -1,  0,  0,  0,  0,  0, -1, -1, -1, -1, -1},
    {-1, -1, -1, -1, -1,  0, -1,  0, -1, -1, -1, -1, -1, -1, -1},
    {-1, -1, -1, -1, -1,  0,  0,  0, -1, -1, -1, -1, -1, -1, -1},
    {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,  0, -1}
};

// vector<vector<int>> maze = {
//     {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
//     {-1,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, -1},
//     {-1, -1, -1, -1, -1,  0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,  0, -1, -1, -1, -1, -1, -1, -1, -1},
//     {-1, -1, -1, -1, -1,  0, -1, -1, -1, -1,  0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,  0, -1, -1, -1, -1, -1, -1, -1, -1},
//     {-1, -1, -1, -1, -1,  0, -1, -1, -1, -1,  0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,  0, -1, -1, -1, -1, -1, -1, -1, -1},
//     {-1,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, -1},
//     {-1, -1, -1, -1, -1,  0, -1, -1, -1, -1, -1, -1, -1,  0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,  0, -1},
//     {-1, -1, -1, -1, -1,  0, -1, -1, -1, -1, -1, -1, -1,  0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,  0, -1},
//     {-1, -1, -1, -1, -1,  0, -1, -1, -1, -1, -1, -1, -1,  0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,  0, -1},
//     {-1, -1, -1, -1, -1,  0, -1, -1, -1, -1, -1, -1, -1,  0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,  0, -1},
//     {-1, -1, -1, -1, -1,  0,  0,  0,  0,  0,  0,  0,  0,  0, -1, -1, -1, -1, -1, -1,  0,  0,  0,  0,  0,  0,  0,  0,  0, -1},
//     {-1, -1, -1, -1, -1,  0, -1,  0, -1, -1, -1, -1, -1,  0, -1, -1, -1, -1, -1, -1,  0, -1, -1, -1, -1, -1, -1, -1, -1, -1},
//     {-1, -1, -1, -1, -1,  0,  0,  0, -1, -1, -1, -1, -1,  0, -1, -1, -1, -1, -1, -1,  0, -1, -1, -1, -1, -1, -1, -1, -1, -1},
//     {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,  0, -1, -1, -1, -1, -1, -1,  0, -1, -1, -1, -1, -1, -1, -1, -1, -1},
//     {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,  0, -1, -1, -1, -1, -1, -1,  0, -1, -1, -1, -1, -1, -1, -1, -1, -1},
//     {-1,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, -1},
//     {-1,  0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,  0, -1},
//     {-1,  0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,  0, -1},
//     {-1,  0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,  0, -1},
//     {-1,  0, -1, -1, -1, -1, -1, -1,  0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,  0, -1},
//     {-1,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, -1},
//     {-1, -1, -1, -1, -1, -1, -1, -1,  0, -1,  0, -1, -1, -1, -1, -1, -1, -1, -1, -1,  0, -1, -1, -1, -1, -1, -1, -1, -1, -1},
//     {-1, -1, -1, -1, -1, -1, -1, -1,  0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,  0, -1, -1, -1, -1, -1, -1, -1, -1, -1},
//     {-1, -1, -1, -1, -1, -1, -1, -1,  0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,  0, -1, -1, -1, -1, -1, -1, -1, -1, -1},
//     {-1,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, -1},
//     {-1,  0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
//     {-1,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, -1},
//     {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,  0, -1},
//     {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,  0, -1},
//     {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}
// };

vector<vector<omp_lock_t>> cell_locks;

int next_thread_id = 1;
int total_threads = 0;
int cells_visited[1000] = {0};
int children_count[1000] = {0};

int dx[] = {0, 1, 0, -1};
int dy[] = {1, 0, -1, 0};

void init_maze() {
    cell_locks.resize(HEIGHT);
    for(int i = 0; i < HEIGHT; i++) {
        cell_locks[i].resize(WIDTH);
        for(int j = 0; j < WIDTH; j++) {
            omp_init_lock(&cell_locks[i][j]);
        }
    }
}

void destroy_locks() {
    for(int i = 0; i < HEIGHT; i++) {
        for(int j = 0; j < WIDTH; j++) {
            omp_destroy_lock(&cell_locks[i][j]);
        }
    }
}

void explore_maze(int start_x, int start_y, int parent_id) {
    int my_id;
    #pragma omp critical
    {
        my_id = next_thread_id++;
        total_threads++;
    }
    
    cout << "Wątek " << my_id << " (rodzic: " << parent_id 
         << ") rozpoczyna eksplorację od (" << start_x << ", " << start_y << ")" << endl;
    
    int x = start_x;
    int y = start_y;
    
    while(true) {
        bool can_continue = false;
        
        omp_set_lock(&cell_locks[x][y]);
        
        if(maze[x][y] != 0) {
            cout << "Wątek " << my_id << " - komórka (" << x << ", " << y 
                 << ") już zajęta lub ściana, kończy pracę" << endl;
            omp_unset_lock(&cell_locks[x][y]);
            break;
        }
        
        maze[x][y] = my_id;
        cells_visited[my_id]++;
        can_continue = true;
        
        cout << "Wątek " << my_id << " oznaczył komórkę (" << x << ", " << y << ")" << endl;
        
        omp_unset_lock(&cell_locks[x][y]);
        
        if(!can_continue) {
            break;
        }
        
        this_thread::sleep_for(chrono::milliseconds(10));
        
        vector<int> available_directions;
        
        for(int dir = 0; dir < 4; dir++) {
            int nx = x + dx[dir];
            int ny = y + dy[dir];
            
            if(nx < 0 || nx >= HEIGHT || ny < 0 || ny >= WIDTH) {
                continue;
            }
            
            omp_set_lock(&cell_locks[nx][ny]);
            if(maze[nx][ny] == 0) {
                available_directions.push_back(dir);
            }
            omp_unset_lock(&cell_locks[nx][ny]);
        }
        
        if(available_directions.empty()) {
            cout << "Wątek " << my_id << " brak dostępnych ścieżek, kończy pracę" << endl;
            break;
        }
        
        int main_direction = available_directions[0];
        int main_x = x + dx[main_direction];
        int main_y = y + dy[main_direction];
        
        cout << "Wątek " << my_id << " kontynuuje do (" << main_x << ", " << main_y << ")" << endl;
        
        if(available_directions.size() > 1) {
            cout << "Wątek " << my_id << " wykrył rozwidlenie w (" << x << ", " << y 
                 << ") - tworzy " << (available_directions.size() - 1) << " potomków" << endl;
            
            for(size_t i = 1; i < available_directions.size(); i++) {
                int dir = available_directions[i];
                int nx = x + dx[dir];
                int ny = y + dy[dir];
                
                cout << "Wątek " << my_id << " tworzy potomka dla rozwidlenia do (" 
                     << nx << ", " << ny << ")" << endl;
                
                #pragma omp task
                {
                    explore_maze(nx, ny, my_id);
                }
                children_count[my_id]++;
            }
        }
        
        x = main_x;
        y = main_y;
    }
    
    #pragma omp taskwait
    
    cout << "Wątek " << my_id << " zakończył całkowitą eksplorację (dzieci: " 
         << children_count[my_id] << ", odwiedzone komórki: " 
         << cells_visited[my_id] << ")" << endl;
}

void save_to_ppm(const char* filename) {
    ofstream file(filename);
    
    file << "P3\n";
    file << WIDTH << " " << HEIGHT << "\n";
    file << "255\n";
    
    int max_id = 0;
    for(int i = 0; i < HEIGHT; i++) {
        for(int j = 0; j < WIDTH; j++) {
            if(maze[i][j] > max_id) {
                max_id = maze[i][j];
            }
        }
    }
    
    for(int i = 0; i < HEIGHT; i++) {
        for(int j = 0; j < WIDTH; j++) {
            if(maze[i][j] == -1) {
                file << "0 0 0\n";
            } else if(maze[i][j] == 0) {
                file << "255 255 255\n";
            } else {
                int id = maze[i][j];
                int hue = (id * 360 / max_id) % 360;
                
                int r, g, b;
                int h_sector = hue / 60;
                int f = hue % 60;
                
                switch(h_sector) {
                    case 0: r = 255; g = f * 255 / 60; b = 0; break;
                    case 1: r = (60-f) * 255 / 60; g = 255; b = 0; break;
                    case 2: r = 0; g = 255; b = f * 255 / 60; break;
                    case 3: r = 0; g = (60-f) * 255 / 60; b = 255; break;
                    case 4: r = f * 255 / 60; g = 0; b = 255; break;
                    default: r = 255; g = 0; b = (60-f) * 255 / 60; break;
                }
                
                file << r << " " << g << " " << b << "\n";
            }
        }
    }
    
    file.close();
}

void display_statistics() {
    cout << "\nSTATYSTYKI" << endl;
    cout << "Całkowita liczba wątków: " << total_threads << endl;
    
    int max_children = 0;
    int max_children_thread = 0;
    int max_cells = 0;
    int max_cells_thread = 0;
    
    for(int i = 1; i < next_thread_id; i++) {
        if(children_count[i] > max_children) {
            max_children = children_count[i];
            max_children_thread = i;
        }
        if(cells_visited[i] > max_cells) {
            max_cells = cells_visited[i];
            max_cells_thread = i;
        }
    }
    
    cout << "Wątek z największą liczbą dzieci: " << max_children_thread 
         << " (dzieci: " << max_children << ")" << endl;
    cout << "Wątek odwiedzający najwięcej komórek: " << max_cells_thread 
         << " (komórki: " << max_cells << ")" << endl;
}

int main() {
    
    init_maze();
    
    #pragma omp parallel
    {
        #pragma omp single
        {
            explore_maze(1, 1, 0);
        }
    }
    
    display_statistics();
    
    save_to_ppm("labirynt.ppm");
    
    destroy_locks();
    
    return 0;
}