
int adddumbVar(int typeofdumbvar, int varRow, int varColumn);


// This function takes the postfix string from expr() function and process it. Adds the necessary C code to char pointer line:
int processStack(char str[N], char *line, char *lasttoken){
    memset(stack, 0, sizeof(stack));
    memset(typeoftokensinstack, 0, sizeof(typeoftokensinstack));
    currentindexofstack = 0;
    strcpy(index1, "");
    strcpy(index2, "");
    strcpy(token1, "");
    strcpy(token2, "");
    strcpy(token3, "");
    strcpy(token4, "");
    numofindexes = 0;
    stackcur = 0;
    char *p;
    char *stacktoken;
    numstacktokens = 0;
    memset(stacktokens, 0, sizeof(stacktokens[0][0]) * strlen(stacktokens[0]) * TOKENLENGTH);
    // Postfix string is divided into tokens and added to stacktokens array:
    p = separateLine(str, strlen(str), false);
    while( (stacktoken = strsep(&p," ")) != NULL ){
        while( (strcmp(stacktoken, " ") == 0) || (strcmp(stacktoken, "") == 0) || (strcmp(stacktoken, "\n") == 0)) {
            stacktoken = strsep(&p," ");
            if( stacktoken == NULL)
                break;
        }
        if(stacktoken != NULL){
            strcpy(stacktokens[numstacktokens], stacktoken);
            numstacktokens++;
        }
        else break;
    }
    strcat(line, "\t");

    // Until all of the tokens in the stacktokens are processed, the following while loop runs:
    while(stackcur < numstacktokens){
        // If the current token is '*':
        if(strcmp(stacktokens[stackcur] , "*") == 0){
            stackcur++;
            // In all of the cases, 2 tokens will be needed. They will be taken from stack array
            // If there is not enough token, or the tokens' types do not match with the expected type, 0 is returned immediately.
            // If the operator is scalar multiplication:
            if(strcmp(stacktokens[stackcur], "scalar") == 0){
                if(currentindexofstack < 2) return 0;
                strcpy(token2, stack[currentindexofstack]);
                if((!is_integer(token2)) && (typeoftokensinstack[currentindexofstack] != 7) && (typeoftokensinstack[currentindexofstack] != -7)) return 0;
                typeoftokensinstack[currentindexofstack] = 0;
                strcat(stack[currentindexofstack], "");
                currentindexofstack--;
                strcpy(token1, stack[currentindexofstack]);
                if((!is_integer(token1)) && (typeoftokensinstack[currentindexofstack] != 7) && (typeoftokensinstack[currentindexofstack] != -7)) return 0;
                typeoftokensinstack[currentindexofstack] = 0;
                strcat(stack[currentindexofstack], "");
                currentindexofstack--;
                // The result of the operation will be stored in the following dumb variable:
                int idofdumbvar = adddumbVar(-7, 0, 0);
                strcat(line, "\n\tfloat ");
                strcat(line, IDs[idofdumbvar].name);
                strcat(line, " = ");
                strcat(line, token1);
                strcat(line, " * ");
                strcat(line, token2);
                strcat(line, " ; ");
                currentindexofstack++;
                strcpy(stack[currentindexofstack] , IDs[idofdumbvar].name);
                typeoftokensinstack[currentindexofstack] = -7;
            }
                // If the operator is matrix multiplication:
            else if(strcmp(stacktokens[stackcur], "matrix") == 0){
                if(currentindexofstack < 2) return 0;
                strcpy(token2, stack[currentindexofstack]);
                if((typeoftokensinstack[currentindexofstack] == 7) || (typeoftokensinstack[currentindexofstack] == -7)) return 0;
                typeoftokensinstack[currentindexofstack] = 0;
                strcat(stack[currentindexofstack], "");
                currentindexofstack--;
                strcpy(token1, stack[currentindexofstack]);
                if((typeoftokensinstack[currentindexofstack] == 7) || (typeoftokensinstack[currentindexofstack] == -7)) return 0;
                typeoftokensinstack[currentindexofstack] = 0;
                strcat(stack[currentindexofstack], "");
                currentindexofstack--;
                int token1id = isID(token1);
                int token2id = isID(token2);
                if(IDs[token1id].col != IDs[token2id].row) return 0;
                int idofdumbvarptr = adddumbVar(-10, -1, -1);
                // The result of the operation will be stored in the following dumb variable:
                int idofdumbvar = adddumbVar(-9, IDs[token1id].row, IDs[token2id].col);
                strcat(line, "\n\trow = sizeof(");
                strcat(line, token1);
                strcat(line, ")/sizeof(");
                strcat(line, token1);
                strcat(line, "[0]);\n\tcolumn = sizeof(");
                strcat(line, token2);
                strcat(line, "[0])/sizeof(");
                strcat(line, token2);
                strcat(line, "[0][0]);\n\tmiddle = sizeof(");
                strcat(line, token1);
                strcat(line, "[0])/sizeof(");
                strcat(line, token1);
                strcat(line, "[0][0]);\n\tfloat *");
                strcat(line, IDs[idofdumbvarptr].name);
                strcat(line, "[row];\n\tfloat ");
                strcat(line, IDs[idofdumbvar].name);
                strcat(line, "[row][column];\n\tmultiplyMatrix(row, middle, column, ");
                strcat(line, token1);
                strcat(line, " , ");
                strcat(line, token2);
                strcat(line, " , ");
                strcat(line, IDs[idofdumbvarptr].name);
                strcat(line, " );\n\tcopyMatrix( ");
                strcat(line, IDs[idofdumbvarptr].name);
                strcat(line, " , row, column, ");
                strcat(line, IDs[idofdumbvar].name);
                strcat(line, " );");
                currentindexofstack++;
                strcpy(stack[currentindexofstack] , IDs[idofdumbvar].name);
                typeoftokensinstack[currentindexofstack] = -9;
            }
                // If the operator is scalar*matrix multiplication:
            else if(strcmp(stacktokens[stackcur], "scalarmatrix") == 0){
                if(currentindexofstack < 2) return 0;
                strcpy(token2, stack[currentindexofstack]);
                if((typeoftokensinstack[currentindexofstack] != 9) && (typeoftokensinstack[currentindexofstack] != -9)) return 0;
                typeoftokensinstack[currentindexofstack] = 0;
                strcat(stack[currentindexofstack], "");
                currentindexofstack--;
                strcpy(token1, stack[currentindexofstack]);
                if((typeoftokensinstack[currentindexofstack] != 7) && (typeoftokensinstack[currentindexofstack] != -7)) return 0;
                typeoftokensinstack[currentindexofstack] = 0;
                strcat(stack[currentindexofstack], "");
                currentindexofstack--;
                // token1: scalar, token2: matrix
                int matrixID = isID(token2);
                if(matrixID == -1) return 0;
                int idofdumbvarptr = adddumbVar(-10, -1, -1);
                // The result of the operation will be stored in the following dumb variable:
                int idofdumbvar = adddumbVar(-9, IDs[matrixID].row, IDs[matrixID].col);
                // Following lines are added to char pointer line:
                // row = sizeof(x)/sizeof(x[0]);
                // column = sizeof(x[0])/sizeof(x[0][0]);
                // float *y2[row];
                // float y3[row][column];
                // multiplymatrixwithscalar(token1, row, column, x, y2);
                // copyMatrix(y2, row, column, y3);
                strcat(line, "\n\trow = sizeof(");
                strcat(line, token2);
                strcat(line, ")/sizeof(");
                strcat(line, token2);
                strcat(line, "[0]);\n\tcolumn = sizeof(");
                strcat(line, token2);
                strcat(line, "[0])/sizeof(");
                strcat(line, token2);
                strcat(line, "[0][0]);\n\tfloat *");
                strcat(line, IDs[idofdumbvarptr].name);
                strcat(line, "[row];\n\tfloat ");
                strcat(line, IDs[idofdumbvar].name);
                strcat(line, "[row][column];\n\tmultiplymatrixwithscalar(");
                strcat(line, token1);
                strcat(line, " , row, column, ");
                strcat(line, token2);
                strcat(line, ", ");
                strcat(line, IDs[idofdumbvarptr].name);
                strcat(line, " );\n\tcopyMatrix(");
                strcat(line, IDs[idofdumbvarptr].name);
                strcat(line, " , row, column, ");
                strcat(line, IDs[idofdumbvar].name);
                strcat(line, " );");
                currentindexofstack++;
                strcpy(stack[currentindexofstack] , IDs[idofdumbvar].name);
                typeoftokensinstack[currentindexofstack] = -9;

            }
                // If the operator is matrix*scalar multiplication:
            else if(strcmp(stacktokens[stackcur], "matrixscalar") == 0){
                if(currentindexofstack < 2) return 0;
                strcpy(token2, stack[currentindexofstack]);
                if((typeoftokensinstack[currentindexofstack] != 7) && (typeoftokensinstack[currentindexofstack] != -7)) return 0;
                typeoftokensinstack[currentindexofstack] = 0;
                strcat(stack[currentindexofstack], "");
                currentindexofstack--;
                strcpy(token1, stack[currentindexofstack]);
                if((typeoftokensinstack[currentindexofstack] != 9) && (typeoftokensinstack[currentindexofstack] != -9)) return 0;
                typeoftokensinstack[currentindexofstack] = 0;
                strcat(stack[currentindexofstack], "");
                currentindexofstack--;
                // token1: matrix, token2: scalar
                int matrixID = isID(token1);
                if(matrixID == -1) return 0;
                int idofdumbvarptr = adddumbVar(-10, -1, -1);
                // The result of the operation will be stored in the following dumb variable:
                int idofdumbvar = adddumbVar(-9, IDs[matrixID].row, IDs[matrixID].col);
                // Following lines are added to char pointer line:
                // row = sizeof(x)/sizeof(x[0]);
                // column = sizeof(x[0])/sizeof(x[0][0]);
                // float *y2[row];
                // float y3[row][column];
                // multiplymatrixwithscalar(token2, row, column, x, y2);
                // copyMatrix(y2, row, column, y3);

                strcat(line, "\n\trow = sizeof(");
                strcat(line, token1);
                strcat(line, ")/sizeof(");
                strcat(line, token1);
                strcat(line, "[0]);\n\tcolumn = sizeof(");
                strcat(line, token1);
                strcat(line, "[0])/sizeof(");
                strcat(line, token1);
                strcat(line, "[0][0]);\n\tfloat *");
                strcat(line, IDs[idofdumbvarptr].name);
                strcat(line, "[row];\n\tfloat ");
                strcat(line, IDs[idofdumbvar].name);
                strcat(line, "[row][column];\n\tmultiplymatrixwithscalar(");
                strcat(line, token2);
                strcat(line, " , row, column, ");
                strcat(line, token1);
                strcat(line, ", ");
                strcat(line, IDs[idofdumbvarptr].name);
                strcat(line, " );\n\tcopyMatrix(");
                strcat(line, IDs[idofdumbvarptr].name);
                strcat(line, " , row, column, ");
                strcat(line, IDs[idofdumbvar].name);
                strcat(line, " );");
                currentindexofstack++;
                strcpy(stack[currentindexofstack] , IDs[idofdumbvar].name);
                typeoftokensinstack[currentindexofstack] = -9;
            }
        }
        else if(strcmp(stacktokens[stackcur] , "+") == 0){
            // In all of the cases, 2 tokens will be needed. They will be taken from stack array
            stackcur++;
            // If the operator is scalar addition:
            if(strcmp(stacktokens[stackcur], "scalar") == 0){
                if(currentindexofstack < 2) return 0;
                strcpy(token2, stack[currentindexofstack]);
                if((!is_integer(token2)) && (typeoftokensinstack[currentindexofstack] != 7) && (typeoftokensinstack[currentindexofstack] != -7)) return 0;
                typeoftokensinstack[currentindexofstack] = 0;
                strcat(stack[currentindexofstack], "");
                currentindexofstack--;
                strcpy(token1, stack[currentindexofstack]);
                if((!is_integer(token1)) && (typeoftokensinstack[currentindexofstack] != 7) && (typeoftokensinstack[currentindexofstack] != -7)) return 0;
                typeoftokensinstack[currentindexofstack] = 0;
                strcat(stack[currentindexofstack], "");
                currentindexofstack--;
                // The result of the operation will be stored in the following dumb variable:
                int idofdumbvar = adddumbVar(-7, 0, 0);
                strcat(line, "\n\tfloat ");
                strcat(line, IDs[idofdumbvar].name);
                strcat(line, " = ");
                strcat(line, token1);
                strcat(line, " + ");
                strcat(line, token2);
                strcat(line, " ; ");
                currentindexofstack++;
                strcpy(stack[currentindexofstack] , IDs[idofdumbvar].name);
                typeoftokensinstack[currentindexofstack] = -7;
            }
            // If the operator is matrix addition:
            else if(strcmp(stacktokens[stackcur], "matrix") == 0){
                if(currentindexofstack < 2) return 0;
                strcpy(token2, stack[currentindexofstack]);
                if(typeoftokensinstack[currentindexofstack] == 7) return 0;
                typeoftokensinstack[currentindexofstack] = 0;
                strcat(stack[currentindexofstack], "");
                currentindexofstack--;
                strcpy(token1, stack[currentindexofstack]);
                if(typeoftokensinstack[currentindexofstack] == 7) return 0;
                typeoftokensinstack[currentindexofstack] = 0;
                strcat(stack[currentindexofstack], "");
                currentindexofstack--;
                int token1id = isID(token1);
                int token2id = isID(token2);
                if(IDs[token1id].col != IDs[token2id].col) return 0;
                if(IDs[token1id].row != IDs[token2id].row) return 0;
                // Following lines are added to char pointer line:
                // row = sizeof(y)/sizeof(y[0]);
                // column = sizeof(y[0])/sizeof(y[0][0]);
                // float *y2[row];
                // float y3[row][column];
                // addMatrix(row, column, y, x, y2);
                // copyMatrix(y2, row, column, y3);
                int idofdumbvarptr = adddumbVar(-10, -1, -1);
                // The result of the operation will be stored in the following dumb variable:
                int idofdumbvar = adddumbVar(-9, IDs[token1id].row, IDs[token1id].col);
                strcat(line, "\n\trow = sizeof(");
                strcat(line, token1);
                strcat(line, ")/sizeof(");
                strcat(line, token1);
                strcat(line, "[0]);\n\tcolumn = sizeof(");
                strcat(line, token1);
                strcat(line, "[0])/sizeof(");
                strcat(line, token1);
                strcat(line, "[0][0]);\n\tfloat *");
                strcat(line, IDs[idofdumbvarptr].name);
                strcat(line, "[row];\n\tfloat ");
                strcat(line, IDs[idofdumbvar].name);
                strcat(line, "[row][column];\n\taddMatrix(row, column, ");
                strcat(line, token1);
                strcat(line, " , ");
                strcat(line, token2);
                strcat(line, " , ");
                strcat(line, IDs[idofdumbvarptr].name);
                strcat(line, " );\n\tcopyMatrix( ");
                strcat(line, IDs[idofdumbvarptr].name);
                strcat(line, " , row, column, ");
                strcat(line, IDs[idofdumbvar].name);
                strcat(line, " );");
                currentindexofstack++;
                strcpy(stack[currentindexofstack] , IDs[idofdumbvar].name);
                typeoftokensinstack[currentindexofstack] = -9;
            }
        }
        else if(strcmp(stacktokens[stackcur] , "-") == 0){
            // In all of the cases, 2 tokens will be needed. They will be taken from stack array
            stackcur++;
            // If the operator is scalar subtraction:
            if(strcmp(stacktokens[stackcur], "scalar") == 0){
                if(currentindexofstack < 2) return 0;
                strcpy(token2, stack[currentindexofstack]);
                if((!is_integer(token2)) && (typeoftokensinstack[currentindexofstack] != 7) && (typeoftokensinstack[currentindexofstack] != -7)) return 0;
                typeoftokensinstack[currentindexofstack] = 0;
                strcat(stack[currentindexofstack], "");
                currentindexofstack--;
                strcpy(token1, stack[currentindexofstack]);
                if((!is_integer(token1)) && (typeoftokensinstack[currentindexofstack] != 7) && (typeoftokensinstack[currentindexofstack] != -7)) return 0;
                typeoftokensinstack[currentindexofstack] = 0;
                strcat(stack[currentindexofstack], "");
                currentindexofstack--;
                // The result of the operation will be stored in the following dumb variable:
                int idofdumbvar = adddumbVar(-7, 0, 0);
                strcat(line, "\n\tfloat ");
                strcat(line, IDs[idofdumbvar].name);
                strcat(line, " = ");
                strcat(line, token1);
                strcat(line, " - ");
                strcat(line, token2);
                strcat(line, " ; ");
                currentindexofstack++;
                strcpy(stack[currentindexofstack] , IDs[idofdumbvar].name);
                typeoftokensinstack[currentindexofstack] = -7;
            }
            // If the operator is matrix subtraction:
            else if(strcmp(stacktokens[stackcur], "matrix") == 0){
                if(currentindexofstack < 2) return 0;
                strcpy(token2, stack[currentindexofstack]);
                if(typeoftokensinstack[currentindexofstack] == 7) return 0;
                typeoftokensinstack[currentindexofstack] = 0;
                strcat(stack[currentindexofstack], "");
                currentindexofstack--;
                strcpy(token1, stack[currentindexofstack]);
                if(typeoftokensinstack[currentindexofstack] == 7) return 0;
                typeoftokensinstack[currentindexofstack] = 0;
                strcat(stack[currentindexofstack], "");
                currentindexofstack--;
                int token1id = isID(token1);
                int token2id = isID(token2);
                if(IDs[token1id].col != IDs[token2id].col) return 0;
                if(IDs[token1id].row != IDs[token2id].row) return 0;
                // Following lines are added to char pointer line:
                // row = sizeof(y)/sizeof(y[0]);
                // column = sizeof(y[0])/sizeof(y[0][0]);
                // float *y2[row];
                // float y3[row][column];
                // subtractMatrix(row, column, y, x, y2);
                // copyMatrix(y2, row, column, y3);
                int idofdumbvarptr = adddumbVar(-10, -1, -1);
                // The result of the operation will be stored in the following dumb variable:
                int idofdumbvar = adddumbVar(-9, IDs[token1id].row, IDs[token1id].col);
                strcat(line, "\n\trow = sizeof(");
                strcat(line, token1);
                strcat(line, ")/sizeof(");
                strcat(line, token1);
                strcat(line, "[0]);\n\tcolumn = sizeof(");
                strcat(line, token1);
                strcat(line, "[0])/sizeof(");
                strcat(line, token1);
                strcat(line, "[0][0]);\n\tfloat *");
                strcat(line, IDs[idofdumbvarptr].name);
                strcat(line, "[row];\n\tfloat ");
                strcat(line, IDs[idofdumbvar].name);
                strcat(line, "[row][column];\n\tsubtractMatrix(row, column, ");
                strcat(line, token1);
                strcat(line, " , ");
                strcat(line, token2);
                strcat(line, " , ");
                strcat(line, IDs[idofdumbvarptr].name);
                strcat(line, " );\n\tcopyMatrix( ");
                strcat(line, IDs[idofdumbvarptr].name);
                strcat(line, " , row, column, ");
                strcat(line, IDs[idofdumbvar].name);
                strcat(line, " );");
                currentindexofstack++;
                strcpy(stack[currentindexofstack] , IDs[idofdumbvar].name);
                typeoftokensinstack[currentindexofstack] = -9;
            }
        }
        else if(strcmp(stacktokens[stackcur], "sqrt") == 0){
            // 1 token with scalar type is needed:
            if(currentindexofstack < 1) return 0;
            strcpy(token1, stack[currentindexofstack]);
            if((!is_integer(token1)) && (typeoftokensinstack[currentindexofstack] != 7) && (typeoftokensinstack[currentindexofstack] != -7)) return 0;
            typeoftokensinstack[currentindexofstack] = 0;
            strcat(stack[currentindexofstack], "");
            currentindexofstack--;
            // Result will be stored in this dumb variable:
            int idofdumbvar = adddumbVar(-7, 0, 0);
            strcat(line, "\n\tfloat ");
            strcat(line, IDs[idofdumbvar].name);
            strcat(line, " = sqrt( ");
            strcat(line, token1);
            strcat(line, " );");
            currentindexofstack++;
            strcpy(stack[currentindexofstack] , IDs[idofdumbvar].name);
            typeoftokensinstack[currentindexofstack] = -7;
        }
        else if(strcmp(stacktokens[stackcur], "tr") == 0){
            // 1 token is needed:
            if(currentindexofstack < 1) return 0;
            strcpy(token1, stack[currentindexofstack]);
            int typeoftoken1 = typeoftokensinstack[currentindexofstack];
            typeoftokensinstack[currentindexofstack] = 0;
            strcat(stack[currentindexofstack], "");
            currentindexofstack--;
            if((is_integer(token1)) || (typeoftoken1 == 7 || typeoftoken1 == -7) ){
                // type of token is scalar:
                // Following line is added to char pointer line:
                // float id = scalartr(token1);
                int idofdumbvar = adddumbVar(-7, 0, 0);
                strcat(line, "\n\tfloat ");
                strcat(line, IDs[idofdumbvar].name);
                strcat(line, " = scalartr( ");
                strcat(line, token1);
                strcat(line, " ); ");
                currentindexofstack++;
                strcpy(stack[currentindexofstack] , IDs[idofdumbvar].name);
                typeoftokensinstack[currentindexofstack] = -7;
            }
            else if(typeoftoken1 == 9 || typeoftoken1 == -9){
                int token1id = isID(token1);
                // type of token is matrix:
                // Following lines are added to char pointer line:
                // row = sizeof(y3[0])/sizeof(y3[0][0]);
                // column = sizeof(y3)/sizeof(y3[0]);
                // float *y4[row];
                // float y5[row][column];
                // transposeMatrix(column, row, y3, y4);
                // copyMatrix(y4, row, column, y5);
                int idofdumbvarptr = adddumbVar(-10, -1, -1);
                int idofdumbvar = adddumbVar(-9, IDs[token1id].col, IDs[token1id].row);
                strcat(line, "\n\trow = sizeof(");
                strcat(line, token1);
                strcat(line, "[0])/sizeof(");
                strcat(line, token1);
                strcat(line, "[0][0]);\n\tcolumn = sizeof(");
                strcat(line, token1);
                strcat(line, ")/sizeof(");
                strcat(line, token1);
                strcat(line, "[0]);\n\tfloat *");
                strcat(line, IDs[idofdumbvarptr].name);
                strcat(line, "[row];\n\tfloat ");
                strcat(line, IDs[idofdumbvar].name);
                strcat(line, "[row][column];\n\ttransposeMatrix(column, row, ");
                strcat(line, token1);
                strcat(line, " , ");
                strcat(line, IDs[idofdumbvarptr].name);
                strcat(line, " );\n\tcopyMatrix( ");
                strcat(line, IDs[idofdumbvarptr].name);
                strcat(line, " , row, column, ");
                strcat(line, IDs[idofdumbvar].name);
                strcat(line, " );");
                currentindexofstack++;
                strcpy(stack[currentindexofstack] , IDs[idofdumbvar].name);
                typeoftokensinstack[currentindexofstack] = -9;
            }
            else
                return 0;
        }
        else if(strcmp(stacktokens[stackcur], "choose") == 0){
            // 4 tokens with types scalar are needed. If this do not match, return 0.
            if(currentindexofstack < 4) return 0;
            strcpy(token4, stack[currentindexofstack]);
            if((!is_integer(token4)) && (typeoftokensinstack[currentindexofstack] != 7) && (typeoftokensinstack[currentindexofstack] != -7) ) return 0;
            typeoftokensinstack[currentindexofstack] = 0;
            strcat(stack[currentindexofstack], "");
            currentindexofstack--;
            strcpy(token3, stack[currentindexofstack]);
            if((!is_integer(token3)) && (typeoftokensinstack[currentindexofstack] != 7) && (typeoftokensinstack[currentindexofstack] != -7) ) return 0;
            typeoftokensinstack[currentindexofstack] = 0;
            strcat(stack[currentindexofstack], "");
            currentindexofstack--;
            strcpy(token2, stack[currentindexofstack]);
            if((!is_integer(token2)) && (typeoftokensinstack[currentindexofstack] != 7) && (typeoftokensinstack[currentindexofstack] != -7) ) return 0;
            typeoftokensinstack[currentindexofstack] = 0;
            strcat(stack[currentindexofstack], "");
            currentindexofstack--;
            strcpy(token1, stack[currentindexofstack]);
            if((!is_integer(token1)) && (typeoftokensinstack[currentindexofstack] != 7) && (typeoftokensinstack[currentindexofstack] != -7) ) return 0;
            typeoftokensinstack[currentindexofstack] = 0;
            strcat(stack[currentindexofstack], "");
            currentindexofstack--;
            int idofdumbvar = adddumbVar(-7, 0, 0);
            // Following line is added to char pointer line:
            // float dumb = choose (token1, token2, token3, token4);
            strcat(line, "\n\tfloat ");
            strcat(line, IDs[idofdumbvar].name);
            strcat(line, " = choose( ");
            strcat(line, token1);
            strcat(line, " , ");
            strcat(line, token2);
            strcat(line, " , ");
            strcat(line, token3);
            strcat(line, " , ");
            strcat(line, token4);
            strcat(line, " ); ");
            currentindexofstack++;
            strcpy(stack[currentindexofstack] , IDs[idofdumbvar].name);
            typeoftokensinstack[currentindexofstack] = -7;

        }
        else if(strcmp(stacktokens[stackcur], "[") == 0){
            // Index is expected, it is already in index1 or index2:
            stackcur++;
            if(strcmp(stacktokens[stackcur], "]") != 0) return 0;
            if(typeoftokensinstack[currentindexofstack] != 7 && typeoftokensinstack[currentindexofstack] != -7) return 0;
            strcpy(token1, stack[currentindexofstack]);
            typeoftokensinstack[currentindexofstack] = 0;
            strcat(stack[currentindexofstack], "");
            currentindexofstack--;
            if(numofindexes == 0){
                // No index is waiting in line, add this new index to index1:
                strcpy(index1, "[");
                strcat(index1, token1);
                strcat(index1, " -1]");
                numofindexes++;
            }
            else if(numofindexes == 1){
                // 1 index is already waiting in index1 variable, add this new index to index2:
                strcpy(index2, "[");
                strcat(index2, token1);
                strcat(index2, " -1]");
                numofindexes++;
            }
        }
        else if( is_integer(stacktokens[stackcur])){
            // Token is integer, just add id to stack array:
            currentindexofstack++;
            strcpy(stack[currentindexofstack] , stacktokens[stackcur] );
            typeoftokensinstack[currentindexofstack] = 7;
        }
        else{
            int idcheck = isID(stacktokens[stackcur]);
            if(idcheck == -1) return 0;
            else if(IDs[idcheck].type== 7 || IDs[idcheck].type== -7){
                // Token is a scalar id, just add id to stack array:
                currentindexofstack++;
                strcpy(stack[currentindexofstack] , stacktokens[stackcur] );
                typeoftokensinstack[currentindexofstack] = 7;
            }
            else if(IDs[idcheck].type== 8 || IDs[idcheck].type== -8){
                // Token is a vector id:
                if(numofindexes == 0){
                    // No index is waiting for an id, add vector id to stack:
                    currentindexofstack++;
                    strcpy(stack[currentindexofstack] , stacktokens[stackcur] );
                    typeoftokensinstack[currentindexofstack] = 9;
                }
                else if(numofindexes == 1){
                    // 1 index is waiting for an id, add vector with its index to stack, as scalar:
                    currentindexofstack++;
                    strcpy(stack[currentindexofstack] , stacktokens[stackcur] );
                    strcat(stack[currentindexofstack] , index1);
                    strcat(stack[currentindexofstack] , "[0]");
                    typeoftokensinstack[currentindexofstack] = 7;
                    strcpy(index1, "");
                    numofindexes = 0;
                }
            }
            else if(IDs[idcheck].type== 9 || IDs[idcheck].type== -9){
                // Token is a matrix id:
                if(numofindexes == 0){
                    // No index is waiting for an id, add matrix id to stack:
                    currentindexofstack++;
                    strcpy(stack[currentindexofstack] , stacktokens[stackcur] );
                    typeoftokensinstack[currentindexofstack] = 9;
                }
                else if(numofindexes == 1){
                    // Matrix id cannot be accessed by just one index, Error:
                    return 0;
                }
                else if(numofindexes == 2){
                    // 2 indexes are waiting for an id, add matrix with its indexs to stack, as scalar:
                    currentindexofstack++;
                    strcpy(stack[currentindexofstack] , stacktokens[stackcur] );
                    strcat(stack[currentindexofstack] , index1);
                    strcat(stack[currentindexofstack] , index2);
                    typeoftokensinstack[currentindexofstack] = 7;
                    strcpy(index1, "");
                    strcpy(index2, "");
                    numofindexes = 0;
                }
            }
            else return 0;
        }
        stackcur++;
    }
    // Add last token in the stack to lasttoken:
    strcpy(lasttoken, stack[currentindexofstack]);
    return 1;
}



int moreterms(char *str){
    char str1[N], str2[N], str3[N] ;
    int result = 1;
    str1[0] = str2[0] = str3[0] = '\0' ;
    char func[N] ;
    func[0] = '\0' ;

    if ( (strcmp(tokens[cur],"+") == 0 ) || (strcmp(tokens[cur],"-") == 0 ) ) {
        isprevtermscalar = istermscalar;
        strcpy(func,tokens[cur]) ;
        cur++ ;
        int termvalue =  term(str2);
        if(isprevtermscalar && istermscalar){
            strcpy(str1, " ") ;
            strcat(str1, func);
            strcat(str1, "scalar ");
        } else if(!isprevfactorscalar && !isfactorscalar){
            strcpy(str1, " ") ;
            strcat(str1, func);
            strcat(str1, "matrix ");
        } else{
            return 0;
        }
        isprevtermscalar = istermscalar;
        int moretermvalue = moreterms(str3);
        if (termvalue == 0 || moretermvalue == 0) {
            return(0) ;
        }
        else if(moretermvalue == 1){
            result = termvalue;
        }
        else if(moretermvalue == 2){
            if(termvalue != 2) result = 0;
            else result = 2;
        }
        if(moretermvalue == 3){
            if(termvalue != 3) return 0;
            else result = 3;
        }
    }
    // str1: operator,   str2: term,   str3: moreterms
    strcat(str2,str1) ;
    strcat(str2,str3) ;
    strcpy(str,str2) ;
    return(result) ;
}



int morefactors(char *str)
{
    char str1[N], str2[N], str3[N] ;
    int result = 1;
    str1[0] = str2[0] = str3[0] = '\0' ;
    if ( strcmp(tokens[cur],"*") == 0 ) {
        isprevfactorscalar = isfactorscalar;

        cur++ ;
        int factorvalue = factor(str2);
        if(isprevfactorscalar && isfactorscalar){
            strcpy(str1, " *scalar ") ;
        } else if(isprevfactorscalar && !isfactorscalar){
            strcpy(str1, " *scalarmatrix ") ;
        } else if(!isprevfactorscalar && isfactorscalar){
            strcpy(str1, " *matrixscalar ") ;
        } else if(!isprevfactorscalar && !isfactorscalar){
            strcpy(str1, " *matrix ") ;
        }
        isprevfactorscalar = isfactorscalar;
        int morefactorvalue = morefactors(str3);
        if (factorvalue == 0 || morefactorvalue == 0) {
            result = 0;
        }
        else if(morefactorvalue == 1 || morefactorvalue == 2){
            result = factorvalue;
        }
        else if(morefactorvalue == 3){
            result = morefactorvalue;
        }
    }
    // str1: operator,   str2: factor,   str3: morefactors
    strcat(str2,str1) ;
    strcat(str2,str3) ;
    strcpy(str,str2) ;
    return(result) ;
}



int adddumbVar(int typeofdumbvar, int varRow, int varColumn){
    // typeofdumbvar: scalar: -7, vector: -8 , matrix: -9, pointer = -10;
    strcpy(dumbvariablename, "buse");
    sprintf(dumbvariablenum, "%d", dumbvariableid);
    dumbvariableid++;
    strcat(dumbvariablename, dumbvariablenum);
    while(isID(dumbvariablename) != -1){
        strcpy(dumbvariablename, "buse");
        sprintf(dumbvariablenum, "%d", dumbvariableid);
        dumbvariableid++;
        strcat(dumbvariablename, dumbvariablenum);
    }
    struct ID a = { .col= varColumn, .row = varRow, .type = typeofdumbvar};
    strcpy(a.name, dumbvariablename);
    IDs[currentID] = a;
    currentID++;
    return (currentID-1);
}

