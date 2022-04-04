#include <stdio.h>
#include <stdlib.h>

void allocateMatrix(float *A[], int n, int m);
void subtractMatrix(float *A[], float *B[], float *C[], int n, int m);
void multiplyMatrix(float *A[], float *B[], float *C[], int n, int m);
void copyMatrix( float *x[], int n, int m, float a[n][m]);
void addMatrix(int n, int m, float a[n][m], float b[n][m], float *c[]);

float A[3][4], B[3][4], C[3][4], D[3][4], K[3][4];

int main() {
    int row = 3;
    int column = 4;

    float *E[3], *res[3];
    allocateMatrix(E, row, column);
    allocateMatrix(res, row, column);

    float k=1;
    for(int i=0; i<row; i++){
        for(int j=0; j<column; j++){
            A[i][j] = k;
            B[i][j] = k;
            C[i][j] = k;
            k++;
        }
    }

    for(int i=0; i<row; i++){
        for(int j=0; j<column; j++){
            printf("%f ", A[i][j]);
        }
        printf("\n");
    }

    addMatrix(row, column, A, B, E);
    printf("\nE = A+B after addition\n");
    for(int i=0; i<row; i++){
        for(int j=0; j<column; j++){
            printf("%f ", E[i][j]);
        }
        printf("\n");
    }
    copyMatrix(E, row, column, D);
    printf("\nD = E after copy: \n");
    for(int i=0; i<row; i++){
        for(int j=0; j<column; j++){
            printf("%f ", D[i][j]);
        }
        printf("\n");
    }

    addMatrix(row, column, D, C, res);
    printf("\nres = D+B after addition\n");
    for(int i=0; i<row; i++){
        for(int j=0; j<column; j++){
            printf("%f ", res[i][j]);
        }
        printf("\n");
    }

    copyMatrix(res, row, column, K);
    printf("\n[3][4] global result:\n");
    for(int i=0; i<row; i++){
        for(int j=0; j<column; j++){
            printf("%f ", K[i][j]);
        }
        printf("\n");
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

void multiplyMatrix(int n, int m, float a[n][m], float b[n][m], float *c[]){
    for(int i=0; i<n; i++){
        for(int j=0; j<m; j++){
            c[i][j] = 0;
            for(int k=0; k<n; k++){
                c[i][j] += a[i][k]*b[k][j];
            }
        }
    }
}
