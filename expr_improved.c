#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>

#define MAXTOKENS   100
#define TOKENLENGTH 20
#define N  1000

char tokens[MAXTOKENS][TOKENLENGTH] ;
char result[N] = "";
char str[N] ;
int  cur = 0 ;

int  expr(char *) ;
int  term(char *) ;
int  moreterms(char *) ;
int  factor(char *) ;
int  morefactors(char *) ;
int  is_integer(char *) ;
char *separateLine(char line1[], int length, bool nw);

typedef struct ID{
    char *name;
    //these dimensions can be 0 for scalars
    int row;
    int col;
};

struct ID IDs[MAXIDS] ;

int main(int argc,char *argv[])
{
    int numtokens ;
    // read the tokens
    numtokens = 0 ;
    ////////////////////////////////////////
    char result[1000];
    strcpy(result, "");
    char *q;
    char *token ;
    FILE *fp;
    char buff[80];
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

    int t = 0;
    while(t++ < 6){
        fgets(buff,80,fp);
        q = separateLine(buff, strlen(buff), false) ;
        // printf("51 - q : %s\n", q);
        numtokens = 0;
        memset(tokens, 0, sizeof(tokens[0][0]) * strlen(tokens[0]) * TOKENLENGTH);
        cur = 0;
        while( (token = strsep(&q," ")) != NULL ){
            while( (strcmp(token, " ") == 0) || (strcmp(token, "") == 0) || (strcmp(token, "\n") == 0)) {
                token = strsep(&str," ");
                if( token == NULL)
                    break;
            }
            // tokens[numtokens] = *token;
            if(token != NULL){
                strcpy(tokens[numtokens], token);
                numtokens++;
            }
            else break;
        }
        sprintf(tokens[numtokens],"$") ;
        numtokens++ ;
//        printf("%dst line:\n", t);
//        printf("initially:\t");
//        for(int i = 0; i < numtokens; i++){
//            printf("%s ", tokens[i]);
//        }
//        printf("\n");
        // parse the expression
        char str[N] = "";
        expr(str);
//        printf("result:\t\t%s\n\n",str) ;
        strcat(result, str);
        strcat(result, "\n");
    }
    printf("\nFinal file:\n%s\n", result);
    return(0) ;
}

int expr(char *str)
{
    char str1[N], str2[N] ;

    str1[0] = str2[0] = '\0' ;
    if (!  term(str1)) {
        return(0) ;
    }
    if (!  moreterms(str2)) {
        return(0) ;
    }
    strcat(str1,str2) ;
    strcpy(str,str1) ;
    return(1) ;
}

int term(char *str)
{
    char str1[N], str2[N] ;

    str1[0] = str2[0] = '\0' ;
    if (!  factor(str1)) {
        return(0) ;
    }
    if (!  morefactors(str2)) {
        return(0) ;
    }

    strcat(str1,str2) ;
    strcpy(str,str1) ;
    return(1) ;
}

int moreterms(char *str)
{
    char str1[N], str2[N], str3[N] ;

    str1[0] = str2[0] = str3[0] = '\0' ;

    if ( (strcmp(tokens[cur],"+") == 0 ) || (strcmp(tokens[cur],"-") == 0 ) ) {
        strcpy(str1,tokens[cur]) ;
        strcat(str1," ") ;
        cur++ ;
        if (!  term(str2)) {
            return(0) ;
        }
        if (!  moreterms(str3)) {
            return(0) ;
        }
    }
    strcat(str2,str3) ;
    strcat(str2,str1) ;
    strcpy(str,str2) ;
    return(1) ;
}

int factor(char *str)
{
    char str1[N] ;

    str1[0] = '\0' ;

    // printf("141 - %s\n",tokens[cur]) ;
    if ( is_integer(tokens[cur])  ) {
        strcpy(str,tokens[cur]) ;
        strcat(str," ") ;
        cur++ ;
        return(1) ;
    }
    if ( strcmp(tokens[cur],"(") == 0 ) {
        cur++ ;
        if ( ! expr(str1) ) {
            return(0) ;
        }
        if ( strcmp(tokens[cur],")") != 0 ) {
            printf("Error: expecting paranthesis\n") ;
            return(0) ;
        }
        cur++ ;
        strcpy(str,str1) ;
        return(1) ;
    }
    //// From this point on, I added the parts for 'tr', 'choose' and 'sqrt'
    //// the parts for ( id | id[expr] | id[expr, expr] ) are left
    if( (strcmp(tokens[cur],"tr") == 0) || (strcmp(tokens[cur],"sqrt") == 0) ){
        char func[N] ;
        func[0] = '\0' ;
        strcpy(func,tokens[cur]) ;
        cur++;
        if ( strcmp(tokens[cur],"(") != 0 ) {
            printf("Error: expecting paranthesis\n") ;
            return(0) ;
        }
        cur++;
        if ( ! expr(str1) ) {
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
        return (1);
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
        if ( ! expr(str1) ) {
            return(0) ;
        }
        if ( strcmp(tokens[cur],",") != 0 ) {
            printf("Error: expecting comma\n") ;
            return(0) ;
        }
        cur++;
        // expr2:
        if ( ! expr(str2) ) {
            return(0) ;
        }
        if ( strcmp(tokens[cur],",") != 0 ) {
            printf("Error: expecting comma\n") ;
            return(0) ;
        }
        cur++;
        // expr3:
        if ( ! expr(str3) ) {
            return(0) ;
        }
        if ( strcmp(tokens[cur],",") != 0 ) {
            printf("Error: expecting comma\n") ;
            return(0) ;
        }
        cur++;
        // expr4:
        if ( ! expr(str4) ) {
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
        return (1);
    }

    //// if the current token is an id......

    //// if the current token is an id......
    //the parts for ( id | id[expr] | id[expr, expr] ) are left

    // adding to the str as <id>exp[]exp[] or <id>exp[] if vector
    int check = isID(tokens[cur]);
    if(check==-1){
        printf("ERROR: undefined id");
        return 0;

    }else if(IDs[check].col == 0){
        //variable is a scalar
        strcat(str, tokens[cur]);
        cur++;
        return 1;

    }else if(IDs[check].col == 1){
        //variable is a vector
        strcat(str, tokens[cur]);
        cur++;
        if(strcmp(tokens[cur], "[") != 0) {
            printf("ERROR expected expression");
            return 0;
        }
        cur++;
        if(!expr(str1)){
            printf("ERROR expected expression");
            return 0;
        }

        if(strcmp(tokens[cur], "]")!=0){
            printf("Error!! expected ]");
            return 0;
        }
        cur++;
        strcat(str, str1);
        strcat(str, " [] ");
        return 1;

    }else{
        //variable is a matrix
        strcat(str, tokens[cur]);
        cur++;
        char str2[N];
        str2[0] = '\0' ;

        if(strcmp(tokens[cur],"[")!=0){
            printf("error!, need [");
            return 0;
        }
        cur++;
        if(!expr(str1)){
            printf("error!, need expr");
            return 0;
        }
        if(strcmp(tokens[cur],",")!=0){
            printf("error!, need comma");
            return 0;
        }
        cur++;
        if(!expr(str2)){
            printf("error!, need expr");
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
    }

    printf("Error: expecting factor\n") ;
    return(0) ;
}



int morefactors(char *str)
{
    char str1[N], str2[N], str3[N] ;

    str1[0] = str2[0] = str3[0] = '\0' ;

    if ( (strcmp(tokens[cur],"*") == 0 ) /*|| (strcmp(tokens[cur],"/") == 0 )*/ ) {
        strcpy(str1,tokens[cur]) ;
        strcat(str1," ") ;
        cur++ ;
        if (!  factor(str2)) {
            return(0) ;
        }
        if (!  morefactors(str3)) {
            return(0) ;
        }
    }
    strcat(str2,str3) ;
    strcat(str2,str1) ;
    strcpy(str,str2) ;
    return(1) ;
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


char *separateLine(char line[], int length, bool nw){
    if(strcat(line, "\n") == 0) return NULL;
    static char result[256];
    strcpy(result, "");
    for(int i = 0; i < length; i++) {
        //// MAYBE if line[i] == " ", we can pass that to avoid multiple spaces. TRY IT::
        if(line[i] != ' '){
            if (!(isalnum(line[i]) | (line[i]=='.'))) {
                strcat(result, " ");
                nw = true;
            } else {
                if (nw) strcat(result, " ");
                nw = false;
            }
            strncat(result, &line[i], 1);
        }
    }
    return result;
}

int isID(char *s){
    for(int i=0; i<sizeof(IDs)/sizeof(IDs[0]); i++){
        if(strcmp(s, IDs[i].name)==0)
            return i;
    }
    return -1;
}

