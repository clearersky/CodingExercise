# Sparse Matrix Multiplication
### Problem Description
The question asks to write a program to perform multiplication of two sparse matrices.
1. First, user is asked to input the dimensions of Matrix A (m x n) and Matrix B (n x k);
1. Then the program should generate two random sparse matrices with the corresponding dimensions, and the sparsities of both matrices are 0.95 (i.e., 95% of the elements in a matrix are zero). The non-zero elements are generated uniformly at random, and take random values from 2.0 to 15.5. 
1. The question asks to design an **efficient sparse matrix multiplication algorithm** to multiply A and B. 
1. The naive matrix multiplication method is also required to be implemented, and its running time is compared with that of the sparse matrix multiplication algorithm.

### Compact Form of a Sparse Matrix
To reduce the memory used to store the sparse matrix, I use three arrays with length the same as the number of non-zero elements in the matrix: one array to store the values of the elements; one array to store the corresponding row indices of the elements; one array to store the corresponding column indices of the elements.

### Multiplication with Compact Form
Here is a brief description of the multiplication algorithm:
1. Write matrix A and matrix B in compact form;
1. Transpose matrix B (in compact form): create three arrays for B-transpose; first count the number of non-zero elements in each column in matrix B; then start from the first non-zero element in B, calculate its index in the B-transpose arrays, and put in the corresponding locations;
1. Multiply each row of matrix A with each row of B-transpose (in compact form): for each pair, first set sum to zero; traverse the two rows at the same time; if elements of the two rows have the same column index, multiply them and add to the sum.
