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
char str[N] ;
int  cur = 0 ;
int currentRow = 0;
int currentColumn = 0;

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

struct ID{
    char name[256];
    //these dimensions can be 0 for scalars
    int row;
    int col;
};

struct ID IDs[MAXIDS] ;

int maxDimension = 0;

int main (int argc,char *argv[]) {

//    struct ID x = {"x", 0,0};
//    struct ID i = {"i", 0,0};
//    struct ID A = {"A", 2, 2};
//    IDs[0] = x;
//    IDs[1] = A;
//    IDs[2] = i;

    int numtokens ;
    // read the tokens
    numtokens = 0 ;
    char result[1000];
    strcpy(result, "");
    char *q;
    char *token ;
    FILE *fp;
    FILE *wp;
    char buff[80];
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

    while( strcmp(fgets(buff,256,fp), "\n")) {
        q = separateLine(buff, strlen(buff), false) ;
        line = defineVariable(q);
        // printf("**:\n%s\n", line);
        strcat(result, line);
    }
    // printf("Definition result is:\n%s\n", result);
    // printf("maxDimension: %d\n", maxDimension);
    strcat(result, "\n########\n");
    while( fgets(buff,256,fp) != NULL ) {
        q = separateLine(buff, strlen(buff), false) ;
        // printf("76 - %s\n", q);
        numtokens = 0;
        memset(tokens, 0, sizeof(tokens[0][0]) * strlen(tokens[0]) * TOKENLENGTH);
        cur = 0;
        while( (token = strsep(&q," ")) != NULL ){
            while( (strcmp(token, " ") == 0) || (strcmp(token, "") == 0) || (strcmp(token, "\n") == 0)) {
                token = strsep(&q," ");
                if( token == NULL)
                    break;
            }
            // tokens[numtokens] = *token;
            if(token != NULL){
                if((strcmp(token, "#") == 0)){
                    break;
                }
                strcpy(tokens[numtokens], token);
                numtokens++;
            }
            else break;
        }
        sprintf(tokens[numtokens],"$") ;
        numtokens++ ;
        // parse the expression
        char str[N] = "";
        expr(str);
        strcat(result, str);
        printf("99 - str:%s\n", str);
        strcat(result, "\n");
    }
    printf("Result is:\n%s\n", result);
    return(0);
}


char *separateLine(char line[], int length, bool nw){
    if(strcat(line, "\n") == 0) return NULL;
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
        result = factorvalue;
    }
    if(morefactorvalue == 3){
        result = morefactorvalue;
    }
    strcat(str1,str2) ;
    strcpy(str,str1) ;
    return(result) ;
}

int moreterms(char *str)
{
    char str1[N], str2[N], str3[N] ;
    int result = 1;
    str1[0] = str2[0] = str3[0] = '\0' ;
    //printf("291 - curr:%d\n", cur);
    if ( (strcmp(tokens[cur],"+") == 0 ) || (strcmp(tokens[cur],"-") == 0 ) ) {
        strcpy(str1,tokens[cur]) ;
        strcat(str1," ") ;
        cur++ ;
        int termvalue =  term(str2);
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
    strcat(str2,str3) ;
    strcat(str2,str1) ;
    strcpy(str,str2) ;
    // null:
    return(result) ;
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
        return(exprvalue) ;
    }
    //// From this point on, I added the parts for 'tr', 'choose' and 'sqrt'
    //// the parts for ( id | id[expr] | id[expr, expr] ) are left
    if( strcmp(tokens[cur],"tr") == 0 ){
        char func[N] ;
        func[0] = '\0' ;
        strcpy(func,tokens[cur]) ;
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
        // strcat(str, " ");
        strcat(str, func);
        strcat(str, " ");
        return (exprvalue);
    }
    if( strcmp(tokens[cur],"sqrt") == 0 ){
        char func[N] ;
        func[0] = '\0' ;
        strcpy(func,tokens[cur]) ;
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
        // strcat(str, " ");
        strcat(str, func);
        strcat(str, " ");
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
        strcat(str, "choose ");
        return (2);
    }
    //// if the current token is an id......

    // adding to the str as <id>exp[]exp[] or <id>exp[] if vector
    int check = isID(tokens[cur]);
    if(check==-1){
        printf("ERROR: undefined id");
        return 0;

    }else if(IDs[check].col == 0){
        //variable is a scalar
        strcat(str, tokens[cur]);
        cur++;
        return 2;

    }else if(IDs[check].col == 1){
        //variable is a vector
        strcat(str, tokens[cur]);
        cur++;
        if(strcmp(tokens[cur], "[") != 0) {
            strcat(str, " ");
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
        //// Should we add [0] as well??
        return 2;

    }else{
        //variable is a matrix
        strcat(str, tokens[cur]);
        cur++;
        char str2[N];
        str2[0] = '\0' ;

        if(strcmp(tokens[cur],"[")!=0){
            strcat(str, " ");
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
        strcpy(str1,tokens[cur]) ;
        strcat(str1," ") ;
        cur++ ;
        int factorvalue = factor(str2);
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
    strcat(str2,str3) ;
    strcat(str2,str1) ;
    strcpy(str,str2) ;
    // null:
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
