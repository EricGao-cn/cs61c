#include <emmintrin.h>
#include <time.h>
#include <stdio.h>
#include <x86intrin.h>
#include "ex1.h"

long long int sum(int vals[NUM_ELEMS]) {
    clock_t start = clock();

    long long int sum = 0;
    for(unsigned int w = 0; w < OUTER_ITERATIONS; w++) {
        for(unsigned int i = 0; i < NUM_ELEMS; i++) {
            if(vals[i] >= 128) {
                sum += vals[i];
            }
        }
    }
    clock_t end = clock();
    printf("Time taken: %Lf s\n", (long double)(end - start) / CLOCKS_PER_SEC);
    return sum;
}

long long int sum_unrolled(int vals[NUM_ELEMS]) {
    clock_t start = clock();
    long long int sum = 0;

    for(unsigned int w = 0; w < OUTER_ITERATIONS; w++) {
        for(unsigned int i = 0; i < NUM_ELEMS / 4 * 4; i += 4) {
            if(vals[i] >= 128) sum += vals[i];
            if(vals[i + 1] >= 128) sum += vals[i + 1];
            if(vals[i + 2] >= 128) sum += vals[i + 2];
            if(vals[i + 3] >= 128) sum += vals[i + 3];
        }

        // TAIL CASE, for when NUM_ELEMS isn't a multiple of 4
        // NUM_ELEMS / 4 * 4 is the largest multiple of 4 less than NUM_ELEMS
        // Order is important, since (NUM_ELEMS / 4) effectively rounds down first
        for(unsigned int i = NUM_ELEMS / 4 * 4; i < NUM_ELEMS; i++) {
            if (vals[i] >= 128) {
                sum += vals[i];
            }
        }
    }
    clock_t end = clock();
    printf("Time taken: %Lf s\n", (long double)(end - start) / CLOCKS_PER_SEC);
    return sum;
}

long long int sum_simd(int vals[NUM_ELEMS]) {
    clock_t start = clock();
    __m128i _127 = _mm_set1_epi32(127); // This is a vector with 127s in it... Why might you need this?
    long long int result = 0; // This is where you should put your final result!
    /* DO NOT MODIFY ANYTHING ABOVE THIS LINE (in this function) */

    for(unsigned int w = 0; w < OUTER_ITERATIONS; w++) {
        /* YOUR CODE GOES HERE */
        __m128i sum_vec = _mm_setzero_si128();
        for (unsigned int i = 0; i < NUM_ELEMS / 4 * 4; i += 4) {
            __m128i tmp_vec = _mm_loadu_si128((__m128i*) (vals + i));
            __m128i cmp_vec = _mm_cmpgt_epi32(tmp_vec, _127);
            tmp_vec = _mm_and_si128(cmp_vec, tmp_vec);
            sum_vec = _mm_add_epi32(sum_vec, tmp_vec);
        }
        int tmp_arr[4];
        _mm_storeu_si128((__m128i *) tmp_arr, sum_vec);
        result += (tmp_arr[0] + tmp_arr[1] + tmp_arr[2] + tmp_arr[3]);

        for (unsigned int i = NUM_ELEMS / 4 * 4; i < NUM_ELEMS; i++) {
          if (vals[i] >= 128) {
                result += vals[i];
            }
        }
        /* Hint: you'll need a tail case. */
    }

    /* DO NOT MODIFY ANYTHING BELOW THIS LINE (in this function) */
    clock_t end = clock();
    printf("Time taken: %Lf s\n", (long double)(end - start) / CLOCKS_PER_SEC);
    return result;
}

long long int sum_simd_unrolled(int vals[NUM_ELEMS]) {
    clock_t start = clock();
    __m128i _127 = _mm_set1_epi32(127);
    long long int result = 0;
    /* DO NOT MODIFY ANYTHING ABOVE THIS LINE (in this function) */

    for(unsigned int w = 0; w < OUTER_ITERATIONS; w++) {
        /* YOUR CODE GOES HERE */
        /* Copy your sum_simd() implementation here, and unroll it */
        __m128i sum_vec_1 = _mm_setzero_si128();
        __m128i sum_vec_2 = _mm_setzero_si128();
        __m128i sum_vec_3 = _mm_setzero_si128();
        __m128i sum_vec_4 = _mm_setzero_si128();
        for (unsigned int i = 0; i < NUM_ELEMS / 16 * 16; i += 16) {
            __m128i tmp_vec_1 = _mm_loadu_si128((__m128i*) (vals + i));
            __m128i tmp_vec_2 = _mm_loadu_si128((__m128i*) (vals + i + 4));
            __m128i tmp_vec_3 = _mm_loadu_si128((__m128i*) (vals + i + 8));
            __m128i tmp_vec_4 = _mm_loadu_si128((__m128i*) (vals + i + 12));
            
            __m128i cmp_vec_1 = _mm_cmpgt_epi32(tmp_vec_1, _127);
            __m128i cmp_vec_2 = _mm_cmpgt_epi32(tmp_vec_2, _127);
            __m128i cmp_vec_3 = _mm_cmpgt_epi32(tmp_vec_3, _127);
            __m128i cmp_vec_4 = _mm_cmpgt_epi32(tmp_vec_4, _127);
            tmp_vec_1 = _mm_and_si128(cmp_vec_1, tmp_vec_1);
            tmp_vec_2 = _mm_and_si128(cmp_vec_2, tmp_vec_2);
            tmp_vec_3 = _mm_and_si128(cmp_vec_3, tmp_vec_3);
            tmp_vec_4 = _mm_and_si128(cmp_vec_4, tmp_vec_4);

            sum_vec_1 = _mm_add_epi32(sum_vec_1, tmp_vec_1);
            sum_vec_2 = _mm_add_epi32(sum_vec_2, tmp_vec_2);
            sum_vec_3 = _mm_add_epi32(sum_vec_3, tmp_vec_3);
            sum_vec_4 = _mm_add_epi32(sum_vec_4, tmp_vec_4);
        }
        sum_vec_1 = _mm_add_epi32(sum_vec_1, sum_vec_2);
        sum_vec_3 = _mm_add_epi32(sum_vec_3, sum_vec_4);
        sum_vec_1 = _mm_add_epi32(sum_vec_1, sum_vec_3);

        int tmp_arr[4];
        _mm_storeu_si128((__m128i *) tmp_arr, sum_vec_1);
        result += (tmp_arr[0] + tmp_arr[1] + tmp_arr[2] + tmp_arr[3]);

        for (unsigned int i = NUM_ELEMS / 16 * 16; i < NUM_ELEMS; i++) {
          if (vals[i] >= 128) {
                result += vals[i];
            }
        }
 
        /* Hint: you'll need 1 or maybe 2 tail cases here. */
    }

    /* DO NOT MODIFY ANYTHING BELOW THIS LINE (in this function) */
    clock_t end = clock();
    printf("Time taken: %Lf s\n", (long double)(end - start) / CLOCKS_PER_SEC);
    return result;
}
