#include <iostream>
#include <thread>
#include <chrono>

using namespace std;
 
static const int num_threads = 1;

const int N = 256;
 
int A[N][N], B[N][N], C[N][N], BT[N][N];
 
void call_from_thread(int tid) {
    int i, j, k, lb, ub;
 
    lb = (N/num_threads) * tid; // N ma być podzielne przez liczbę wątków
    ub = lb + (N/num_threads) - 1;
 
    for(i=lb; i<=ub; i++){
        for(j=0; j<N; j++){
            C[i][j] = 0;
            for(k=0; k<N; k++){
                C[i][j] += A[i][k] * B[k][j];
                //C[i][j] += A[i][k] * BT[j][k];
            }
        }
    }
}
 
int main() {

    std::thread t[num_threads];

    int i, j;

    const auto start(std::chrono::steady_clock::now());

    // for(i = 0; i < N; i++){
    //     for(j = 0; j < N; j++){
    //         BT[i][j] = B[j][i];
    //     }
    // }
 
    // Proste uruchomienie wszystkich wątków
    for (int i = 0; i < num_threads; ++i) {
        t[i] = std::thread(call_from_thread, i);
    }
 
    // Połączenie join wszystkich wątków
    for (int i = 0; i < num_threads; ++i) {
        t[i].join();
    }

    const auto end(std::chrono::steady_clock::now());
    const std::chrono::duration<double> elapsed(end - start);
    std::cout << elapsed.count() << std::endl;
 
    return 0;
}