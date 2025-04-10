#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <math.h>

#ifndef CACHELINE_SIZE

#define CACHELINE_SIZE 64

#endif

#ifndef N
#define N 1024
#endif

#define NREPS 3

/* Block (tail) size */

double a[N][N]; // __attribute__ ((aligned(CLSIZE)));
double b[N][N]; // __attribute__ ((aligned(CLSIZE)));
double c[N][N]; // __attribute__ ((aligned(CLSIZE)));

double wtime()
{
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec + ts.tv_nsec * 1E-9;
}

void matrix_init(double a[N][N], double b[N][N], double c[N][N])
{
    srand(0);
    for (int i = 0; i < N; i++)
    {
        for (int j = 0; j < N; j++)
        {
            a[i][j] = rand() % 100;
            b[i][j] = rand() % 100;
            c[i][j] = 0;
        }
    }
}

// самое обыкновенное умножение
void dgemm_def(double a[N][N], double b[N][N], double c[N][N])
{
    for (int i = 0; i < N; i++)
    {
        for (int j = 0; j < N; j++)
        {
            for (int k = 0; k < N; k++)
            {
                c[i][j] += a[i][k] * b[k][j];
            } /* b[][] stride-N read */
        }
    }
}

void dgemm_transpose(double a[N][N], double b[N][N], double c[N][N])
{
    double *tmp = malloc(sizeof(*tmp) * N * N);
    for (int i = 0; i < N; i++)
    {
        for (int j = 0; j < N; j++)
        {
            tmp[i * N + j] = b[j][i];
        }
    }

    for (int i = 0; i < N; i++)
    {
        for (int j = 0; j < N; j++)
        {
            for (int k = 0; k < N; k++)
            {
                c[i][j] += a[i][k] * tmp[j * N + k];
            } /* tmp[] stride-1 read */
        }
    }
    free(tmp);
}
// чуть лучше
void dgemm_interchange(double a[N][N], double b[N][N], double c[N][N])
{
    for (int i = 0; i < N; i++) /*проходит по строкам матрицы a, для 
                                каждого элемента в строке i матрицы a будут 
                                обрабатываться все соответствующие элементы из матрицы b.*/
    {
        for (int k = 0; k < N; k++) /*проходит столбцы a и строки b, значит умножаем
                                    элементы из одной и той же строки матрицы a на элементы из одной и той же строки матрицы b.*/
        {
            for (int j = 0; j < N; j++) /*проходит по столбцам матрицы c и столбцам матрицы b, значит
                                        накапливать сумму произведений для каждого элемента в матрице c*/
            {
                c[i][j] += a[i][k] * b[k][j];
            }
        }
    }
}
// #define BS (CACHELINE_SIZE / sizeof(double))
#define BS 256
//  блочное умножение
void dgemm_block(double a[N][N], double b[N][N], double c[N][N])
{
    for (int i = 0; i < N; i += BS)
    {
        for (int j = 0; j < N; j += BS)
        {
            for (int k = 0; k < N; k += BS)
            {
                for (int i2 = i; i2 < i + BS; ++i2)
                {
                    for (int j2 = j; j2 < j + BS; ++j2)
                    {
                        for (int k2 = k; k2 < k + BS; ++k2)
                        {
                            c[i2][j2] += a[i2][k2] * b[k2][j2];
                        }
                    }
                }
            }
        }
    }
}

void dgemm_verify(double a[N][N], double b[N][N], double c[N][N], const char *msg)
{
    double *c0 = malloc(sizeof(*c0) * N * N);
    for (int i = 0; i < N; i++)
    {
        for (int j = 0; j < N; j++)
            c0[i * N + j] = 0;
    }
    dgemm_def(a, b, (double(*)[N])c0);

    int failed = 0;
    for (int i = 0; i < N && !failed; i++)
    {
        for (int j = 0; j < N; j++)
        {
            double diff = fabs(c[i][j] - c0[i * N + j]);
            if (diff > 1E-6)
            {
                fprintf(stderr, "dgemm: %s: verification failed %.6f != %.6f\n", msg, c0[i * N + j], c[i][j]);
                failed = 1;
                break;
            }
        }
    }
    if (!failed)
    {
        fprintf(stderr, "dgemm: %s: verification passed\n", msg);
    }
    free(c0);
}

int main()
{
    double t1, t2, t3;

#if 1
    matrix_init(a, b, c);
    t1 = wtime();
    for (int i = 0; i < NREPS; i++)
    {
        dgemm_def(a, b, c);
    }
    t1 = wtime() - t1;
    t1 /= NREPS;
    printf("# DGEMM def: N=%d, elapsed time (sec) %.6f\n", N, t1);
#endif

// #if 1
//     matrix_init(a, b, c);
//     t2 = wtime();
//     for (int i = 0; i < NREPS; i++)
//     {
//         dgemm_interchange(a, b, c);
//     }
//     t2 = wtime() - t2;
//     t2 /= NREPS;
//     printf("# DGEMM interchange: N=%d, elapsed time (sec) %.6f\n", N, t2);
// #endif

// #if 1
//     matrix_init(a, b, c);
//     t3 = wtime();
//     for (int i = 0; i < NREPS; i++)
//     {
//         dgemm_block(a, b, c);
//     }
//     t3 = wtime() - t3;
//     t3 /= NREPS;
//     printf("# DGEMM bloc: N=%d, BS=%d, elapsed time (sec) %.6f\n", N, BS, t3);
// #endif

/* Verification */
#if 0

#if 0
    matrix_init(a, b, c);
    dgemm_interchange(a, b, c);
    dgemm_verify(a, b, c, "interchange");
#endif

#if 0
    matrix_init(a, b, c);
    dgemm_block(a, b, c);
    dgemm_verify(a, b, c, "block");
#endif

#endif
    return 0;
}
