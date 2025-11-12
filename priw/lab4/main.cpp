// -lgomp -fopenmp -O3
// kopia Mandelbrota z równolegnieniem OpenMP

#include <iostream>
#include <iomanip>
#include <omp.h>

using namespace std;

int main() {
    int tab[8][8];
    int id;
    double s,f;

    s = omp_get_wtime();
    #pragma omp parallel num_threads(4) private(id)
    {
        id = omp_get_thread_num();

        #pragma omp for schedule (static, 1)
        for (int j=0; j<8; j++)
            for (int i=0; i<8; i++)
                tab[j][i] = id;
    }

    f = omp_get_wtime();
    for (int j=0; j<8; j++) {
        for (int i=0; i<8; i++)
            cout << tab[j][i] << "\t";
        cout << "\n";
    }
    cout << fixed << setprecision(9) << f-s << endl;

    return 0;
}