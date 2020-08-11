
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#define NULLL 150
#define PROG 10
#define VAR 11
#define ARRAY 12
#define OF 13
#define INT 14
#define REAL 15
#define FUNC 16
#define PROC 17
#define BEGIN 18
#define END 19
#define IF 20
#define THEN 21
#define ELSE 22
#define WHILE 23
#define DO 24
#define NOT 25
#define OP 26
#define CP 27
#define SC 28
#define DOT 29
#define COMMA 31
#define COLON 32
#define OB 33
#define CB 34
#define DOTDOT 35
#define ASSIGNOP 36
#define ADDOP 30
#define MULOP 40
#define OR 51
#define MINUS 53
#define DIV 54
#define MOD 55
#define SLASH 56
#define STAR 57
#define EQ 58
#define NEQ 59
#define RELOP 50
#define LT 60
#define LTE 61
#define GT 62
#define GTE 63
#define LEXERR 99
#define ID 130



struct token{
    char name[20];
    char token[10];
    int tokenID;
    char attr[10];
    int attrID;
};

struct resWd{
    char name[20];
    char token[10];
    int tokenID;
    char attr[10];
    int attrID;
    struct resWd* next;
};

//global vars
FILE* srcProg;
FILE* listingFile;
FILE* tokenFile;
FILE* resWdList;
FILE* symbolTable;
int charIdx;
int numResWords;
struct resWd* beginningResWd; 
struct token idArray[200];
int idIdx= 0;


struct token catchallMachine(char proLine[], int* idx){

    struct token returnToken;
    printf("ERROR, catchallmachine invoked\n");
    strcpy(returnToken.name, "UnknownSymbol");
    strcpy(returnToken.token, "LEXERR");
    returnToken.tokenID = 99;
    strcpy(returnToken.attr, "UNKWNSYMBOL");
    returnToken.attrID = 20;
    return returnToken;
}


struct token idMachine(char proLine[], int* idx){

    int origIdx = *idx;
    char buffer[20] = "";
    int charIsAlnum = isalnum(proLine[*idx]);


    if(charIsAlnum == 0){
        return catchallMachine(proLine, idx);
    }

    while(charIsAlnum != 0){
        //printf("is alnum: %c", proLine[*idx]);
        strncat(buffer, &proLine[*idx], 1);
        charIdx++;
        charIsAlnum = isalnum(proLine[*idx]);
    }
    charIdx--;

    if (strlen(buffer) > 10){
        struct token returnToken;
        strcpy(returnToken.name, buffer);
        strcpy(returnToken.token, "LEXERR");
        returnToken.tokenID = 99;
        strcpy(returnToken.attr, "TooLongID");
        returnToken.attrID = 999; 
        printf("Too long ID detected: %s\n", returnToken.name);
        return returnToken;
    }
    struct token returnToken;
    strcpy(returnToken.name, buffer);
    strcpy(returnToken.token, "ID");
    returnToken.tokenID = 250;
    strcpy(returnToken.attr, "Address");
    returnToken.attrID = 9001; //placeholder until id address put in
    printf("ID detected: %s\n", returnToken.name);
    return returnToken;
}


struct token intMachine(char proLine[], int* idx){
    //printf("int mach w/ %c\n", proLine[*idx]);
    struct token returnToken;
    char buffer[40]= "";
    int numLength = 0;
    int origIdx = *idx;

    if (isdigit(proLine[*idx]) == 0){
        return idMachine(proLine, idx);
    }

    while(isdigit(proLine[*idx]) != 0){
        numLength++;
        strncat(buffer, &proLine[*idx], 1);
        charIdx++;
    }
    charIdx--;
    //simple int, 10 digit max
    strcpy(returnToken.name, buffer);
    strcpy(returnToken.token, "NUM");
    returnToken.tokenID = 2;
    strcpy(returnToken.attr, "INT");
    returnToken.attrID = 1;
    printf("INT detected: %s\n", buffer);

    if(numLength > 10){
        strcpy(returnToken.token, "LEXERR");
        returnToken.tokenID = 99;
        strcpy(returnToken.attr, "LongInt");
        returnToken.attrID = 2;
        fputs("LEXERR: More than 10 digits in Int\n", listingFile);
    }

    if(buffer[0] == '0' && strlen(buffer) != 1){
        strcpy(returnToken.token, "LEXERR");
        returnToken.tokenID = 99;
        strcpy(returnToken.attr, "LeadingZero");
        returnToken.attrID = 11;
        fputs("LEXERR: Leading Zeros\n", listingFile);
    }

    return returnToken;

}


struct token realMachine(char proLine[], int* idx){

    //listingFile = fopen("listingFile.txt", "w"); 
    struct token returnToken;
    char buffer[40]= "";
    int numLength = 0;
    int postDecLen = 0;
    int origIdx = *idx;

    while(isdigit(proLine[*idx]) != 0){
        numLength++;
        strncat(buffer, &proLine[*idx], 1);
        charIdx++;
    }

    switch(proLine[*idx]){
        case '.':
            strncat(buffer, &proLine[*idx], 1);
            charIdx++;
            while (isdigit(proLine[*idx]) != 0)
            {
                postDecLen++;
                strncat(buffer, &proLine[*idx], 1);
                charIdx++;
            }
            charIdx--;

            if (postDecLen == 0){
                charIdx = origIdx;
                return intMachine(proLine, idx);
            }

            strcpy(returnToken.name, buffer);
            strcpy(returnToken.token, "NUM");
            returnToken.tokenID = 2;
            strcpy(returnToken.attr, "REAL");
            returnToken.attrID = 2;
            printf("REAL detected: %s\n", buffer);

            if(numLength > 5){
                strcpy(returnToken.token, "LEXERR");
                returnToken.tokenID = 99;
                strcpy(returnToken.attr, "DigBefDec");
                returnToken.attrID = 4;
                fputs("LEXERR: Too Many Digits Before Decimal\n", listingFile);
            }

            if(postDecLen > 5){
                strcpy(returnToken.token, "LEXERR");
                returnToken.tokenID = 99;
                strcpy(returnToken.attr, "DigAfDec");
                returnToken.attrID = 5;
                fputs("LEXERR: Too Many Digits After Decimal\n", listingFile);
            }

            if(buffer[strlen(buffer) - 1] == '0' && buffer[strlen(buffer) - 2] == '0'){
                strcpy(returnToken.token, "LEXERR");
                returnToken.tokenID = 99;
                strcpy(returnToken.attr, "TrailZero");
                returnToken.attrID = 10;
                fputs("LEXERR: Trailing Zeros\n", listingFile);
            }

            if(buffer[0] == '0' && buffer[1] != '.'){
                strcpy(returnToken.token, "LEXERR");
                returnToken.tokenID = 99;
                strcpy(returnToken.attr, "LeadingZero");
                returnToken.attrID = 11;
                fputs("LEXERR: Leading Zeros\n", listingFile);
            }
            return returnToken;
        default:
            charIdx = origIdx;
            return intMachine(proLine, idx);
    }
}


struct token longRealMachine(char proLine[], int* idx){

    //listingFile = fopen("listingFile.txt", "w"); 
    struct token returnToken;
    char buffer[40]= "";
    int numLength = 0;
    int postDecLen = 0;
    int postELen = 0;
    int origIdx = *idx;

    while(isdigit(proLine[*idx]) != 0){
        numLength++;
        strncat(buffer, &proLine[*idx], 1);
        charIdx++;
    }

    switch(proLine[*idx]){
        case '.':

            strncat(buffer, &proLine[*idx], 1);
            charIdx++;
            while (isdigit(proLine[*idx]) != 0)
            {
                postDecLen++;
                strncat(buffer, &proLine[*idx], 1);
                charIdx++;
            }
            switch(proLine[*idx]){
                case 'E':
                    strncat(buffer, &proLine[*idx], 1);
                    charIdx++;
                    if (proLine[*idx] == '+' || proLine[*idx] == '-'){
                        strncat(buffer, &proLine[*idx], 1);
                        charIdx++;
                    }

                    while (isdigit(proLine[*idx]) != 0)
                    {
                        postELen++;
                        strncat(buffer, &proLine[*idx], 1);
                        charIdx++;
                    }
                    charIdx--;

                    if (postDecLen == 0){
                        charIdx = origIdx;
                        return realMachine(proLine, idx);
                    }

                    if (postELen == 0){
                        charIdx = origIdx;
                        return realMachine(proLine, idx);
                    }

                    strcpy(returnToken.name, buffer);
                    strcpy(returnToken.token, "NUM");
                    returnToken.tokenID = 2;
                    strcpy(returnToken.attr, "LONGREAL");
                    returnToken.attrID = 2;
                    printf("LONGREAL detected: %s\n", buffer);

                    if(numLength > 5){
                        strcpy(returnToken.token, "LEXERR");
                        returnToken.tokenID = 99;
                        strcpy(returnToken.attr, "DigBefDec");
                        returnToken.attrID = 4;
                        fputs("LEXERR: Too Many Digits Before Decimal\n", listingFile);
                    }

                    if(postDecLen > 5){
                        strcpy(returnToken.token, "LEXERR");
                        returnToken.tokenID = 99;
                        strcpy(returnToken.attr, "DegAfDec");
                        returnToken.attrID = 5;
                        fputs("LEXERR: Too Many Digits After Decimal\n", listingFile);
                    }

                    if(postELen > 2){
                        strcpy(returnToken.token, "LEXERR");
                        returnToken.tokenID = 99;
                        strcpy(returnToken.attr, "ExpTooLong");
                        returnToken.attrID = 6;
                        fputs("LEXERR: Exponent Too Long\n", listingFile);
                    }

                    if(buffer[numLength + postDecLen] == '0' && buffer[numLength + postDecLen - 1] == '0'){
                        strcpy(returnToken.token, "LEXERR");
                        returnToken.tokenID = 99;
                        strcpy(returnToken.attr, "TrailingZeros");
                        returnToken.attrID = 10;
                        fputs("LEXERR: Trailing Zeros\n", listingFile);
                    }

                    if(buffer[0] == '0' && buffer[1] != '.'){
                        strcpy(returnToken.token, "LEXERR");
                         returnToken.tokenID = 99;
                        strcpy(returnToken.attr, "LeadingZero");
                        returnToken.attrID = 11;
                        fputs("LEXERR: Leading Zero\n", listingFile);
                    }
                    return returnToken;
                default:
                    charIdx = origIdx;
                    return realMachine(proLine, idx);
            }


        default:
            charIdx = origIdx;
            return realMachine(proLine, idx);
        

    }

}


struct token relopMachine(char proLine[], int* idx){
    
    struct token returnToken;
    switch(proLine[*idx]){
        
        case '>':
            charIdx++;
            switch(proLine[*idx]){
                case '=':
                    strcpy(returnToken.name, ">=");
                    strcpy(returnToken.token, "RELOP");
                    returnToken.tokenID = 50;
                    strcpy(returnToken.attr, "GTE");
                    returnToken.attrID = 63;
                    printf("RELOP detected: >=\n");
                    return returnToken;
                    
                default:
                    charIdx--;
                    strcpy(returnToken.name, ">");
                    strcpy(returnToken.token, "RELOP");
                    returnToken.tokenID = 50;
                    strcpy(returnToken.attr, "GT");
                    returnToken.attrID = 62;
                    printf("RELOP detected: >\n");
                    return returnToken;
            }
                
        case '=':
            strcpy(returnToken.name, "=");
            strcpy(returnToken.token, "RELOP");
            returnToken.tokenID = 50;
            strcpy(returnToken.attr, "EQ");
            returnToken.attrID = 58;
            printf("RELOP detected: =\n");
            return returnToken;
        case '<':
            charIdx++;
            switch(proLine[*idx]){
                case '=':
                    strcpy(returnToken.name, "<=");
                    strcpy(returnToken.token, "RELOP");
                    returnToken.tokenID = 50;
                    strcpy(returnToken.attr, "LTE");
                    returnToken.attrID = 61;
                    printf("RELOP detected: <=\n");
                    return returnToken;
                case '>':
                    strcpy(returnToken.name, "<>");
                    strcpy(returnToken.token, "RELOP");
                    returnToken.tokenID = 50;
                    strcpy(returnToken.attr, "NEQ");
                    returnToken.attrID = 59;
                    printf("RELOP detected: <>\n");
                    return returnToken;
                default:
                    charIdx--;
                    strcpy(returnToken.name, "<");
                    strcpy(returnToken.token, "RELOP");
                    returnToken.tokenID = 50;
                    strcpy(returnToken.attr, "LT");
                    returnToken.attrID = 60;
                    printf("RELOP detected: <\n");
                    return returnToken;
            }
        default:
            return longRealMachine(proLine, idx);
    }
}


struct token resWdMachine(char proLine[], int* idx){

    struct token returnToken;
    int origIdx = *idx;
    int resWdIdx = 0;
    struct resWd* curResWd;
    curResWd = beginningResWd;
    //while(proLine[*idx])
    int resWdCount = 0;

    while(resWdCount < numResWords){
        while(proLine[*idx] == curResWd->name[resWdIdx]){
            if (((charIdx - origIdx) + 1) == strlen(curResWd->name)){  
                strcpy(returnToken.name, curResWd->name);
                strcpy(returnToken.token, curResWd->token);
                returnToken.tokenID = curResWd->tokenID;
                strcpy(returnToken.attr, curResWd->attr);
                returnToken.attrID = curResWd->attrID;
                printf("ResWd Detected: %s\n", returnToken.name);
                return returnToken;
            }
            charIdx++;
            resWdIdx++;
        }
        charIdx = origIdx;
        resWdIdx = 0;
        resWdCount++;
        curResWd = curResWd->next;
    }
    charIdx = origIdx;
    return (relopMachine(proLine, idx));

}


struct token whitespaceMachine(char proLine[], int* idx){

    struct token returnToken;
    switch(proLine[*idx]){
        case ' ':
            strcpy(returnToken.name, "Whitespace");
            strcpy(returnToken.token, "WS");
            returnToken.tokenID = 105;
            strcpy(returnToken.attr, "NULL");
            returnToken.attrID = 150;
            return returnToken;
        case '\n':
            strcpy(returnToken.name, "Newline");
            strcpy(returnToken.token, "NL");
            returnToken.tokenID = 110;
            strcpy(returnToken.attr, "NULL");
            returnToken.attrID = 150;
            return returnToken;
        case '\t':
            strcpy(returnToken.name, "Tab");
            strcpy(returnToken.token, "TAB");
            returnToken.tokenID = 115;
            strcpy(returnToken.attr, "NULL");
            returnToken.attrID = 150;
            return returnToken;
        case '\r':
            strcpy(returnToken.name, "Carriage Return");
            strcpy(returnToken.token, "CR");
            returnToken.tokenID = 120;
            strcpy(returnToken.attr, "NULL");
            returnToken.attrID = 150;
            return returnToken;
        default:
            return resWdMachine(proLine, idx);

    }
}


int main(){

    //read resWds into linked list
    resWdList = fopen("reservedWords.txt", "r");
    symbolTable = fopen("symbolTable.txt", "w");
    listingFile = fopen("listingFile.txt", "w");
    char name[20];
    char token[20];
    int tokenID;
    char attr[20];
    int attrID;
    int SAIdx = 0;

    struct resWd firstResWd;
    struct resWd* current;
    fscanf(resWdList, "%s %s %d %s %d", name, token, &tokenID, attr, &attrID);
    strcpy(firstResWd.name, name);
    strcpy(firstResWd.token, token);
    firstResWd.tokenID = tokenID;
    strcpy(firstResWd.attr, attr);
    firstResWd.attrID = attrID;
    current = &firstResWd;
    beginningResWd = &firstResWd;
    numResWords = 1;

    while (fscanf(resWdList, "%s %s %d %s %d", name, token, &tokenID, attr, &attrID) == 5){
        numResWords++;
        current->next = malloc(sizeof(struct resWd));
        strcpy(current->next->name, name);
        strcpy(current->next->token, token);
        current->next->tokenID = tokenID;
        strcpy(current->next->attr, attr);
        current->next->attrID = attrID;
        current = current->next;
    }

    printf("NUM RES WDS: %d\n", numResWords);

    srcProg = fopen("srcProg.txt", "r");
    tokenFile = fopen("tokenFile.txt", "w");
    fputs("Line #        Lexeme        Token-Type        Attribute\n", tokenFile);
    int lineNum = 1;
    charIdx = 0;
    char progLine[72];
    char currentChar;
    while((fgets(progLine, 72, srcProg)) != NULL){
        printf("~~~NEW LINE~~~\n");
        char lineBuf[10];
 
        sprintf(lineBuf, "%d", lineNum);
        fputs(lineBuf, listingFile);
        fputs("        ", listingFile);
        fputs(progLine, listingFile);
        while(charIdx < strlen(progLine) - 1 || progLine[charIdx] == 46){

            struct token nextToken = whitespaceMachine(progLine, &charIdx);
            if (strcmp(nextToken.token, "CR") == 0 || strcmp(nextToken.token, "WS") == 0 || strcmp(nextToken.token, "NL") == 0 || strcmp(nextToken.token, "TAB") == 0){
                charIdx++;
                continue;
            }
            
            
            char buffer[20];
            sprintf(buffer, "%d", lineNum);
            fputs(buffer, tokenFile);
            fputs("        ", tokenFile);
            fputs(nextToken.name, tokenFile);
            fputs("        ", tokenFile);
            fputs(nextToken.token, tokenFile);
            fputs("        ", tokenFile);
            sprintf(buffer, "%d", nextToken.tokenID);
            if (nextToken.tokenID == 110)
              lineNum++;
            if (nextToken.tokenID == 250){
                int isInTable = 0;
                for (int i = 0; i < idIdx; i++){
                    if (strcmp(nextToken.name, idArray[i].name) == 0){
                        isInTable = 1;
                        char buffer2[20] = "";
                        sprintf(buffer2, "%d", &idArray[i]);
                        char **ptr;
                        nextToken.attrID = strtol(buffer2, ptr, 10);
                    }
                }
                if (isInTable == 0){
                    fputs("ID: ", symbolTable);
                    fputs(nextToken.name, symbolTable);
                    fputs(", with address  ", symbolTable);
                    char buffer2[20] = "";
                    idArray[idIdx] = nextToken;
                    sprintf(buffer2, "%d", &idArray[idIdx]);
                    idIdx++;
                    fputs(buffer2, symbolTable);
                    fputs("\n", symbolTable);
                    char **ptr;
                    nextToken.attrID = strtol(buffer2, ptr, 10);
                    SAIdx++;
                }
            }
            if (nextToken.tokenID == 99){
                if (nextToken.attrID == 20){
                    fputs("LEXERR: Unrecognized Symbol:   ", listingFile);
                    fputc(progLine[charIdx], listingFile);
                    fputs("\n", listingFile);
                }
                if (nextToken.attrID == 99){
                    fputs("LEXERR: ID Too Long:   ", listingFile);
                    fputs(nextToken.name, listingFile);
                    fputs("\n", listingFile);
                }
            }
            fputs(buffer, tokenFile);
            fputs("        ", tokenFile);
            fputs(nextToken.attr, tokenFile);
            fputs("        ", tokenFile);
            sprintf(buffer, "%d", nextToken.attrID);
            fputs(buffer, tokenFile);
            fputc('\n', tokenFile);
            charIdx++;
        }  
        struct token nextToken = whitespaceMachine(progLine, &charIdx); 
        
        char buffer[20];
        if (nextToken.tokenID == 110)
            lineNum++;
        charIdx = 0;
    }
    char buffer[20] = "";
    sprintf(buffer, "%d", lineNum);
    fputs(buffer, tokenFile);
    fputs("        ", tokenFile);
    fputs("EndOfFile", tokenFile);
    fputs("        ", tokenFile);
    fputs("EOF", tokenFile);
    fputs("        ", tokenFile);
    sprintf(buffer, "%d", 200);
    fputs(buffer, tokenFile);
    fputs("        ", tokenFile);
    fputs("NULL", tokenFile);
    fputs("        ", tokenFile);
    sprintf(buffer, "%d", 150);
    fputs(buffer, tokenFile);
    printf("END OF FILE \n");
}




