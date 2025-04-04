#include "ex2.h"
#include <omp.h>

double dotp_naive(double* x, double* y, int arr_size) {
    double global_sum = 0.0;
    for (int i = 0; i < arr_size; i++)
        global_sum += x[i] * y[i];
    return global_sum;
}

// Critical Keyword
double dotp_critical(double* x, double* y, int arr_size) {
    double global_sum = 0.0;
    // TODO: Implement this function
    // Use the critical keyword here!
    # pragma omp critical
    {
        for (int i = 0; i < arr_size; i++)
            global_sum += x[i] * y[i];
    }
    return global_sum;
}

// Reduction Keyword
double dotp_reduction(double* x, double* y, int arr_size) {
    double global_sum = 0.0;
    // TODO: Implement this function
    // Use the reduction keyword here!
    #pragma omp parallel for reduction(+ : global_sum)
        for (int i = 0; i < arr_size; i++) {
            global_sum += x[i] * y[i];
        }
    return global_sum;
}

// double dotp_manual_reduction(double* x, double* y, int arr_size) {
//     double global_sum = 0.0;
//     double sums[8] = {0, 0, 0, 0, 0, 0, 0, 0};
//     #pragma omp parallel
//     {
//         int num_threads = omp_get_num_threads();
//         int thread_id = omp_get_thread_num();
//         double local_sum = 0.0;  // 每个线程的局部和
        
//         for (int i = thread_id; i < arr_size; i += num_threads) {
//             local_sum += x[i] * y[i];
//         }

//         #pragma omp critical
//         {
//             sums[thread_id % 8] += local_sum;
//         }
//     }
//     global_sum = sums[0] + sums[1] + sums[2] + sums[3] +
//                  sums[4] + sums[5] + sums[6] + sums[7];
//     return global_sum;
// }

// Manual Reduction
double dotp_manual_reduction(double* x, double* y, int arr_size) {
    double global_sum = 0.0;
    // TODO: Implement this function
    // Do NOT use the `reduction` directive here!
    int NUM_THREADS = 0;
    #pragma omp parallel
    {
        #pragma omp single
        {
        NUM_THREADS = omp_get_num_threads();
        }
    }
    double sum[NUM_THREADS];
    for (int i = 0; i < NUM_THREADS; i++) sum[i] = 0;
#pragma omp parallel
    {
      int id = omp_get_thread_num();
      double local_sum = 0.0;
      for (int i = id; i < arr_size; i += NUM_THREADS){
           local_sum += x[i] * y[i];
      }
      sum[id] = local_sum;
  }
    for (int i = 0; i < NUM_THREADS; i++) global_sum += sum[i];
    return global_sum;
}

