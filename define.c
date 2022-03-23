
// This includes seperateLine and defineVariable functions.

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdlib.h>

char *separateLine(char line1[], int length, bool nw);
const char* defineVariable(char *str);
int isAlphabetic(char str[]);
int isNumber(char s[]);

int maxDimension = 0;

int main (int argc,char *argv[]) {
    char result[1000];
    strcpy(result, "");
    char *q;
    FILE *fp;
    char buff[80];
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

    while( strcmp(fgets(buff,80,fp), "\n")) {
        q = separateLine(buff, strlen(buff), false) ;
        line = defineVariable(q);
        // printf("**:\n%s\n", line);
        strcat(result, line);
    }
    printf("37: Result is:\n%s\n", result);
    printf("maxDimension: %d\n", maxDimension);
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
