#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdlib.h>

#define MAXTOKENS   100
#define TOKENLENGTH 20
#define N  1000
#define MAXIDS 10

char tokens[MAXTOKENS][TOKENLENGTH] ;
char result[N] = "";
char stacktokens[MAXTOKENS][TOKENLENGTH] ;
char cexpr[N];

int cur = 0 ;
int numtokens;
int stackcur = 0;
int numstacktokens = 0;
int currentID = 0;
int maxDimension = 0;
int lineNum = 0;
int IDtype = -1; ////take matrix row and col from here

int stack[MAXTOKENS];
int exprneeded[MAXTOKENS];
int current;
char index1[10] = "";
char index2[10] = "";
int numofindexes;
bool isexprfinished;
char string[20] = "";

bool isInForLoop1 = false;
bool isInForLoop2 = false;
bool isfactorscalar = false;
bool isprevfactorscalar = false;
bool istermscalar = false;
bool isprevtermscalar = false;
bool inDefPart = true;

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
char *processStack(char str[N], char *line);
int isAssign();
int findComma();
int findColon();
int isValidVarName(char* name);

struct ID{
    char name[20];
    //these dimensions can be 0 for scalars
    int row;
    int col;
};

struct ID IDs[MAXIDS] ;

int main (int argc,char *argv[]) {

//    struct ID x = {"x", 0,0};
//    struct ID i = {"i", 0,0};
//    struct ID A = {"A", 2, 2};
//    IDs[0] = x;
//    IDs[1] = A;
//    IDs[2] = i;

    strcpy(result, "");
    char *q;
    char *token ;
    FILE *fp;
    // FILE *wp;

    char buff[256];
    const char* line;
    /* Open file for reading Filename is given on the command line */
    if (argc != 2) {
        printf("Give filename as command line argument\n") ;
        return(1);
    }
    fp = fopen(argv[1], "r");
    // wp = fopen(argv[2], "w");

    if(fp == NULL) {
        printf("Cannot open %s\n",argv[1]);
        return(1);
    }

    strcat(result, "########\n");
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
            if(process(str, numtokens, cexpr)==0){
                printf("ERROR LINE %d", lineNum);
                return 0;
            }
            /*           if(process(str, numtokens, cexpr)==0){
                           printf("ERROR IN FILE!");
                           return 0;
                       }*/
            //           printf("%s\n", str);
//            processStack(str, cexpr);
            strcat(result, cexpr);
            strcat(result, "\n");
        }
        //       expr(str);

    }

    if(isInForLoop1==true || isInForLoop2==true){
        printf("ERROR! DIDN'T CLOSE THE FOR LOOP\n");
        return 0;
    }
    printf("Result is:\n%s\n", result);
//    printf("CFile is:\n%s\n", cfile);
    return(0);
}


char *separateLine(char line[], int length, bool nw){
    if(strcat(line, "\n") == 0) return NULL;
    static char result[256] = "";
    strcpy(result, "");
    for(int i = 0; i < length; i++) {
        //// MAYBE if line[i] == " ", we can pass that to avoid multiple spaces. TRY IT
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
        printf("name has non-alphabetic characters");
        return 0;
        //// ERROR !!!
    }
    if(strcmp(type, "vector") == 0){    // vector    y[4]   -->    float y[4][1];
        column = "1";
        if(strcmp(tokens[cur], "[") == 0){
            strcat(str, tokens[cur]);
            cur++;
        } else{
            //// ERROR !!!
        }
        row = tokens[cur];
        cur++;
        if(isNumber(row) != 0){
            strcat(str, row); ////this is row

            if(maxDimension < atoi(row)) maxDimension = atoi(row);
        } else{
            //// ERROR !!!
        }
        if(strcmp(tokens[cur], "]") == 0){
            strcat(str, tokens[cur]);
            cur++;
        } else{
            //// ERROR !!!
        }
        strcat(str, "[1]");
    }
    else if(strcmp(type, "matrix") == 0){     // matrix    z[3,4]   -->   float x[3][4];
        if(strcmp(tokens[cur], "[") == 0){
            strcat(str, tokens[cur]);
            cur++;
        } else{
            //// ERROR !!!
        }
        row = tokens[cur];
        cur++;
        if(isNumber(row) != 0){
            ////THIS TOKEN IS ROW
            strcat(str, row);
            if(maxDimension < atoi(row)) maxDimension = atoi(row);
        } else{
            //// ERROR !!!
        }
        // token should be ','
        if(strcmp(tokens[cur], ",") != 0){
            //// ERROR !!!
        }
        cur++;
        strcat(str, "][");
        column = tokens[cur];
        cur++;
        // column should be num
        if(isNumber(column) != 0){
            strcat(str, column);

            if(maxDimension < atoi(column)) maxDimension = atoi(column);
        } else{
            //// ERROR !!!
        }
        // token should be ']'
        if(strcmp(tokens[cur], "]") == 0){
            strcat(str, tokens[cur]);
            cur++;
        } else{
            //// ERROR !!!
        }

    }
    else{
        //// ERROR !!!
    }
    strcat(str, ";");
//    strcat(line, extra);

    // if id already exists, then error
    if(isID(name)!=-1){
        printf("ERROR, an id cannot be defined more than once!");
    }else{
        struct ID a = {.name = *name, .col= atoi(column), .row = atoi(row)};
        int i=1;
        while(name[i]!='\0'){
            a.name[i] = name[i];
            i++;
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

//    char *forLine = "";
//    strcpy(forLine, " ");

    if(strcmp(tokens[cur], "scalar")==0 || strcmp(tokens[cur], "vector")==0 || strcmp(tokens[cur], "matrix")==0){
        defineVariable(res);
        return 1;
    }
    //print line
    if(strcmp(tokens[cur], "print")==0){
        cur++;
        if(strcmp(tokens[cur], "(")!=0){
            printf("ERROR! 288");
            return 0;
        }
        cur++;

        int idCheck = isID(tokens[cur]);
        cur++;

        if(idCheck==-1){
            printf("ERROR! 295");
            return 0;
        }
        if(IDs[idCheck].col==0){
            strcat(res, "printf(\"%d\", ");
            strcat(res, IDs[idCheck].name);
            strcat(res, ");");
        }else{
            char row[10], col[10];
            row[0] = '\0';
            col[0] = '\0';
            strcat(res, "printMatrix(");
            sprintf(row, "%d", IDs[idCheck].row);
            strcat(res, row);
            strcat(res, ", ");
            sprintf(col, "%d", IDs[idCheck].col);
            strcat(res, col);
            strcat(res, ", ");
            strcat(res, IDs[idCheck].name);
            strcat(res, ");");
        }

        if(strcmp(tokens[cur],")")!=0){
            printf("ERROR!\n");
            return 0;
        }
        cur++;
        if(cur!=numTokens-1){

            printf("extra token?????");
            return 0;
        }
        return 1;
    }

        //printsep line
    else if(strcmp(tokens[cur], "printsep")==0){
        cur++;
        if(strcmp(tokens[cur],"(")!=0){
            printf("ERROR!\n");
            return 0;
        }
        cur++;
        if(strcmp(tokens[cur],")")!=0){
            printf("ERROR!\n");
            return 0;
        }
        cur++;
        strcat(res, "printSep();");
        return 1;
    }

    else if(strcmp(tokens[cur], "for")==0){
        cur++;

        if(strcmp(tokens[cur],"(")!=0){
            printf("error in for loop (\n");
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
            printf("ERROR! NEED SCALAR VAR IN FOR\n");
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
                printf("error, need scalar id in for\n");
                return 0;
            }
            if(strcmp(tokens[cur], "in")!=0){
                printf("ERROR 'IN' FOR LOOP\n");
                return 0;
            }
            cur++;
            colonIndex = findColon();
            if(colonIndex==-1){
                printf("colon needed\n");
                return 0;
            }
            sprintf(tokens[colonIndex],"$");
            exp1val = expr(str1);
            if(exp1val!=2){
                printf("expr1 in for loop must be scalar\n");
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
            strcat(res, "for(float ");
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
            strcat(res, "for(float ");
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
            strcat(res, "){\n\t");

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
            strcat(res, "for(float ");
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
            strcat(res, "){\n");
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
            strcat(res, "\t}\n}\n");
            return 1;
        }
        if(isInForLoop1==true){
            isInForLoop1 = false;
            strcat(res, "}\n");
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
    strcat(str2,str1) ;
    strcpy(str,str2) ;
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
    strcat(str2,str1) ;
    strcpy(str,str2) ;
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
    strcat(str1,str2) ;
    strcat(str1,str3) ;
    strcpy(str,str1) ;
    return(result) ;
}

int assign(int numTokens, char* res, int equalIndex){

    char str1[N], str2[N], processStr[N], processStr2[N];
    str1[0] = '\0';
    str2[0] = '\0';
    processStr[0] = '\0';
    processStr2[0] = '\0';

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
            processStack(str2, processStr);
            strcat(res, str1);
            strcat(res, "=");
            strcat(res, processStr);
            return 1;
        }
        if(exprvalue != 2){
            printf("ERROR\n");
            return 0;
        }
    }else if(factorvalue == 3){

        //if token is {
        if(strcmp(tokens[cur],"{")==0){
            cur++;
            strcat(res, str1);
            strcat(res, "= ");
            strcat(res, "{");

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
            strcat(res, "};");

            return 1;
        }
        exprvalue = expr(str2);
        if(exprvalue != 3){
            printf("ERROR!\n");
            return 0;
        }
    }

    processStack(str2, processStr);
    strcat(res, str1);
    strcat(res, " = ");
    strcat(res, processStr);
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
        strcat(str, " tr ");
        strcat(str, str1);
        strcat(str, " ");
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
        strcat(str, " sqrt ");
        strcat(str, str1);
        strcat(str, " ");
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
        strcat(str, " choose ");
        strcat(str, str1);
        strcat(str, " ");
        strcat(str, str2);
        strcat(str, " ");
        strcat(str, str3);
        strcat(str, " ");
        strcat(str, str4);
        strcat(str, " ");
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

    }else if(IDs[check].col == 0){
        //variable is a scalar
        strcat(str, tokens[cur]);
        strcat(str, " ");
        cur++;
        isfactorscalar = true;
        return 2;

    }else if(IDs[check].col == 1){
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
        strcat(str, " [] ");
        strcat(str, str1);
        strcat(str, " ");
        strcat(str, idname);
        //// Should we add [0] as well??
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
            strcat(str, " [] ");
            strcat(str, str1);
            strcat(str, " ");
            strcat(str, idname);
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
        strcat(str, " [] ");
        strcat(str, str1);
        strcat(str, " [] ");
        strcat(str, str2);
        strcat(str, " ");
        strcat(str, idname);
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
    strcat(str1,str3) ;
    strcat(str1,str2) ;
    strcpy(str,str1) ;
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
    for (int i = 0; s[i]!= '\0'; i++)
    {
        if (isdigit(s[i]) == 0)
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

int push(int type, int needed){
    if(current > MAXTOKENS - 1) return (-1);
    stack[current] = type;
    exprneeded[current] = needed;
    current++;
    return (1);
}

int pop(){
    if(current == 0 ) return (-1);
    int temp = stack[current-1];
    stack[current-1] = 0;
    exprneeded[current-1] = 0;
    current--;
    return (temp);
}

char *processStack(char str[N], char *line){
    memset(stack, 0, sizeof(stack));
    memset(exprneeded, 0, sizeof(exprneeded));
    current = 0;
    strcpy(index1, "");
    strcpy(index2, "");
    numofindexes = 0;
    isexprfinished = false;
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
    strcpy(line, "");
    while(stackcur < numstacktokens){
        int check = isID(stacktokens[stackcur]);
        if(strcmp(stacktokens[stackcur] , "*") == 0){
            stackcur++;
            if(strcmp(stacktokens[stackcur], "scalar") == 0){
                strcat(line, "scalarmultiply( ");
                push(1, 2);
            }
            if(strcmp(stacktokens[stackcur], "matrix") == 0){
                strcat(line, "matrixmultiply( ");
                push(2, 2);
            }
            if(strcmp(stacktokens[stackcur], "scalarmatrix") == 0){
                strcat(line, "scalarmatrixmultiply( ");
                push(3, 2);
            }
            if(strcmp(stacktokens[stackcur], "matrixscalar") == 0){
                strcat(line, "matrixscalarmultiply( ");
                push(4, 2);
            }
        }
        else if(strcmp(stacktokens[stackcur] , "+") == 0){
            stackcur++;
            if(strcmp(stacktokens[stackcur], "scalar") == 0){
                strcat(line, "scalarsum( ");
                push(5, 2);
            }
            if(strcmp(stacktokens[stackcur], "matrix") == 0){
                strcat(line, "matrixsum( ");
                push(6, 2);
            }
        }
        else if(strcmp(stacktokens[stackcur] , "-") == 0){
            stackcur++;
            if(strcmp(stacktokens[stackcur], "scalar") == 0){
                strcat(line, "scalarsubs( ");
                push(7, 2);
            }
            if(strcmp(stacktokens[stackcur], "matrix") == 0){
                strcat(line, "matrixsub( ");
                push(8, 2);
            }
        }
        else if(strcmp(stacktokens[stackcur], "sqrt") == 0){
            strcat(line, "sqrt( ");
            push(9, 1);
        }
        else if(strcmp(stacktokens[stackcur], "choose") == 0){
            strcat(line, "choose( ");
            push(10, 4);
        }
        else if(strcmp(stacktokens[stackcur], "tr") == 0){
            strcat(line, "tr( ");
            push(11, 1);
        }
        else if( is_integer(stacktokens[stackcur])) {       // number
            int type = stack[current-1];
            if(type == 1 || type == 5 || type == 7){
                strcat(line, stacktokens[stackcur]);
                if(exprneeded[current-1] == 2){
                    strcat(line, ", ");
                    exprneeded[current-1] = exprneeded[current-1]-1;
                    isexprfinished = false;
                }
                else if(exprneeded[current-1] == 1){
                    strcat(line, ") ");
                    pop();
                    isexprfinished = true;
                }
            }
            else if(type == 3 ){
                strcat(line, stacktokens[stackcur]);
                if(exprneeded[current-1] == 2){
                    strcat(line, ", ");
                    exprneeded[current-1] = exprneeded[current-1]-1;
                    isexprfinished = false;
                }
                else if(exprneeded[current-1] == 1){
                    strcat(line, ") ");
                    pop();
                    isexprfinished = true;
                }
            }
            else if(type == 4){
                strcat(line, stacktokens[stackcur]);
                if(exprneeded[current-1] == 2){
                    strcat(line, ", ");
                    exprneeded[current-1] = exprneeded[current-1]-1;
                    isexprfinished = false;
                }
                else if(exprneeded[current-1] == 1){
                    strcat(line, ") ");
                    pop();
                    isexprfinished = true;
                }
            }
            else if(type == 9 || type == 11){           // sqrt or tr
                strcat(line, stacktokens[stackcur]);
                strcat(line, ") ");
                pop();
                isexprfinished = true;
            }
            else if( type == 10 ){
                strcat(line, stacktokens[stackcur]);
                if( exprneeded[current-1] > 1 ){
                    strcat(line, " , ");
                    exprneeded[current-1] = exprneeded[current-1] - 1;
                    isexprfinished = false;
                } else if( exprneeded[current-1] == 1 ){
                    strcat(line, ") ");
                    exprneeded[current-1] = exprneeded[current-1] - 1;
                    pop();
                    isexprfinished = true;
                }
            }
            else if( type == 16){
                if( numofindexes == 0 ){
                    strcpy(index1, "[");
                    strcat(index1, stacktokens[stackcur]);
                    strcat(index1, "]");
                    numofindexes++;
                }
                else if( numofindexes == 1){
                    strcpy(index2, "[");
                    strcat(index2, stacktokens[stackcur]);
                    strcat(index2, "]");
                    numofindexes++;
                }
                pop();
            }
        }
            // id    //    id[]    // id[][]
        else if(strcmp(stacktokens[stackcur], "[") == 0){
            stackcur++;
            if(strcmp(stacktokens[stackcur], "]") == 0){
                push(16, 1);
                isexprfinished = false;
            }
        }
        else if(check == -1) {
            // undefined
            printf("277 - error: indefined token\n");
        }
        else if(IDs[check].col == 0) {
            // scalar
            // REST is totally same with the number case above.
            int type = stack[current-1];
            if(type == 1 || type == 5 || type == 7){
                strcat(line, stacktokens[stackcur]);
                if(exprneeded[current-1] == 2){
                    strcat(line, ", ");
                    exprneeded[current-1] = exprneeded[current-1]-1;
                    isexprfinished = false;
                }
                else if(exprneeded[current-1] == 1){
                    strcat(line, ") ");
                    pop();
                    isexprfinished = true;
                }
            }
            else if(type == 3 ){
                strcat(line, stacktokens[stackcur]);
                if(exprneeded[current-1] == 2){
                    strcat(line, ", ");
                    exprneeded[current-1] = exprneeded[current-1]-1;
                    isexprfinished = false;
                }
                else if(exprneeded[current-1] == 1){
                    strcat(line, ") ");
                    pop();
                    isexprfinished = true;
                }
            }
            else if(type == 4){
                strcat(line, stacktokens[stackcur]);
                if(exprneeded[current-1] == 2){
                    strcat(line, ", ");
                    exprneeded[current-1] = exprneeded[current-1]-1;
                    isexprfinished = false;
                }
                else if(exprneeded[current-1] == 1){
                    strcat(line, ") ");
                    pop();
                    isexprfinished = true;
                }
            }
            else if(type == 9 || type == 11){           // sqrt or tr
                strcat(line, stacktokens[stackcur]);
                strcat(line, ") ");
                pop();
                isexprfinished = true;
            }
            else if( type == 10 ){
                strcat(line, stacktokens[stackcur]);
                if( exprneeded[current-1] > 1 ){
                    strcat(line, " , ");
                    exprneeded[current-1] = exprneeded[current-1] - 1;
                    isexprfinished = false;
                } else if( exprneeded[current-1] == 1 ){
                    strcat(line, ") ");
                    exprneeded[current-1] = exprneeded[current-1] - 1;
                    pop();
                    isexprfinished = true;
                }
            }
            else if( type == 16){
                if( numofindexes == 0 ){
                    strcpy(index1, "[");
                    strcat(index1, stacktokens[stackcur]);
                    strcat(index1, "]");
                    numofindexes++;
                }
                else if( numofindexes == 1){
                    strcpy(index2, "[");
                    strcat(index2, stacktokens[stackcur]);
                    strcat(index2, "]");
                    numofindexes++;
                }
                pop();
            }
        }
        else if(IDs[check].col == 1){
            // vector
            int type = stack[current-1];
            if(numofindexes == 0){
                switch (type) {
                    case 2:
                    case 6:
                    case 8:
                        strcat(line, stacktokens[stackcur]);
                        strcat(line, ", ");
                        sprintf(string, "%d", IDs[check].row);
                        strcat(line, string);
                        strcat(line, ", ");
                        sprintf(string, "%d", IDs[check].col);
                        strcat(line, string);
                        if(exprneeded[current-1] == 2){
                            strcat(line, ", ");
                            exprneeded[current-1] = exprneeded[current-1]-1;
                            isexprfinished = false;
                        }
                        else if(exprneeded[current-1] == 1){
                            strcat(line, ") ");
                            pop();
                            isexprfinished = true;
                        }
                        break;
                    case 3:
                        if(exprneeded[current-1] == 2){
                            strcat(line, stacktokens[stackcur]);
                            strcat(line, ", ");
                            sprintf(string, "%d", IDs[check].row);
                            strcat(line, string);
                            strcat(line, ", ");
                            sprintf(string, "%d", IDs[check].col);
                            strcat(line, string);
                            strcat(line, ", ");
                            exprneeded[current-1] = exprneeded[current-1] - 1;
                            isexprfinished = false;
                        }
                        else{
                            printf("448 - error: invalid numofdumbs\n");
                        }
                        break;
                    case 4:
                        strcat(line, stacktokens[stackcur]);
                        strcat(line, ", ");
                        sprintf(string, "%d", IDs[check].row);
                        strcat(line, string);
                        strcat(line, ", ");
                        sprintf(string, "%d", IDs[check].col);
                        strcat(line, string);
                        if(exprneeded[current-1] == 2){
                            printf("448 - error: invalid numofdumbs\n");
                        }
                        else if(exprneeded[current-1] == 1){
                            strcat(line, ") ");
                            pop();
                            isexprfinished = true;
                        }
                        break;
                    case 9:
                    case 10:
                        printf("447 - error : invalid type\n");
                        break;
                    case 11:
                        strcat(line, stacktokens[stackcur]);
                        strcat(line, ", ");
                        sprintf(string, "%d", IDs[check].row);
                        strcat(line, string);
                        strcat(line, ", ");
                        sprintf(string, "%d", IDs[check].col);
                        strcat(line, string);
                        strcat(line, ") ");
                        pop();
                        isexprfinished = true;
                        break;
                    case 16:
                        printf("458 - error : invalid type\n");
                }
            }
            else if(numofindexes == 1){
                // id[expr1]  -->> id[expr1][0]
                // which means we have a scalar
                strcat(index1, "[0]");
                switch (type) {
                    case 0:
                        strcat(line, stacktokens[stackcur]);
                        strcat(line, index1);
                        break;
                    case 1:
                    case 4:
                    case 5:
                    case 7:
                        strcat(line, stacktokens[stackcur]);
                        if(exprneeded[current-1] == 2){
                            strcat(line, ", ");
                            exprneeded[current-1] = exprneeded[current-1]-1;
                            isexprfinished = false;
                        }
                        else if(exprneeded[current-1] == 1){
                            strcat(line, ") ");
                            pop();
                            isexprfinished = true;
                        }
                        break;
                    case 9:
                    case 11:
                        strcat(line, stacktokens[stackcur]);
                        strcat(line, index1);
                        strcat(line, ") ");
                        pop();
                        isexprfinished = true;
                        break;
                    case 10:
                        strcat(line, stacktokens[stackcur]);
                        if(exprneeded[current-1] == 2){
                            strcat(line, ", ");
                            exprneeded[current-1] = exprneeded[current-1]-1;
                            isexprfinished = false;
                        }
                        else if(exprneeded[current-1] == 1){
                            strcat(line, ") ");
                            pop();
                            isexprfinished = true;
                        }
                        ////case 16:

                }
                strcpy(index1, "");
                numofindexes = 0;
            }
        }
        else {
            //// matrix
            int type = stack[current - 1];
            if (numofindexes == 0) {
                switch (type) {
                    case 2:
                    case 3:
                    case 4:
                    case 6:
                    case 8:
                        strcat(line, stacktokens[stackcur]);
                        strcat(line, ", ");
                        sprintf(string, "%d", IDs[check].row);
                        strcat(line, string);
                        strcat(line, ", ");
                        sprintf(string, "%d", IDs[check].col);
                        strcat(line, string);
                        if (exprneeded[current - 1] == 2) {
                            strcat(line, ", ");
                            exprneeded[current - 1] = exprneeded[current - 1] - 1;
                            isexprfinished = false;
                        } else if (exprneeded[current - 1] == 1) {
                            strcat(line, ") ");
                            pop();
                            isexprfinished = true;
                        }
                        break;
                    case 11:
                        strcat(line, stacktokens[stackcur]);
                        strcat(line, ", ");
                        sprintf(string, "%d", IDs[check].row);
                        strcat(line, string);
                        strcat(line, ", ");
                        sprintf(string, "%d", IDs[check].col);
                        strcat(line, string);
                        strcat(line, ") ");
                        isexprfinished = true;
                        pop();
                        break;
                }
            }
            else if (numofindexes == 1) {
                // represents a vector
                switch (type) {
                    case 0:
                        strcat(line, stacktokens[stackcur]);
                        strcat(line, index1);
                        break;
                    case 2:
                    case 3:
                    case 4:
                    case 6:
                    case 8:
                        strcat(line, stacktokens[stackcur]);
                        strcat(line, ", ");
                        sprintf(string, "%d", IDs[check].col);
                        strcat(line, string);
                        strcat(line, ", [1]");
                        if (exprneeded[current - 1] == 2) {
                            strcat(line, ", ");
                            exprneeded[current - 1] = exprneeded[current - 1] - 1;
                            isexprfinished = false;
                        } else if (exprneeded[current - 1] == 1) {
                            strcat(line, ") ");
                            pop();
                            isexprfinished = true;
                        }
                        break;
                    case 11:
                        strcat(line, stacktokens[stackcur]);
                        strcat(line, index1);
                        strcat(line, ", ");
                        sprintf(string, "%d", IDs[check].col);
                        strcat(line, string);
                        strcat(line, ", [1]");
                        strcat(line, ") ");
                        isexprfinished = true;
                        pop();
                        break;
                }
                strcpy(index1, "");
                numofindexes = 0;
            }
            else if (numofindexes == 2) {         // scalar
                switch (type) {
                    case 0:
                        strcat(line, stacktokens[stackcur]);
                        strcat(line, index1);
                        break;
                    case 1:
                    case 4:
                    case 5:
                    case 7:
                        strcat(line, stacktokens[stackcur]);
                        strcat(line, index1);
                        strcat(line, index2);
                        if (exprneeded[current - 1] == 2) {
                            strcat(line, ", ");
                            exprneeded[current - 1] = exprneeded[current - 1] - 1;
                            isexprfinished = false;
                        } else if (exprneeded[current - 1] == 1) {
                            strcat(line, ") ");
                            pop();
                            isexprfinished = true;
                        }
                        break;
                    case 9:
                    case 11:
                        strcat(line, stacktokens[stackcur]);
                        strcat(line, index1);
                        strcat(line, index2);
                        strcat(line, ") ");
                        pop();
                        isexprfinished = true;
                        break;
                    case 10:
                        strcat(line, stacktokens[stackcur]);
                        strcat(line, index1);
                        strcat(line, index2);
                        if (exprneeded[current - 1] == 2) {
                            strcat(line, ", ");
                            exprneeded[current - 1] = exprneeded[current - 1] - 1;
                            isexprfinished = false;
                        } else if (exprneeded[current - 1] == 1) {
                            strcat(line, ") ");
                            pop();
                            isexprfinished = true;
                        }
                        ////case 16:

                }
                strcpy(index1, "");
                strcpy(index2, "");
                numofindexes = 0;
            }
        }
        while(isexprfinished) {
            if (stack[current - 1] != 0) {     // there are operators in the line
                if (exprneeded[current - 1] == 1) {     // expressions are finished
                    exprneeded[current - 1] = exprneeded[current - 1] - 1;
                    strcat(line, ") ");
                    pop();
                    isexprfinished = true;
                }
                else if (exprneeded[current - 1] > 1) {
                    exprneeded[current - 1] = exprneeded[current - 1] - 1;
                    strcat(line, ", ");
                    isexprfinished = false;
                }
            }
            else{
                isexprfinished = false;
            }
        }
        stackcur++;
    }
    strcat(line, ";");
    // printf("%s", line);
    return NULL;
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
