#include <stdio.h>
#include <stdlib.h>
#include <math.h>


void allocateMatrix(float *A[], int n, int m);
void subtractMatrix(int n, int m, float a[n][m], float b[n][m], float *c[]);
void multiplyMatrix(int n, int k, int m, float a[n][m], float b[m][k], float *c[]);
void copyMatrix( float *x[], int n, int m, float a[n][m]);
void addMatrix(int n, int m, float a[n][m], float b[n][m], float *c[]);
void declareMatrix(int row, int column, float temp[], float a[row][column]);
void printMatrix(int r, int c, float matrix[r][c]);
int isInt(float x);
void transposeMatrix(int n, int m, float a[n][m], float *c[]);
void copyMatrixtoMatrix(int r, int c, float a[r][c], float b[r][c]);
void printSep();
int row = 0, column = 0, middle = 0;
float *ptr;

float A[3][3];
float T[1][1];
float x[3][1];
float y[3][1];
float r;
float i;


int main() {

    float x1[(sizeof(A)/sizeof(A[0])) * (sizeof(A[0])/sizeof(A[0][0]))] = {0.5, 0, 0.5, 0, 0, 0.5, 0.5, 1, 0};
    declareMatrix(sizeof(A)/sizeof(A[0]), sizeof(A[0])/sizeof(A[0][0]), x1, A);
    float x11[(sizeof(x)/sizeof(x[0])) * (sizeof(x[0])/sizeof(x[0][0]))] = { 1, 1, 1};
    declareMatrix(sizeof(x)/sizeof(x[0]), sizeof(x[0])/sizeof(x[0][0]), x11, x);

    for(int i=1 ; i<=10 ; i += 1 ){

        row = sizeof(A)/sizeof(A[0]);
        column = sizeof(x[0])/sizeof(x[0][0]);
        middle = sizeof(A[0])/sizeof(A[0][0]);
        float *y0[row];
        float y1[row][column];
        multiplyMatrix(row, middle, column, A, x, y0);
        copyMatrix(y0, 3, 1, y1);
        copyMatrixtoMatrix(row, column, y, y1);

        row = sizeof(y)/sizeof(y[0]);
        column = sizeof(y[0])/sizeof(y[0][0]);
        float *y2[row];
        float y3[row][column];
        subtractMatrix(row, column, y, x, y2);
        copyMatrix(y2, row, column, y3);

        row = sizeof(y3[0])/sizeof(y3[0][0]);
        column = sizeof(y3)/sizeof(y3[0]);
        float *y4[row];
        float y5[row][column];
        transposeMatrix(column, row, y3, y4);
        copyMatrix(y4, row, column, y5);

        row = sizeof(y)/sizeof(y[0]);
        column = sizeof(y[0])/sizeof(y[0][0]);
        float *y6[row];
        float y7[row][column];
        subtractMatrix(row, column, y, x, y6);
        copyMatrix(y6, row, column, y7);

        row = sizeof(y5) / sizeof(y5[0]);
        column = sizeof(y7[0])/sizeof(y7[0][0]);
        middle = sizeof(y5[0]) / sizeof(y5[0][0]);
        float *y8[row];
        float y9[row][column];
        multiplyMatrix(row, middle, column, y5, y7, y8);
        copyMatrix(y8, row, column, y9);
        copyMatrixtoMatrix(row, column, T, y9);

        r = sqrt(T[0][0]);
        printf("%f\n", r);
        copyMatrixtoMatrix(sizeof(x) / sizeof(x[0]), sizeof(x[0])/sizeof(x[0][0]), x, y);
        
    }
    printSep();
    printMatrix(sizeof(x) / sizeof(x[0]), sizeof(x[0])/sizeof(x[0][0]), x);

    return 0;
}

void allocateMatrix(float *a[], int n, int m){
    ptr = (float *)calloc(m*n, sizeof(float));
    for(int i=0; i<n; i++){
        a[i] = ptr+(m*i);
    }
}

void copyMatrix( float *x[], int n, int m, float a[n][m]){
    for(int i=0; i<n; i++){
        for(int j=0; j<m; j++){
            a[i][j] = x[i][j];
        }
    }
    free(ptr);
}

void addMatrix(int n, int m, float a[n][m], float b[n][m], float *c[]){
    allocateMatrix(c, n, m);

    for(int i=0; i<n; i++){
        for(int j=0; j<m; j++){
            c[i][j] = a[i][j] + b[i][j];
        }
    }
}


void subtractMatrix(int n, int m, float a[n][m], float b[n][m], float *c[]){
    allocateMatrix(c, n, m);

    for(int i=0; i<n; i++){
        for(int j=0; j<m; j++){
            c[i][j] = a[i][j] - b[i][j];
        }
    }
}

void multiplyMatrix(int n, int k, int m, float a[n][k], float b[k][m], float *c[]){
    allocateMatrix(c, n, m);

    for(int i=0; i<n; i++){
        for(int j=0; j<m; j++){
            c[i][j] = 0;
            for(int l=0; l<k; l++){
                c[i][j] += a[i][l]*b[l][j];
            }
        }
    }
}

void transposeMatrix(int n, int m, float a[n][m], float *c[]){
    allocateMatrix(c, m, n);

    for(int i = 0; i < n; i++){
        for(int j = 0; j < m; j++){
            c[j][i] = a[i][j];
        }
    }
}


void declareMatrix(int row, int column, float temp[], float a[row][column]){
    int k = 0;
    for(int i = 0; i < row; i ++){
        for(int j = 0; j < column; j ++){
            a[i][j] = temp[k];
            k++;
        }
    }
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

int isInt(float x){
    int n = (int)x;
    double check = x-n;
    if(check>0)
        return 0;
    return 1;
}

void copyMatrixtoMatrix(int r, int c, float a[r][c], float b[r][c]){
    for(int i = 0; i < r; i++){
        for(int j = 0; j < c; j++){
            a[i][j] = b[i][j];
        }
    }
    return;
}

void printSep(){
    printf("----------\n");
    return ;
}
