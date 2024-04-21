
#ifndef MYSTRING_H_INCLUDED
#define MYSTRING_H_INCLUDED
#include <stdlib.h>

typedef struct mystring{
    int maxLength;
    int length;
    char* sequence;
}MYSTRING,*PMYSTRING;

char charAt(MYSTRING str , int index);
PMYSTRING createString(int length);
void addChar(PMYSTRING pString , char ch);
void myStrcat(PMYSTRING pString , char* str);


char charAt(MYSTRING str , int index){
    if(index >= str.length)
    {
        return ' ';
    }
    else
    {
        return str.sequence[index];
    }
}

PMYSTRING createString(int length){
    PMYSTRING pstr = (PMYSTRING)malloc(sizeof(MYSTRING));
    pstr->maxLength = length;
    pstr->length = 0;
    pstr->sequence = (char*)malloc(pstr->maxLength);
    memset(pstr->sequence , 0 , pstr->maxLength);
    //printf("%s is new\n" , pstr->sequence);
    return pstr;
}
void addChar(PMYSTRING pString , char ch)
{
    if(pString->length == pString->maxLength - 1)
    {
        pString->maxLength *= 2;
        pString->sequence = (char*)realloc(pString->sequence , pString->maxLength);
    }
    pString->sequence[pString->length++] = ch;
}

void myStrcat(PMYSTRING pString , char* str)
{
    if(pString->length + strlen(str) >= pString->maxLength)
    {
        pString->maxLength = (pString->length + strlen(str)) * 2;
        pString->sequence = (char*)realloc(pString->sequence , pString->maxLength);
    }
    pString->length += strlen(str);
    strcat(pString->sequence , str);
}


#endif // MYSTRING_H_INCLUDED
