/* 
 Mandelbrot set - porównanie 3 metod podziału pracy między wątki
 Automatyczne testowanie dla 1,2,4,8,16,32,64,128,512 wątków
*/
#include <stdio.h>
#include <math.h>
#include <chrono>
#include <thread>
#include <iostream>
#include <mutex>
#include <vector>
#include <iomanip>

const int iXmax = 6400; 
const int iYmax = 6400;

const double CxMin=-2.5;
const double CxMax=1.5;
const double CyMin=-2.0;
const double CyMax=2.0;

const int IterationMax=200;
const double EscapeRadius=2;
const int MaxColorComponentValue=255; 

unsigned char tab[iYmax][iXmax][3];

// Dla metody dynamicznej (mutex)
std::mutex mtx;
int current_row = 0;

// Statystyki iteracji
long long iteration_count[512] = {0};

// Funkcja generująca odcień niebieskiego dla danego wątku
void get_thread_color(int tid, int num_threads, unsigned char &r, unsigned char &g, unsigned char &b) {
    if (num_threads == 1) {
        r = 0; g = 77; b = 255;
        return;
    }
    float intensity = 50.0f + (205.0f * tid) / (num_threads - 1);
    r = 0;
    g = (unsigned char)(intensity * 0.3f);
    b = (unsigned char)intensity;
}

// ==================== METODA 1: PODZIAŁ NA BLOKI ====================
void method1_blocks(int tid, int num_threads) {
    int lb, ub;
    double Cx, Cy;
    double PixelWidth = (CxMax-CxMin)/iXmax;
    double PixelHeight = (CyMax-CyMin)/iYmax;
    double Zx, Zy, Zx2, Zy2;
    int Iteration;
    double ER2 = EscapeRadius*EscapeRadius;
    
    unsigned char r, g, b;
    get_thread_color(tid, num_threads, r, g, b);
    
    lb = (iYmax/num_threads) * tid;
    ub = lb + (iYmax/num_threads) - 1;
    
    for(int iY=lb; iY<=ub; iY++) {
        Cy = CyMin + iY*PixelHeight;
        if (fabs(Cy) < PixelHeight/2) Cy = 0.0;
        
        for(int iX=0; iX<iXmax; iX++) {
            Cx = CxMin + iX*PixelWidth;
            Zx = 0.0;
            Zy = 0.0;
            Zx2 = Zx*Zx;
            Zy2 = Zy*Zy;
            
            for (Iteration=0; Iteration<IterationMax && ((Zx2+Zy2)<ER2); Iteration++) {
                Zy = 2*Zx*Zy + Cy;
                Zx = Zx2-Zy2 + Cx;
                Zx2 = Zx*Zx;
                Zy2 = Zy*Zy;
            }
            
            iteration_count[tid] += Iteration;
            
            tab[iY][iX][0] = r;
            tab[iY][iX][1] = g;
            tab[iY][iX][2] = b;
            
            if (Iteration == IterationMax) {
                tab[iY][iX][0] /= 2;
                tab[iY][iX][1] /= 2;
                tab[iY][iX][2] /= 2;
            }
        }
    }
}

// ==================== METODA 2: ROLETA (ROUND-ROBIN) ====================
void method2_roundrobin(int tid, int num_threads) {
    double Cx, Cy;
    double PixelWidth = (CxMax-CxMin)/iXmax;
    double PixelHeight = (CyMax-CyMin)/iYmax;
    double Zx, Zy, Zx2, Zy2;
    int Iteration;
    double ER2 = EscapeRadius*EscapeRadius;
    
    unsigned char r, g, b;
    get_thread_color(tid, num_threads, r, g, b);
    
    // CO N WIERSZY - każdy wątek bierze co num_threads-ty wiersz
    for(int iY=tid; iY<iYmax; iY+=num_threads) {
        Cy = CyMin + iY*PixelHeight;
        if (fabs(Cy) < PixelHeight/2) Cy = 0.0;
        
        for(int iX=0; iX<iXmax; iX++) {
            Cx = CxMin + iX*PixelWidth;
            Zx = 0.0;
            Zy = 0.0;
            Zx2 = Zx*Zx;
            Zy2 = Zy*Zy;
            
            for (Iteration=0; Iteration<IterationMax && ((Zx2+Zy2)<ER2); Iteration++) {
                Zy = 2*Zx*Zy + Cy;
                Zx = Zx2-Zy2 + Cx;
                Zx2 = Zx*Zx;
                Zy2 = Zy*Zy;
            }
            
            iteration_count[tid] += Iteration;
            
            tab[iY][iX][0] = r;
            tab[iY][iX][1] = g;
            tab[iY][iX][2] = b;
            
            if (Iteration == IterationMax) {
                tab[iY][iX][0] /= 2;
                tab[iY][iX][1] /= 2;
                tab[iY][iX][2] /= 2;
            }
        }
    }
}

// ==================== METODA 3: DYNAMICZNY Z MUTEXEM ====================
void method3_dynamic(int tid, int num_threads) {
    double Cx, Cy;
    double PixelWidth = (CxMax-CxMin)/iXmax;
    double PixelHeight = (CyMax-CyMin)/iYmax;
    double Zx, Zy, Zx2, Zy2;
    int Iteration;
    double ER2 = EscapeRadius*EscapeRadius;
    
    unsigned char r, g, b;
    get_thread_color(tid, num_threads, r, g, b);
    
    while(true) {
        int iY;
        
        // ============ SEKCJA KRYTYCZNA ============
        // Tylko 1 wątek naraz może wejść tutaj
        mtx.lock();
        if(current_row >= iYmax) {
            mtx.unlock();
            break;
        }
        iY = current_row;
        current_row++;
        mtx.unlock();
        // ========== KONIEC SEKCJI KRYTYCZNEJ ==========
        
        Cy = CyMin + iY*PixelHeight;
        if (fabs(Cy) < PixelHeight/2) Cy = 0.0;
        
        for(int iX=0; iX<iXmax; iX++) {
            Cx = CxMin + iX*PixelWidth;
            Zx = 0.0;
            Zy = 0.0;
            Zx2 = Zx*Zx;
            Zy2 = Zy*Zy;
            
            for (Iteration=0; Iteration<IterationMax && ((Zx2+Zy2)<ER2); Iteration++) {
                Zy = 2*Zx*Zy + Cy;
                Zx = Zx2-Zy2 + Cx;
                Zx2 = Zx*Zx;
                Zy2 = Zy*Zy;
            }
            
            iteration_count[tid] += Iteration;
            
            tab[iY][iX][0] = r;
            tab[iY][iX][1] = g;
            tab[iY][iX][2] = b;
            
            if (Iteration == IterationMax) {
                tab[iY][iX][0] /= 2;
                tab[iY][iX][1] /= 2;
                tab[iY][iX][2] /= 2;
            }
        }
    }
}

// Funkcja uruchamiająca test dla danej metody i liczby wątków
double run_test(int method, int num_threads, const char* filename) {
    // Reset zmiennych globalnych
    current_row = 0;
    for(int i=0; i<512; i++) iteration_count[i] = 0;
    
    std::vector<std::thread> threads;
    
    auto start = std::chrono::steady_clock::now();
    
    // Uruchom wątki
    for(int i=0; i<num_threads; i++) {
        if(method == 1)
            threads.push_back(std::thread(method1_blocks, i, num_threads));
        else if(method == 2)
            threads.push_back(std::thread(method2_roundrobin, i, num_threads));
        else if(method == 3)
            threads.push_back(std::thread(method3_dynamic, i, num_threads));
    }
    
    // Czekaj na zakończenie
    for(auto& t : threads) {
        t.join();
    }
    
    auto finish = std::chrono::steady_clock::now();
    std::chrono::duration<double> elapsed = finish - start;
    
    // Zapisz obrazek
    FILE* fp = fopen(filename, "wb");
    fprintf(fp, "P6\n# \n%d\n%d\n%d\n", iXmax, iYmax, MaxColorComponentValue);
    fwrite(tab, 1, 3*iXmax*iYmax, fp);
    fclose(fp);
    
    return elapsed.count();
}

void print_statistics(int num_threads) {
    long long total = 0;
    long long min_iter = iteration_count[0];
    long long max_iter = iteration_count[0];
    
    for(int i=0; i<num_threads; i++) {
        total += iteration_count[i];
        if(iteration_count[i] < min_iter) min_iter = iteration_count[i];
        if(iteration_count[i] > max_iter) max_iter = iteration_count[i];
    }
    
    double avg = (double)total / num_threads;
    
    std::cout << "   Suma iteracji: " << total << "\n";
    std::cout << "   Min/Avg/Max: " << min_iter << " / " 
              << (long long)avg << " / " << max_iter << "\n";
    std::cout << "   Balans obciążenia: " << std::fixed << std::setprecision(2) 
              << (100.0 * min_iter / max_iter) << "%\n";
}

int main() {
    std::vector<int> thread_counts = {1, 2, 4, 8, 16, 32, 64, 128, 512};
    
    std::cout << "========================================\n";
    std::cout << "MANDELBROT SET - TESTY RÓWNOLEGŁOŚCI\n";
    std::cout << "Rozmiar: " << iXmax << "x" << iYmax << "\n";
    std::cout << "Max iteracji: " << IterationMax << "\n";
    std::cout << "========================================\n\n";
    
    // ==================== METODA 1: BLOKI ====================
    std::cout << "METODA 1: PODZIAŁ NA BLOKI (każdy wątek ma swój ciągły zakres wierszy)\n";
    std::cout << "------------------------------------------------------------------------\n";
    for(int n : thread_counts) {
        if(iYmax % n != 0 && n > 1) {
            std::cout << "Wątków: " << std::setw(3) << n << " - POMINIĘTO (iYmax musi być podzielne)\n";
            continue;
        }
        
        char filename[100];
        sprintf(filename, "method1_blocks_%03d.ppm", n);
        double time = run_test(1, n, filename);
        
        std::cout << "Wątków: " << std::setw(3) << n 
                  << " | Czas: " << std::fixed << std::setprecision(3) << time << "s";
        if(n > 1) {
            std::cout << "\n";
            print_statistics(n);
        } else {
            std::cout << "\n";
        }
    }
    
    // ==================== METODA 2: ROLETA ====================
    std::cout << "\nMETODA 2: ROLETA (round-robin: wątek i bierze wiersze i, i+n, i+2n, ...)\n";
    std::cout << "------------------------------------------------------------------------\n";
    for(int n : thread_counts) {
        char filename[100];
        sprintf(filename, "method2_roundrobin_%03d.ppm", n);
        double time = run_test(2, n, filename);
        
        std::cout << "Wątków: " << std::setw(3) << n 
                  << " | Czas: " << std::fixed << std::setprecision(3) << time << "s";
        if(n > 1) {
            std::cout << "\n";
            print_statistics(n);
        } else {
            std::cout << "\n";
        }
    }
    
    // ==================== METODA 3: DYNAMICZNY (MUTEX) ====================
    std::cout << "\nMETODA 3: DYNAMICZNY Z MUTEXEM (kto wolny, ten bierze następny wiersz)\n";
    std::cout << "------------------------------------------------------------------------\n";
    std::cout << "UWAGA: Za każdym razem inna liczba iteracji na wątek (niedeterministyczne!)\n\n";
    
    for(int n : thread_counts) {
        char filename[100];
        sprintf(filename, "method3_dynamic_%03d.ppm", n);
        double time = run_test(3, n, filename);
        
        std::cout << "Wątków: " << std::setw(3) << n 
                  << " | Czas: " << std::fixed << std::setprecision(3) << time << "s";
        if(n > 1) {
            std::cout << "\n";
            print_statistics(n);
        } else {
            std::cout << "\n";
        }
    }
    
    std::cout << "\n========================================\n";
    std::cout << "WYJAŚNIENIA:\n";
    std::cout << "========================================\n";
    std::cout << "1. CZASY SIĘ REDUKUJĄ pomimo > CPU:\n";
    std::cout << "   - Do ~16 wątków: prawie liniowe przyspieszenie (ile rdzeni)\n";
    std::cout << "   - Powyżej: spłaszczenie (wątki konkurują o te same rdzenie)\n";
    std::cout << "   - Overhead: przełączanie kontekstu, cache misses\n\n";
    
    std::cout << "2. BALANS OBCIĄŻENIA:\n";
    std::cout << "   - Metoda 1 (bloki): Słaby - środek Mandelbrota liczy się dłużej\n";
    std::cout << "   - Metoda 2 (roleta): Lepszy - praca równomiernie rozłożona\n";
    std::cout << "   - Metoda 3 (mutex): Najlepszy - dynamiczne przydzielanie\n\n";
    
    std::cout << "3. SEKCJA KRYTYCZNA (mutex):\n";
    std::cout << "   - Fragment kodu gdzie tylko 1 wątek może być w danym momencie\n";
    std::cout << "   - mtx.lock() = zamknij drzwi, tylko ja mogę wejść\n";
    std::cout << "   - mtx.unlock() = otwórz drzwi, inni mogą wejść\n";
    std::cout << "   - Chroni współdzieloną zmienną current_row\n\n";
    
    std::cout << "4. DLACZEGO ZA KAŻDYM RAZEM INNA LICZBA (metoda 3):\n";
    std::cout << "   - Zależy od schedulera systemu operacyjnego\n";
    std::cout << "   - Który wątek \"zdąży\" pierwszy złapać wiersz = losowe\n";
    std::cout << "   - To jest NIEDETERMINISTYCZNE zachowanie!\n\n";
    
    std::cout << "Obrazki zapisane jako method{1,2,3}_*_{liczba_wątków}.ppm\n";
    std::cout << "Każdy wątek ma inny odcień niebieskiego koloru!\n";
    
    return 0;
}