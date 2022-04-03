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
int cur = 0 ;
int numtokens;
char result[N] = "";
char cexpr[N];
char str[N] ;
char stacktokens[MAXTOKENS][TOKENLENGTH] ;
int stackcur = 0;
int numstacktokens = 0;
int currentRow = 0;
int currentColumn = 0;

bool isfactorscalar = false;
bool isprevfactorscalar = false;
bool istermscalar = false;
bool isprevtermscalar = false;

int stack[MAXTOKENS];
int exprneeded[MAXTOKENS];
int current;
char index1[10] = "";
char index2[10] = "";
int numofindexes;
bool isexprfinished;
char string[20] = "";

int  expr(char *) ;
int  term(char *) ;
int  moreterms(char *) ;
int  factor(char *) ;
int  morefactors(char *) ;
int  is_integer(char *) ;
char *separateLine(char line1[], int length, bool nw);
const char* defineVariable(char *str);
int isAlphabetic(char str[]);
int isNumber(char s[]);
int isID(char *s);
char *processStack(char str[N], char *line);


struct ID{
    char name[256];
    //these dimensions can be 0 for scalars
    int row;
    int col;
};

struct ID IDs[MAXIDS] ;

int maxDimension = 0;

int main (int argc,char *argv[]) {
    struct ID x = {"x", 0,0};
    struct ID i = {"i", 0,0};
    struct ID j = {"j", 0,0};
    struct ID A = {"A", 2, 2};
    struct ID B = {"B", 2, 2};
    struct ID y = {"y", 2, 1};
    struct ID xy2 = {"xy2", 4, 1};
    IDs[0] = x;
    IDs[1] = i;
    IDs[2] = j;
    IDs[3] = A;
    IDs[4] = B;
    IDs[5] = y;
    IDs[6] = xy2;

    strcpy(result, "");
    char *q;
    char *token ;
    FILE *fp;
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
    while( strcmp(fgets(buff,256,fp), "\n")) {
        q = separateLine(buff, strlen(buff), false) ;
        line = defineVariable(q);
        // printf("**:\n%s\n", line);
        strcat(result, line);
    }

    strcat(result, "########\n");
    while( fgets(buff,256,fp) != NULL ) {
        q = separateLine(buff, strlen(buff), false) ;
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
            // parse the expression
            char str[N] = "";
            expr(str);
            // printf("%s\n", str);
            processStack(str, cexpr);
            strcat(result, cexpr);
        }
    }
    printf("\nResult is:\n\n%s\n", result);

//    FILE *wp;
//    wp = fopen(argv[2], "w");
    return(0);
}


char *separateLine(char line[], int length, bool nw){
    if(strcmp(line, "\n") == 0) return NULL;
    static char result[256] = "";
    strcpy(result, "");
    for(int i = 0; i < length; i++) {
        //// MAYBE if line[i] == " ", we can pass that to avoid multiple spaces. TRY IT
        if (!(isalnum(line[i]) | (line[i]=='.'))) {
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
const char* defineVariable(char *str){
    int length = strlen(str);
    char *token;
    char *type;
    char name[length-7];
    static char line[100];               // line will store the C code
    char extra[100];
    strcpy(extra, "");
    strcpy(line, "float ");             // adding 'float ' at the beginning
    token = strsep(&str," ");
    while( (strcmp(token, " ") == 0) || (strcmp(token, "") == 0)) token = strsep(&str," ");
    type = token;
    // scalar  |  vector  |  matrix
    token = strsep(&str," ");
    while( (strcmp(token, " ") == 0) || (strcmp(token, "") == 0)) token = strsep(&str," ");
    strcpy(name, token);
    if( isAlphabetic(name) ){
        strcat(line, name);
    } else{
        // name has non-alphabetic characters
        //// ERROR !!!
    }
    if(strcmp(type, "vector") == 0){    // vector    y[4]   -->    float y[4][1];
        strcpy(extra, "int ");
        strcat(extra, name);
        strcat(extra, "Row = ");
        token = strsep(&str," ");
        while( (strcmp(token, " ") == 0) || (strcmp(token, "") == 0)) token = strsep(&str," ");        // token should be '['
        if(strcmp(token, "[") == 0){
            strcat(line, token);
        } else{
            //// ERROR !!!
        }
        token = strsep(&str," ");
        while( (strcmp(token, " ") == 0) || (strcmp(token, "") == 0)) token = strsep(&str," ");        // token should be num
        if(isNumber(token) != 0){
            strcat(line, token);
            strcat(extra, token);
            strcat(extra, ";\nint ");
            strcat(extra, name);
            strcat(extra, "Column = 1;\n");
            if(maxDimension < atoi(token)) maxDimension = atoi(token);
        } else{
            //// ERROR !!!
        }
        token = strsep(&str," ");
        while( (strcmp(token, " ") == 0) || (strcmp(token, "") == 0)) token = strsep(&str," ");        // token should be ']'
        if(strcmp(token, "]") == 0){
            strcat(line, token);
        } else{
            //// ERROR !!!
        }
        strcat(line, "[1]");
    }
    else if(strcmp(type, "matrix") == 0){     // matrix    z[3,4]   -->   float x[3][4];
        strcpy(extra, "int ");
        strcat(extra, name);
        strcat(extra, "Row = ");
        token = strsep(&str," ");
        while( (strcmp(token, " ") == 0) || (strcmp(token, "") == 0)) token = strsep(&str," ");        // token should be '['
        if(strcmp(token, "[") == 0){
            strcat(line, token);
        } else{
            //// ERROR !!!
        }
        token = strsep(&str," ");
        while( (strcmp(token, " ") == 0) || (strcmp(token, "") == 0)) token = strsep(&str," ");        // token should be num
        if(isNumber(token) != 0){
            strcat(line, token);
            strcat(extra, token);
            strcat(extra, ";\nint ");
            strcat(extra, name);
            strcat(extra, "Column = ");
            if(maxDimension < atoi(token)) maxDimension = atoi(token);
        } else{
            //// ERROR !!!
        }
        token = strsep(&str," ");
        while( (strcmp(token, " ") == 0) || (strcmp(token, "") == 0)) token = strsep(&str," ");        // token should be ','
        if(strcmp(token, ",") != 0){
            //// ERROR !!!
        }
        strcat(line, "][");
        token = strsep(&str," ");
        while( (strcmp(token, " ") == 0) || (strcmp(token, "") == 0)) token = strsep(&str," ");        // token should be num
        if(isNumber(token) != 0){
            strcat(line, token);
            strcat(extra, token);
            strcat(extra, ";\n");
            if(maxDimension < atoi(token)) maxDimension = atoi(token);
        } else{
            //// ERROR !!!
        }
        token = strsep(&str," ");
        while( (strcmp(token, " ") == 0) || (strcmp(token, "") == 0)) token = strsep(&str," ");        // token should be ']'
        if(strcmp(token, "]") == 0){
            strcat(line, token);
        } else{
            //// ERROR !!!
        }
    }
    else{
        //// ERROR !!!
    }
    token = strsep(&str," ");
    while( (strcmp(token, " ") == 0) || (strcmp(token, "") == 0)) token = strsep(&str," ");
    if((strcmp(token, " ") != 0) && (strcmp(token, "") != 0)){
        //// ERROR !!!
    }
    strcat(line, ";\n");
    strcat(line, extra);
    return line;
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
    // str1: term,   str2: moreterms
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
    // str1: factor,   str2: morefactor
    strcat(str2,str1) ;
    strcpy(str,str2) ;
    return(result) ;
}

int moreterms(char *str)
{
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


int is_integer(char *token)
{
    int isnumber = 1 ;
    char *q ;

    for(q = token ; *q != '\0' ; q++) {
        isnumber = isnumber && isdigit(*q) ;
    }

    return(isnumber) ;
}

int isAlphabetic(char str[]){
    int length = strlen(str);
    for(int i = 0; i < length; i++){
        if(isalnum(str[i]) == 0)
            return (0);
    }
    return (1);
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
    for(int i=0; i<sizeof(IDs)/sizeof(IDs[0]); i++){
        if(strcmp(s, IDs[i].name)==0)
            return i;
    }
    return -1;
}


// scalar* = 1                  // expecting 2 exprs
// matrix* = 2                  // expecting 2 exprs
// scalarmatrix* = 3            // expecting 2 exprs
// matrixscalar* = 4            // expecting 2 exprs
// scalar+ = 5                  // expecting 2 exprs
// matrix+ = 6                  // expecting 2 exprs
// scalar- = 7                  // expecting 2 exprs
// matrix- = 8                  // expecting 2 exprs
// sqrt = 9                     // expecting 1 expr
// choose = 10                  // expecting 4 exprs
// tr = 11                      // expecting 1 expr
// number = 12
// id = 13
// id[] = 14
// id[][] = 15
// [] = 16


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
    strcat(line, ";\n");
    // printf("%s", line);
    return NULL;
}





