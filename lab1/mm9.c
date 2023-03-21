#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <time.h>

#define _first_(i, j) first[(j)*local_size + (i)]
#define _second_(i, j) second[(j)*local_size + (i)]
#define _multiply_(i, j) multiply[(j)*local_size + (i)]


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


//subroutine for optimization
int mm(double * first, double * second, double * multiply, int SIZE)
{
  register unsigned int i,j,k;
  register unsigned local_size = SIZE;
  register double sum = 0;
  for (i = 0; i < local_size; i++) { //rows in multiply
    for (j = 0; j < local_size; j++) { //columns in multiply
      for (k = 0; k < local_size;) { //columns in first and rows in second
          if (k < local_size - 8) {
              sum = sum + _first_(i, k) * _second_(j, k);
              sum = sum + _first_(i, k + 1)  * _second_(j, k + 1);
              sum = sum + _first_(i, k + 2)  * _second_(j, k + 2);
              sum = sum + _first_(i, k + 3)  * _second_(j, k + 3);
              sum = sum + _first_(i, k + 4)  * _second_(j, k + 4);
              sum = sum + _first_(i, k + 5)  * _second_(j, k + 5);
              sum = sum + _first_(i, k + 6)  * _second_(j, k + 6);
              sum = sum + _first_(i, k + 7)  * _second_(j, k + 7);
              k = k + 8;
          } else {
              sum = sum + _first_(i, k) * _second_(j, k);
              k++;
          }
      } 
      _multiply_(i, j) = sum;
      sum = 0;
    }
  }
  return 0;
}



int main( int argc, const char* argv[] ){
  int i,j,iret;
  double ** first;
  double ** second;
  double ** multiply;

  double * first_;
  double * second_;
  double * multiply_;
  
  double dtime;

  int SIZE = 1500;

//allocate blocks of continous memory
  first_ = (double*) malloc(SIZE*SIZE*sizeof(double));
  second_ = (double*) malloc(SIZE*SIZE*sizeof(double));
  multiply_ = (double*) malloc(SIZE*SIZE*sizeof(double));

//allocate 2D matrices
  first = (double**) malloc(SIZE*sizeof(double*));
  second = (double**) malloc(SIZE*sizeof(double*));
  multiply = (double**) malloc(SIZE*sizeof(double*));

//set pointers to continous blocks
  for (i = 0; i < SIZE; i++) {
    first[i] = first_ + i*SIZE;
    second[i] = second_ + i*SIZE;
    multiply[i] = multiply_ + i*SIZE;
  }

//fill matrices with test data
  for (i = 0; i < SIZE; i++) { //rows in first
    for (j = 0; j < SIZE; j++) { //columns in first
      first[i][j]=i+j;
      second[i][j]=i-j;
    }
  }

//measure mm subroutine computation time
  dtime = dclock();
  iret = mm(first_,second_,multiply_,SIZE);
  dtime = dclock()-dtime;
  printf( "Time: %le \n", dtime);


  fflush( stdout );

//cleanup
  free(first_);
  free(second_);
  free(multiply_);

  free(first);
  free(second);
  free(multiply);

  return iret;
}
 
