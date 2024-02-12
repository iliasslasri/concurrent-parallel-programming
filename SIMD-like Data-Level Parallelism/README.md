iliass@DESKTOP-ILIASS:~/Desktop/SE/concurrent-programming/SIMD-like Data-Level Parallelism$ gcc -fopt-info-vec-optimized -std=c99 -fno-inline \
-msse -O3 -g -o vector_addition td1.2_vector_addition.c
    td1.2_vector_addition.c:10:20: optimized: loop vectorized using 16 byte vectors
    td1.2_vector_addition.c:10:20: optimized:  loop versioned for vectorization because of possible aliasing
    td1.2_vector_addition.c:10:20: optimized: loop vectorized using 8 byte vectors
    td1.2_vector_addition.c:35:7: optimized: basic block part vectorized using 16 byte vectors
    td1.2_vector_addition.c:35:7: optimized: basic block part vectorized using 16 byte vectors
    td1.2_vector_addition.c:35:7: optimized: basic block part vectorized using 16 byte vectors

<pre>movdqa 0xfb8(%rip),%xmm0</pre>


iliass@DESKTOP-ILIASS:~/Desktop/SE/concurrent-programming/SIMD-like Data-Level Parallelism$ gcc -fopt-info-vec-optimized -std=c99 -fno-inline \ -msse -O3 -g -o matrix_vector_multiply td1.3_matrix_vector_multiply.c
    td1.3_matrix_vector_multiply.c:21:22: optimized: loop vectorized using 16 byte vectors
    td1.3_matrix_vector_multiply.c:21:22: optimized:  loop versioned for vectorization because of possible aliasing
    td1.3_matrix_vector_multiply.c:23:21: optimized: basic block part vectorized using 4 byte vectors


iliass@DESKTOP-ILIASS:~/Desktop/SE/concurrent-programming/SIMD-like Data-Level Parallelism$ gcc -fopt-info-vec-missed -std=c99 -fno-inline \
-msse -O3 -g -o matrix_vector_multiply td1.3_matrix_vector_multiply.c
    td1.3_matrix_vector_multiply.c:19:20: missed: couldn't vectorize loop
    td1.3_matrix_vector_multiply.c:23:28: missed: not vectorized: complicated access pattern.
    td1.3_matrix_vector_multiply.c:32:20: missed: couldn't vectorize loop
    td1.3_matrix_vector_multiply.c:36:17: missed: statement clobbers memory: _1 = rand ();
    td1.3_matrix_vector_multiply.c:34:22: missed: couldn't vectorize loop
    td1.3_matrix_vector_multiply.c:36:17: missed: statement clobbers memory: _1 = rand ();
    td1.3_matrix_vector_multiply.c:36:17: missed: statement clobbers memory: _1 = rand ();
    td1.3_matrix_vector_multiply.c:45:20: missed: couldn't vectorize loop
    td1.3_matrix_vector_multiply.c:49:7: missed: statement clobbers memory: printf ("%3d ", _4);
    td1.3_matrix_vector_multiply.c:47:22: missed: couldn't vectorize loop
    td1.3_matrix_vector_multiply.c:49:7: missed: statement clobbers memory: printf ("%3d ", _4);
    td1.3_matrix_vector_multiply.c:49:7: missed: statement clobbers memory: printf ("%3d ", _4);
    td1.3_matrix_vector_multiply.c:51:5: missed: statement clobbers memory: __builtin_putchar (10);
    td1.3_matrix_vector_multiply.c:59:13: missed: statement clobbers memory: p_3 = malloc (1000000);
    td1.3_matrix_vector_multiply.c:67:20: missed: couldn't vectorize loop
    td1.3_matrix_vector_multiply.c:69:12: missed: statement clobbers memory: _1 = rand ();
    td1.3_matrix_vector_multiply.c:69:12: missed: statement clobbers memory: _1 = rand ();
    td1.3_matrix_vector_multiply.c:80:5: missed: couldn't vectorize loop
    td1.3_matrix_vector_multiply.c:80:5: missed: not vectorized: control flow in loop.
    td1.3_matrix_vector_multiply.c:77:3: missed: statement clobbers memory: __builtin_putchar (40);
    td1.3_matrix_vector_multiply.c:83:7: missed: statement clobbers memory: printf ("%d ", _6);
    td1.3_matrix_vector_multiply.c:81:7: missed: statement clobbers memory: printf ("%d)\n", _9);
    td1.3_matrix_vector_multiply.c:93:19: missed: statement clobbers memory: dest_3 = matrix_alloc ();
    td1.3_matrix_vector_multiply.c:94:19: missed: statement clobbers memory: msrc_5 = matrix_alloc ();
    td1.3_matrix_vector_multiply.c:98:3: missed: statement clobbers memory: matrix_init_rand (msrc_5);
    td1.3_matrix_vector_multiply.c:99:3: missed: statement clobbers memory: vector_init_rand (&vsrc);
    td1.3_matrix_vector_multiply.c:102:3: missed: statement clobbers memory: matrix_vector_multiply (dest_3, msrc_5, &vsrc);
    td1.3_matrix_vector_multiply.c:105:3: missed: statement clobbers memory: __builtin_puts (&"vsrc:"[0]);
    td1.3_matrix_vector_multiply.c:105:22: missed: statement clobbers memory: vector_print (&vsrc);
    td1.3_matrix_vector_multiply.c:106:3: missed: statement clobbers memory: __builtin_puts (&"msrc:"[0]);
    td1.3_matrix_vector_multiply.c:106:22: missed: statement clobbers memory: matrix_print (msrc_5);
    td1.3_matrix_vector_multiply.c:107:3: missed: statement clobbers memory: __builtin_puts (&"dest:"[0]);
    td1.3_matrix_vector_multiply.c:107:22: missed: statement clobbers memory: matrix_print (dest_3);
    td1.3_matrix_vector_multiply.c:110:3: missed: statement clobbers memory: free (dest_3);
    td1.3_matrix_vector_multiply.c:111:3: missed: statement clobbers memory: free (msrc_5);



The two loops have to be interchanged, i.e., the outer loop has to use the counter variable j and the inner variable i. The inner loop now visits the matrix elements row-wise, which allows their elements to be packed immediately into vectors.