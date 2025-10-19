#include <iostream>
#include <thread>
#include <chrono>

using namespace std;
 
static const int num_threads = 1;
const int N = 256;

int **A, *_a, **B, *_b, **C, *_c, **BT, *_bt;
 

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

		_a = new int[N*N];

    A = new int*[N];
    for(i = 0; i < N; i++){
    	A[i] = _a + N * i;
    }

		_b = new int[N*N];

    B = new int*[N];
    for(i = 0; i < N; i++){
    	B[i] = _b + N * i;
    }

		_c = new int[N*N];

    C = new int*[N];
    for(i = 0; i < N; i++){
    	C[i] = _c + N * i;
    }

		_bt = new int[N*N];

    BT = new int*[N];
    for(i = 0; i < N; i++){
    	BT[i] = _bt + N * i;
    }

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

    delete[] _a;
		delete[] A;
		delete[] _b;
		delete[] B;
		delete[] _c;
		delete[] C;
		delete[] _bt;
		delete[] BT;
 
    return 0;
}