#include <stdio.h>

/// @param a A vector of integers represented as a simple array
/// @param n Size of the vector
/// @return The sum over all elements in the vector, i.e., 
/// a[1] + a[2] + ... + a[n-1]
int vector_reduction_sum(int *a, int n)
{
  int sum = 0;
  for(int i = 0; i < n; i++)
    sum += a[i];

  return sum;
}

/// Compute the sum over all elements of a simple vector and terminate.
/// @return Always returns 0.
int main()
{
  // initialize a simple vector
  int a[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

  // compute and print the sum over all vector elements
  int sum = vector_reduction_sum(a, sizeof(a) / sizeof(int));
  printf("Sum: %d\n", sum);

  return 0;
}
