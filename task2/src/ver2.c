#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <time.h>
#include <math.h>
#include <assert.h>

#define SIZE 1500
#define IDX(i, j, n) (((j)+ (i)*(n)))

static double gtod_ref_time_sec = 0.0;

/* Adapted from the bl2_clock() routine in the BLIS library */

double dclock() {
    double the_time, norm_sec;
    struct timeval tv;
    gettimeofday(&tv, NULL);
    if (gtod_ref_time_sec == 0.0) {
        gtod_ref_time_sec = (double) tv.tv_sec;
    }
    norm_sec = (double) tv.tv_sec - gtod_ref_time_sec;
    the_time = norm_sec + tv.tv_usec * 1.0e-6;
    return the_time;
}

/* INPUT: A - array of pointers to rows of a square matrix having dimension N
 *        Tol - small tolerance number to detect failure when the matrix is near degenerate
 * OUTPUT: Matrix A is changed, it contains a copy of both matrices L-E and U as A=(L-E)+U such that P*A=L*U.
 *        The permutation matrix is not stored as a matrix, but in an integer vector P of size N+1 
 *        containing column indexes where the permutation matrix has "1". The last element P[N]=S+N, 
 *        where S is the number of row exchanges needed for determinant computation, det(P)=(-1)^S    
 */
int LUPDecompose(double *A, int N, double Tol, int *P) {

    int i, j, k, imax; 
    double maxA, tmp, absA;

    for (i = 0; i <= N; i++)
        P[i] = i; //Unit permutation matrix, P[N] initialized with N

    for (i = 0; i < N; i++) {
        maxA = 0.0;
        imax = i;

        for (k = i; k < N; k++)
            if ((absA = fabs(A[IDX(k, i, SIZE)])) > maxA) { 
                maxA = absA;
                imax = k;
            }

        if (maxA < Tol) return 0; //failure, matrix is degenerate

        if (imax != i) {
            //pivoting P
            j = P[i];
            P[i] = P[imax];
            P[imax] = j;

            //pivoting rows of A
            for (j = 0; j < N; j++) {
                tmp = A[IDX(i, j, SIZE)];
                A[IDX(i, j, SIZE)] = A[IDX(imax, j, SIZE)];
                A[IDX(imax, j, SIZE)] = tmp;
            }

            //counting pivots starting from N (for determinant)
            P[N]++;
        }

        for (j = i + 1; j < N; j++) {
            A[IDX(j, i, SIZE)] /= A[IDX(i, i, SIZE)];

            for (k = i + 1; k < N; k++)
                A[IDX(j, k, SIZE)] -= A[IDX(j, i, SIZE)] * A[IDX(i, k, SIZE)];
        }
    }

    return 1;  //decomposition done 
}

int main(void) {
    int i, j, iret;
    double dtime;
    double *A = malloc(SIZE * SIZE * sizeof(double));
    assert(A != NULL);
    int P[SIZE + 1];

    srand(1);
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            A[IDX(i, j, SIZE)] = rand();
        }
    }

    printf("call LUPDecompose\n");
    dtime = dclock();
    iret = LUPDecompose(A, SIZE, 0.05, P);
    dtime = dclock() - dtime;
    printf("Time: %le \n", dtime);

    if (!iret) {
        fprintf(stderr, "LUPDecompose error!\n");
        return EXIT_FAILURE;
    }

    double check = 0.0;
    for (i = 0; i < SIZE; i++) {
        for (j = 0; j < SIZE; j++) {
        check = check + A[IDX(i, j, SIZE)];
        }
    }
    printf("Check: %le \n", check);
    fflush(stdout);

    free(A);
    
    return 0;
}
