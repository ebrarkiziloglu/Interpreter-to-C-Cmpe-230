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
}
