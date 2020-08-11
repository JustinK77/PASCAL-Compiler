#include <stdio.h>
#include <string.h>
#include <stdlib.h>

struct token{
    char name[20];
    char token[10];
    int tokenID;
    char attr[10];
    int attrID;
};

struct stackNode{
    char name[50];
    int type; //1 = INT, 2 = REAL, 3 = AINT, 4 = AREAL, add 4 for PP (-1 for green?)
    struct token *tokenContained;
    struct stackNode *under;
    char profile[10]; 
};

struct token curToken;
FILE* tokenFile;
FILE* listingFile;
FILE* newListingFile;
int lineNumber = 1;
char errorText[100] = "";
struct stackNode *eye;
struct stackNode *greenNodeStackTop;
struct stackNode *allNodeStackTop;


void checkAddGreenNode(struct stackNode *nodeToCheck){   //1 = Good, 0 = Error/cant add
    int returnVal = 1;
    struct stackNode *curGreenNode = greenNodeStackTop; 
    while(curGreenNode != NULL){
        if(strcmp(curGreenNode->name, nodeToCheck->name) == 0)  //duplicate detected
            fputs("SEMERR green node", listingFile);
            returnVal = 0;
        curGreenNode = curGreenNode->under;
    }

    struct stackNode greenNodeStackToAdd = *nodeToCheck;
    struct stackNode allNodeStackToAdd = *nodeToCheck;

    if(greenNodeStackTop == NULL){
        greenNodeStackTop = &greenNodeStackToAdd;
    }
    else{
        greenNodeStackToAdd.under = greenNodeStackTop;
        greenNodeStackTop = &greenNodeStackToAdd;
    }

    if(allNodeStackTop == NULL){
        allNodeStackTop = &greenNodeStackToAdd;
    }
    else{
        greenNodeStackToAdd.under = allNodeStackTop;
        allNodeStackTop = &allNodeStackToAdd;
    }

}

void checkAddBlueNode(struct stackNode *nodeToCheck){   //1 = Good, 0 = Error/cant add
    int returnVal = 1;
    struct stackNode *curNode = allNodeStackTop; 
    while(nodeToCheck->type != -1){
        if(strcmp(curNode->name, nodeToCheck->name) == 0)  //same name of variable
            fputs("SEMERR for bluenodes\n", listingFile);
            returnVal = 0;
        curNode = curNode->under;
    }

    struct stackNode nodeToAdd = *nodeToCheck;
    if (allNodeStackTop == NULL){
        allNodeStackTop = &nodeToAdd;
    }
    else{
        nodeToAdd.under = allNodeStackTop;
        allNodeStackTop = &nodeToAdd;
    }

}


struct token getToken(){
    char name[40], token[20], attr[20];
    int tokenID, attrID, lineNum;
    struct token returnToken;
    fscanf(tokenFile, "%d %s %s %d %s %d", &lineNum, name, token, &tokenID, attr, &attrID);
    if(lineNumber != lineNum  && tokenID != 200){
        while(lineNumber != lineNum){
            printf("current line %d\n", lineNumber);
            lineNumber++;
            char buffer[200];
            fgets(buffer, 200, listingFile);
            while(buffer[0] == 'L' || buffer[0] == 'S'){
                fputs(buffer, newListingFile);
                fgets(buffer, 200, listingFile);
            }
            printf(buffer);
            fputs(buffer, newListingFile);
        }
    }
    strcpy(returnToken.name, name);
    strcpy(returnToken.token, token);
    returnToken.tokenID = tokenID;
    strcpy(returnToken.attr, attr);
    returnToken.attrID = attrID;
    printf("Now looking for: %s\n", returnToken.name);
    return returnToken;
}


void match(char tokenName[]){ //tn = id, curTokenname = PROG, 10/150
    if((strcmp(curToken.name, tokenName) == 0) || ((strcmp(tokenName, "id") == 0) && (curToken.tokenID == 250)) || ((strcmp(tokenName, "id") == 0) && (curToken.tokenID == 99) && (curToken.attrID == 999)) || ((strcmp(tokenName, "num") == 0) && ((curToken.tokenID == 2) || (curToken.tokenID == 99))) || ((strcmp(tokenName, "relop") == 0) && (curToken.tokenID == 50)) || ((strcmp(tokenName, "addop") == 0) && (curToken.tokenID == 30)) || ((strcmp(tokenName, "mulop") == 0) && (curToken.tokenID == 40))){
        printf("Successful match for %s\n", tokenName);
        if (strcmp(tokenName, "EndOfFile") != 0)
            curToken = getToken();
    }
    else
    {
        strcpy(errorText, "");
        strcat(errorText, "SYNERR: on token '");
        strcat(errorText, curToken.name);
        strcat(errorText, "', did you mean '");
        strcat(errorText, tokenName);
        strcat(errorText, "'?\n");
        fputs(errorText, newListingFile);
        printf("| | | no match between %s and %s\n", tokenName, curToken.name);
        if (strcmp(curToken.name, "EndOfFile") != 0)
            curToken = getToken();
            // if(strcmp(tokenName, "EndOfFile") == 0)
            //     match("EndOfFile");

    }
    

}


void idlistTail();
void idList();
int type();
void decsTail();
void decs();
int stdtype();
void subpdecsTail();
void subpdecs();
void subpdecTwoTail();
void subpdecTail();
void subpdec();
void subpheadTail();
void subphead();
void args();
void sign();
void prog();
void progTail();
void progTwoTail();
int expr();
int expTail();
void varb();
void varbTail();
int factor();
int factorTail();
void explist();
void explistTail();
int term();
int termTail();
void pcdrstmt();
void pcdrstmtTail();
void stmt();
void stmtTail();
void cmpdstmt();
void idlist();
void paramlist();
void optstmts();
void stmtlist();


void progTwoTail(){
    switch(curToken.tokenID){
        case(17):
            subpdecs();
            cmpdstmt();
            match(".");
            break;
        case(18):
            cmpdstmt();
            match(".");
            break;
        default:
            printf("PRM with %s in %s\n", curToken.name, __func__);
            strcpy(errorText, "SYNERR: on token ");
            strcat(errorText, curToken.name);
            strcat(errorText, ": Switching to Panic Recovery Mode!\n");
            fputs(errorText, newListingFile);
            while(strcmp(curToken.name, "EndOfFile") != 0)
                curToken = getToken();
    }
}

void progTail(){
    switch(curToken.tokenID){
        case(11):
            decs();
            progTwoTail();
            break;

        case(17):
            subpdecs();
            cmpdstmt();
            match(".");
            break;

        case(18):
            cmpdstmt();
            match(".");
            break;
        default:
            printf("PRM with %s in %s\n", curToken.name, __func__);
            strcpy(errorText, "SYNERR: on token ");
            strcat(errorText, curToken.name);
            strcat(errorText, ": Switching to Panic Recovery Mode!\n");
            fputs(errorText, newListingFile);
            while(strcmp(curToken.name, "EndOfFile") != 0)
                curToken = getToken();  
    }

}

void prog(){
    
    switch(curToken.tokenID){

        case(10):
            match("program");
            match("id");
            match("(");
            idlist();
            match(")");
            match(";");
            progTail(); 
            break;

        default:
            printf("PRM with %s in %s\n", curToken.name, __func__);
            strcpy(errorText, "SYNERR: on token ");
            strcat(errorText, curToken.name);
            strcat(errorText, ": Switching to Panic Recovery Mode!\n");
            fputs(errorText, newListingFile);
            while(strcmp(curToken.name, "EndOfFile") != 0)
                curToken = getToken();
    }

}


void idlistTail(){
    switch(curToken.tokenID){
        case(31):
            match(",");
            match("id");
            idlistTail();
            break;
        case(27):
            break;
        default:
            printf("PRM with %s in %s\n", curToken.name, __func__);
            strcpy(errorText, "SYNERR: on token ");
            strcat(errorText, curToken.name);
            strcat(errorText, ": Switching to Panic Recovery Mode!\n");
            fputs(errorText, newListingFile);
            while(strcmp(curToken.name, ")") != 0 && strcmp(curToken.name, "EndOfFile") != 0 )
                curToken = getToken();
    }
}

void idlist(){
    
    switch(curToken.tokenID){
        case(250):
            match("id");
            idlistTail();
            break;
        default:
            printf("PRM with %s in %s\n", curToken.name, __func__);
            strcpy(errorText, "SYNERR: on token ");
            strcat(errorText, curToken.name);
            strcat(errorText, ": Switching to Panic Recovery Mode!\n");
            fputs(errorText, newListingFile);
            while(strcmp(curToken.name, ")") != 0 && strcmp(curToken.name, "EndOfFile") != 0)
                curToken = getToken();
    }
}


int type(){
    switch(curToken.tokenID){
        case(14):
            int t = stdtype();
            return t;
            break;
        case(15):
            int t = stdtype();
            return t;
            break;
        case(12):
            match("array");
            match("[");
            match("num");
            match("..");
            match("num");
            match("]");
            match("of");
            int t = stdtype() + 2;
            return t;
            break;
        default:
            printf("PRM with %s in %s\n", curToken.name, __func__);
            strcpy(errorText, "SYNERR: on token ");
            strcat(errorText, curToken.name);
            strcat(errorText, ": Switching to Panic Recovery Mode!\n");
            fputs(errorText, newListingFile);
            while(strcmp(curToken.name, ";") != 0 && strcmp(curToken.name, "EndOfFile") != 0){
                curToken = getToken();
            }
    }
}


void decsTail(){
    switch(curToken.tokenID){
        case(11):
            match("var");
            match("id");
            match(":");
            type();
            match(";");
            decsTail();
            break;
        case(17):
            break;
        case(18):
            break;
        default:
            printf("PRM with %s in %s\n", curToken.name, __func__);
            strcpy(errorText, "SYNERR: on token ");
            strcat(errorText, curToken.name);
            strcat(errorText, ": Switching to Panic Recovery Mode!\n");
            fputs(errorText, newListingFile);
            while(strcmp(curToken.name, "procedure") != 0 && strcmp(curToken.name, "begin") != 0 && strcmp(curToken.name, "EndOfFile") != 0)
                curToken = getToken();
    }
}

void decs(){
    switch(curToken.tokenID){
        case(11):
            struct stackNode toAdd;
            match("var");
            strcpy(toAdd.name, curToken.name);
            match("id");
            match(":");
            type(); //store type
            checkAddBlueNode(&toAdd);
            match(";");
            decsTail();
            break;
        default:
            printf("PRM with %s in %s\n", curToken.name, __func__);
            strcpy(errorText, "SYNERR: on token ");
            strcat(errorText, curToken.name);
            strcat(errorText, ": Switching to Panic Recovery Mode!\n");
            fputs(errorText, newListingFile);
            while(strcmp(curToken.name, "procedure") != 0 && strcmp(curToken.name, "begin") != 0 && strcmp(curToken.name, "EndOfFile") != 0)
                curToken = getToken();
    }
}


int stdtype(){
    switch(curToken.tokenID){
        case(14):
            match("integer");
            return 1;
        case(15):
            match("real");
            return 2;
            break;
        default:
            printf("PRM with %s in %s\n", curToken.name, __func__);
            strcpy(errorText, "SYNERR: on token ");
            strcat(errorText, curToken.name);
            strcat(errorText, ": Switching to Panic Recovery Mode!\n");
            fputs(errorText, newListingFile);
            while(strcmp(curToken.name, ";") != 0 && strcmp(curToken.name, "EndOfFile") != 0)
                curToken = getToken();
            return 9; //err
    }
}


void subpdecsTail(){
    switch(curToken.tokenID){
        case(17):
            subpdec();
            match(";");
            subpdecsTail();
            break;
        case(18):
            break;
        default:
            printf("PRM with %s in %s\n", curToken.name, __func__);
            strcpy(errorText, "SYNERR: on token ");
            strcat(errorText, curToken.name);
            strcat(errorText, ": Switching to Panic Recovery Mode!\n");
            fputs(errorText, newListingFile);
            while(strcmp(curToken.name, "begin") != 0 && strcmp(curToken.name, "EndOfFile") != 0)
                curToken = getToken();  
    }

}

void subpdecs(){
    switch(curToken.tokenID){
        case(17):
            subpdec();
            match(";");
            subpdecsTail();
            break;
        default:
            printf("PRM with %s in %s\n", curToken.name, __func__);
            strcpy(errorText, "SYNERR: on token ");
            strcat(errorText, curToken.name);
            strcat(errorText, ": Switching to Panic Recovery Mode!\n");
            fputs(errorText, newListingFile);
            while(strcmp(curToken.name, "begin") != 0 && strcmp(curToken.name, "EndOfFile") != 0)
                curToken = getToken();
    }
}


void subpdecTwoTail(){
    switch(curToken.tokenID){
        case(17):
            subpdecs();
            cmpdstmt();
            greenNodeStackTop = greenNodeStackTop->under; //pop GNS
            break;
        case(18):
            cmpdstmt();
            greenNodeStackTop = greenNodeStackTop->under; //pop GNS
            break;
        default:
            printf("PRM with %s in %s\n", curToken.name, __func__);
            strcpy(errorText, "SYNERR: on token ");
            strcat(errorText, curToken.name);
            strcat(errorText, ": Switching to Panic Recovery Mode!\n");
            fputs(errorText, newListingFile);
            while(strcmp(curToken.name, ";") != 0 && strcmp(curToken.name, "EndOfFile") != 0)
                curToken = getToken();
    }
}

void subpdecTail(){
    switch(curToken.tokenID){
        case(17):
            subpdecs();
            cmpdstmt();
            greenNodeStackTop = greenNodeStackTop->under; //pop GNS
            break;
        case(18):
            cmpdstmt();
            greenNodeStackTop = greenNodeStackTop->under; //pop GNS
            break;
        case(11):
            decs();
            subpdecTwoTail();
            break;
        default:
            printf("PRM with %s in %s\n", curToken.name, __func__);
            strcpy(errorText, "SYNERR: on token ");
            strcat(errorText, curToken.name);
            strcat(errorText, ": Switching to Panic Recovery Mode!\n");
            fputs(errorText, newListingFile);
            while(strcmp(curToken.name, ";") != 0 && strcmp(curToken.name, "EndOfFile") != 0)
                curToken = getToken();   
    }

}

void subpdec(){
    switch(curToken.tokenID){
        case(17):
            subphead();
            subpdecTail();
            break;
        default:
            printf("PRM with %s in %s\n", curToken.name, __func__);
            strcpy(errorText, "SYNERR: on token ");
            strcat(errorText, curToken.name);
            strcat(errorText, ": Switching to Panic Recovery Mode!\n");
            fputs(errorText, newListingFile);
            while(strcmp(curToken.name, ";") != 0 && strcmp(curToken.name, "EndOfFile") != 0)
                curToken = getToken();
    }
}


void subpheadTail(){
    switch(curToken.tokenID){
        case(26):
            args();
            match(";");
            break;
        case(28):
            match(";");
            break;
        default:
            printf("PRM with %s in %s\n", curToken.name, __func__);
            strcpy(errorText, "SYNERR: on token ");
            strcat(errorText, curToken.name);
            strcat(errorText, ": Switching to Panic Recovery Mode!\n");
            fputs(errorText, newListingFile);
            while(strcmp(curToken.name, "procedure") != 0 && strcmp(curToken.name, "begin") != 0 && strcmp(curToken.name, "var") != 0 && strcmp(curToken.name, "EndOfFile") != 0)
                curToken = getToken();  
    }

}

void subphead(){
    switch(curToken.tokenID){
        case(17):
            match("procedure");
            struct stackNode nodeToCheck;
            strcpy(nodeToCheck.name, curToken.name);
            nodeToCheck.type = -1;
            checkAddGreenNode(&nodeToCheck);
            match("id");
            subpheadTail();
            break;
        default:
            printf("PRM with %s in %s\n", curToken.name, __func__);
            strcpy(errorText, "SYNERR: on token ");
            strcat(errorText, curToken.name);
            strcat(errorText, ": Switching to Panic Recovery Mode!\n");
            fputs(errorText, newListingFile);
            while(strcmp(curToken.name, "procedure") != 0 && strcmp(curToken.name, "begin") != 0 && strcmp(curToken.name, "var") != 0 && strcmp(curToken.name, "EndOfFile") != 0)
                curToken = getToken();
    }
}


void args(){
    switch(curToken.tokenID){
        case(26):
            match("(");
            paramlist();
            match(")");
            break;
        default:
            printf("PRM with %s in %s\n", curToken.name, __func__);
            strcpy(errorText, "SYNERR: on token ");
            strcat(errorText, curToken.name);
            strcat(errorText, ": Switching to Panic Recovery Mode!\n");
            fputs(errorText, newListingFile);
            while(strcmp(curToken.name, ";") != 0 && strcmp(curToken.name, "EndOfFile") != 0)
                curToken = getToken();
    }
}


void paramlistTail(){
    switch(curToken.tokenID){
        case(28):
            match(";");
            match("id");
            match(":");
            type();
            paramlistTail();
            break;   
        case(27):
            break;
        default:
            printf("PRM with %s in %s\n", curToken.name, __func__);
            strcpy(errorText, "SYNERR: on token ");
            strcat(errorText, curToken.name);
            strcat(errorText, ": Switching to Panic Recovery Mode!\n");
            fputs(errorText, newListingFile);
            while(strcmp(curToken.name, ")") != 0 && strcmp(curToken.name, "EndOfFile") != 0)
                curToken = getToken();
    }
}

void paramlist(){
    switch(curToken.tokenID){
        case(250):
            match("id");
            match(":");
            type();
            paramlistTail();
            break;
        default:
            printf("PRM with %s in %s\n", curToken.name, __func__);
            strcpy(errorText, "SYNERR: on token ");
            strcat(errorText, curToken.name);
            strcat(errorText, ": Switching to Panic Recovery Mode!\n");
            fputs(errorText, newListingFile);
            while(strcmp(curToken.name, ")") != 0 && strcmp(curToken.name, "EndOfFile") != 0)
                curToken = getToken();
    }
}


void cmpdstmtTail(){
    switch(curToken.tokenID){
        case(19):
            match("end");
            break;
        case(250):
            optstmts();
            match("end");
            break;   
        case(41):
            optstmts();
            match("end");
            break; 
        case(18):
            optstmts();
            match("end");
            break; 
        case(23):
            optstmts();
            match("end");
            break; 
        case(20):
            optstmts();
            match("end");
            break; 
        default:
            printf("PRM with %s in %s\n", curToken.name, __func__);
            strcpy(errorText, "SYNERR: on token ");
            strcat(errorText, curToken.name);
            strcat(errorText, ": Switching to Panic Recovery Mode!\n");
            fputs(errorText, newListingFile);
            while(strcmp(curToken.name, ";") != 0 && strcmp(curToken.name, "end") != 0 && strcmp(curToken.name, "else") != 0 && strcmp(curToken.name, "EndOfFile") != 0)
                curToken = getToken();
    }

}

void cmpdstmt(){
    switch(curToken.tokenID){
        case(18):
            match("begin");
            cmpdstmtTail();
            break;
        default:
            printf("PRM with %s in %s\n", curToken.name, __func__);
            strcpy(errorText, "SYNERR: on token ");
            strcat(errorText, curToken.name);
            strcat(errorText, ": Switching to Panic Recovery Mode!\n");
            fputs(errorText, newListingFile);
            while(strcmp(curToken.name, ";") != 0 && strcmp(curToken.name, "end") != 0 && strcmp(curToken.name, "else") != 0 && strcmp(curToken.name, "EndOfFile") != 0)
                curToken = getToken();
    }
}


void optstmts(){
    switch(curToken.tokenID){
        case(250):
            stmtlist();
            break;
        case(41):
            stmtlist();
            break;
        case(18):
            stmtlist();
            break;
        case(23):
            stmtlist();
            break;
        case(20):
            stmtlist();
            break;
        default:
            printf("PRM with %s in %s\n", curToken.name, __func__);
            strcpy(errorText, "SYNERR: on token ");
            strcat(errorText, curToken.name);
            strcat(errorText, ": Switching to Panic Recovery Mode!\n");
            fputs(errorText, newListingFile);
            while(strcmp(curToken.name, "end") != 0 && strcmp(curToken.name, "EndOfFile") != 0)
                curToken = getToken();
    }
}


void stmtlistTail(){
    switch(curToken.tokenID){
        case(28):
            match(";");
            stmt();
            stmtlistTail();
            break;
        case(19):
            break;
        default:
            printf("PRM with %s in %s\n", curToken.name, __func__);
            strcpy(errorText, "SYNERR: on token ");
            strcat(errorText, curToken.name);
            strcat(errorText, ": Switching to Panic Recovery Mode!\n");
            fputs(errorText, newListingFile);
            while(strcmp(curToken.name, "end") != 0 && strcmp(curToken.name, "EndOfFile") != 0)
                curToken = getToken();
    }
}

void stmtlist(){
    switch(curToken.tokenID){
        case(250):
            stmt();
            stmtlistTail();
            break;
        case(41):
            stmt();
            stmtlistTail();
            break;
        case(18):
            stmt();
            stmtlistTail();
            break;
        case(23):
            stmt();
            stmtlistTail();
            break;
        case(20):
            stmt();
            stmtlistTail();
            break;
        default:
            printf("PRM with %s in %s\n", curToken.name, __func__);
            strcpy(errorText, "SYNERR: on token ");
            strcat(errorText, curToken.name);
            strcat(errorText, ": Switching to Panic Recovery Mode!\n");
            fputs(errorText, newListingFile);
            while(strcmp(curToken.name, "end") != 0 && strcmp(curToken.name, "EndOfFile") != 0)
                curToken = getToken();
    }

}


void stmtTail(){
    switch(curToken.tokenID){
        case(22):
            match("else");
            stmt();
            break;
        case(28):
            break;
        case(19):
            break;
        // case(22):  //DANGLING ELSE
        //     return;
        default:
            printf("PRM with %s in %s\n", curToken.name, __func__);
            strcpy(errorText, "SYNERR: on token ");
            strcat(errorText, curToken.name);
            strcat(errorText, ": Switching to Panic Recovery Mode!\n");
            fputs(errorText, newListingFile);
            while(strcmp(curToken.name, "end") != 0 && strcmp(curToken.name, ";") != 0 && strcmp(curToken.name, "else") != 0&& strcmp(curToken.name, "EndOfFile") != 0)
                curToken = getToken();
    }
}

void stmt(){
    switch(curToken.tokenID){
        case(250):
            varb();
            match(":=");
            expr();
            break;
        case(41):
            pcdrstmt();
            break;
        case(18):
            cmpdstmt();
            break;
        case(23):
            match("while");
            expr();
            match("do");
            stmt();
            break;
        case(20):
            match("if");
            expr();
            match("then");
            stmt();
            stmtTail();
            break;
        default:
            printf("PRM with %s in %s\n", curToken.name, __func__);
            strcpy(errorText, "SYNERR: on token ");
            strcat(errorText, curToken.name);
            strcat(errorText, ": Switching to Panic Recovery Mode!\n");
            fputs(errorText, newListingFile);
            while(strcmp(curToken.name, "end") != 0 && strcmp(curToken.name, ";") != 0 && strcmp(curToken.name, "else") != 0&& strcmp(curToken.name, "EndOfFile") != 0)
                curToken = getToken();
    }
}


void varbTail(){
    switch(curToken.tokenID){
        case(33):
            printf("using varbTail");
            match("[");
            expr();
            match("]");
            break;
        case(36):
            break;
        default:
            printf("PRM with %s in %s\n", curToken.name, __func__);
            strcpy(errorText, "SYNERR: on token ");
            strcat(errorText, curToken.name);
            strcat(errorText, ": Switching to Panic Recovery Mode!\n");
            fputs(errorText, newListingFile);
            while(strcmp(curToken.name, ":=") != 0&& strcmp(curToken.name, "EndOfFile") != 0)
                curToken = getToken();
    }

}

void varb(){
    switch(curToken.tokenID){
        case(250):
            match("id");
            varbTail();
            break;
        default:
            printf("PRM with %s in %s\n", curToken.name, __func__);
            strcpy(errorText, "SYNERR: on token ");
            strcat(errorText, curToken.name);
            strcat(errorText, ": Switching to Panic Recovery Mode!\n");
            fputs(errorText, newListingFile);
            while(strcmp(curToken.name, ":=") != 0&& strcmp(curToken.name, "EndOfFile") != 0)
                curToken = getToken();
    }
}


void pcdrstmtTail(){
    switch(curToken.tokenID){
        case(26):
            match("(");
            explist();
            match(")");
            break;
        case(28):
            break;
        case(19):
            break;
        case(22):
            break;
        default:
            printf("PRM with %s in %s\n", curToken.name, __func__);
            strcpy(errorText, "SYNERR: on token ");
            strcat(errorText, curToken.name);
            strcat(errorText, ": Switching to Panic Recovery Mode!\n");
            fputs(errorText, newListingFile);
            while(strcmp(curToken.name, "end") != 0 && strcmp(curToken.name, ";") != 0 && strcmp(curToken.name, "else") != 0&& strcmp(curToken.name, "EndOfFile") != 0)
                curToken = getToken();
    }
}

void pcdrstmt(){
    switch(curToken.tokenID){
        case(41):
            match("call");
            match("id");
            pcdrstmtTail();
            break;
        default:
            printf("PRM with %s in %s\n", curToken.name, __func__);
            strcpy(errorText, "SYNERR: on token ");
            strcat(errorText, curToken.name);
            strcat(errorText, ": Switching to Panic Recovery Mode!\n");
            fputs(errorText, newListingFile);
            while(strcmp(curToken.name, "end") != 0 && strcmp(curToken.name, ";") != 0 && strcmp(curToken.name, "else") != 0&& strcmp(curToken.name, "EndOfFile") != 0)
                curToken = getToken();
    }
}


void explistTail(){
    switch(curToken.tokenID){
        case(31):
            match(",");
            expr();
            explistTail();
            break;
        case(27):
            break;
        default:
            printf("PRM with %s in %s\n", curToken.name, __func__);
            strcpy(errorText, "SYNERR: on token ");
            strcat(errorText, curToken.name);
            strcat(errorText, ": Switching to Panic Recovery Mode!\n");
            fputs(errorText, newListingFile);
            while(strcmp(curToken.name, ")") != 0 && strcmp(curToken.name, "EndOfFile") != 0)
                curToken = getToken();
    }
}

void explist(){
    switch(curToken.tokenID){
        case(30):
            expr();
            explistTail();
            break;
        case(250):
            expr();
            explistTail();
            break;
        case(2):
            expr();
            explistTail();
            break;
        case(26):
            expr();
            explistTail();
            break;
        case(25):
            expr();
            explistTail();
            break;
        default:
            printf("PRM with %s in %s\n", curToken.name, __func__);
            strcpy(errorText, "SYNERR: on token ");
            strcat(errorText, curToken.name);
            strcat(errorText, ": Switching to Panic Recovery Mode!\n");
            fputs(errorText, newListingFile);
            while(strcmp(curToken.name, ")") != 0 && strcmp(curToken.name, "EndOfFile") != 0)
                curToken = getToken();
    }
}


int smpexpTail(){
    switch(curToken.tokenID){
        case(30):
            match("addop");
            int i = term();
            switch(i){
                case 1:
                case 2:
                case 5:
                case 6:
                case 9:
                    break;
                case 11:
                    i = 9;
                    break;
                default:
                    fputs("SEMERR: Non-Num in Addop\n", listingFile);
                    i = 11;
            }
            int iSmpexpTail = smpexpTail();
            if(iSmpexpTail != 11 && iSmpexpTail != 9 && i != 11 && i != 9)  //This is assuming an int can be added to a real, etc. 
                return i;
            else 
                return 9;
            break;
        case 50:
        case 31:
        case 27:
        case 28:
        case 19:
        case 24:
        case 21:
        case 22: 
        case 34:
            return 0;
        default:
            printf("PRM with %s in %s\n", curToken.name, __func__);
            strcpy(errorText, "SYNERR: on token ");
            strcat(errorText, curToken.name);
            strcat(errorText, ": Switching to Panic Recovery Mode!\n");
            fputs(errorText, newListingFile);
            while(strcmp(curToken.name, ")") != 0 && curToken.tokenID != 50 && strcmp(curToken.name, ",") != 0 && strcmp(curToken.name, "]") != 0 && strcmp(curToken.name, ";") != 0 && strcmp(curToken.name, "end") != 0 && strcmp(curToken.name, "do") != 0 && strcmp(curToken.name, "then") != 0 && strcmp(curToken.name, "else") != 0 && strcmp(curToken.name, "EndOfFile") != 0)
                curToken = getToken();
            return 9;

    }
}

int smpexp(){
    switch(curToken.tokenID){
        case 250:
        case 2:
        case 26:
        case 25:
            int i = term();
            switch(i){
                case 1:
                case 2:
                case 5:
                case 6:
                case 9:
                    break;
                case 11:
                    i = 9;
                    break;
                default:
                    fputs("SEMERR: Non-num in Smpexp\n", listingFile);
                    i = 11;
            }
            int iSmpexpTail = smpexpTail();
            if(iSmpexpTail != 11 && iSmpexpTail != 9 && i != 11 && i != 9)  //This is assuming an int can be added to a real, etc. 
                return i;
            else
                return 9; //error exists in one of the smpexps
            break;
        case(30):
            sign();
            int i = term();
            switch(i){
                case 1:
                case 2:
                case 5:
                case 6:
                case 9:
                    break;
                case 11:
                    i = 9;
                    break;
                default:
                    i = 11;
            }
            int iSmpexpTail = smpexpTail();
            if(iSmpexpTail != 11 && iSmpexpTail != 9 && i != 11 && i != 9)  //This is assuming an int can be added to a real, etc. 
                return i;
            break;
        default:
            printf("PRM with %s in %s\n", curToken.name, __func__);
            strcpy(errorText, "SYNERR: on token ");
            strcat(errorText, curToken.name);
            strcat(errorText, ": Switching to Panic Recovery Mode!\n");
            fputs(errorText, newListingFile);
            while(strcmp(curToken.name, ")") != 0 && curToken.tokenID != 50 && strcmp(curToken.name, ",") != 0 && strcmp(curToken.name, "]") != 0 && strcmp(curToken.name, ";") != 0 && strcmp(curToken.name, "end") != 0 && strcmp(curToken.name, "do") != 0 && strcmp(curToken.name, "then") != 0 && strcmp(curToken.name, "else") != 0 && strcmp(curToken.name, "EndOfFile") != 0)
                curToken = getToken();
    }

}


int termTail(){
    switch(curToken.tokenID){
        case(40):
            match("mulop");
            int i = factor();
            switch(i){
                case 1:
                case 2:
                case 5:
                case 6:
                case 9:
                case 11:
                    break;
                default:
                    fputs("Non-Num in TermTail", listingFile);
                    i = 11;
            }
            int iTermTail = termTail();
            if(i != 9 && i != 11 & iTermTail != 9 && iTermTail != 11)
                return i;
            else
                return 9;
            break;
        case 30:
        case 50:
        case 31:
        case 27:
        case 28:
        case 19:
        case 24:
        case 21:
        case 22:
        case 34:
            return 0;
        default:
            printf("PRM with %s in %s\n", curToken.name, __func__);
            strcpy(errorText, "SYNERR: on token ");
            strcat(errorText, curToken.name);
            strcat(errorText, ": Switching to Panic Recovery Mode!\n");
            fputs(errorText, newListingFile);
            while(strcmp(curToken.name, ")") != 0 && curToken.tokenID != 50 && curToken.tokenID != 30 && strcmp(curToken.name, ",") != 0 && strcmp(curToken.name, "]") != 0 && strcmp(curToken.name, ";") != 0 && strcmp(curToken.name, "end") != 0 && strcmp(curToken.name, "do") != 0 && strcmp(curToken.name, "then") != 0 && strcmp(curToken.name, "else") != 0 && strcmp(curToken.name, "EndOfFile") != 0)
                curToken = getToken();
            return 9;

    }
}

int term(){
    switch(curToken.tokenID){
        case 250:
        case 2:
        case 26:
        case 25:
            int i = factor();
            switch(i){
                case 1:
                case 2:
                case 5:
                case 6:
                case 9:
                case 11:
                    break;
                default:
                    fputs("Non-Num in Term", listingFile);
                    i = 11;
            }
            int iTermTail = termTail();
            if(i != 9 && i != 11 & iTermTail != 9 && iTermTail != 11)
                return i;
            else
                return 9;
            break;
        default:
            printf("PRM with %s in %s\n", curToken.name, __func__);
            strcpy(errorText, "SYNERR: on token ");
            strcat(errorText, curToken.name);
            strcat(errorText, ": Switching to Panic Recovery Mode!\n");
            fputs(errorText, newListingFile);
            while(strcmp(curToken.name, ")") != 0 && curToken.tokenID == 50 && curToken.tokenID == 30 && strcmp(curToken.name, ",") != 0 && strcmp(curToken.name, "]") != 0 && strcmp(curToken.name, ";") != 0 && strcmp(curToken.name, "end") != 0 && strcmp(curToken.name, "do") != 0 && strcmp(curToken.name, "then") != 0 && strcmp(curToken.name, "else") != 0 && strcmp(curToken.name, "EndOfFile") != 0)
                curToken = getToken();
    }
}


int factorTail(int i){
    switch(curToken.tokenID){
        case(33):
            match("[");
            int tExpr = expr();
            int t;
            if(tExpr == 1){
                switch (i){
                    case 3:
                        t = 1;
                    case 4:
                        t = 2;
                    default:
                        fputs("SEMERR: Non-Array Referenced as Array\n", listingFile);
                        t = 11; 
                }
            }
            else
                t = 9;
            match("]");
            return t;
        case(40):
            return i;
        case(30):
            return i;
        case(50):
            return i;
        case(31):
            return i;
        case(27):
            return i;
        case(28):
            return i;
        case(19):
            return i;
        case(24):
            return i;
        case(21):
            return i;
        case(22):
            return i;
        case(34):
            return i;
        default:
            printf("PRM with %s in %s\n", curToken.name, __func__);
            strcpy(errorText, "SYNERR: on token ");
            strcat(errorText, curToken.name);
            strcat(errorText, ": Switching to Panic Recovery Mode!\n");
            fputs(errorText, newListingFile);
            while(strcmp(curToken.name, ")") != 0 && curToken.tokenID != 50 && curToken.tokenID != 40 && curToken.tokenID != 30 && strcmp(curToken.name, ",") != 0 && strcmp(curToken.name, "]") != 0 && strcmp(curToken.name, ";") != 0 && strcmp(curToken.name, "end") != 0 && strcmp(curToken.name, "do") != 0 && strcmp(curToken.name, "then") != 0 && strcmp(curToken.name, "else") != 0 && strcmp(curToken.name, "EndOfFile") != 0)
                curToken = getToken();
            return 9;
    }

}

int factor(){
    switch(curToken.tokenID){
        case(250):
            int i = curToken.attrID; //CHANGE ID MACHINE, (id, Lexeme)
            match("id");
            int t = factorTail(i);
            return t;
        case(2):
            int t = curToken.attrID; //1 for int, 2 for real, converts to PP later
            match("num");
            return t;
        case(26):
            match("(");
            int t = expr();
            match(")");
            return t;
        case(25):
            match("not");
            int t = factor();
            switch (t){
                case 10: //bool
                    return 10;
                case 9: //ERR
                    return 9;
                default: //other, err*
                    fputs("SEMERR: using non-bool as bool \n", listingFile);
                    return 11;
            }
        default:
            printf("PRM with %s in %s\n", curToken.name, __func__);
            strcpy(errorText, "SYNERR: on token ");
            strcat(errorText, curToken.name);
            strcat(errorText, ": Switching to Panic Recovery Mode!\n");
            fputs(errorText, newListingFile);
            while(strcmp(curToken.name, ")") != 0 && curToken.tokenID != 50 && curToken.tokenID != 40 && curToken.tokenID != 30 && strcmp(curToken.name, ",") != 0 && strcmp(curToken.name, "]") != 0 && strcmp(curToken.name, ";") != 0 && strcmp(curToken.name, "end") != 0 && strcmp(curToken.name, "do") != 0 && strcmp(curToken.name, "then") != 0 && strcmp(curToken.name, "else") != 0 && strcmp(curToken.name, "EndOfFile") != 0)
                curToken = getToken();
            return 9;
    }
}


int exprTail(){

    switch(curToken.tokenID){
        case(50):
            match("relop");
            return smpexp();
        case(31):
            return 0;
        case(27):
            return 0;
        case(28):
            return 0;
        case(19):
            return 0;
        case(24):
            return 0;
        case(21):
            return 0;
        case(22):
            return 0;
        case(34):
            return 0;
        default:
            printf("PRM with %s in %s\n", curToken.name, __func__);
            strcpy(errorText, "SYNERR: on token ");
            strcat(errorText, curToken.name);
            strcat(errorText, ": Switching to Panic Recovery Mode!\n");
            fputs(errorText, newListingFile);
            while(strcmp(curToken.name, ")") != 0 && strcmp(curToken.name, ",") != 0 && strcmp(curToken.name, "]") != 0 && strcmp(curToken.name, ";") != 0 && strcmp(curToken.name, "end") != 0 && strcmp(curToken.name, "do") != 0 && strcmp(curToken.name, "then") != 0 && strcmp(curToken.name, "else") != 0 && strcmp(curToken.name, "EndOfFile") != 0)
                curToken = getToken();
    }
}

int expr(){
    switch(curToken.tokenID){
        case(30):
            int i = smpexp();
            if (i != 1 && i != 2 && i != 5 && i != 6){
                i = 11;
                fputs("SEMERR: Non-Num Used In Relational Expression \n", listingFile);
            }
            int expTailI = exprTail();
            if (expTailI == 0)
                return i;
            if(expTailI == 9 || expTailI == 11 || i == 9 || i == 11)
                return 9;
            if (expTailI == 1 || expTailI == 2 || expTailI == 5 || expTailI == 6){
                return 10; //BOOL
            }
            else{
                fputs("SEMERR: Non-Num in Second Half of Relational Expression \n", listingFile);
                return 11;
            }
        case(250):
            int i = smpexp();
            if (i != 1 && i != 2 && i != 5 && i != 6){
                i = 11;
                fputs("SEMERR: Non-Num Used In Relational Expression \n", listingFile);
            }
            int expTailI = exprTail();
            if (expTailI == 0)
                return i;
            if(expTailI == 9 || expTailI == 11 || i == 9 || i == 11)
                return 9;
            if (expTailI == 1 || expTailI == 2 || expTailI == 5 || expTailI == 6){
                return 10; //BOOL
            }
            else{
                fputs("SEMERR: Non-Num in Second Half of Relational Expression \n", listingFile);
                return 11;
            }
        case(2):
            int i = smpexp();
            if (i != 1 && i != 2 && i != 5 && i != 6){
                i = 11;
                fputs("SEMERR: Non-Num Used In Relational Expression \n", listingFile);
            }
            int expTailI = exprTail();
            if (expTailI == 0)
                return i;
            if(expTailI == 9 || expTailI == 11 || i == 9 || i == 11)
                return 9;
            if (expTailI == 1 || expTailI == 2 || expTailI == 5 || expTailI == 6){
                return 10; //BOOL
            }
            else{
                fputs("SEMERR: Non-Num in Second Half of Relational Expression \n", listingFile);
                return 11;
            }
        case(26):
            int i = smpexp();
            if (i != 1 && i != 2 && i != 5 && i != 6){
                i = 11;
                fputs("SEMERR: Non-Num Used In Relational Expression \n", listingFile);
            }
            int expTailI = exprTail();
            if (expTailI == 0)
                return i;
            if(expTailI == 9 || expTailI == 11 || i == 9 || i == 11)
                return 9;
            if (expTailI == 1 || expTailI == 2 || expTailI == 5 || expTailI == 6){
                return 10; //BOOL
            }
            else{
                fputs("SEMERR: Non-Num in Second Half of Relational Expression \n", listingFile);
                return 11;
            }
        case(25):
            int i = smpexp();
            if (i != 1 && i != 2 && i != 5 && i != 6){
                i = 11;
                fputs("SEMERR: Non-Num Used In Relational Expression \n", listingFile);
            }
            int expTailI = exprTail();
            if (expTailI == 0)
                return i;
            if(expTailI == 9 || expTailI == 11 || i == 9 || i == 11)
                return 9;
            if (expTailI == 1 || expTailI == 2 || expTailI == 5 || expTailI == 6){
                return 10; //BOOL
            }
            else{
                fputs("SEMERR: Non-Num in Second Half of Relational Expression \n", listingFile);
                return 11;
            }
        default:
            printf("PRM with %s in %s\n", curToken.name, __func__);
            strcpy(errorText, "SYNERR: on token ");
            strcat(errorText, curToken.name);
            strcat(errorText, ": Switching to Panic Recovery Mode!\n");
            fputs(errorText, newListingFile);
            while(strcmp(curToken.name, ")") != 0 && strcmp(curToken.name, ",") != 0 && strcmp(curToken.name, "]") != 0 && strcmp(curToken.name, ";") != 0 && strcmp(curToken.name, "end") != 0 && strcmp(curToken.name, "do") != 0 && strcmp(curToken.name, "then") != 0 && strcmp(curToken.name, "else") != 0 && strcmp(curToken.name, "EndOfFile") != 0)
                curToken = getToken();
            return 9;
    }
}


void sign(){
    switch(curToken.attrID){
        case(52):
            match("+");
            break;
        case(53):
            match("-");
            break;
        default:
            printf("PRM with %s in %s\n", curToken.name, __func__);
            strcpy(errorText, "SYNERR: on token ");
            strcat(errorText, curToken.name);
            strcat(errorText, ": Switching to Panic Recovery Mode!\n");
            fputs(errorText, newListingFile);
            while(curToken.tokenID == 2 && curToken.tokenID == 250 && (curToken.name, "(") != 0 && strcmp(curToken.name, "not") != 0 && strcmp(curToken.name, "EndOfFile") != 0)
                curToken = getToken();
    }

}



void parse(){
    prog();
    match("EndOfFile");
}


int main(){
    tokenFile = fopen("tokenFile.txt", "r");
    listingFile = fopen("listingFile.txt", "r");
    newListingFile = fopen("newListingFile.txt", "w");
    char buffer[72];
    fgets(buffer, 72, listingFile);
    fputs(buffer, newListingFile);

    char dummy[30];
    fscanf(tokenFile, "%s %s %s %s %s", dummy, dummy, dummy, dummy, dummy);

    curToken = getToken();

    parse();

    fclose(listingFile);
    fclose(newListingFile);
    strcpy(buffer, "");

    listingFile = fopen("listingFile.txt", "w");
    newListingFile = fopen("newListingFile.txt", "r");
    while(fgets(buffer, 72, newListingFile) != NULL){
        fputs(buffer, listingFile);
    }

}