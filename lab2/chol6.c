#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
//#include <x86intrin.h>
#include <immintrin.h>

#define IDX(i, j, n) (((j)+ (i)*(n)))
#define max(a,b) (((a)>(b))?(a):(b))
#define BLKSIZE 16


static double gtod_ref_time_sec = 0.0;

/* Adapted from the bl2_clock() routine in the BLIS library */

double dclock(){
    double the_time, norm_sec;
    struct timeval tv;
    gettimeofday( &tv, NULL );
    if ( gtod_ref_time_sec == 0.0 )
        gtod_ref_time_sec = ( double ) tv.tv_sec;
    norm_sec = ( double ) tv.tv_sec - gtod_ref_time_sec;
    the_time = norm_sec + tv.tv_usec * 1.0e-6;
    return the_time;
}



int chol(double * A, unsigned int n){
    register unsigned int i, j, k;
    register double tmp;
    register __m256d tmp0 , tmp1 , tmp2 , tmp3 , tmp4 , tmp5 , tmp6 , tmp7 ;

    for (j = 0; j < n; j++) {
        for (i = j; i < n; i++) {
            tmp = A[IDX(i, j, n)];
            for (k = 0; k < j; ) {
                if (k < max(j - BLKSIZE, 0)) {
                    tmp0 = _mm256_loadu_pd (A+IDX( i , k , n ) );
                    tmp1 = _mm256_loadu_pd (A+IDX( j , k , n ) );
//                    tmp2 = _mm256_loadu_pd (A+IDX( i , k + 2 , n ) );
//                    tmp3 = _mm256_loadu_pd (A+IDX( j , k + 2 , n ) );
                    tmp2 = _mm256_loadu_pd (A+IDX( i , k + 4 , n ) );
                    tmp3 = _mm256_loadu_pd (A+IDX( j , k + 4 , n ) );
//                    tmp6 = _mm256_loadu_pd (A+IDX( i , k + 6 , n ) );
//                    tmp7 = _mm256_loadu_pd (A+IDX( j , k + 6 , n ) );
                    tmp4 = _mm256_loadu_pd (A+IDX( i , k + 8 , n ) );
                    tmp5 = _mm256_loadu_pd (A+IDX( j , k + 8 , n ) );
//                    tmp10 = _mm256_loadu_pd (A+IDX( i , k + 10 , n ) );
//                    tmp11 = _mm256_loadu_pd (A+IDX( j , k + 10 , n ) );
                    tmp6 = _mm256_loadu_pd (A+IDX( i , k + 12 , n ) );
                    tmp7 = _mm256_loadu_pd (A+IDX( j , k + 12 , n ) );
//                    tmp14 = _mm256_loadu_pd (A+IDX( i , k + 14 , n ) );
//                    tmp15 = _mm256_loadu_pd (A+IDX( j , k + 14 , n ) );

                    tmp0 = _mm256_mul_pd(tmp0 , tmp1 ) ;
                    tmp2 = _mm256_mul_pd(tmp2 , tmp3 ) ;
                    tmp4 = _mm256_mul_pd(tmp4 , tmp5 ) ;
                    tmp6 = _mm256_mul_pd(tmp6 , tmp7 ) ;
//                    tmp8 = _mm256_mul_pd(tmp8 , tmp9 ) ;
//                    tmp10 = _mm256_mul_pd(tmp10 , tmp11 ) ;
//                    tmp12 = _mm256_mul_pd(tmp12 , tmp13 ) ;
//                    tmp14 = _mm256_mul_pd(tmp14 , tmp15 ) ;

                    tmp0 = _mm256_add_pd(tmp0 , tmp2 ) ;
                    tmp4 = _mm256_add_pd(tmp4 , tmp6 ) ;

//                    tmp8 = _mm256_add_pd(tmp8 , tmp10 ) ;
//                    tmp12 = _mm256_add_pd(tmp12 , tmp14 ) ;
//
//                    tmp0 = _mm256_add_pd(tmp0 , tmp4 ) ;
//                    tmp8 = _mm256_add_pd(tmp8 , tmp12 ) ;

                    tmp0 = _mm256_add_pd(tmp0 , tmp4 ) ;

                    tmp -= tmp0[0] + tmp0[1] + tmp0[2] + tmp0[3];

//                    tmp -= A[IDX( i , k , n ) ] * A[IDX( j , k , n ) ] +
//                    A[IDX( i , k + 1 , n ) ] * A[IDX( j , k + 1 , n ) ] +
//                    A[IDX( i , k + 2 , n ) ] * A[IDX( j , k + 2 , n ) ] +
//                    A[IDX( i , k + 3 , n ) ] * A[IDX( j , k + 3 , n ) ] +
//                    A[IDX( i , k + 4 , n ) ] * A[IDX( j , k + 4 , n ) ] +
//                    A[IDX( i , k + 5 , n ) ] * A[IDX( j , k + 5 , n ) ] +
//                    A[IDX( i , k + 6 , n ) ] * A[IDX( j , k + 6 , n ) ] +
//                    A[IDX( i , k + 7 , n ) ] * A[IDX( j , k + 7 , n ) ] ;
                    k += BLKSIZE;
                } else {
                    tmp -= A[IDX( i , k , n ) ] * A[IDX( j , k , n ) ] ;
                    k++;
                }
            }
            A[IDX(i, j, n)] = tmp;
        }

        if (A[IDX(j, j, n)] < 0.0) {
            return (1);
        }

//        A[IDX(j, j, n)] = sqrt(A[IDX(j, j, n)]);
        tmp = sqrt(A[IDX(j, j, n)]);
        for (i = j + 1; i < n; i++){
//            A[IDX(i, j, n)] /= A[IDX(j, j, n)];
            A[IDX(i, j, n)] /= tmp;
        }
        A[IDX(j, j, n)] = tmp;
    }

    return (0);
}

int main(int argc, char ** argv){
    double* A;
    double t1, t2;
    int i, j, n, ret;
    double dtime;

    n = atoi(argv[1]);
    A = malloc(n * n * sizeof(double));
    assert(A != NULL);

    for (i = 0; i < n; i++) {
        A[IDX(i, i, n)] = 1.0;
    }

    dtime = dclock();
    if (chol(A, n)) {
      fprintf(stderr,"Matrix is not symmetric or not positive definite\n");
    }
    dtime = dclock()-dtime;
    printf( "Time: %le \n", dtime);

    fflush( stdout );

    free(A);
    return 0;
}

