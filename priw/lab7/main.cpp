#include <stdio.h>
#include <stdlib.h>
#include <math.h>
// #include <omp.h>
#include <tbb.h>

const double CxMin = -2.5;
const double CxMax =  1.5;
const double CyMin = -2.0;
const double CyMax =  2.0;

const int    IterationMax   = 200;
const double EscapeRadius   = 2.0;
const double ER2            = 4.0;
const int    MaxColorComponentValue = 255;

void run_test(int iXmax, int iYmax, int numThreads)
{
    tbb::task_arena arena(numThreads);
    int max_threads = numThreads;

    size_t imageSize = (size_t)iXmax * (size_t)iYmax * 3;
    unsigned char *image = (unsigned char*)malloc(imageSize);
    if (!image) {
        fprintf(stderr, "Blad alokacji pamieci (%zu bajtow) dla obrazka %dx%d\n",
                imageSize, iXmax, iYmax);
        return;
    }

    long long *iterPerThread = (long long*)calloc(max_threads + 128, sizeof(long long)); 
    if (!iterPerThread) {
        fprintf(stderr, "Blad alokacji pamieci dla statystyki watkow\n");
        free(image);
        return;
    }

    double PixelWidth  = (CxMax - CxMin) / iXmax;
    double PixelHeight = (CyMax - CyMin) / iYmax;

    printf("= %d, watki = %d\n",
           iXmax, numThreads);
    tbb::tick_count t_start = tbb::tick_count::now();

    int used_threads = numThreads;
    
    arena.execute([&]() {
        tbb::parallel_for(tbb::blocked_range<int>(0, iYmax, 20), [&](const tbb::blocked_range<int>& range) {
            int tid = tbb::this_task_arena::current_thread_index();
            int iX, iY, Iteration;
            double Cx, Cy;
            double Zx, Zy, Zx2, Zy2;

            for (iY = range.begin(); iY < range.end(); iY++)
            {
                long long rowIterSum = 0;

                Cy = CyMin + iY * PixelHeight;
                if (fabs(Cy) < PixelHeight / 2) Cy = 0.0;

                size_t rowOffset = (size_t)iY * (size_t)iXmax * 3;

                for (iX = 0; iX < iXmax; iX++)
                {
                    Cx = CxMin + iX * PixelWidth;

                    Zx = 0.0;
                    Zy = 0.0;
                    Zx2 = 0.0;
                    Zy2 = 0.0;

                    for (Iteration = 0;
                         Iteration < IterationMax && (Zx2 + Zy2) < ER2;
                         Iteration++)
                    {
                        Zy = 2.0 * Zx * Zy + Cy;
                        Zx = Zx2 - Zy2 + Cx;

                        Zx2 = Zx * Zx;
                        Zy2 = Zy * Zy;
                    }
                    rowIterSum += Iteration;

                    unsigned char r, g, b;

                    if (Iteration == IterationMax) {
                        r = (unsigned char)(30 * (tid + 1));
                        g = (unsigned char)(20 * (tid + 1));
                        b = (unsigned char)(40 * (tid + 1));
                    }
                    else {
                        int num_threads = used_threads;
                        if (num_threads > 1) {
                            double tfrac = (double)tid / (double)(num_threads - 1);
                            r = (unsigned char)(255.0 * tfrac);
                            g = (unsigned char)(255.0 * (1.0 - tfrac));
                            b = (unsigned char)(128.0 * tfrac);
                        } else {
                            r = g = b = 255;
                        }
                    }

                    size_t idx = rowOffset + (size_t)iX * 3;
                    image[idx + 0] = r;
                    image[idx + 1] = g;
                    image[idx + 2] = b;
                }
                iterPerThread[tid] += rowIterSum;
            }
        });
    });

    // double t_end = omp_get_wtime();
    // double elapsed = t_end - t_start;
    tbb::tick_count t_end = tbb::tick_count::now();
    double elapsed = (t_end - t_start).seconds();


    char filename[128];
    snprintf(filename, sizeof(filename),
             "tbb_%dx%d_%dw.ppm", iXmax, iYmax, numThreads);

    FILE *fp = fopen(filename, "wb");
    if (!fp) {
        fprintf(stderr, "Nie moge otworzyc pliku %s do zapisu\n", filename);
        free(image);
        free(iterPerThread);
        return;
    }

    const char *comment = "# Mandelbrot Intel TBB";
    fprintf(fp, "P6\n%s\n%d\n%d\n%d\n",
            comment, iXmax, iYmax, MaxColorComponentValue);

    fwrite(image, 1, imageSize, fp);
    fclose(fp);

    long long totalIter = 0;
    for (int t = 0; t < used_threads; t++) {
        totalIter += iterPerThread[t];
    }

    printf("  Czas: %.6f s\n", elapsed);

    int maxThread = 0, minThread = 0;
    long long maxIter = iterPerThread[0];
    long long minIter = iterPerThread[0];

    for (int t = 1; t < used_threads; t++) {
        if (iterPerThread[t] > maxIter) {
            maxIter = iterPerThread[t];
            maxThread = t;
        }
        if (iterPerThread[t] < minIter) {
            minIter = iterPerThread[t];
            minThread = t;
        }
    }

    double maxPct = (totalIter > 0) ? (100.0 * (double)maxIter / (double)totalIter) : 0.0;
    double minPct = (totalIter > 0) ? (100.0 * (double)minIter / (double)totalIter) : 0.0;

    printf("  max: %d (%lld, %.2f %%)\n",
           maxThread, maxIter, maxPct);

    printf("  min: %d (%lld, %.2f %%)\n",
           minThread, minIter, minPct);

    printf("\n");

    free(image);
    free(iterPerThread);
}

int main(void)
{
    int sizes[] = {800, 6400};
    int nSizes = sizeof(sizes) / sizeof(sizes[0]);
    int threads[] = {1, 2, 4, 8};
    int nThreads = sizeof(threads) / sizeof(threads[0]);

    for (int s = 0; s < nSizes; s++) {
        int N = sizes[s];
        for (int t = 0; t < nThreads; t++) {
            run_test(N, N, threads[t]);
        }
    }

    return 0;
}