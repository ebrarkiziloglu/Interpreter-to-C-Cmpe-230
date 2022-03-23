#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdlib.h>

#define MAXDIMENSION 10


float* transposeMatrix(int r, int c, float matrix[r][c]);

int main(){
  
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
