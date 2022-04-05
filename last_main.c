int main (int argc,char *argv[]) {

    strcat(executableBlock, "int main()\n{\n");
    char *q;
    char *token ;
    FILE *fp;
    FILE *wp;

    char buff[256];
    const char* line;
    /* Open file for reading Filename is given on the command line */
    if (argc != 3) {
        printf("Give filename as command line argument\n") ;
        return(1);
    }
    fp = fopen(argv[1], "r");
    wp = fopen(argv[2], "w");

    if(fp == NULL) {
        printf("Cannot open %s\n",argv[1]);
        return(1);
    }

    while( fgets(buff,256,fp) != NULL ) {
        q = separateLine(buff, strlen(buff), false) ;
        lineNum++;
        numtokens = 0;
        memset(tokens, 0, sizeof(tokens[0][0]) * strlen(tokens[0]) * TOKENLENGTH);
        cexpr[0] = '\0';
        cur = 0;
        while( (token = strsep(&q," ")) != NULL ){
            while( (strcmp(token, " ") == 0) || (strcmp(token, "") == 0) || (strcmp(token, "\n") == 0)) {
                token = strsep(&q," ");
                if( token == NULL)
                    break;
            }
            if(token != NULL){
                if((strcmp(token, "#") == 0)){
                    break;
                }
                strcpy(tokens[numtokens], token);
                numtokens++;
            }
            else break;
        }

        if(numtokens != 0){
            sprintf(tokens[numtokens],"$") ;
            numtokens++ ;
            char str[N] = "";
            int typeOfLine = process(str, numtokens, cexpr);
            if( typeOfLine ==0){
                printf("ERROR LINE %d\n", lineNum);
//                return 0;
            }
            if( typeOfLine == 1){
                strcat(executableBlock, "\t");
                strcat(executableBlock, cexpr);
                // strcat(executableBlock, "\n");
            }

        }
    }

    if(isInForLoop1==true || isInForLoop2==true){
        printf("ERROR! DIDN'T CLOSE THE FOR LOOP\n");
        return 0;
    }
    strcat(definitionBlock, "\n");
    strcat(executableBlock, "\treturn (0);\n}\n");

    strcat(funcDeclarationBlock, "#include <stdio.h>\n");
    strcat(funcDeclarationBlock, "#include <stdlib.h>\n");
    strcat(funcDeclarationBlock, "#include <math.h>\n\n");
    strcat(funcDeclarationBlock, "void allocateMatrix(float *A[], int n, int m);\n");
    strcat(funcDeclarationBlock, "void addMatrix(int n, int m, float a[n][m], float b[n][m], float *c[]);\n");
    strcat(funcDeclarationBlock, "void subtractMatrix(int n, int m, float a[n][m], float b[n][m], float *c[]);\n");
    strcat(funcDeclarationBlock, "void multiplyMatrix(int n, int k, int m, float a[n][m], float b[m][k], float *c[]);\n");
    strcat(funcDeclarationBlock, "void multiplymatrixwithscalar(float x, int n, int m, float a[n][m], float *c[]);\n");
    strcat(funcDeclarationBlock, "void transposeMatrix(int n, int m, float a[n][m], float *c[]);\n");
    strcat(funcDeclarationBlock, "float scalartr(float x);\n");
    strcat(funcDeclarationBlock, "void copyMatrix( float *x[], int n, int m, float a[n][m]);\n");
    strcat(funcDeclarationBlock, "void copyMatrixtoMatrix(int r, int c, float a[r][c], float b[r][c]);\n");
    strcat(funcDeclarationBlock, "int choose (float x1, float x2, float x3, float x4);\n");
    strcat(funcDeclarationBlock, "void printMatrix(int r, int c, float matrix[r][c]);\n");
    strcat(funcDeclarationBlock, "void printSep();\n");
    strcat(funcDeclarationBlock, "void declareMatrix(int row, int column, float temp[], float a[row][column]);\n");
    strcat(funcDeclarationBlock, "int isInt(float x);\n");
    strcat(funcDeclarationBlock, "int row = 0, column = 0, middle = 0;\n");
    strcat(funcDeclarationBlock, "float *ptr;\n\n");

    strcat(funcDefinitionBlock, "void allocateMatrix(float *a[], int n, int m){\n"
                   "    ptr = (float *)calloc(m*n, sizeof(float));\n"
                   "    for(int i=0; i<n; i++){\n"
                   "        a[i] = ptr+(m*i);\n"
                   "    }\n"
                   "}\n\n");
    strcat(funcDefinitionBlock, "void addMatrix(int n, int m, float a[n][m], float b[n][m], float *c[]){\n"
                                "    for(int i=0; i<n; i++){\n"
                                "        for(int j=0; j<m; j++){\n"
                                "            c[i][j] = a[i][j] + b[i][j];\n"
                                "        }\n"
                                "    }\n"
                                "}\n\n");
    strcat(funcDefinitionBlock, "void subtractMatrix(int n, int m, float a[n][m], float b[n][m], float *c[]){\n"
                   "    for(int i=0; i<n; i++){\n"
                   "        for(int j=0; j<m; j++){\n"
                   "            c[i][j] = a[i][j] - b[i][j];\n"
                   "        }\n"
                   "    }\n"
                   "}\n\n");
    strcat(funcDefinitionBlock, "void multiplyMatrix(int n, int k, int m, float a[n][k], float b[k][m], float *c[]){\n"
                   "    for(int i=0; i<n; i++){\n"
                   "        for(int j=0; j<m; j++){\n"
                   "            c[i][j] = 0;\n"
                   "            for(int l=0; l<k; l++){\n"
                   "                c[i][j] += a[i][l]*b[l][j];\n"
                   "            }\n"
                   "        }\n"
                   "    }\n"
                   "}\n\n");
    strcat(funcDefinitionBlock, "void multiplymatrixwithscalar(float x, int n, int m, float a[n][m], float *c[]){\n"
                                "    allocateMatrix(c, n, m);\n"
                                "    for(int i=0; i<n; i++){\n"
                                "        for(int j=0; j<m; j++){\n"
                                "            c[i][j] = x * a[i][j];\n"
                                "        }\n"
                                "    }\n"
                                "    return;\n"
                                "}\n\n");
    strcat(funcDefinitionBlock, "void transposeMatrix(int n, int m, float a[n][m], float *c[]){\n"
                                "    allocateMatrix(c, m, n);\n"
                                "    for(int i = 0; i < n; i++){\n"
                                "        for(int j = 0; j < m; j++){\n"
                                "            c[j][i] = a[i][j];\n"
                                "        }\n"
                                "    }\n"
                                "}\n\n");
    strcat(funcDefinitionBlock, "float scalartr(float x){\n"
                                "    return x;\n"
                                "}\n\n");
    strcat(funcDefinitionBlock, "void copyMatrix( float *x[], int n, int m, float a[n][m]){\n"
                   "    for(int i=0; i<n; i++){\n"
                   "        for(int j=0; j<m; j++){\n"
                   "            a[i][j] = x[i][j];\n"
                   "        }\n"
                   "    }\n"
                   "}\n\n");
    strcat(funcDefinitionBlock, "void copyMatrixtoMatrix(int r, int c, float a[r][c], float b[r][c]){\n"
                                "    for(int i = 0; i < r; i++){\n"
                                "        for(int j = 0; j < c; j++){\n"
                                "            a[i][j] = b[i][j];\n"
                                "        }\n"
                                "    }\n"
                                "    return;\n"
                                "}\n\n");
    strcat(funcDefinitionBlock, "int choose (float x1, float x2, float x3, float x4){\n"
                                "    if( fabs(x1) < 0.0001) return x2;\n"
                                "    if(x1 > 0 ) return x3;\n"
                                "    else return x4;\n"
                                "}\n\n");
    strcat(funcDefinitionBlock, "void printMatrix(int r, int c, float matrix[r][c]){\n"
                                "    float temp;\n"
                                "    for(int i=0; i<r; i++){\n"
                                "        for(int j=0; j<c; j++){\n"
                                "            temp = matrix[i][j];\n"
                                "            if(isInt(temp)==1)\n"
                                "                printf(\"%d \", (int)temp);\n"
                                "            else\n"
                                "                printf(\"%f \", temp);\n"
                                "        }\n"
                                "        printf(\"\\n\");\n"
                                "    }\n"
                                "}\n\n");
    strcat(funcDefinitionBlock, "void printSep(){\n"
                                "    printf(\"------------\\n\");\n"
                                "    return ;\n"
                                "}\n\n");
    strcat(funcDefinitionBlock, "void declareMatrix(int row, int column, float temp[], float a[row][column]){\n"
                   "    int k = 0;\n"
                   "    for(int i = 0; i < row; i ++){\n"
                   "        for(int j = 0; j < column; j ++){\n"
                   "            a[i][j] = temp[k];\n"
                   "            k++;\n"
                   "        }\n"
                   "    }\n"
                   "}\n\n");
    strcat(funcDefinitionBlock, "int isInt(float x){\n"
                   "    int n = (int)x;\n"
                   "    double check = x-n;\n"
                   "    if(check>0)\n"
                   "        return 0;\n"
                   "    return 1;\n"
                   "}\n\n");

    fprintf(wp, "%s", funcDeclarationBlock);
    fprintf(wp, "%s", definitionBlock);
    fprintf(wp, "%s", executableBlock);
    fprintf(wp, "%s", funcDefinitionBlock);

    fclose(wp);
    return(0);
}
