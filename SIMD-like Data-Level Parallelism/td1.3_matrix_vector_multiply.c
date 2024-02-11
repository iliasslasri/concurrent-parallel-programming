#include <stdio.h>
#include <stdlib.h>

/// define the size of the matrices
#define SIZE 500

/// Matrix representation as two-dimensional array
typedef int (*matrix_t)[SIZE];

/// Vector representation as a simple array
typedef int vector_t[SIZE];

/// Multiply matrix m with vector v and store the result in d.
/// @param d The result matrix.
/// @param m The input matrix.
/// @param v The input vector.
void matrix_vector_multiply(matrix_t d, matrix_t m, vector_t v)
{
  for(int i = 0; i < SIZE; i++ )
  {
    for(int j = 0; j < SIZE; j++ )
    {
      d[j][i] = m[j][i] * v[i];
    }
  }
}

/// Initialize a given matrix with random values (module 5).
/// @param a The matrix to initialize.
void matrix_init_rand(matrix_t a)
{
  for(int i = 0; i < SIZE; i++ )
  {
    for(int j = 0; j < SIZE; j++ )
    {
      a[i][j] = rand() % 5;
    }
  }
}

/// Print the elements of a matrix.
/// @param m The matrix to print.
void matrix_print(matrix_t m)
{
  for(int i = 0; i < SIZE; i++ )
  {
    for(int j = 0; j < SIZE; j++ )
    {
      printf("%3d ", m[i][j]);
    }
    printf("\n");
  }
}

/// Allocate a matrix of SIZE x SIZE elements on the heap.
/// @return A pointer to a newly allocated block of memory for a matrix.
matrix_t matrix_alloc()
{
  void *p = malloc(sizeof(int) * SIZE * SIZE);
  return p;
}

/// Initialize a given vector with random values (module 5).
/// @param v The vector to initialize.
void vector_init_rand(vector_t v)
{
  for(int i = 0; i < SIZE; i++ )
  {
    v[i] = rand() % 5;
  }
}

/// Print the value of each element of a vector.
/// @param v The vector to print.
void vector_print(int *v)
{
  printf("(");
  for(int i = 0; i < SIZE; i++) 
  {
    if (i == SIZE - 1)
      printf("%d)\n", v[i]);
    else
      printf("%d ", v[i]);
  }
}

/// Create an input matrix and vector with random data, multiply them, and 
/// display the result.
/// @return Always returns 0.
int main()
{
  // allocate two matrices and a vector
  matrix_t dest = matrix_alloc();
  matrix_t msrc = matrix_alloc();
  vector_t vsrc = {0,};

  // initialize two of them
  matrix_init_rand(msrc);
  vector_init_rand(vsrc);

  // perform the matrix vector product
  matrix_vector_multiply(dest, msrc, vsrc);

  // print the matrices and the vector
  printf("vsrc:\n"); vector_print(vsrc);
  printf("msrc:\n"); matrix_print(msrc);
  printf("dest:\n"); matrix_print(dest);

  // free dynamically allocated memory
  free(dest);
  free(msrc);
  
  return 0;
}
