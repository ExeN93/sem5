/* 
dodać energię na 5
bez energii będzie na 4
*/ 

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <omp.h>

#define WIDTH 1000
#define MAX_VAL 255

typedef struct {
    unsigned char r, g, b;
} Pixel;

int ulam_get_map(int x, int y, int n) {
    x = x - ((n - 1) / 2);
    y = y - (n / 2);
    
    int mx = abs(x);
    int my = abs(y);
    int l = 2 * (mx > my ? mx : my);
    int d;
    if (y >= x) {
        d = l * 3 + x + y;
    } else {
        d = l - x - y;
    }
    
    return pow(l - 1, 2) + d;
}

int isprime(int n) {
    if (n < 2) return 0;
    if (n == 2) return 1;
    if (n % 2 == 0) return 0;
    
    for (int p = 3; p * p <= n; p += 2) {
        if (n % p == 0) return 0;
    }
    return 1;
}

void save_ppm(const char *filename, Pixel *image, int size) {
    FILE *fp = fopen(filename, "wb");

    fprintf(fp, "P6\n%d %d\n%d\n", size, size, MAX_VAL);
    fwrite(image, sizeof(Pixel), size * size, fp);
    fclose(fp);
}

int main() {
    omp_set_num_threads(16);

    int n = WIDTH;
    size_t data_size = n * n * sizeof(Pixel);
    Pixel *image = (Pixel *)malloc(data_size);

    printf("Liczba watkow: %d\n", omp_get_max_threads());

    double start, end;

    // Podział horyzontalny
    start = omp_get_wtime();
    
    #pragma omp parallel for schedule(guided)
    for (int y = 0; y < n; y++) {
        for (int x = 0; x < n; x++) {
            int val = ulam_get_map(x, y, n);
            
            if (isprime(val)) {
                image[y * n + x] = (Pixel){0, 0, 0};
            } else {
                image[y * n + x] = (Pixel){255, 255, 255};
            }
        }
    }
    
    end = omp_get_wtime();
    printf("Horyzontalny: %f s\n", end - start);
    
    save_ppm("ulam_strips.ppm", image, n);

    // Wyczyszczenie pamięci dla pewności
    for(int i=0; i<n*n; i++) image[i] = (Pixel){200, 200, 200};

    // Podział Collapse
    start = omp_get_wtime();

    #pragma omp parallel for collapse(2) schedule(dynamic)
    for (int y = 0; y < n; y++) {
        for (int x = 0; x < n; x++) {
            int val = ulam_get_map(x, y, n);
            
            if (isprime(val)) {
                image[y * n + x] = (Pixel){0, 0, 0};
            } else {
                image[y * n + x] = (Pixel){255, 255, 255};
            }
        }
    }

    end = omp_get_wtime();
    printf("Collapse: %f s\n", end - start);

    save_ppm("ulam_spiral.ppm", image, n);

    free(image);
    return 0;
}