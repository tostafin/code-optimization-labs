//requires additional changes to the code to make it work

#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <time.h>

#define BLKSIZE 8
#define max(a,b) (((a)>(b))?(a):(b))

static double gtod_ref_time_sec = 0.0;

/* Adapted from the bl2_clock() routine in the BLIS library */

double dclock()
{
  double the_time, norm_sec;
  struct timeval tv;
  gettimeofday( &tv, NULL );
  if ( gtod_ref_time_sec == 0.0 )
    gtod_ref_time_sec = ( double ) tv.tv_sec;
  norm_sec = ( double ) tv.tv_sec - gtod_ref_time_sec;
  the_time = norm_sec + tv.tv_usec * 1.0e-6;
  return the_time;
}

int ge(double * A, int SIZE)
{
  register int i,j,k;
  for (k = 0; k < SIZE; k++) { 
    for (i = k+1; i < SIZE; i++) {
    register double multiplier = A[i*SIZE + k]/A[k*SIZE + k];
      for (j = k+1; j < SIZE; ) {
          if (j < max((SIZE - BLKSIZE), 0)) {
              A[i*SIZE + j] = A[i*SIZE + j]-A[k*SIZE + j]*multiplier;
              A[i*SIZE + j + 1] = A[i*SIZE + j + 1]-A[k*SIZE + j + 1]*multiplier;
              A[i*SIZE + j + 2] = A[i*SIZE + j + 2]-A[k*SIZE + j + 2]*multiplier;
              A[i*SIZE + j + 3] = A[i*SIZE + j + 3]-A[k*SIZE + j + 3]*multiplier;
              A[i*SIZE + j + 4] = A[i*SIZE + j + 4]-A[k*SIZE + j + 4]*multiplier;
              A[i*SIZE + j + 5] = A[i*SIZE + j + 5]-A[k*SIZE + j + 5]*multiplier;
              A[i*SIZE + j + 6] = A[i*SIZE + j + 6]-A[k*SIZE + j + 6]*multiplier;
              A[i*SIZE + j + 7] = A[i*SIZE + j + 7]-A[k*SIZE + j + 7]*multiplier;
              j += BLKSIZE;
          } else {
              A[i*SIZE + j] = A[i*SIZE + j]-A[k*SIZE + j]*multiplier;
              j++;
          }
      } 
    }
  }
  return 0;
}

int main( int argc, const char* argv[] )
{
  int i,j,k,iret;
  double dtime;
  int SIZE = 1500;
//  double matrix[SIZE][SIZE]; // TODO - make near optimal dynamic allocation
  double* matrix = (double*) malloc(sizeof(double) * SIZE*SIZE);
  srand(1);
  for (i = 0; i < SIZE; i++) { 
    for (j = 0; j < SIZE; j++) { 
      matrix[i * SIZE + j] = rand();
    }
  }
  printf("call GE");
  dtime = dclock();
  iret = ge(matrix, SIZE);
  dtime = dclock()-dtime;
  printf("Time: %le \n", dtime);

  double check=0.0;
  for (i = 0; i < SIZE; i++) {
    for (j = 0; j < SIZE; j++) {
      check = check + matrix[i*SIZE + j];
    }
  }
  printf( "Check: %le \n", check);
  fflush( stdout );


  return iret;
}

