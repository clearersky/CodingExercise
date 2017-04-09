#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

typedef unsigned int uint32_t;

//struct to store a matrix in the naive form
typedef struct
{
	float* matrixPtr;
	int    rowNum;
	int    columnNum;
}NaiveMatrix;

//struct to store a sparse matrix
typedef struct
{
	float* 	value;
	int*	rowIndex;
	int* 	columnIndex;
	int 	length;		//total number of non-zero elements
	int 	rowNum;		//number of rows in the naive form
	int 	columnNum;	//number of columns in the naive form
}SparseMatrix; 

void readMatrixDimension(int& m, int& n, int& k)
{
	printf("Input the value of m:\n");
	scanf("%d", &m);

	if (m <= 0)
	{
		m = 0;
		printf("Invalid value\n");
	}

	printf("Input the value of n:\n");
	scanf("%d", &n);

	if (n <= 0)
	{
		n = 0;
		printf("Invalid value\n");
	}

	printf("Input the value of k:\n");
	scanf("%d", &k);

	if (k <= 0)
	{
		k = 0;
		printf("Invalid value\n");
	}
}

void generateRandomMatrix(NaiveMatrix matrix)
{
	float* matrixPtr = matrix.matrixPtr;
	int rowNum = matrix.rowNum;
	int columnNum = matrix.columnNum;

	int nonZeroElementNum = rowNum*columnNum/20;
	int randomIndex = 0;
	float randomValue = 0;

	printf("nonzero number is %d\n", nonZeroElementNum);

	memset(matrixPtr, 0, sizeof(float)*rowNum*columnNum);

	for (int i = 0; i < nonZeroElementNum; i++)
	{
		//find a random index that has value 0 
		do{
			randomIndex = rand()%(rowNum*columnNum);
		}while(matrixPtr[randomIndex] != 0);

		matrixPtr[randomIndex] = (float)rand()/(float)(RAND_MAX/13.5) + 2;
	}
}

//allocate space to store data of a compact sparse matrix
void allocateForCompactStorage(SparseMatrix* compactForm, int length)
{
	compactForm->value = (float*) malloc(sizeof(float)*length);
	compactForm->rowIndex = (int*) malloc(sizeof(int)*length);
	compactForm->columnIndex = (int*) malloc(sizeof(int)*length);
	
	compactForm->length = 0;
	compactForm->rowNum = 0;
	compactForm->columnNum = 0;
}

//convert a naive matrix into compact form
void convertCompactForm(NaiveMatrix naiveForm, SparseMatrix* compactForm)
{
	int rowNum = naiveForm.rowNum;
	int columnNum = naiveForm.columnNum;
	float*	matrixPtr = naiveForm.matrixPtr;
	int length = rowNum*columnNum/20;	
	int count = 0;

	//as we have a predefined sparse ratio, we just pre-calculate the length
	allocateForCompactStorage(compactForm, length);

	//write the compact form
	for (int i = 0; i < rowNum; i++)
	{
		for (int j = 0; j < columnNum; j++)
		{
			if (matrixPtr[i*columnNum+j] != 0)
			{
				compactForm->value[count] = matrixPtr[i*columnNum+j];
				compactForm->rowIndex[count] = i;
				compactForm->columnIndex[count] = j;
				count++;
			}
		}
	}
	compactForm->length = count;
	compactForm->rowNum = rowNum;
	compactForm->columnNum = columnNum;
}

//free the memory used to store the compact form of a sparse matrix
void freeCompactStorage(SparseMatrix* compactForm)
{
	free(compactForm->value);
	free(compactForm->rowIndex);
	free(compactForm->columnIndex);
	compactForm->length = 0;
}

//transpose the given matrix
void sparseTranspose(SparseMatrix* originalMatrix, SparseMatrix* transposeMatrix)
{
	//array to store the number of non-zero elements in each column
	int* countEachColumn = (int*) malloc(sizeof(int)*originalMatrix->columnNum);
	
	//index in transposeMatrix's compact form to write the next element of each row (of the transposeMatrix)
	int* indexToWriteNextElement = (int*) malloc(sizeof(int)*originalMatrix->columnNum);

	//initialize to zero
	memset(countEachColumn, 0, sizeof(int)*originalMatrix->columnNum);
	memset(indexToWriteNextElement, 0, sizeof(int)*originalMatrix->columnNum);
	
	//count the non-zero element in each column
	for (int i = 0; i < originalMatrix->length; i++)
	{
		countEachColumn[originalMatrix->columnIndex[i]]++;
	}
	
	//initialize indexToWriteNextElement for each column
	for (int i = 1; i < originalMatrix->columnNum; i++)
	{
		indexToWriteNextElement[i] = indexToWriteNextElement[i-1] + countEachColumn[i-1];
	}
	
	//generate transposeMatrix
	allocateForCompactStorage(transposeMatrix, originalMatrix->length);
		
	for (int i = 0; i < originalMatrix->length; i++)
	{
		int oldColumnIndex = originalMatrix->columnIndex[i]; 
		int oldRowIndex = originalMatrix->rowIndex[i];
		
		transposeMatrix->value[indexToWriteNextElement[oldColumnIndex]] = originalMatrix->value[i];
		transposeMatrix->rowIndex[indexToWriteNextElement[oldColumnIndex]] = oldColumnIndex;
		transposeMatrix->columnIndex[indexToWriteNextElement[oldColumnIndex]] = oldRowIndex;
		indexToWriteNextElement[oldColumnIndex]++;
	}
	
	transposeMatrix->length = originalMatrix->length;
	transposeMatrix->rowNum = originalMatrix->columnNum;
	transposeMatrix->columnNum = originalMatrix->rowNum;	
}

//multiplication of matrix in compact form: B is the transposed matrix
void compactMultiplication(SparseMatrix matrixA, SparseMatrix matrixB, NaiveMatrix matrixC)
{
	clock_t start, end;
    double 	cpu_time_used;
     
    start = clock();
	
	int 	positionA = 0;	//current position in A's array
	int 	positionB = 0; 	//current position in B's array
	int 	currentRowStartA = 0;	//start index of the current row in A
	int 	currentRowStartB = 0;	//start index of the current row in B
	float 	sum = 0;
	
	while (positionA < matrixA.length && positionB < matrixB.length)
	{
		if (matrixA.columnIndex[positionA] < matrixB.columnIndex[positionB])
		{
			positionA++;
		}
		else if (matrixA.columnIndex[positionA] > matrixB.columnIndex[positionB])
		{
			positionB++;
		}
		else
		{
			sum+= matrixA.value[positionA]*matrixB.value[positionB];
			positionA++;
			positionB++;
		}
		
		//if either positionA or positionB jumps to a new row: multiplication of the last pair is finished
		//move positionB to the start of next row. If this is already the last row of B,
		//move positionA to the start of next row; otherwise, move positionA back to currentRowStartA,
		//update row start, update matrixC, reset sum
		if ((positionA == matrixA.length) ||
			(positionB == matrixB.length) ||
			(matrixA.rowIndex[positionA] != matrixA.rowIndex[currentRowStartA]) ||
			(matrixB.rowIndex[positionB] != matrixB.rowIndex[currentRowStartB]))
		{
			int oldRowIndexA = matrixA.rowIndex[currentRowStartA];
			int oldRowIndexB = matrixB.rowIndex[currentRowStartB];
			
			//move positionB to the new row
			while ((positionB < matrixB.length) && (matrixB.rowIndex[positionB] == oldRowIndexB))
			{
				positionB++;
				
				//optimization: we may optimize this by storing the start index of each row, so we
				//do not need to search the start of next row one by one 
			}
			
			if (positionB == matrixB.length)
			{
				//already the last row of B, move to A's next row
				while ((positionA < matrixA.length) && (matrixA.rowIndex[positionA] == oldRowIndexA))
				{
					positionA++;
				
					//optimization: we may optimize this by storing the start index of each row, so we
					//do not need to search the start of next row one by one 
				}
				
				if (positionA == matrixA.length)
				{
					//reaches A's end, do nothing
				}
				else
				{
					//A moves to a new row
					currentRowStartA = positionA;
					positionB = 0;
					currentRowStartB = 0;
				}
			}
			else
			{
				//moved to a new row of B, A returns to the currentRowStartA
				positionA = currentRowStartA;
				currentRowStartB = positionB;
			}
			
			//update matrixC, reset sum
			matrixC.matrixPtr[oldRowIndexA*matrixC.columnNum+oldRowIndexB] = sum;
			sum = 0;
		}

	}
	
	end = clock();
	cpu_time_used = ((double) (end - start))/(CLOCKS_PER_SEC);
	printf("Sparse multiplication: %lf seconds\n", cpu_time_used);
}

//perform sparse matrix multiplication
void sparseMatrixMultiplication(NaiveMatrix matrixA, NaiveMatrix matrixB, NaiveMatrix matrixC)
{
	SparseMatrix compactA;
	SparseMatrix compactB;
	SparseMatrix transposeB;

	//store matrixA and B in compact form
	convertCompactForm(matrixA, &compactA);
	convertCompactForm(matrixB, &compactB);

	//transpose matrix B
	sparseTranspose(&compactB, &transposeB);
	
	//multiply two matrix in compact form
	compactMultiplication(compactA, transposeB, matrixC);

	//free memory used to store compact form
	freeCompactStorage(&compactA);
	freeCompactStorage(&compactB);
	freeCompactStorage(&transposeB);
}

//perform traditional matrix multiplication
void traditionalMatrixMultiplication(NaiveMatrix matrixA, NaiveMatrix matrixB, NaiveMatrix matrixC)
{
	clock_t start, end;
    double 	cpu_time_used;
     
    start = clock();

	float*	matrixAPtr = matrixA.matrixPtr;
	float* 	matrixBPtr = matrixB.matrixPtr;
	float*	matrixCPtr = matrixC.matrixPtr;

	int 	rowNumA = matrixA.rowNum;
	int 	rowNumB = matrixB.rowNum;
	int 	rowNumC = matrixC.rowNum;

	int 	columnNumA = matrixA.columnNum;
	int 	columnNumB = matrixB.columnNum;
	int 	columnNumC = matrixC.columnNum;

	if (columnNumA != rowNumB)
	{
		return;
	}

	for (int i = 0; i < rowNumC; i++)
	{
		for (int j = 0; j < columnNumC; j++)
		{
			float*	elementPtr = &matrixCPtr[i*columnNumC+j];
			*elementPtr = 0;
			
			//multiply ith row of A and jth column of B	
			for (int index = 0; index < columnNumA; index++)
			{
				*elementPtr += matrixAPtr[i*columnNumA+index]*matrixBPtr[index*columnNumB+j];
			}	
		}
	}
	
	end = clock();
	cpu_time_used = ((double) (end - start))/(CLOCKS_PER_SEC);
	printf("Traditional multiplication: %lf seconds\n", cpu_time_used);
}

void printMatrix(NaiveMatrix matrix)
{
	float*  matrixPtr = matrix.matrixPtr;
	int 	rowNum = matrix.rowNum;
	int 	columnNum = matrix.columnNum;

	for (int i = 0; i < rowNum; i++)
	{
		for (int j = 0; j < columnNum; j++)
		{
			printf("%.4f ", matrixPtr[i*columnNum+j]);
		}
		printf("\n");
	}
	printf("\n");
}

//allocate memory for matrix and store its dimension
void allocateForMatrix(NaiveMatrix* matrix, int rowNum, int columnNum)
{
	matrix->matrixPtr = (float*) malloc(sizeof(float)*rowNum*columnNum);
	matrix->rowNum = rowNum;
	matrix->columnNum = columnNum;
}

//check if two matrices equal
void compareMatrix(NaiveMatrix matrixA, NaiveMatrix matrixB)
{
	if ((matrixA.rowNum != matrixB.rowNum) ||
		(matrixA.columnNum != matrixB.columnNum))
	{
		printf("Dimension not same!\n");
	}
		
	for (int i = 0; i < matrixA.rowNum; i++)
	{
		for (int j = 0; j < matrixA.columnNum; j++)
		{
			if (matrixA.matrixPtr[i*matrixA.columnNum+j] != 
				matrixB.matrixPtr[i*matrixB.columnNum+j])
			{
				printf("Not equal!\n");
				return;
			}
		}
	}
	
	printf("Matrices equal.\n");
}

int main()
{
	int m = 0;	//row number of matrix A
	int n = 0;	//number of columns of A
	int k = 0; 	//column number of matrix B

	NaiveMatrix matrixA;
	NaiveMatrix matrixB;
	NaiveMatrix productSparse;	//product by sparse multiplication
	NaiveMatrix productNaive;	//product by naive multiplication

	srand((int)time(0));

	//read the dimension
	readMatrixDimension(m, n, k);

	//allocate memory for all matrix
	allocateForMatrix(&matrixA, m, n);
	allocateForMatrix(&matrixB, n, k);
	allocateForMatrix(&productSparse, m, k);
	allocateForMatrix(&productNaive, m, k);

	//generate random matrices (store in naive form)
	generateRandomMatrix(matrixA);
	generateRandomMatrix(matrixB);

	//print out generated matrix
	//printMatrix(matrixA);
	//printMatrix(matrixB);

	//perform sparse matrix multiplication
	sparseMatrixMultiplication(matrixA, matrixB, productSparse);

	//perform traditional multiplication
	traditionalMatrixMultiplication(matrixA, matrixB, productNaive);

	//print out matrix
	//printMatrix(productSparse);
	//printMatrix(productNaive);
	
	//check results
	compareMatrix(productSparse, productNaive);

	//release memory
	free(matrixA.matrixPtr);
	free(matrixB.matrixPtr);
	free(productNaive.matrixPtr);
	free(productSparse.matrixPtr);

	return 0;
}