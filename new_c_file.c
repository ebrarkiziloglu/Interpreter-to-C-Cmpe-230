#include <stdio.h>
#include <stdlib.h>
#include <math.h>


void allocateMatrix(float *A[], int n, int m);
void addMatrix(int n, int m, float a[n][m], float b[n][m], float *c[]);
void subtractMatrix(int n, int m, float a[n][m], float b[n][m], float *c[]);
void multiplyMatrix(int n, int k, int m, float a[n][m], float b[m][k], float *c[]);
void multiplymatrixwithscalar(float x, int n, int m, float a[n][m], float *c[]);
void transposeMatrix(int n, int m, float a[n][m], float *c[]);
void copyMatrix( float *x[], int n, int m, float a[n][m]);
void copyMatrixtoMatrix(int r, int c, float a[r][c], float b[r][c]);
int choose (float x1, float x2, float x3, float x4);
void printMatrix(int r, int c, float matrix[r][c]);
void printSep();
void declareMatrix(int row, int column, float temp[], float a[row][column]);
int isInt(float x);
int row = 0, column = 0, middle = 0;
float *ptr;

int main() {

    return 0;
}

void allocateMatrix(float *a[], int n, int m){
    ptr = (float *)calloc(m*n, sizeof(float));
    for(int i=0; i<n; i++){
        a[i] = ptr+(m*i);
    }
    return;
}

void addMatrix(int n, int m, float a[n][m], float b[n][m], float *c[]){
    allocateMatrix(c, n, m);

    for(int i=0; i<n; i++){
        for(int j=0; j<m; j++){
            c[i][j] = a[i][j] + b[i][j];
        }
    }
    return;
}

void subtractMatrix(int n, int m, float a[n][m], float b[n][m], float *c[]){
    allocateMatrix(c, n, m);

    for(int i=0; i<n; i++){
        for(int j=0; j<m; j++){
            c[i][j] = a[i][j] - b[i][j];
        }
    }
    return;
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
    return;
}

void multiplymatrixwithscalar(float x, int n, int m, float a[n][m], float *c[]){
    allocateMatrix(c, n, m);
    for(int i=0; i<n; i++){
        for(int j=0; j<m; j++){
            c[i][j] = x * a[i][j];
        }
    }
    return;
}

void transposeMatrix(int n, int m, float a[n][m], float *c[]){
    allocateMatrix(c, m, n);

    for(int i = 0; i < n; i++){
        for(int j = 0; j < m; j++){
            c[j][i] = a[i][j];
        }
    }
    return;
}

void copyMatrix( float *x[], int n, int m, float a[n][m]){
    for(int i=0; i<n; i++){
        for(int j=0; j<m; j++){
            a[i][j] = x[i][j];
        }
    }
    free(ptr);
    return;
}

void copyMatrixtoMatrix(int r, int c, float a[r][c], float b[r][c]){
    for(int i = 0; i < r; i++){
        for(int j = 0; j < c; j++){
            a[i][j] = b[i][j];
        }
    }
    return;
}

int choose (float x1, float x2, float x3, float x4){
    if( fabs(x1) < 0.0001) return x2;
    if(x1 > 0 ) return x3;
    else return x4;
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
    return;
}

void printSep(){
    printf("----------\n");
    return ;
}

void declareMatrix(int row, int column, float temp[], float a[row][column]){
    int k = 0;
    for(int i = 0; i < row; i ++){
        for(int j = 0; j < column; j ++){
            a[i][j] = temp[k];
            k++;
        }
    }
    return;
}

int isInt(float x){
    int n = (int)x;
    double check = x-n;
    if(check>0)
        return 0;
    return 1;
}
