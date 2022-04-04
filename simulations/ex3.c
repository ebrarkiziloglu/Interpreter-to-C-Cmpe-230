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

float A[3][3];
float T[1][1];
float x[3][1];
float y[3][1];
float r;
float i;
float *ptr;

int main() {

    float x1[(sizeof(A)/sizeof(A[0])) * (sizeof(A[0])/sizeof(A[0][0]))] = {0.5, 0, 0.5, 0, 0, 0.5, 0.5, 1, 0};
    declareMatrix(sizeof(A)/sizeof(A[0]), sizeof(A[0])/sizeof(A[0][0]), x1, A);
    float x11[(sizeof(x)/sizeof(x[0])) * (sizeof(x[0])/sizeof(x[0][0]))] = { 1, 1, 1};
    declareMatrix(sizeof(x)/sizeof(x[0]), sizeof(x[0])/sizeof(x[0][0]), x11, x);

    for(int i=1 ; i<=10 ; i += 1 ){
        float *y1[sizeof(A)/sizeof(A[0])];
        allocateMatrix(y1, sizeof(A)/sizeof(A[0]), sizeof(x[0])/sizeof(x[0][0]));
        multiplyMatrix(sizeof(A)/sizeof(A[0]), sizeof(A[0])/sizeof(A[0][0]), sizeof(x[0])/sizeof(x[0][0]), A, x, y1);
        copyMatrix(y1, 3, 1, y);
        free(ptr);

        float *y2[sizeof(y)/sizeof(y[0])];
        allocateMatrix(y2, sizeof(y)/sizeof(y[0]), sizeof(y[0])/sizeof(y[0][0]));
        subtractMatrix(sizeof(y)/sizeof(y[0]), sizeof(y[0])/sizeof(y[0][0]), y, x, y2);
        float y3[sizeof(y)/sizeof(y[0])][sizeof(y[0])/sizeof(y[0][0])];
        copyMatrix(y2, sizeof(y)/sizeof(y[0]), sizeof(y[0])/sizeof(y[0][0]), y3);
        free(ptr);

        float *y4[sizeof(y3[0])/sizeof(y3[0][0])];
        allocateMatrix(y4, sizeof(y3[0])/sizeof(y3[0][0]), sizeof(y3)/sizeof(y3[0]));
        transposeMatrix(sizeof(y3)/sizeof(y3[0]), sizeof(y3[0])/sizeof(y3[0][0]), y3, y4);
        float y5[sizeof(y3[0])/sizeof(y3[0][0])][sizeof(y3)/sizeof(y3[0])];
        copyMatrix(y4, sizeof(y3[0])/sizeof(y3[0][0]), sizeof(y3)/sizeof(y3[0]), y5);
        free(ptr);

        float *y6[sizeof(y)/sizeof(y[0])];
        allocateMatrix(y6, sizeof(y)/sizeof(y[0]), sizeof(y[0])/sizeof(y[0][0]));
        subtractMatrix(sizeof(y)/sizeof(y[0]), sizeof(y[0])/sizeof(y[0][0]), y, x, y6);
        float y7[sizeof(y)/sizeof(y[0])][sizeof(y[0])/sizeof(y[0][0])];
        copyMatrix(y6, sizeof(y)/sizeof(y[0]), sizeof(y[0])/sizeof(y[0][0]), y7);
        free(ptr);

        float *y8[sizeof(y5) / sizeof(y5[0])];
        allocateMatrix(y8, sizeof(y5) / sizeof(y5[0]), sizeof(y7[0])/sizeof(y7[0][0]));
        multiplyMatrix(sizeof(y5) / sizeof(y5[0]), sizeof(y5[0]) / sizeof(y5[0][0]), sizeof(y7[0]) / sizeof(y7[0][0]), y5, y7, y8);
        float y9[sizeof(y5) / sizeof(y5[0])][sizeof(y7[0])/sizeof(y7[0][0])];
        copyMatrix(y8, sizeof(y5) / sizeof(y5[0]), sizeof(y7[0])/sizeof(y7[0][0]), y9);
        copyMatrixtoMatrix(sizeof(y5) / sizeof(y5[0]), sizeof(y7[0])/sizeof(y7[0][0]), T, y9);
        free(ptr);

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
}

void addMatrix(int n, int m, float a[n][m], float b[n][m], float *c[]){
    for(int i=0; i<n; i++){
        for(int j=0; j<m; j++){
            c[i][j] = a[i][j] + b[i][j];
        }
    }
}


void subtractMatrix(int n, int m, float a[n][m], float b[n][m], float *c[]){
    for(int i=0; i<n; i++){
        for(int j=0; j<m; j++){
            c[i][j] = a[i][j] - b[i][j];
        }
    }
}

void multiplyMatrix(int n, int k, int m, float a[n][k], float b[k][m], float *c[]){
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
    for(int i = 0; i < n; i++){
        for(int j = 0; j < m; j++){
            c[j][i] = a[i][j];
//            printf("i:%d\tj:%d\tvalue:%f\n", i, j, c[j][i]);
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
