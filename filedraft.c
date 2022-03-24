#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdlib.h>

#define MAXDIMENSION 10

float* transposeMatrix(int r, int c, float matrix[r][c]);
float* transposeSquareMatrix(int r, float matrix[r][r]);
void printMatrix(float matrix[r][c]);
float* scalarMultiplication(float x, int r, int c, float matrix[r][c]);
float* matrixMultiplication(int r, int n, int c, float matrix1[r][n], float matrix2[n][c]);
int isInt(float x);

int main(){
    float *ptr;
    ptr = transposeMatrix(2, 2, {{1,2},{3,4}});
    printMatrix(ptr);
    return (0);
}

float* transposeMatrix(int r, int c, float matrix[r][c]){
    static float tr[MAXDIMENSION][MAXDIMENSION];
    for(int i = 0; i < r; i++){
        for(int j = 0; j < c; j++){
            tr[j][i] = matrix[i][j];
        }
    }
    return tr;
}

//doesn't create a new matrix, modifies the current one
float* transposeSquareMatrix(int r, float matrix[r][r]){
    for(int i = 0; i < r; i++){
        for(int j = i; j < r; j++){
            float temp = matrix[i][j];
            matrix[i][j] = matrix[j][i];
            matrix[j][i] = temp;
        }
    }
    return matrix;
}

void printMatrix(int r, int c, float matrix[r][c]){
    float temp;
    for(int i=0; i<r; i++){
        for(int j=0; j<c; j++){
            temp = matrix[i][j];
            if(isInt(temp)==1)
                printf("%d ", (int)temp);
            else
                printf("%f ", temp);
        }
        printf("\n");
    }
}

float* scalarMultiplication(float x, int r, int c, float matrix[r][c]){
    for(int i=0; i<r; i++) {
        for(int j=0; j<c; j++){
            matrix[i][j] *= x;
        }
    }
    return matrix;
}

//PROBLEM while printing the res
//printing the whole 10x10 matrix works fine but with smaller values, prints 0
float* matrixMultiplication(int r, int n, int c, float matrix1[r][n], float matrix2[n][c]){
    static float res[MAXDIMENSION][MAXDIMENSION];
    for(int i=0; i<r; i++){
        for(int j=0; j<c; j++){
            float temp=0;
            for(int k=0; k<n; k++){
                temp += matrix1[i][k] * matrix2[k][j];
                res[i][j] = temp;
            }
        }
    }
    return res;
}

//mustn't print out float values with .000000
int isInt(float x){
    int n = (int)x;
    double check = x-n;
    if(check>0)
        return 0;
    return 1;
}
