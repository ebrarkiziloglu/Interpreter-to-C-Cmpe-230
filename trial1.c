#include <stdio.h>
#include <string.h>

int main() {
    printf("Hello, World!\n");

}

// SYNTAX ERROR!!
// default value for variables is 0

// matlang file read
// c file create


//e definition (scalar, vector, matrix)  &1
//b declaration  &3
//e transpose  &2
//b square root &1
//b for 1    &2
//e for 2 (nested)   &4
//b print &1
//e operation + -  &3
//b operation *    &5
//e choose &1
//b printsep &0
//b # check  &1
//e check for operations + - * &2

// makefile

// documentation



//make
//./matlang2c file.mat
//gcc file.c -o prog && ./prog


//gcc main.c -o matlang2c
//./matlang2c file.mat
//gcc file.c -o prog && ./prog




////e definition (scalar, vector, matrix)  &1:
// this function is tested with the "  fgets(buff, 300, file); "  function in the main method, where buff is defined as "  char buff[300];  "
// this function reads the variable definitions and converts them to C language.
char * defineVar(char buff[]){
    int length = strlen(buff);
    buff[length-1] = '\0';      // removing '\n' char at the end
    char type[6];
    char name[length-7];
    strncpy(type, buff, 6);     // type indicates the type of the variable

    // scalar    x          -->         float x;
    // vector    y[4]       -->         float y[4][1];          // We're adding the 2nd dimensional, to facilitate transposing later.
    // matrix    z[3,4]     -->         float x[3][4];

    char line[length+13];               // line will store the C code
    strcpy(line, "float ");             // adding 'float ' at the beginning

    int startIndex, endIndex;
    char dimension[length-8];
    char dimension1[length-8];
    char dimension2[length-8];
    if(type[0] != 's'){                                 // vector or matrix
        startIndex = (int) (strchr(buff, '[') - buff);
        strncpy(name, buff+7, startIndex-7);
        strcat(line, name);
        endIndex = (int) (strchr(buff, ']') - buff);
        // printf("endindex: %d\n", endIndex);
        strncpy(dimension, buff+startIndex, endIndex+1-startIndex);
        // printf("dimension: %s\n", dimension);
        if(type[0] == 'v'){                             // vector
            strcat(line, dimension);
            strcat(line, "[1]");
        } else{                                         // matrix
            int commaIndex = (int)(strchr(buff, ',') - buff);
            strncpy(dimension1, buff+startIndex, commaIndex-startIndex);
            strncpy(dimension2, buff+commaIndex+1, endIndex+1-commaIndex);
            printf("1 : %s\n", dimension1);
            printf("2 : %s\n", dimension2);
            strcat(line, dimension1);
            strcat(line, "][");
            strcat(line, dimension2);
        }
    } else{                                             // scalar
        strcpy(name, buff+7);
        strcat(line, name);

    }
    strcat(line, ";\n");
    printf("buff is: \" %s \"  ||  line is: \" %s \" \n", buff, line);
    // Examining the example testcases given, I think giving the type 'float' will be OK.
    // But we can ask whether a char, or a vector of chars can be given as well.
    return line;
}

// checks if the line contains comment
// returns the non-comment part of the line
char *checkForComment(char line[]){
    char *ptr = strchr(line, '#');
    char *res;

    //if # is not found at all, returns the original line
    if(ptr != NULL){

        if(line[0]=='#'){
            line = "";
        }else{
            char *token = strtok(line, "#");
            line = token;
        }
    }
    res = line;
    return res;
}

char *printSep(){
    char *p;
    p = "printf(\"----------\\n\");\n";
    return p;
}



