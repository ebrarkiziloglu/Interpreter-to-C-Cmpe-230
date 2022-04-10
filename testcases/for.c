else if(strcmp(tokens[cur], "for")==0){
        char *forStr = "";
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
            sprintf(tokens[colonIndex],"$");
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
            sprintf(tokens[colonIndex],"$");
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
            sprintf(tokens[commaIndex],"$");
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
            sprintf(tokens[colonIndex],"$");
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
            sprintf(tokens[colonIndex],"$");

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
            sprintf(tokens[colonIndex], "$");
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
            sprintf(tokens[colonIndex], "$");
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
