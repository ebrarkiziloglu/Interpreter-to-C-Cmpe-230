#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdlib.h>

#define MAXTOKENS   100
#define TOKENLENGTH 30
#define N  500
#define MAXIDS 50

char *separateLine(char line1[], int length, bool nw);
int process(char *str, int numTokens, char *res);
int defineVariable(char *str);
int assign(int numTokens, char* res, int equalIndex);
int processStack(char str[N], char *line, char *lasttoken);
int isAssign();
int  expr(char *) ;
int  term(char *) ;
int  moreterms(char *) ;
int  factor(char *) ;
int  morefactors(char *) ;
int  is_integer(char *) ;
int isNumber(char s[]);
int isID(char *s);
int findComma();
int findColon();
void editVarName(char *old, char *new);
int isValidVarName(char* name);
int adddumbVar(int typeofdumbvar, int varRow, int varColumn);
void addFunctionDefinitions();
void addFunctionDeclarations();
char *strsep(char **stringp, const char *delim);

// to store and keep track of tokens from the input file:
char tokens[MAXTOKENS][TOKENLENGTH] ;
int numtokens;
int cur = 0 ;
// to store and keep track of tokens in the postfix form of the line taken from the input:
char stacktokens[MAXTOKENS][TOKENLENGTH] ;
int numstacktokens = 0;
int stackcur = 0;
// to store the tokens during the process of postfix form:
int typeoftokensinstack[MAXTOKENS];    // scalar: 7 vector: 8 matrix: 9
char stack[MAXTOKENS][TOKENLENGTH];
int currentindexofstack = 0;
char token1[TOKENLENGTH] = "";
char token2[TOKENLENGTH] = "";
char token3[TOKENLENGTH] = "";
char token4[TOKENLENGTH] = "";
char index1[TOKENLENGTH] = "";
char index2[TOKENLENGTH] = "";
int numofindexes;
char lasttoken[TOKENLENGTH];        // At the end of postfix process, the last result token is stored here.
// Afterwards, this is assigned to the LEFT-HAND-SIDE of the assignement line from the input file.
// to add some intermediate variables:
int dumbvariableid = 0;
char dumbvariablenum[N];
char dumbvariablename[N];
// to store the produced lines of expressions to be written in the output file:
char cexpr[10*N];

// to store the lines to be written in the output C file:
char definitionBlock[40*N] = "";
char executableBlock[40*N] = "";
char funcDeclarationBlock[40*N] = "";
char funcDefinitionBlock[40*N] = "";

// to store and keep track of IDs defined so far:
struct ID{
    char name[20];
    int row;        //these dimensions can be 0 for scalars
    int col;
    //type=> scalar:7 vector:8 matrix:9 dumbscalar: -1 dumbmatrix: -2
    int type;
};
struct ID IDs[MAXIDS] ;
int currentID = 0;

int maxDimension = 0;       // to keep track of the maximum dimension defined in the input file:
int lineNum = 0;            // to keep track of the number of line processed from the input file
char temp_name[N];          // I DON'T KNOW what this does :D

// To keep track of for loops:
bool isInForLoop1 = false;
bool isInForLoop2 = false;

// To keep track of the type compatibility during the conversion from infix to postfix:
bool isfactorscalar = false;
bool isprevfactorscalar = false;
bool istermscalar = false;
bool isprevtermscalar = false;

int main (int argc,char *argv[]) {
    strcat(executableBlock, "int main()\n{\n");
    memset(IDs, 0, sizeof(IDs));
    char *q;
    char *token ;
    FILE *fp;
    FILE *wp;
    char buff[256];
    const char* line;
    /* Open file for reading Filename is given on the command line */
    if (argc != 2) {
        printf("Give filename as command line argument\n") ;
        return(1);
    }
    fp = fopen(argv[1], "r");

    if(fp == NULL) {
        printf("Cannot open %s\n",argv[1]);
        return(1);
    }

    // Read line by line from .mat file
    while( fgets(buff,256,fp) != NULL ) {
        // Following function call adds white spaces to the line read
        q = separateLine(buff, strlen(buff), false) ;
        lineNum++;
        numtokens = 0;
        memset(tokens, 0, sizeof(tokens[0][0]) * strlen(tokens[0]) * TOKENLENGTH);
        cexpr[0] = '\0';
        cur = 0;
        // Line is separated into tokens and tokens are added to tokens array one-by-one
        while( (token = strsep(&q," ")) != NULL ){
            while( (strcmp(token, " ") == 0) || (strcmp(token, "") == 0) || (strcmp(token, "\n") == 0)) {
                token = strsep(&q," ");
                if( token == NULL)
                    break;
            }
            if(token != NULL){
                // If # token is seen, stops reading the rest of the line
                if((strcmp(token, "#") == 0)){
                    break;
                }
                strcpy(tokens[numtokens], token);
                numtokens++;
            }
            else break;
        }

        // If there is any token in the line:
        if(numtokens != 0){
            sprintf(tokens[numtokens],"$") ;
            numtokens++ ;
            char str[N] = "";
            // Following call processes the line given, according to its type:
            int typeOfLine = process(str, numtokens, cexpr);
            if( typeOfLine ==0){
                // If the value is 0, there has to be error in .mat file
                printf("Error (Line %d)\n", lineNum);
                return 0;
            }
            else if( typeOfLine == 1){
                // If the value is 1, the line is an executable statement
                // The line is added to executableBlock string
                strcat(executableBlock, "\t");
                strcat(executableBlock, cexpr);
                strcat(executableBlock, "\n");
            }
            else if( typeOfLine == 2){
                // If the value is 1, the line is an definition statement
                // The line is added to definitionBlock string
                strcat(definitionBlock, cexpr);
                strcat(definitionBlock, "\n");
            }

        }
    }

    if(isInForLoop1==true || isInForLoop2==true){
        // If a for loop is left open after all of the file is read: give error:
        printf("Error (Line %d)\n", lineNum);
        return 0;
    }
    strcat(definitionBlock, "\n");
    strcat(executableBlock, "\treturn (0);\n}\n");
    addFunctionDeclarations();
    addFunctionDefinitions();

    // Output .c file's path is created by the input .mat file's path:
    char outputPath[strlen(argv[1])];
    char *position = strstr(argv[1], ".mat");
    strncpy(outputPath, argv[1], position - argv[1]);
    outputPath[position - argv[1]] = '\0';
    strcat(outputPath, ".c");
    // Output .c file is created:
    wp = fopen(outputPath, "w");
    // Respectively, headers and function definitions are written to .c file:
    fprintf(wp, "%s", funcDeclarationBlock);
    // Definiton statements for global variables are written to .c file:
    fprintf(wp, "%s", definitionBlock);
    // With main function, executable statements are written to .c file:
    fprintf(wp, "%s", executableBlock);
    // Function bodies are written to .c file:
    fprintf(wp, "%s", funcDefinitionBlock);
    fclose(wp);
    fclose(fp);
    return(0);
}

// This function takes the string line and by adding the necessary white spaces, facilitates to divide string into tokens.
char *separateLine(char line[], int length, bool nw){
    if(strcmp(line, "\n") == 0) return NULL;
    static char result[256] = "";
    strcpy(result, "");
    for(int i = 0; i < length; i++) {
        if (!(isalnum(line[i]) | (line[i]=='.') | (line[i]=='_'))) {
            strcat(result, " ");
            nw = true;
        } else {
            if (nw) strcat(result, " ");
            nw = false;
        }
        strncat(result, &line[i], 1);
    }
    return result;
}

// a line can be a definition, a print / printsep / for statements or an assignment.
int process(char *str, int numTokens, char *res){

    char str1[N], str2[N], str3[N], str4[N], str5[N], str6[N];
    str1[0] = '\0';
    str2[0] = '\0';
    str3[0] = '\0';
    str4[0] = '\0';
    str5[0] = '\0';
    str6[0] = '\0';

    // checks whether the line is an assignment line:
    int assignment = isAssign();

    int exp1val, exp2val, exp3val, exp4val, exp5val, exp6val;

    // checks whether the line is a variable definition line:
    if(strcmp(tokens[cur], "scalar")==0 || strcmp(tokens[cur], "vector")==0 || strcmp(tokens[cur], "matrix")==0){
        if( defineVariable(res) == 0 ) return 0;
        return 2;
    }
    // checks whether the line is a print line:
    if(strcmp(tokens[cur], "print")==0){
        cur++;
        if(strcmp(tokens[cur], "(")!=0){
            return 0;
        }
        cur++;

        int idCheck = isID(tokens[cur]);
        cur++;
        char row[10], col[10];
        row[0] = '\0';
        col[0] = '\0';

        if(idCheck==-1){
            return 0;
        }
        if(IDs[idCheck].type==7){
            // scalar print
            strcat(res, "printScalar(");
            strcat(res, IDs[idCheck].name);
            strcat(res, ");");

        }else if(IDs[idCheck].type==8){
            // vector type id:
            if(strcmp(tokens[cur],"[")==0){
                // element of vector, scalar print:
                cur++;
                strcat(res, "printScalar(");
                strcat(res, IDs[idCheck].name);
                strcat(res, "[(int)");
                strcat(res, tokens[cur]);
                cur+=2;
                strcat(res, "-1][0]);");
            }else{
                // matrix print:
                strcat(res, "printMatrix(");
                sprintf(row, "%d", IDs[idCheck].row);
                strcat(res, row);
                strcat(res, ", ");
                strcat(res, "1");
                strcat(res, ", ");
                strcat(res, IDs[idCheck].name);
                strcat(res, ");");
            }
        }else{
            // matrix print:
            if(strcmp(tokens[cur], "[")!=0) {
                // matrix print:
                strcat(res, "printMatrix(");
                sprintf(row, "%d", IDs[idCheck].row);
                strcat(res, row);
                strcat(res, ", ");
                sprintf(col, "%d", IDs[idCheck].col);
                strcat(res, col);
                strcat(res, ", ");
                strcat(res, IDs[idCheck].name);
                strcat(res, ");");
            }else if(strcmp(tokens[cur], "[")==0 && strcmp(tokens[cur+2], ",")==0){
                // element of matrix, scalar print:
                cur++;
                strcat(res, "printScalar(");
                strcat(res, IDs[idCheck].name);
                strcat(res, "[(int)");
                strcat(res, tokens[cur]);
                cur+=2;
                strcat(res, "-1][(int)");
                strcat(res, tokens[cur]);
                cur+=2;
                strcat(res, "-1]);");
            }else{
                // matrix print:
                strcat(res, "printMatrix(");
                strcat(res, "1");
                strcat(res, ", ");
                sprintf(col, "%d", IDs[idCheck].col);
                strcat(res, col);
                strcat(res, ", ");
                strcat(res, IDs[idCheck].name);
                strcat(res, ");");
            }
        }

        if(strcmp(tokens[cur],")")!=0){
            return 0;
        }
        cur++;
        if(cur!=numTokens-1){
            return 0;
        }
        strcat(res, "\n");
        return 1;
    }

        // printsep statement:
    else if(strcmp(tokens[cur], "printsep")==0){
        cur++;
        if(strcmp(tokens[cur],"(")!=0)
            return 0;
        cur++;
        if(strcmp(tokens[cur],")")!=0)
            return 0;
        cur++;
        strcat(res, "printSep();\n");
        return 1;
    }

        // for statement:
    else if(strcmp(tokens[cur], "for")==0){
        char forStr[N] = "";
        char processStr1[N], processStr2[N], processStr3[N], processStr4[N], processStr5[N], processStr6[N];
        processStr1[0] = '\0';
        processStr2[0] = '\0';
        processStr3[0] = '\0';
        processStr4[0] = '\0';
        processStr5[0] = '\0';
        processStr6[0] = '\0';
        cur++;
        if(strcmp(tokens[cur],"(")!=0){
            return 0;
        }
        cur++;
        int checkId;
        char forVarName1[30], forVarName2[30];
        forVarName1[0] = '\0';
        forVarName2[0] = '\0';
        int colonIndex;
        int commaIndex;
        checkId = isID(tokens[cur]);
        strcat(forVarName1, tokens[cur]);
        if (checkId == -1) {
            struct ID a = {.name=forVarName1[0], .col=0, .row=0, .type=7};
            int i=1;
            while(forVarName1[i]!='\0'){
                a.name[i] = forVarName1[i];
                i++;
            }
            IDs[currentID] = a;
            currentID++;
        } else if (IDs[checkId].row != 0) {
            return 0;
        }
        cur++;

        // nested for statement:
        if(strcmp(tokens[cur],",")==0){
            cur++;
            checkId = isID(tokens[cur]);
            strcat(forVarName2, tokens[cur]);
            cur++;
            if(checkId==-1){
                struct ID a = {.name=forVarName2[0], .col=0, .row=0, .type=7};
                int i=1;
                while(forVarName2[i]!='\0'){
                    a.name[i] = forVarName2[i];
                    i++;
                }
                IDs[currentID] = a;
                currentID++;
            }else if(IDs[checkId].row!=0){
                return 0;
            }
            if(strcmp(tokens[cur], "in")!=0){
                return 0;
            }
            cur++;
            colonIndex = findColon();
            if(colonIndex==-1){
                return 0;
            }
            exp1val = expr(str1);
            if(exp1val!=2){
                return 0;
            }
            processStack(str1, processStr1, lasttoken);
            strcat(res, processStr1);
            strcat(forStr, "for(int ");
            strcat(forStr, forVarName1);
            strcat(forStr, "=");
            strcat(forStr, lasttoken);
            strcat(forStr, "; ");

            cur = colonIndex+1;
            colonIndex = findColon();
            exp2val = expr(str2);
            if(exp2val!=2){
                return 0;
            }
            processStack(str2, processStr2, lasttoken);
            strcat(res, processStr2);
            strcat(forStr, forVarName1);
            strcat(forStr, "<=");
            strcat(forStr, lasttoken);
            strcat(forStr, "; ");
            strcat(forStr, forVarName1);
            strcat(forStr, " += ");


            cur = colonIndex+1;
            commaIndex = findComma();
            if(commaIndex==-1){
                return 0;
            }
            exp3val = expr(str3);
            if(exp3val!=2){
                return 0;
            }
            processStack(str3, processStr3, lasttoken);

            strcat(res, processStr3);
            strcat(forStr, lasttoken);
            strcat(forStr, "){\n\t\t");

            cur = commaIndex+1;
            isInForLoop1 = true;

            colonIndex = findColon();
            if(colonIndex==-1){
                return 0;
            }
            exp4val = expr(str4);
            if(exp4val!=2){
                return 0;
            }
            processStack(str4, processStr4, lasttoken);
            strcat(res, processStr4);
            strcat(forStr, "for(int ");
            strcat(forStr, forVarName2);
            strcat(forStr, "=");
            strcat(forStr, lasttoken);
            strcat(forStr, "; ");
            strcat(forStr, forVarName2);
            strcat(forStr, "<=");

            cur = colonIndex+1;
            colonIndex = findColon();
            if(colonIndex==-1){
                return 0;
            }

            exp5val = expr(str5);
            if(exp5val!=2){
                return 0;
            }
            processStack(str5, processStr5, lasttoken);
            strcat(res, processStr5);
            strcat(forStr, lasttoken);
            strcat(forStr, "; ");
            strcat(forStr, forVarName2);
            strcat(forStr, " += ");

            cur = colonIndex+1;

            exp6val = expr(str6);
            if(exp6val!=2){
                return 0;
            }
            processStack(str6, processStr6, lasttoken);
            strcat(res, processStr6);
            strcat(forStr, lasttoken);
            strcat(forStr, "){\n\t\t");

            if (strcmp(tokens[cur], ")") != 0) {
                return 0;
            }
            cur++;
            isInForLoop2 = true;
            strcat(res, forStr);
            return 1;
        }
            // 1d for statement:
        else if(strcmp(tokens[cur],"in")==0){

            cur++;
            colonIndex = findColon();
            if (colonIndex == -1) {
                return 0;
            }
            exp1val = expr(str1);
            if (exp1val != 2) {
                return 0;
            }
            processStack(str1, processStr1, lasttoken);
            strcat(res, processStr1);
            strcat(forStr, "for(int ");
            strcat(forStr, forVarName1);
            strcat(forStr, "=");
            strcat(forStr, lasttoken);
            strcat(forStr, "; ");
            strcat(forStr, forVarName1);
            strcat(forStr, "<=");

            cur = colonIndex + 1;
            colonIndex = findColon();
            if (colonIndex == -1) {
                return 0;
            }
            exp2val = expr(str2);
            if (exp2val != 2) {
                return 0;
            }
            processStack(str2, processStr2, lasttoken);
            strcat(res, processStr2);
            strcat(forStr, lasttoken);
            strcat(forStr, "; ");
            strcat(forStr, forVarName1);
            strcat(forStr, " += ");

            cur = colonIndex + 1;
            exp3val = expr(str3);
            if (exp3val != 2) {
                return 0;
            }
            processStack(str3, processStr3, lasttoken);
            strcat(res, processStr3);
            strcat(forStr, lasttoken);
            strcat(forStr, "){\n\t");

            if (strcmp(tokens[cur], ")") != 0) {
                return 0;
            }
            cur++;
            isInForLoop1 = true;

            strcat(res, forStr);
            return 1;
        }
        else{
            return 0;
        }
    }

        // end of for loop:
    else if(strcmp(tokens[cur], "}")==0){
        cur++;
        if(isInForLoop2==true && isInForLoop1==true){
            isInForLoop2 = false;
            isInForLoop1 = false;
            strcat(res, "}\n\t}\n");
            return 1;
        }
        if(isInForLoop1==true){
            isInForLoop1 = false;
            strcat(res, "}\n\t");
            return 1;
        }
        return 0;
    }

        // assignment line
    else if(assignment!=-1){
        // Calls assign function to process assignment
        return assign(numTokens, res, assignment);
    }
    else
        return 0;
}

// this function reads the variable definitions and converts them to C language.
int defineVariable(char *str){
    char *type;
    char *row = "0";
    char *column = "0";
    char name[30];
    name[0] = '\0';

    strcpy(str, "float ");             // adding 'float ' at the beginning
    type = tokens[cur];
    cur++;
    // scalar  |  vector  |  matrix
    strcpy(name, tokens[cur]);
    cur++;
    if( isValidVarName(name)==1 ){
        strcat(str, name);
    } else{
        return 0;
    }
    if(strcmp(type, "vector") == 0){    // vector    y[4]   -->    float y[4][1];
        column = "1";
        if(strcmp(tokens[cur], "[") == 0){
            strcat(str, tokens[cur]);
            cur++;
        } else{
            return 0;
        }
        row = tokens[cur];
        cur++;
        if(is_integer(row) != 0){
            strcat(str, row); ////this is row
            if(maxDimension < atoi(row)) maxDimension = atoi(row);
        } else{
            return 0;
        }
        if(strcmp(tokens[cur], "]") == 0){
            strcat(str, tokens[cur]);
            cur++;
        } else{
            return 0;
        }
        strcat(str, "[1]");
    }
    else if(strcmp(type, "matrix") == 0){     // matrix    z[3,4]   -->   float x[3][4];
        if(strcmp(tokens[cur], "[") == 0){
            strcat(str, tokens[cur]);
            cur++;
        } else{
            return 0;
        }
        row = tokens[cur];
        cur++;
        if(is_integer(row) != 0){
            strcat(str, row);
            if(maxDimension < atoi(row)) maxDimension = atoi(row);
        } else{
            return 0;
        }
        // token should be ','
        if(strcmp(tokens[cur], ",") != 0){
            return 0;
        }
        cur++;
        strcat(str, "][");
        column = tokens[cur];
        cur++;
        // column should be num
        if(is_integer(column) != 0){
            strcat(str, column);
            if(maxDimension < atoi(column)) maxDimension = atoi(column);
        } else{
            return 0;
        }
        // token should be ']'
        if(strcmp(tokens[cur], "]") == 0){
            strcat(str, tokens[cur]);
            cur++;
        } else{
            return 0;
        }
    }
    strcat(str, ";");
    if(isID(name)!=-1){
        // if id already exists, then error
        return 0;
    }else{
        // if id does not exist, create the relative struct and add it to IDs array
        struct ID a = {.name = *name, .col= atoi(column), .row = atoi(row)};
        int i=1;
        while(name[i]!='\0'){
            a.name[i] = name[i];
            i++;
        }
        if(strcmp(type, "vector")==0){
            a.type=8;
        }else if(strcmp(type, "matrix")==0){
            a.type=9;
        }else{
            a.type=7;
        }
        IDs[currentID] = a;
        currentID++;
        return 1;
    }
}

//
int assign(int numTokens, char* res, int equalIndex){

    char str1[N], str2[N], processStr[N], processStr2[N], editVar[N];
    str1[0] = '\0';
    str2[0] = '\0';
    processStr[0] = '\0';
    processStr2[0] = '\0';
    editVar[0] = '\0';
    temp_name[0] = 't';
    char temp_num_char[N];
    temp_num_char[0] = '\0';

    sprintf(tokens[equalIndex],"$") ;

    int factorvalue = factor(str1);

    cur=equalIndex+1;
    int exprvalue;

    if(factorvalue==0){
        return 0;
    }
    if(factorvalue == 2){
        exprvalue = expr(str2);

        if(exprvalue == 2){
            if(processStack(str2, processStr, lasttoken) == 0) return 0;
            editVarName(str1, editVar);
            strcat(res, processStr);
            strcat(res, "\n\t");
            strcat(res, editVar);
            strcat(res, " = ");
            strcat(res, lasttoken);
            strcat(res, ";\n");
            if(isInForLoop2) strcat(res, "\t\t");
            else if(isInForLoop1) strcat(res, "\t\t");
            return 1;
        }
        if(exprvalue != 2){
            return 0;
        }
    }
    else if(factorvalue == 3){
        int IDno = isID(tokens[0]);
        //if token is {
        if(strcmp(tokens[cur],"{")==0){
            char row[N], column[N];
            row[0] = '\0';
            column[0] = '\0';
            sprintf(row, "%d", IDs[IDno].row);
            sprintf(column, "%d", IDs[IDno].col);

            cur++;
            strcat(temp_name, "_");

            while(isID(temp_name)!=-1){
                strcat(temp_name, "_");
            }

            strcat(res, "float ");
            strcat(res, temp_name);
            strcat(res, "[");
            strcat(res, row);
            strcat(res, "][");
            strcat(res, column);
            strcat(res,"] = {");

            while(cur<numTokens-3){
                if(isNumber(tokens[cur])==0){
                    return 0;
                }
                strcat(res, tokens[cur]);
                strcat(res, ", ");
                cur++;
            }
            if(isNumber(tokens[cur])==0){
                return 0;
            }
            strcat(res, tokens[cur]);
            cur++;
            if(strcmp(tokens[cur], "}")!=0){
                return 0;
            }
            strcat(res, "};\n\t");

            strcat(res, "copyMatrixtoMatrix(");
            strcat(res, row);
            strcat(res, ", ");
            strcat(res, column);
            strcat(res, ", ");
            strcat(res, IDs[IDno].name);
            strcat(res, ", ");
            strcat(res, temp_name);
            strcat(res, ");\n");

            if(isInForLoop2) strcat(res, "\t\t");
            else if(isInForLoop1) strcat(res, "\t\t");
            return 1;
        }
        exprvalue = expr(str2);
        if(exprvalue != 3){
            return 0;
        }
    }
    if(processStack(str2, processStr, lasttoken) == 0) return 0;

    editVarName(str1, editVar);
    strcat(res, processStr);
    strcat(res, "\n\t");
    if(typeoftokensinstack[currentindexofstack] == 7 || typeoftokensinstack[currentindexofstack] == -7){
        strcat(res, editVar);
        strcat(res, " = ");
        strcat(res, lasttoken);
        strcat(res, ";\n");
        strcpy(lasttoken, "");
    }
    else{
        // row = sizeof(x)/sizeof(x[0]);
        // column = sizeof(x[0])/sizeof(x[0][0]);
        // copyMatrixtoMatrix( row, column, float a[r][c], float b[r][c]);
        int lasttokenid = isID(lasttoken);
        int varid = isID(editVar);
        if(varid == -1 || lasttokenid == -1) return 0;
        if((IDs[varid].row != IDs[lasttokenid].row) || (IDs[varid].col != IDs[lasttokenid].col)) return 0;
        strcat(res, "row = sizeof( ");
        strcat(res, lasttoken);
        strcat(res, ")/sizeof(");
        strcat(res, lasttoken);
        strcat(res, "[0]);\n\tcolumn = sizeof(");
        strcat(res, lasttoken);
        strcat(res, "[0])/sizeof(");
        strcat(res, lasttoken);
        strcat(res, "[0][0]);\n\tcopyMatrixtoMatrix(row, column, ");
        strcat(res, editVar);
        strcat(res, " , ");
        strcat(res, lasttoken);
        strcat(res, " );\n");
        strcpy(lasttoken, "");
    }
    if(isInForLoop2) strcat(res, "\t\t");
    else if(isInForLoop1) strcat(res, "\t\t");
    return 1;
}

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
                strcat(index1, "(int)");
                strcat(index1, token1);
                strcat(index1, " -1]");
                numofindexes++;
            }
            else if(numofindexes == 1){
                // 1 index is already waiting in index1 variable, add this new index to index2:
                strcpy(index2, "[");
                strcat(index2, "(int)");
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

int isAssign(){
    // If the line includes the token "=", it should be an assigment line:
    for(int i=0; i<numtokens; i++){
        if(strcmp(tokens[i],"=")==0){
            return i;
        }
    }
    return -1;
}

// Following 5 functions (expr-term-moreterms-factor-morefactors) are used to read the infix expressions and convert them to postfix notation
// The structure is generated with the help of a BNF grammar of the expressions
// terms are added to or substracted by each other (together they become moreterms)
// factors are multiplied by each other (together they become morefactors)
// factor is the smallest element of this grammar
// a factor can either be an integer, id of a variable (with or without indexes), sqrt-tr-choose function calls, or an expr in parenthesis
// If any error is encountered, 0 is returned
int expr(char *str)
{
    char str1[N], str2[N] ;
    int result = 1;
    str1[0] = str2[0] = '\0' ;
    int termvalue = term(str1);
    int moretermvalue = moreterms(str2);
    if (termvalue == 0 || moretermvalue == 0) {
        return(0) ;
    }
    else if(moretermvalue == 1){
        result = termvalue;
    }
    else if(moretermvalue == 2){
        if(termvalue != 2) {
            return 0;
        }
        else result = 2;
    }
    if(moretermvalue == 3){
        if(termvalue != 3) {
            return 0;
        }
        else result = 3;
    }
    strcat(str1,str2) ;
    strcpy(str,str1) ;
    return(result) ;
}

int term(char *str)
{

    char str1[N], str2[N] ;
    int result = 1;
    str1[0] = str2[0] = '\0' ;
    int factorvalue = factor(str1);
    int morefactorvalue = morefactors(str2);
    if (factorvalue == 0 || morefactorvalue == 0) {
        return(0) ;
    }
    if(morefactorvalue == 1 || morefactorvalue == 2){
        if(factorvalue == 2) istermscalar = true;
        if(factorvalue == 3) istermscalar = false;
        result = factorvalue;
    }
    if(morefactorvalue == 3){
        istermscalar = false;
        result = morefactorvalue;
    }
    strcat(str1,str2) ;
    strcpy(str,str1) ;
    return(result) ;
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

int factor(char *str)
{
    char str1[N] ;
    int result = 1;
    str1[0] = '\0' ;
    if ( is_integer(tokens[cur])  ) {
        strcpy(str,tokens[cur]) ;
        strcat(str," ") ;
        cur++ ;
        // scalar:
        isfactorscalar = true;
        return(2) ;
    }
    // (expr) :
    if ( strcmp(tokens[cur],"(") == 0 ) {
        cur++ ;
        int exprvalue = expr(str1);
        if ( exprvalue == 0 || exprvalue == 1) {
            return(0) ;
        }
        if ( strcmp(tokens[cur],")") != 0 ) {
            return(0) ;
        }
        cur++ ;
        strcpy(str,str1) ;
        if(exprvalue == 2) isfactorscalar = true;
        if(exprvalue == 3) isfactorscalar = false;
        return(exprvalue) ;
    }

    if( strcmp(tokens[cur],"tr") == 0 ){
        cur++;
        if ( strcmp(tokens[cur],"(") != 0 ) {
            return(0) ;
        }
        cur++;
        int exprvalue = expr(str1);
        if ( exprvalue == 0 || exprvalue == 1) {
            return(0) ;
        }
        if ( strcmp(tokens[cur],")") != 0 ) {
            return(0) ;
        }
        cur++;
        strcat(str, str1);
        strcat(str, " tr ");
        if(exprvalue == 2) isfactorscalar = true;
        if(exprvalue == 3) isfactorscalar = false;
        return (exprvalue);
    }
    if( strcmp(tokens[cur],"sqrt") == 0 ){
        cur++;
        if ( strcmp(tokens[cur],"(") != 0 ) {
            return(0) ;
        }
        cur++;
        int exprvalue = expr(str1);
        if ( exprvalue == 0 || exprvalue == 3 || exprvalue == 1) {
            return(0) ;
        }
        if ( strcmp(tokens[cur],")") != 0 ) {
            return(0) ;
        }
        cur++;
        strcat(str, str1);
        strcat(str, " sqrt ");
        if(exprvalue == 2) isfactorscalar = true;
        return (exprvalue);
    }
    if( strcmp(tokens[cur],"choose") == 0 ){
        // syntax:      choose(expr1,expr2,expr3,expr4)
        char str2[N], str3[N], str4[N] ;
        str2[0] = str3[0] = str4[0] = '\0' ;
        cur++;
        if ( strcmp(tokens[cur],"(") != 0) {
            return(0) ;
        }
        cur++;
        // expr1:
        int expr1value = expr(str1);
        if ( expr1value != 2 ) {
            return(0) ;
        }
        if ( strcmp(tokens[cur],",") != 0 ) {
            return(0) ;
        }
        cur++;
        // expr2:
        int expr2value = expr(str2);
        if ( expr2value != 2 ) {
            return(0) ;
        }
        if ( strcmp(tokens[cur],",") != 0 ) {
            return(0) ;
        }
        cur++;
        // expr3:
        int expr3value = expr(str3);
        if ( expr3value != 2 ) {
            return(0) ;
        }
        if ( strcmp(tokens[cur],",") != 0 ) {
            return(0) ;
        }
        cur++;
        // expr4:
        int expr4value = expr(str4);
        if ( expr4value != 2 ) {
            return(0) ;
        }
        if ( strcmp(tokens[cur],")") != 0 ) {
            return(0) ;
        }
        cur++;
        strcat(str, str1);
        strcat(str, " ");
        strcat(str, str2);
        strcat(str, " ");
        strcat(str, str3);
        strcat(str, " ");
        strcat(str, str4);
        strcat(str, " choose ");
        isfactorscalar = true;
        return (2);
    }

    char idname[N];
    idname[0] = '\0' ;
    // adding to the str as <id>exp[]exp[] or <id>exp[] if vector
    int check = isID(tokens[cur]);
    if(check==-1){
        return 0;

    }
    else if(IDs[check].type==7){
        //variable is a scalar
        strcat(str, tokens[cur]);
        strcat(str, " ");
        cur++;
        isfactorscalar = true;
        return 2;

    }
    else if(IDs[check].type==8){
        //variable is a vector
        strcpy(idname, tokens[cur]);
        cur++;
        if(strcmp(tokens[cur], "[") != 0) {
            strcat(str, idname);
            strcat(str, " ");
            isfactorscalar = false;
            return 3;
        }
        cur++;
        int expr1value = expr(str1);
        if( expr1value != 2){
            return 0;
        }
        if(strcmp(tokens[cur], "]")!=0){
            return 0;
        }
        cur++;
        strcat(str, str1);
        strcat(str, " [] ");
        strcat(str, idname);
        strcat(str, " ");
        isfactorscalar = true;
        return 2;

    }
    else{
        //variable is a matrix
        strcpy(idname, tokens[cur]);
        strcat(idname, " ");
        cur++;
        char str2[N];
        str2[0] = '\0' ;

        if(strcmp(tokens[cur],"[")!=0){
            strcat(str, idname);
            isfactorscalar = false;
            return 3;
        }
        cur++;
        int expr1value = expr(str1);
        if(expr1value != 2){
            return 0;
        }
        if(strcmp(tokens[cur],",")!=0){
            // id[expr] but points to a vector: not valid
            return 0;
        }
        cur++;
        int expr2value = expr(str2);
        if(expr2value != 2){
            return 0;
        }
        if(strcmp(tokens[cur],"]")!=0){
            return 0;
        }
        cur++;
        strcat(str, str1);
        strcat(str, " [] ");
        strcat(str, str2);
        strcat(str, " [] ");
        strcat(str, idname);
        strcat(str, " ");
        isfactorscalar = true;
        return 2;
    }
    return(0) ;
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

int is_integer(char *token){
    int isnumber = 1 ;
    char *q ;

    for(q = token ; *q != '\0' ; q++) {
        isnumber = isnumber && isdigit(*q) ;
    }
    return(isnumber) ;
}

int isNumber(char s[]){
    char *ptr = s;
    int numDots = 0;
    for (int i = 0; s[i]!= '\0'; i++){
        if(ptr[i]== '.'){
            numDots++;
        }else if (isdigit(s[i]) == 0)
            return 0;
    }
    if(numDots>1){
        return 0;
    }
    return 1;
}

int isID(char *s){

    for(int i=0; i<currentID; i++){
        if(strcmp(s, IDs[i].name)==0)
            return i;
    }
    return -1;
}

int findComma(){
    for(int i=cur; i<numtokens; i++){
        if(strcmp(tokens[i], ",")==0){
            return i;
        }
    }
    return -1;
}

int findColon(){
    for(int i=cur; i<numtokens; i++){
        if(strcmp(tokens[i], ":")==0){
            return i;
        }
    }
    return -1;
}

void editVarName(char *old, char *new){
    char *varToken = "";
    char varTokens[N][N];
    int count=0;
    char varDims[100];
    varDims[0] = '\0';

    while( (varToken = strsep(&old," ")) != NULL ){
        while( (strcmp(varToken, " ") == 0) || (strcmp(varToken, "") == 0) || (strcmp(varToken, "\n") == 0)) {
            varToken = strsep(&old," ");
            if( varToken == NULL)
                break;
        }
        if(varToken != NULL){
            strcpy(varTokens[count], varToken);
            count++;
        }
        else break;
    }
    if(strcmp(varTokens[1], "[]")==0){
        strcat(varDims, "[(int)");
        strcat(varDims, varTokens[0]);
        strcat(varDims, "-1]");
        if(strcmp(varTokens[3], "[]")==0){
            strcat(varDims, "[(int)");
            strcat(varDims, varTokens[2]);
            strcat(varDims, "-1]");
            strcat(new, varTokens[4]);
        }else{
            strcat(new, varTokens[2]);
        }
        strcat(new, varDims);
    }else {
        strcat(new, varTokens[0]);
    }
    memset(varTokens, '\0', N*N);
}

int isValidVarName(char* name){
    if(name[0]!='_' && isalpha(name[0])==0){
        return 0;
    }
    for(int i=1; i<strlen(name); i++){
        if(isalnum(name[i])==0 && (name[i])!='_'){
            return 0;
        }
    }
    if(strcmp(name, "scalar")==0)
        return 0;
    if(strcmp(name, "vector")==0)
        return 0;
    if(strcmp(name, "matrix")==0)
        return 0;
    if(strcmp(name, "tr")==0)
        return 0;
    if(strcmp(name, "sqrt")==0)
        return 0;
    if(strcmp(name, "choose")==0)
        return 0;
    if(strcmp(name, "for")==0)
        return 0;
    if(strcmp(name, "print")==0)
        return 0;
    if(strcmp(name, "printsep")==0)
        return 0;

    return 1;
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

void addFunctionDefinitions(){
    strcat(funcDefinitionBlock, "void allocateMatrix(float *a[], int n, int m){\n"
                                "    ptr = (float *)calloc(m*n, sizeof(float));\n"
                                "    for(int i=0; i<n; i++){\n"
                                "        a[i] = ptr+(m*i);\n"
                                "    }\n"
                                "    return;\n"
                                "}\n\n");
    strcat(funcDefinitionBlock, "void addMatrix(int n, int m, float a[n][m], float b[n][m], float *c[]){\n"
                                "    allocateMatrix(c, n, m);\n"
                                "\n"
                                "    for(int i=0; i<n; i++){\n"
                                "        for(int j=0; j<m; j++){\n"
                                "            c[i][j] = a[i][j] + b[i][j];\n"
                                "        }\n"
                                "    }\n"
                                "    return;\n"
                                "}\n\n");
    strcat(funcDefinitionBlock, "void subtractMatrix(int n, int m, float a[n][m], float b[n][m], float *c[]){\n"
                                "    allocateMatrix(c, n, m);\n"
                                "\n"
                                "    for(int i=0; i<n; i++){\n"
                                "        for(int j=0; j<m; j++){\n"
                                "            c[i][j] = a[i][j] - b[i][j];\n"
                                "        }\n"
                                "    }\n"
                                "    return;\n"
                                "}\n\n");
    strcat(funcDefinitionBlock, "void multiplyMatrix(int n, int k, int m, float a[n][k], float b[k][m], float *c[]){\n"
                                "    allocateMatrix(c, n, m);\n"
                                "\n"
                                "    for(int i=0; i<n; i++){\n"
                                "        for(int j=0; j<m; j++){\n"
                                "            c[i][j] = 0;\n"
                                "            for(int l=0; l<k; l++){\n"
                                "                c[i][j] += a[i][l]*b[l][j];\n"
                                "            }\n"
                                "        }\n"
                                "    }\n"
                                "    return;\n"
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
                                "\n"
                                "    for(int i = 0; i < n; i++){\n"
                                "        for(int j = 0; j < m; j++){\n"
                                "            c[j][i] = a[i][j];\n"
                                "        }\n"
                                "    }\n"
                                "    return;\n"
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
                                "    free(ptr);\n"
                                "    return;\n"
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
                                "    return;\n"
                                "}\n\n");
    strcat(funcDefinitionBlock, "void printScalar(float x){\n"
                                "    int isInteger = isInt(x);\n"
                                "    if(isInteger==1){\n"
                                "        printf(\"%d\\n\", (int)x);\n"
                                "    }else{\n"
                                "        printf(\"%f\\n\", x);\n"
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
                                "    return;\n"
                                "}\n\n");
    strcat(funcDefinitionBlock, "int isInt(float x){\n"
                                "    int n = (int)x;\n"
                                "    double check = x-n;\n"
                                "    if(check>0)\n"
                                "        return 0;\n"
                                "    return 1;\n"
                                "}\n\n");
}

void addFunctionDeclarations(){
    strcat(funcDeclarationBlock, "#include <stdio.h>\n");
    strcat(funcDeclarationBlock, "#include <stdlib.h>\n");
    strcat(funcDeclarationBlock, "#include <string.h>\n");
    strcat(funcDeclarationBlock, "#include <math.h>\n\n");
    strcat(funcDeclarationBlock, "void allocateMatrix(float *a[], int n, int m);\n");
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
    strcat(funcDeclarationBlock, "void printScalar(float x);\n");
    strcat(funcDeclarationBlock, "void printSep();\n");
    strcat(funcDeclarationBlock, "void declareMatrix(int row, int column, float temp[], float a[row][column]);\n");
    strcat(funcDeclarationBlock, "int isInt(float x);\n");
    strcat(funcDeclarationBlock, "int row = 0, column = 0, middle = 0;\n");
    strcat(funcDeclarationBlock, "float *ptr;\n\n");
}

char *strsep(char **stringp, const char *delim) {
    char *rv = *stringp;
    if (rv) {
        *stringp += strcspn(*stringp, delim);
        if (**stringp)
            *(*stringp)++ = '\0';
        else
            *stringp = 0; }
    return rv;
}
