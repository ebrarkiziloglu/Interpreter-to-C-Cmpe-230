#include <stdio.h>
#include <stdlib.h>

void allocateMatrix(float *A[], int n, int m);
void subtractMatrix(int n, int m, float a[n][m], float b[n][m], float *c[]);
void multiplyMatrix(int n, int k, int m, float a[n][m], float b[m][k], float *c[]);
void copyMatrix( float *x[], int n, int m, float a[n][m]);
void addMatrix(int n, int m, float a[n][m], float b[n][m], float *c[]);
void declareMatrix(int row, int column, float temp[], float a[row][column]);
void printMatrix(int r, int c, float matrix[r][c]);
int isInt(float x);

float i;
float n;
float x[2][1];
float y[2][1];
float A[2][2];
float B[2][2];


int main() {

    n = 10;
    float x1[2] = {1, 1};
    declareMatrix(2, 1, x1, x);
    float x11[4] = { 1, 1, 1, 0};
    declareMatrix(2, 2, x11, A);
    float x111[4] = { 1, 0, 0, 1};
    declareMatrix(2, 2, x111, B);

    printMatrix(2, 1, x);


    for(int i = 1; i <= n; i+=1){
        float *y1[sizeof(A)/sizeof(A[0])];
        allocateMatrix(y1, sizeof(A)/sizeof(A[0]), sizeof(B[0])/sizeof(B[0][0]));
        multiplyMatrix(2, 2, 2, A, B, y1);
        copyMatrix(y1, 2, 2, B);

        float *y11[sizeof(B)/sizeof(B[0])];
        allocateMatrix(y11, sizeof(B)/sizeof(B[0]), sizeof(x[0])/sizeof(x[0][0]));
        multiplyMatrix(2, 2, 1, B, x, y11);
        copyMatrix(y11, 2, 1, y);
        printf("%f\n", y[0][0]);
    }

    return 0;
}

void allocateMatrix(float *a[], int n, int m){
    float *ptr = (float *)calloc(m*n, sizeof(float));
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
            for(int k=0; k<n; k++){
                c[i][j] += a[i][k]*b[k][j];
            }
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
