#include <stdio.h>

/// Add the two vectors a and b element-wise and store the result in vector c.
/// @param c The result of the vector addition.
/// @param a A vector of integers represented as a simple array
/// @param b A vector of integers represented as a simple array
/// @param n Size of the vectors
void vector_add(int *c, int *a, int *b, int n) 
{
  for(int i = 0; i < n; i++)
    c[i] = a[i] + b[i];
}

/// Print the value of each element of a vector.
/// @param v The vector to print.
void vector_print(int *v, int n)
{
  printf("(");
  for(int i = 0; i < n; i++) 
  {
    if (i == n - 1)
      printf("%d)\n", v[i]);
    else
      printf("%d ", v[i]);
  }
}

/// Create two simple vectors, perform an element-wise addition, and print the 
/// result.
/// @return Always returns 0.
int main()
{
  // initialize two simple vector
  int src1[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
  int src2[] = {10, 9, 8, 7, 6, 5, 4, 3, 2, 1};

  // the size of the vectors
  int vsize = sizeof(src1) / sizeof(int);
  
  // allocate space for an additional vector
  int dest[vsize];


  // perform the vector addition
  vector_add(dest, src1, src2, vsize);

  // print the vectors
  printf("src1: "); vector_print(src1, vsize);
  printf("src2: "); vector_print(src2, vsize);
  printf("dest: "); vector_print(dest, vsize);
  
  return 0;
}
