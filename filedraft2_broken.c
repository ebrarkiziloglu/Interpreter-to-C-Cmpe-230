#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdlib.h>
#include <math.h>

#define MAXDIMENSION 10

int tempRow;
int tempCol;


float (*matrixsum(int r, int c, float matrix1[r][c], int row, int column, float matrix2[row][column]))[];
float (*matrixsubs(int r, int c, float matrix1[r][c], int row, int column, float matrix2[row][column]))[];

void printMatrix(int row, int column, int r, int c, float (*ptr)[]);
float(* tr(int r, int c, float matrix[r][c]))[];
//void printMatrix(int r, int c, float (*ptr)[]);
float(* scalarmatrixmultiply(int r, int c, float matrix[r][c], float x))[];
float (* matrixmultiply(int r, int n, int c, float matrix1[r][n], float matrix2[n][c]))[];
float scalarsum(float x, float y);
float scalarsubs(float x, float y);
float scalarmultiply(float x, float y);
int isInt(float x);
void printSep();
float choose(float a, float b, float c, float d);

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
    printMatrix(2, 2, matrixsum(2, 2, c, d));

    printf("\n");
    printMatrix(2, 2, matrixmultiply(2, 2, 2, pC, pD));
    return (0);
}

//float (*matrix_sum(int r, int c, float matrix1[r][c], float matrix2[r][c]))[]{
//    int i, j;
//    static float matrix_res[MAXDIMENSION][MAXDIMENSION];
//
//    for(i = 0; i < r; i++){
//        for(j = 0; j < c; j++){
//            matrix_res[i][j] = matrix1[i][j] + matrix2[i][j];
//        }
//    }
//    return matrix_res;
//}

float(* tr(int r, int c, float matrix[r][c]))[]{
    static float tr[MAXDIMENSION][MAXDIMENSION];

    for(int i = 0; i < r; i++){
        for(int j = 0; j < c; j++){
            tr[j][i] = matrix[i][j];
        }
    }
    return tr;
}

//void printMatrix(int r, int c, float matrix[r][c]){
//    float temp;
//    for(int i=0; i<r; i++){
//        for(int j=0; j<c; j++){
//            temp = matrix[i][j];
//            if(isInt(temp)==1)
//                printf("%d ", (int)temp);
//            else
//                printf("%f ", temp);
//        }
//        printf("\n");
//    }
//}

float(* scalarmatrixmultiply(int r, int c, float matrix[r][c], float x))[]{
    for(int i=0; i<r; i++) {
        for(int j=0; j<c; j++){
            matrix[i][j] *= x;
        }
    }
    return matrix;
}

//PROBLEM while printing the res
float (*matrixmultiply(int r, int n, int c, float matrix1[r][n], float matrix2[n][c]))[]{
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

float scalarsum(float x, float y){
    return x+y;
}

float scalarsubs(float x, float y){
    return y-x;
}

float scalarmultiply(float x, float y){
    return x*y;
}

//mustn't print out float values with .000000
int isInt(float x){
    int n = (int)x;
    double check = x-n;
    if(check>0)
        return 0;
    return 1;
}

void printSep(){
    printf("----------\n");
    return ;
}

float (*matrixsum(int r, int c, float matrix1[r][c], int row, int column, float matrix2[row][column]))[]{
    int i, j;
    if( (row == MAXDIMENSION && column == MAXDIMENSION ) && ( r != MAXDIMENSION && c != MAXDIMENSION ) ){
        if(tempRow == r  && tempCol == c){
            static float matrix_res[MAXDIMENSION][MAXDIMENSION];
            for(i = 0; i < r; i++){
                for(j = 0; j < c; j++){
                    matrix_res[i][j] = matrix1[i][j] + matrix2[i][j];
                }
            }
            return matrix_res;
        }
        else {
            return NULL;
        }
    }
    else if((row != MAXDIMENSION && column != MAXDIMENSION ) && ( r == MAXDIMENSION && c == MAXDIMENSION )){
        if(tempRow == row  && tempCol == column){
            static float matrix_res[MAXDIMENSION][MAXDIMENSION];
            for(i = 0; i < row; i++){
                for(j = 0; j < column; j++){
                    matrix_res[i][j] = matrix1[i][j] + matrix2[i][j];
                }
            }
            return matrix_res;
        }
        else {
            return NULL;
        }
    }
    else if (row == r && column == c){
        if(r != MAXDIMENSION && c != MAXDIMENSION){
            static float matrix_res[MAXDIMENSION][MAXDIMENSION];

            for(i = 0; i < r; i++){
                for(j = 0; j < c; j++){
                    matrix_res[i][j] = matrix1[i][j] + matrix2[i][j];
                }
            }
        }
        else{
            return NULL;
        }
    }
    return NULL;
}

float (*matrixsubs(int r, int c, float matrix1[r][c], int row, int column, float matrix2[row][column]))[]{
    int i, j;
    if( (row == MAXDIMENSION && column == MAXDIMENSION ) && ( r != MAXDIMENSION && c != MAXDIMENSION ) ){
        if(tempRow == r  && tempCol == c){
            static float matrix_res[MAXDIMENSION][MAXDIMENSION];
            for(i = 0; i < r; i++){
                for(j = 0; j < c; j++){
                    matrix_res[i][j] = matrix2[i][j] - matrix1[i][j];
                }
            }
            return matrix_res;
        }
        else {
            return NULL;
        }
    }
    else if((row != MAXDIMENSION && column != MAXDIMENSION ) && ( r == MAXDIMENSION && c == MAXDIMENSION )){
        if(tempRow == row  && tempCol == column){
            static float matrix_res[MAXDIMENSION][MAXDIMENSION];
            for(i = 0; i < row; i++){
                for(j = 0; j < column; j++){
                    matrix_res[i][j] = matrix2[i][j] - matrix1[i][j];
                }
            }
            return matrix_res;
        }
        else {
            return NULL;
        }
    }
    else if (row == r && column == c){
        if(r != MAXDIMENSION && c != MAXDIMENSION){
            static float matrix_res[MAXDIMENSION][MAXDIMENSION];
            for(i = 0; i < r; i++){
                for(j = 0; j < c; j++){
                    matrix_res[i][j] = matrix2[i][j] - matrix1[i][j];
                }
            }
        }
        else{
            return NULL;
        }
    }
    return NULL;
}

void printMatrix(int row, int column, int r, int c, float matrix[r][c]){
    float temp;
    for(int i=0; i<row; i++){
        for(int j=0; j<column; j++){
            temp = matrix[i][j];
            if(isInt(temp)==1)
                printf("%d ", (int)temp);
            else
                printf("%f ", temp);
            // printf("%f ", matrix[i][j]);
        }
        printf("\n");
    }
}

float choose(float a, float b, float c, float d){
    if( a == 0.0)  return b;
    else if( a > 0 ) return c;
    else return d;
}
