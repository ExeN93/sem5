#include <cuda_runtime.h>
#include <iostream>
#include <vector>
#include <chrono>

#define TILE_WIDTH 16

template <typename T>
struct Matrix2DView {
    T* data;
    int rows;
    int cols;

    __device__ __host__
    T& operator()(int i, int j) {
        return data[i * cols + j];
    }

    __device__ __host__
    const T& operator()(int i, int j) const {
        return data[i * cols + j];
    }
};

__global__ void matrixMulShared(Matrix2DView<float> A, Matrix2DView<float> B, Matrix2DView<float> C) {
    int row = blockIdx.y * TILE_WIDTH + threadIdx.y;
    int col = blockIdx.x * TILE_WIDTH + threadIdx.x;

    __shared__ float tile_A[TILE_WIDTH][TILE_WIDTH];
    __shared__ float tile_B[TILE_WIDTH][TILE_WIDTH];

    float value = 0.0f;

    // Mnożenie bloków
    for (int t = 0; t < A.cols / TILE_WIDTH; ++t) {
                if (row < A.rows && (t * TILE_WIDTH + threadIdx.x) < A.cols)
            tile_A[threadIdx.y][threadIdx.x] = A(row, t * TILE_WIDTH + threadIdx.x);
        else
            tile_A[threadIdx.y][threadIdx.x] = 0.0f;

        if (col < B.cols && (t * TILE_WIDTH + threadIdx.y) < B.rows)
            tile_B[threadIdx.y][threadIdx.x] = B(t * TILE_WIDTH + threadIdx.y, col);
        else
            tile_B[threadIdx.y][threadIdx.x] = 0.0f;

        __syncthreads();

        // Mnożenie
        for (int k = 0; k < TILE_WIDTH; ++k)
            value += tile_A[threadIdx.y][k] * tile_B[k][threadIdx.x];

        __syncthreads();
    }

    // Zapis wyniku do macierzy C
    if (row < C.rows && col < C.cols)
        C(row, col) = value;
}

void run_test(int N) {
    std::cout << "Test dla N = " << N << "... ";
    
    // Unified memory dla hosta i GPU
    float *A_data, *B_data, *C_data;
    cudaMallocManaged(&A_data, N * N * sizeof(float));
    cudaMallocManaged(&B_data, N * N * sizeof(float));
    cudaMallocManaged(&C_data, N * N * sizeof(float));

    // Inicjalizacja danych
    for (int i = 0; i < N * N; ++i) {
        A_data[i] = static_cast<float>(i % 100);
        B_data[i] = static_cast<float>((i * 2) % 100);
        C_data[i] = 0.0f;
    }

    // Inicjalizacja widoku
    Matrix2DView<float> A{A_data, N, N};
    Matrix2DView<float> B{B_data, N, N};
    Matrix2DView<float> C{C_data, N, N};

    dim3 dimBlock(TILE_WIDTH, TILE_WIDTH);
    dim3 dimGrid((N + TILE_WIDTH - 1) / TILE_WIDTH, (N + TILE_WIDTH - 1) / TILE_WIDTH);

    const auto start = std::chrono::steady_clock::now();

    matrixMulShared<<<dimGrid, dimBlock>>>(A, B, C);
    
    cudaDeviceSynchronize();

    const auto end = std::chrono::steady_clock::now();
    const std::chrono::duration<double> elapsed(end - start);

    std::cout << "Czas: " << elapsed.count() << " s" << std::endl;

    // Zwalnianie pamięci
    cudaFree(A_data);
    cudaFree(B_data);
    cudaFree(C_data);
}

int main() {
    std::vector<int> sizes = {256, 512, 1024};

    for (int n : sizes) {
        run_test(n);
    }

    return 0;
}