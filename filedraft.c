#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdlib.h>

#define MAXDIMENSION 10

float(* transposeMatrix(int r, int c, float matrix[r][c]))[];
void printMatrix(int r, int c, float (*ptr)[]);
int isInt(float x);
float(* scalarMultiplication(float x, int r, int c, float matrix[r][c]))[];
float (* matrixMultiplication(int r, int n, int c, float matrix1[r][n], float matrix2[n][c]))[];
float **matrix_sum(int r, int c, float matrix1[r][c], float matrix2[r][c]);

int main(){
    float c[2][2] = {{1,2}, {3, 4}};
    float d[2][2] = {{1,2}, {3, 4}};

    float **k = matrix_sum(2, 2, c, d);
    printMatrix(2, 2, k);

    float (*pC)[sizeof(c[0])/sizeof(float)] = (&c)[0];
    float (*pD)[sizeof(d[0])/sizeof(float)] = (&d)[0];

    printMatrix(2, 2, pC);
    printf("\n");

    printf("c + d = \n");
    printMatrix(2, 2, matrix_sum(2, 2, c, d));

    printf("\n");
    printMatrix(2, 2, matrixMultiplication(2, 2, 2, pC, pD));
    return (0);
}

float **matrix_sum(int r, int c, float matrix1[r][c], float matrix2[r][c]){
    int i, j;
    float **matrix_res;
    matrix_res = malloc(sizeof(int)*r);

    for(i=0; i<r; i++) {
        matrix_res[i] = malloc(sizeof(int)*c);
    }
    for(i = 0; i < r; i++){
        for(j = 0; j < c; j++){
            matrix_res[i][j] = matrix1[i][j] + matrix2[i][j];
        }
    }
    return matrix_res;
}

float(* transposeMatrix(int r, int c, float matrix[r][c]))[]{
    static float tr[MAXDIMENSION][MAXDIMENSION];

    for(int i = 0; i < r; i++){
        for(int j = 0; j < c; j++){
            tr[j][i] = matrix[i][j];
        }
    }
    return tr;
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

float(* scalarMultiplication(float x, int r, int c, float matrix[r][c]))[]{
    for(int i=0; i<r; i++) {
        for(int j=0; j<c; j++){
            matrix[i][j] *= x;
        }
    }
    return matrix;
}

//PROBLEM while printing the res
float (*matrixMultiplication(int r, int n, int c, float matrix1[r][n], float matrix2[n][c]))[]{
    static float res[MAXDIMENSION][MAXDIMENSION];
    float (*ptr)[] = res;
    for(int i=0; i<r; i++){
        for(int j=0; j<c; j++){
            float temp=0;
            for(int k=0; k<n; k++){
                temp += matrix1[i][k] * matrix2[k][j];
            }
            res[i][j] = temp;
        }
    }
    return ptr;
}

//mustn't print out float values with .000000
int isInt(float x){
    int n = (int)x;
    double check = x-n;
    if(check>0)
        return 0;
    return 1;
}
