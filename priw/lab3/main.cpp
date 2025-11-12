/*
INSTRUKCJA
użyć locka (safeguard) do ochrony zasobów współdzielonych
id 
<vector> <thread>
tablica mutexów dla korytarzy
PPM z labiryntem 

rozmiar nie ma żadnego znaczenia
tak żeby się na ekran zmieściło 

wątek potomny powinien sprawdzać czy może wejść do korytarza (czy mutex jest dostępny)

na początku najlepiej zrobić na małym labirynciku coś co tylko sprawdzi czy dobrze wątki się zachowują 

ewentualnie:
wyświetlić liczbę wszystkich wątków, 
który wątek miał najwięcej dzieci, 
który wątek odwiedził najwięcej korytarzy
*/

#include <iostream>
#include <thread>
#include <mutex>
#include <vector>
#include <fstream>
#include <chrono>
#include <atomic>

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

vector<vector<unique_ptr<mutex>>> cell_locks;

mutex id_mutex;

// globalny licznik ID wątków
atomic<int> next_thread_id(1);

atomic<int> total_threads(0);
int cells_visited[1000] = {0}; // ile komórek odwiedził każdy wątek
int children_count[1000] = {0}; // ile dzieci miał każdy wątek

// kierunki: prawo, dół, lewo, góra (priorytet dla głównej ścieżki)
int dx[] = {0, 1, 0, -1};
int dy[] = {1, 0, -1, 0};

void init_maze() {
    cell_locks.resize(HEIGHT);
    for(int i = 0; i < HEIGHT; i++) {
        cell_locks[i].resize(WIDTH);
        for(int j = 0; j < WIDTH; j++) {
            cell_locks[i][j] = make_unique<mutex>();
        }
    }
}

void explore_maze(int start_x, int start_y, int parent_id) {
    // przydzielenie unikalne ID dla tego wątku
    int my_id;
    {
        lock_guard<mutex> lock(id_mutex);
        my_id = next_thread_id++;
        total_threads++;
    }
    
    cout << "Wątek " << my_id << " (rodzic: " << parent_id 
         << ") rozpoczyna eksplorację od (" << start_x << ", " << start_y << ")" << endl;
    
    // wektor wątków potomnych (utworzonych w rozwidleniach)
    vector<thread> children;
    
    // bieżąca pozycja
    int x = start_x;
    int y = start_y;
    
    while(true) {
        bool can_continue = false;
        {
            lock_guard<mutex> lock(*cell_locks[x][y]);
            
            if(maze[x][y] != 0) {
                cout << "Wątek " << my_id << " - komórka (" << x << ", " << y 
                     << ") już zajęta lub ściana, kończy pracę" << endl;
                break;
            }
            
            // oznacz komórkę swoim ID
            maze[x][y] = my_id;
            cells_visited[my_id]++;
            can_continue = true;
            
            cout << "Wątek " << my_id << " oznaczył komórkę (" << x << ", " << y << ")" << endl;
        }
        
        if(!can_continue) {
            break;
        }
        
        // krótkie opóźnienie nałożone sztucznie dla lepszej czytelności logów
        this_thread::sleep_for(chrono::milliseconds(10));
        
        vector<int> available_directions;
        
        for(int dir = 0; dir < 4; dir++) {
            int nx = x + dx[dir];
            int ny = y + dy[dir];
            
            // Sprawdź granice
            if(nx < 0 || nx >= HEIGHT || ny < 0 || ny >= WIDTH) {
                continue;
            }
            
            // Sprawdź czy to potencjalny korytarz
            {
                lock_guard<mutex> lock(*cell_locks[nx][ny]);
                if(maze[nx][ny] == 0) {
                    available_directions.push_back(dir);
                }
            }
        }
        
        // Jeśli nie ma dostępnych kierunków, zakończ
        if(available_directions.empty()) {
            cout << "Wątek " << my_id << " - brak dostępnych ścieżek, kończy pracę" << endl;
            break;
        }
        
        // Główna ścieżka: pierwszy kierunek z dostępnych (wg priorytetu)
        int main_direction = available_directions[0];
        int main_x = x + dx[main_direction];
        int main_y = y + dy[main_direction];
        
        cout << "Wątek " << my_id << " kontynuuje do (" << main_x << ", " << main_y << ")" << endl;
        
        // Jeśli są dodatkowe kierunki (rozwidlenie), twórz wątki potomne
        if(available_directions.size() > 1) {
            cout << "Wątek " << my_id << " wykrył rozwidlenie w (" << x << ", " << y 
                 << ") - tworzy " << (available_directions.size() - 1) << " potomków" << endl;
            
            for(size_t i = 1; i < available_directions.size(); i++) {
                int dir = available_directions[i];
                int nx = x + dx[dir];
                int ny = y + dy[dir];
                
                cout << "Wątek " << my_id << " tworzy potomka dla rozwidlenia do (" 
                     << nx << ", " << ny << ")" << endl;
                children.push_back(thread(explore_maze, nx, ny, my_id));
                children_count[my_id]++;
            }
        }
        
        // Wątek macierzysty kontynuuje główną ścieżkę
        x = main_x;
        y = main_y;
    }
    
    // join
    for(auto& child : children) {
        if(child.joinable()) {
            child.join();
        }
    }
    
    cout << "Wątek " << my_id << " zakończył całkowitą eksplorację (dzieci: " 
         << children_count[my_id] << ", odwiedzone komórki: " 
         << cells_visited[my_id] << ")" << endl;
}

void save_to_ppm(const char* filename) {
    ofstream file(filename);
    
    file << "P3\n";
    file << WIDTH << " " << HEIGHT << "\n";
    file << "255\n";
    
    // Znajdź maksymalny ID do normalizacji kolorów
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
                // Ściana czarny
                file << "0 0 0\n";
            } else if(maze[i][j] == 0) {
                // Nieodwiedzony korytarz biały
                file << "255 255 255\n";
            } else {
                // Odwiedzony korytarz - kolor na podstawie ID wątku
                int id = maze[i][j];
                int hue = (id * 360 / max_id) % 360;
                
                // Prosty HSV do RGB (S=100%, V=100%) - LLM tak zaproponował
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
    cout << "Całkowita liczba wątków: " << total_threads.load() << endl;
    
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
    cout << "EKSPLORACJA LABIRYNTU" << endl;
    
    init_maze();
    
    cout << "\nROZPOCZĘCIE EKSPLORACJI" << endl;
    
    thread main_thread(explore_maze, 1, 1, 0);
    main_thread.join();
    
    cout << "\nEKSPLORACJA ZAKOŃCZONA" << endl;
    
    display_statistics();
    
    save_to_ppm("labirynt.ppm");
    
    return 0;
}