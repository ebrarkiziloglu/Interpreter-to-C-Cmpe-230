#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdlib.h>

#define MAXTOKENS   100
#define TOKENLENGTH 20
#define N  500
#define MAXIDS 10

char tokens[MAXTOKENS][TOKENLENGTH] ;
char definitionBlock[40*N] = "";
char executableBlock[40*N] = "";
char funcDeclarationBlock[40*N] = "";
char funcDefinitionBlock[40*N] = "";

int cur = 0 ;
int numtokens;
int stackcur = 0;
int numstacktokens = 0;
char stacktokens[MAXTOKENS][TOKENLENGTH] ;
char cexpr[10*N];
int currentID = 0;
int maxDimension = 0;
int lineNum = 0;
int temp_num = 0;
char temp_name[N];

int typeoftokensinstack[MAXTOKENS];    // scalar: 7 vector: 8 matrix: 9
char stack[MAXTOKENS][TOKENLENGTH];
int dumbvariableid = 0;
char dumbvariablenum[N];
char dumbvariablename[N];
int currentindexofstack = 0;
char index1[TOKENLENGTH] = "";
char index2[TOKENLENGTH] = "";
char token1[TOKENLENGTH] = "";
char token2[TOKENLENGTH] = "";
char token3[TOKENLENGTH] = "";
char token4[TOKENLENGTH] = "";
char processedline[5*N];
char lasttoken[TOKENLENGTH];
int numofindexes;

bool isInForLoop1 = false;
bool isInForLoop2 = false;
bool isfactorscalar = false;
bool isprevfactorscalar = false;
bool istermscalar = false;
bool isprevtermscalar = false;

int  expr(char *) ;
int  term(char *) ;
int  moreterms(char *) ;
int  factor(char *) ;
int  morefactors(char *) ;
int  is_integer(char *) ;
char *separateLine(char line1[], int length, bool nw);
int defineVariable(char *str);
int isNumber(char s[]);
int isID(char *s);
char *strsep(char **stringp, const char *delim);
int assign(int numTokens, char* res, int equalIndex);
int process(char *str, int numTokens, char *res);
int processStack(char str[N], char *line, char *lasttoken);
int isAssign();
int findComma();
int findColon();
int isValidVarName(char* name);
void editVarName(char *old, char *new);
int adddumbVar(int typeofdumbvar);
void addFunctionDeclarations();
void addFunctionDefinitions();

struct ID{
    char name[20];
    int row;        //these dimensions can be 0 for scalars
    int col;
    //type=> scalar:7 vector:8 matrix:9 dumbscalar: -1 dumbmatrix: -2
    int type;
};

struct ID IDs[MAXIDS] ;

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
                return 0;
            }
            else if( typeOfLine == 1){
                strcat(executableBlock, "\t");
                strcat(executableBlock, cexpr);
                strcat(executableBlock, "\n");
            }
            else if( typeOfLine == 2){
                strcat(definitionBlock, cexpr);
                strcat(definitionBlock, "\n");
            }

        }
    }

    if(isInForLoop1==true || isInForLoop2==true){
        printf("ERROR! DIDN'T CLOSE THE FOR LOOP\n");
        return 0;
    }
    strcat(definitionBlock, "\n");
    strcat(executableBlock, "\treturn (0);\n}\n");
    addFunctionDeclarations();
    addFunctionDefinitions();

    fprintf(wp, "%s", funcDeclarationBlock);
    fprintf(wp, "%s", definitionBlock);
    fprintf(wp, "%s", executableBlock);
    fprintf(wp, "%s", funcDefinitionBlock);

    fclose(wp);
    return(0);
}

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

// this function is tested with the "  fgets(buff, 300, file); "  function in the main method, where buff is defined as "  char buff[300];  "
// this function reads the variable definitions and converts them to C language.
int defineVariable(char *str){
//    int length = strlen(str);
    char *type;
    char *row = "0";
    char *column = "0";

    char name[10];
    name[0] = '\0';
//    static char line[100];               // line will store the C code

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
            ////THIS TOKEN IS ROW
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
    // if id already exists, then error
    if(isID(name)!=-1){
        printf("ERROR, an id cannot be defined more than once!");
        return 0;
    }else{
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

//// a line can be print, printsep, assignment, OR FOR
int process(char *str, int numTokens, char *res){

    char str1[N], str2[N], str3[N], str4[N], str5[N], str6[N];
    str1[0] = '\0';
    str2[0] = '\0';
    str3[0] = '\0';
    str4[0] = '\0';
    str5[0] = '\0';
    str6[0] = '\0';

    int assignment = isAssign();

    int exp1val, exp2val, exp3val, exp4val, exp5val, exp6val;

    if(strcmp(tokens[cur], "scalar")==0 || strcmp(tokens[cur], "vector")==0 || strcmp(tokens[cur], "matrix")==0){
        defineVariable(res);
        return 2;
    }
    //print line
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
            //scalar print
            strcat(res, "printf(\"%f\", ");
            strcat(res, IDs[idCheck].name);
            strcat(res, ");");

        }else if(IDs[idCheck].type==8){
            //vector
            if(strcmp(tokens[cur],"[")==0){
                cur++;
                strcat(res, "printf(\"%f\", ");
                strcat(res, IDs[idCheck].name);
                strcat(res, "[");
                strcat(res, tokens[cur]);
                cur+=2;
                strcat(res, "-1][0]);");
            }else{
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
            if(strcmp(tokens[cur], "[")!=0) {
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
                cur++;
                strcat(res, "printf(\"%f\", ");
                strcat(res, IDs[idCheck].name);
                strcat(res, "[");
                strcat(res, tokens[cur]);
                cur+=2;
                strcat(res, "-1][");
                strcat(res, tokens[cur]);
                cur+=2;
                strcat(res, "-1]);");
            }else{
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

        //printsep line
    else if(strcmp(tokens[cur], "printsep")==0){
        cur++;
        if(strcmp(tokens[cur],"(")!=0){
            return 0;
        }
        cur++;
        if(strcmp(tokens[cur],")")!=0){
            return 0;
        }
        cur++;
        strcat(res, "printSep();\n");
        return 1;
    }

    else if(strcmp(tokens[cur], "for")==0){
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
            struct ID a = {.name=forVarName1[0], .col=0, .row=0};
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

        ////nested for loop
        if(strcmp(tokens[cur],",")==0){
            cur++;
            checkId = isID(tokens[cur]);
            strcat(forVarName2, tokens[cur]);
            cur++;
            if(checkId==-1){
                struct ID a = {.name=forVarName2[0], .col=0, .row=0};
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
            sprintf(tokens[colonIndex],"$");
            exp1val = expr(str1);
            if(exp1val!=2){
                return 0;
            }

            cur = colonIndex+1;

            colonIndex = findColon();
            sprintf(tokens[colonIndex],"$");

            exp2val = expr(str2);
            if(exp2val!=2){
                printf("expr2 in for loop must be scalar\n");
                return 0;
            }

            cur = colonIndex+1;

            commaIndex = findComma();
            if(commaIndex==-1){
                printf("NEED COMMA\n");
                return 0;
            }
            sprintf(tokens[commaIndex],"$");
            exp3val = expr(str3);
            if(exp3val!=2){
                printf("expr3 in for loop must be scalar\n");
                return 0;
            }

            cur = commaIndex+1;
            isInForLoop1 = true;
            strcat(res, "for(int ");
            strcat(res, forVarName1);
            strcat(res, "=");
            strcat(res, str1);
            strcat(res, "; ");
            strcat(res, forVarName1);
            strcat(res, "<");
            strcat(res, str2);
            strcat(res, "; ");
            strcat(res, forVarName1);
            strcat(res, " += ");
            strcat(res, str3);
            strcat(res, "){\n\t\t");

            colonIndex = findColon();
            if(colonIndex==-1){
                printf("colon needed\n");
                return 0;
            }
            sprintf(tokens[colonIndex],"$");
            exp4val = expr(str4);
            if(exp4val!=2){
                printf("expr4 in for loop must be scalar\n");
                return 0;
            }

            cur = colonIndex+1;

            colonIndex = findColon();
            if(colonIndex==-1){
                printf("colon needed\n");
                return 0;
            }
            sprintf(tokens[colonIndex],"$");

            exp5val = expr(str5);
            if(exp5val!=2){
                printf("expr5 in for loop must be scalar\n");
                return 0;
            }
            cur = colonIndex+1;

            exp6val = expr(str6);
            if(exp6val!=2){
                printf("expr6 in for loop must be scalar\n");
                return 0;
            }
            if (strcmp(tokens[cur], ")") != 0) {
                printf("error needed )");
                return 0;
            }
            cur++;
            isInForLoop2 = true;
            strcat(res, "for(int ");
            strcat(res, forVarName2);
            strcat(res, "=");
            strcat(res, str4);
            strcat(res, "; ");
            strcat(res, forVarName2);
            strcat(res, "<");
            strcat(res, str5);
            strcat(res, "; ");
            strcat(res, forVarName2);
            strcat(res, " += ");
            strcat(res, str6);
            strcat(res, "){\n\t\t");
            return 1;
        }

            ////1d for loop
        else if(strcmp(tokens[cur],"in")==0){

            cur++;
            colonIndex = findColon();
            if (colonIndex == -1) {
                printf("colon needed\n");
                return 0;
            }
            sprintf(tokens[colonIndex], "$");
            exp1val = expr(str1);
            if (exp1val != 2) {
                printf("expr1 in for loop must be scalar\n");
                return 0;
            }

            cur = colonIndex + 1;
            colonIndex = findColon();
            if (colonIndex == -1) {
                printf("colon needed\n");
                return 0;
            }
            sprintf(tokens[colonIndex], "$");

            exp2val = expr(str2);
            if (exp2val != 2) {
                printf("expr2 in for loop must be scalar\n");
                return 0;
            }

            cur = colonIndex + 1;
            exp3val = expr(str3);
            if (exp3val != 2) {
                printf("expr3 in for loop must be scalar\n");
                return 0;
            }
            if (strcmp(tokens[cur], ")") != 0) {
                printf("error needed )");
                return 0;
            }
            cur++;
            isInForLoop1 = true;
            strcat(res, "for(int ");
            strcat(res, forVarName1);
            strcat(res, "=");
            strcat(res, str1);
            strcat(res, "; ");
            strcat(res, forVarName1);
            strcat(res, "<");
            strcat(res, str2);
            strcat(res, "; ");
            strcat(res, forVarName1);
            strcat(res, " += ");
            strcat(res, str3);
            strcat(res, "){\n\t");
            return 1;
//        isForLine = false;
        }

        else{
            printf("syntax error in for loop\n");
            return 0;
        }
    }

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
        printf("more than one }");
        return 0;
    }

        //assignment line
    else if(assignment!=-1){
        return assign(numTokens, res, assignment);
    }

    else{
        printf("ERROR NO SUCH TYPE OF LINE\n");
        return 0;
    }
}

int expr(char *str)
{
    //printf("241 - curr:%d\n", cur);
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
            printf("incompatible types!\n");
            return 0;
        }
        else result = 2;
    }
    if(moretermvalue == 3){
        if(termvalue != 3) {
            printf("incompatible types!\n");
            return 0;
        }
        else result = 3;
    }
    strcat(str1,str2) ;
    strcpy(str,str1) ;
    //printf("263 - curr:%d\n", cur);
    return(result) ;
}

int term(char *str)
{
    //printf("267 - curr:%d\n", cur);
    char str1[N], str2[N] ;
    int result = 1;
    str1[0] = str2[0] = '\0' ;
    int factorvalue = factor(str1);
    //printf("274 - curr:%d\n", cur);
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
    //printf("291 - curr:%d\n", cur);
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
            printf("incompatible types\n");
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
    strcat(str2,str3) ;
    strcat(str2,str1) ;
    strcpy(str,str2) ;
    return(result) ;
}

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
        printf("ERROR!\n");
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
            printf("ERROR\n");
            return 0;
        }
    }else if(factorvalue == 3){

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
 /*           editVarName(str1, editVar);
            strcat(res, editVar);*/
            strcat(res, "float ");
            strcat(res, temp_name);
            strcat(res, "[");
            strcat(res, row);
            strcat(res, "][");
            strcat(res, column);
            strcat(res,"] = {");

            while(cur<numTokens-3){
                if(isNumber(tokens[cur])==0){
                    printf("ERROR NUMBER NEEDED\n");
                    return 0;
                }
                strcat(res, tokens[cur]);
                strcat(res, ", ");
                cur++;
            }
            if(isNumber(tokens[cur])==0){
                printf("ERROR NUMBER NEEDED\n");
                return 0;
            }
            strcat(res, tokens[cur]);
            cur++;
            if(strcmp(tokens[cur], "}")!=0){
                printf("ERROR NEEDED SUSLU PARANTEZ\n");
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
            printf("ERROR!\n");
            return 0;
        }
    }
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

int factor(char *str)
{
    char str1[N] ;
    int result = 1;
    str1[0] = '\0' ;
    //printf("325 - curr:%d\n", cur);
    // printf("141 - %s\n",tokens[cur]) ;
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
            printf("Error: expecting parenthesis\n") ;
            return(0) ;
        }
        cur++ ;
        strcpy(str,str1) ;
        if(exprvalue == 2) isfactorscalar = true;
        if(exprvalue == 3) isfactorscalar = false;
        return(exprvalue) ;
    }
    //// From this point on, I added the parts for 'tr', 'choose' and 'sqrt'
    //// the parts for ( id | id[expr] | id[expr, expr] ) are left
    if( strcmp(tokens[cur],"tr") == 0 ){
        cur++;
        if ( strcmp(tokens[cur],"(") != 0 ) {
            printf("Error: expecting paranthesis\n") ;
            return(0) ;
        }
        cur++;
        int exprvalue = expr(str1);
        if ( exprvalue == 0 || exprvalue == 1) {
            return(0) ;
        }
        if ( strcmp(tokens[cur],")") != 0 ) {
            printf("Error: expecting paranthesis\n") ;
            return(0) ;
        }
        cur++;
        //// For now, I added the tokens in infix order for these functions, could be changed later.
        strcat(str, str1);
        strcat(str, " tr ");
        if(exprvalue == 2) isfactorscalar = true;
        if(exprvalue == 3) isfactorscalar = false;
        return (exprvalue);
    }
    if( strcmp(tokens[cur],"sqrt") == 0 ){
        cur++;
        if ( strcmp(tokens[cur],"(") != 0 ) {
            printf("Error: expecting paranthesis\n") ;
            return(0) ;
        }
        cur++;
        int exprvalue = expr(str1);
        if ( exprvalue == 0 || exprvalue == 3 || exprvalue == 1) {
            return(0) ;
        }
        if ( strcmp(tokens[cur],")") != 0 ) {
            printf("Error: expecting paranthesis\n") ;
            return(0) ;
        }
        cur++;
        //// For now, I added the tokens in infix order for these functions, could be changed later.
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
            printf("Error: expecting paranthesis\n") ;
            return(0) ;
        }
        cur++;
        // expr1:
        int expr1value = expr(str1);
        if ( expr1value != 2 ) {
            return(0) ;
        }
        if ( strcmp(tokens[cur],",") != 0 ) {
            printf("Error: expecting comma\n") ;
            return(0) ;
        }
        cur++;
        // expr2:
        int expr2value = expr(str2);
        if ( expr2value != 2 ) {
            return(0) ;
        }
        if ( strcmp(tokens[cur],",") != 0 ) {
            printf("Error: expecting comma\n") ;
            return(0) ;
        }
        cur++;
        // expr3:
        int expr3value = expr(str3);
        if ( expr3value != 2 ) {
            return(0) ;
        }
        if ( strcmp(tokens[cur],",") != 0 ) {
            printf("Error: expecting comma\n") ;
            return(0) ;
        }
        cur++;
        // expr4:
        int expr4value = expr(str4);
        if ( expr4value != 2 ) {
            return(0) ;
        }
        if ( strcmp(tokens[cur],")") != 0 ) {
            printf("Error: expecting paranthesis\n") ;
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
        printf("ERROR: undefined id");
        return 0;

    }else if(IDs[check].type==7){
        //variable is a scalar
        strcat(str, tokens[cur]);
        strcat(str, " ");
        cur++;
        isfactorscalar = true;
        return 2;

    }else if(IDs[check].type==8){
        //variable is a vector
        strcpy(idname, tokens[cur]);
        cur++;
        if(strcmp(tokens[cur], "[") != 0) {
            strcat(str, idname);
            strcat(str, " ");
            isfactorscalar = false;
            return 3;
//            printf("ERROR expected expression");
//            return 0;
        }
        cur++;
        int expr1value = expr(str1);
        if( expr1value != 2){
            printf("ERROR expected scalar");
            return 0;
        }
        if(strcmp(tokens[cur], "]")!=0){
            printf("Error!! expected ]");
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
//            printf("error!, need [");
//            return 0;
        }
        cur++;
        int expr1value = expr(str1);
        if(expr1value != 2){
            printf("error!, need scalar");
            return 0;
        }
        if(strcmp(tokens[cur],",")!=0){
            //// id[expr] but points to a vector:
            if(strcmp(tokens[cur],"]")!=0){
                printf("error!, need paranthesis");
                return 0;
            }
            strcat(str, str1);
            strcat(str, " [] ");
            strcat(str, idname);
            strcat(str, " ");
            isfactorscalar = false;
            return 3;
        }
        cur++;
        int expr2value = expr(str2);
        if(expr2value != 2){
            printf("error!, need scalar");
            return 0;
        }
        if(strcmp(tokens[cur],"]")!=0){
            printf("error!, need ]");
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
    printf("Error: expecting factor\n") ;
    return(0) ;
}

int morefactors(char *str)
{
    char str1[N], str2[N], str3[N] ;
    int result = 1;
    str1[0] = str2[0] = str3[0] = '\0' ;
    if ( (strcmp(tokens[cur],"*") == 0 ) /*|| (strcmp(tokens[cur],"/") == 0 )*/ ) {
        isprevfactorscalar = isfactorscalar;
//        strcpy(str1,tokens[cur]) ;
//        strcat(str1," ") ;
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
    strcat(str2,str3) ;
    strcat(str2,str1) ;
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

    for(int i=0; i<MAXIDS; i++){
        if(strcmp(s, IDs[i].name)==0)
            return i;
    }
    return -1;
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
//    isexprfinished = false;
    stackcur = 0;
    char *p;
    char *stacktoken;
    p = separateLine(str, strlen(str), false);
    numstacktokens = 0;
    memset(stacktokens, 0, sizeof(stacktokens[0][0]) * strlen(stacktokens[0]) * TOKENLENGTH);
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
//    printf("process:\n");
//    for(int i = 0; i < numstacktokens; i++){
//        printf("%d - %s\n", i, stacktokens[i]);
//    }
    strcat(line, "\t");
    while(stackcur < numstacktokens){
        //// * is not finished yet
        if(strcmp(stacktokens[stackcur] , "*") == 0){
            stackcur++;
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
                int idofdumbvar = adddumbVar(-7);
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

                int idofdumbvarptr = adddumbVar(-10);
                int idofdumbvar = adddumbVar(-9);
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

                int idofdumbvarptr = adddumbVar(-10);
                int idofdumbvar = adddumbVar(-9);
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

            }
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
                int idofdumbvarptr = adddumbVar(-10);
                int idofdumbvar = adddumbVar(-9);
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
            }
        }
        else if(strcmp(stacktokens[stackcur] , "+") == 0){
            stackcur++;
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
                int idofdumbvar = adddumbVar(-7);
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

                // row = sizeof(y)/sizeof(y[0]);
                // column = sizeof(y[0])/sizeof(y[0][0]);
                // float *y2[row];
                // float y3[row][column];
                // addMatrix(row, column, y, x, y2);
                // copyMatrix(y2, row, column, y3);

                int idofdumbvarptr = adddumbVar(-10);
                int idofdumbvar = adddumbVar(-9);

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
            stackcur++;
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
                int idofdumbvar = adddumbVar(-7);
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

                // row = sizeof(y)/sizeof(y[0]);
                // column = sizeof(y[0])/sizeof(y[0][0]);
                // float *y2[row];
                // float y3[row][column];
                // subtractMatrix(row, column, y, x, y2);
                // copyMatrix(y2, row, column, y3);

                int idofdumbvarptr = adddumbVar(-10);
                int idofdumbvar = adddumbVar(-9);

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
            if(currentindexofstack < 1) return 0;
            strcpy(token1, stack[currentindexofstack]);
            if((!is_integer(token1)) && (typeoftokensinstack[currentindexofstack] != 7) && (typeoftokensinstack[currentindexofstack] != -7)) return 0;
            typeoftokensinstack[currentindexofstack] = 0;
            strcat(stack[currentindexofstack], "");
            currentindexofstack--;
            int idofdumbvar = adddumbVar(-7);
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
            if(currentindexofstack < 1) return 0;
            strcpy(token1, stack[currentindexofstack]);
            int typeoftoken1 = typeoftokensinstack[currentindexofstack];
            typeoftokensinstack[currentindexofstack] = 0;
            strcat(stack[currentindexofstack], "");
            currentindexofstack--;
            int idofdumbvar = adddumbVar(-7);
            if((is_integer(token1)) || (typeoftoken1 == 7 || typeoftoken1 == -7) ){
                // scalar:
                // float id = scalartr(token1);
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
                // matrix:
                // row = sizeof(y3[0])/sizeof(y3[0][0]);
                // column = sizeof(y3)/sizeof(y3[0]);
                // float *y4[row];
                // float y5[row][column];
                // transposeMatrix(column, row, y3, y4);
                // copyMatrix(y4, row, column, y5);
                int idofdumbvarptr = adddumbVar(-10);
                int idofdumbvar = adddumbVar(-9);

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
            else{
                printf("WHY ARE WE HERE?? what is wrong?\n");
            }
        }
        else if(strcmp(stacktokens[stackcur], "choose") == 0){
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

            int idofdumbvar = adddumbVar(-7);
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
            stackcur++;
            if(strcmp(stacktokens[stackcur], "]") != 0) return 0;
            if(typeoftokensinstack[currentindexofstack] != 7 && typeoftokensinstack[currentindexofstack] != -7) return 0;
            strcpy(token1, stack[currentindexofstack]);
            typeoftokensinstack[currentindexofstack] = 0;
            strcat(stack[currentindexofstack], "");
            currentindexofstack--;
            if(numofindexes == 0){
                strcpy(index1, "[");
                strcat(index1, token1);
                strcat(index1, " -1]");
                numofindexes++;
            }
            else if(numofindexes == 1){
                strcpy(index2, "[");
                strcat(index2, token1);
                strcat(index2, " -1]");
                numofindexes++;
            }
        }
        else if( is_integer(stacktokens[stackcur])){
            currentindexofstack++;
            strcpy(stack[currentindexofstack] , stacktokens[stackcur] );
            typeoftokensinstack[currentindexofstack] = 7;
        }
        else{
            int idcheck = isID(stacktokens[stackcur]);
            if(idcheck == -1) return 0;
            else if(IDs[idcheck].type== 7 || IDs[idcheck].type== -7){
                // scalar
                currentindexofstack++;
                strcpy(stack[currentindexofstack] , stacktokens[stackcur] );
                typeoftokensinstack[currentindexofstack] = 7;
            }
            else if(IDs[idcheck].type== 8 || IDs[idcheck].type== -8){
                // vector
                if(numofindexes == 0){
                    currentindexofstack++;
                    strcpy(stack[currentindexofstack] , stacktokens[stackcur] );
                    typeoftokensinstack[currentindexofstack] = 9;
                }
                else if(numofindexes == 1){
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
                // matrix:
                if(numofindexes == 0){
                    currentindexofstack++;
                    strcpy(stack[currentindexofstack] , stacktokens[stackcur] );
                    typeoftokensinstack[currentindexofstack] = 9;
                }
                else if(numofindexes == 1){
                    // vector
                    currentindexofstack++;
                    strcpy(stack[currentindexofstack] , stacktokens[stackcur] );
                    strcat(stack[currentindexofstack] , index1);
                    typeoftokensinstack[currentindexofstack] = 8;
                    strcpy(index1, "");
                    numofindexes = 0;
                }
                else if(numofindexes == 2){
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
            else { return 0;}
        }
        stackcur++;
//        printf("556 - line:\t%s\n", line);
    }
    printf("%s\n", line);
    strcpy(lasttoken, stack[currentindexofstack]);
    return 1;
}

int isAssign(){
    for(int i=0; i<numtokens; i++){
        if(strcmp(tokens[i],"=")==0){
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

int findComma(){
    for(int i=cur; i<numtokens; i++){
        if(strcmp(tokens[i], ",")==0){
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
    if(strcmp(varTokens[0], "[]")==0){
        strcat(varDims, "[");
        strcat(varDims, varTokens[1]);
        strcat(varDims, "]");
        if(strcmp(varTokens[2], "[]")==0){
            strcat(varDims, "[");
            strcat(varDims, varTokens[3]);
            strcat(varDims, "]");
            strcat(new, varTokens[4]);
        }else{
            strcat(new, varTokens[2]);
        }
        strcat(new, varDims);
    }else {
        strcat(new, varTokens[0]);
    }
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
    return 1;
}

int adddumbVar(int typeofdumbvar){
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
    struct ID a = { .col= -1, .row = -1, .type = typeofdumbvar};
    strcpy(a.name, dumbvariablename);
    IDs[currentID] = a;
    currentID++;
    return (currentID-1);
}

void addFunctionDeclarations(){
    strcat(funcDeclarationBlock, "#include <stdio.h>\n");
    strcat(funcDeclarationBlock, "#include <stdlib.h>\n");
    strcat(funcDeclarationBlock, "#include <string.h>\n");
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
}

void addFunctionDefinitions(){
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
}
