#ifndef MACRO_H_INCLUDED
#define MACRO_H_INCLUDED
#include "mystring.h"

typedef struct formatString{
    /**
        参照printf等格式化输出方法，设计了格式化字
    符串数据结构，在字符串中用#0~#(n-1)来表示n个需要
    替换参数，如paramNum为0,则为无参类型，直接进行字符串替换
    */
    int paramNum;//参数个数
    char* formatString;//格式化字符串
}FORMATSTRING;

typedef struct macro{
    /**
        一个宏定义的数据类型，一个宏名，对应是否有参数及相应的
    格式化字符串。
    */
    char* symbols;//宏定义名
    FORMATSTRING formats;
}MACRO,*PMACRO;

typedef struct macroSet{
    /**
        参照ArrayLsit（java)设计的集合类，支持添加时上限情况的自动
    扩容，用于维持宏定义的列表。
    */
    int maxLength;
    int num;
    PMACRO* list;
}MACROSET,*PMACROSET;

//生成一个宏集合对象
PMACROSET generate_macro_set(int max_length);
//添加一个宏
int addMacro(PMACROSET pSet , PMACRO pMacro);
//找到一个宏在集合中的位置
int findMacroIndex(PMACROSET pSet , const char* symbol , int paramNum);



PMACRO generate_define_macro(char* def , int length);


PMACROSET generate_macro_set(int max_length)
{
    PMACROSET pMacroSet = (PMACROSET)malloc(sizeof(MACROSET));
    pMacroSet->list = (PMACRO*)malloc(sizeof(PMACRO) * max_length);
    pMacroSet->maxLength = max_length;
    pMacroSet->num = 0;
    return pMacroSet;
}

/**
 *
 *返回值:n
  n < 0 not found
  0 <= n < pSet->num the index of target macro
 *
 */

int findMacroIndex(PMACROSET pSet , const char* symbol , int param_num)
{
    int i = 0;
    for(; i < pSet->num; i++)
    {
        if(strcmp(symbol , pSet->list[i]->symbols) == 0 && pSet->list[i]->formats.paramNum == param_num)
        {
            return i;
        }
    }
    return -1;
}
int addMacro(PMACROSET pSet , PMACRO pMacro)
{
    if(pSet->maxLength == pSet->num)
    {
        pSet->maxLength *= 2;
        pSet->list = (PMACRO*)realloc(pSet->list , sizeof(PMACRO) * pSet->maxLength);
    }
    pSet->list[pSet->num++] = pMacro;
    return 0;
}

int hasMacroCalled(PMACROSET pSet , char* name)
{
    int i = 0;
    for( ; i < pSet->num; i++)
    {
        if(strcmp(pSet->list[i]->symbols , name) == 0)
        {
            return 1;
        }

    }
    return 0;
}


#endif // MACRO_H_INCLUDED