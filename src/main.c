#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "macro.h"

#define ERROR 0
#define DEFINE 1
#define INCLUDE 2
#define IF 3
#define ELIF 4
#define ELSE 5
#define ENDIF 6
#define IFDEF 7
#define IFNDEF 8

PMACROSET pms;

char *root_path = "/Users/kingsblade/CProjects/c_pre_compiler/test/include/";

int func_include(FILE *fpRead , FILE *fpWrite)//用于将某个被引入的头文件输出到文件中
{
    char c;
    while(1)
    {
        c = fgetc(fpRead);
        if(feof(fpRead))
        {
            break;
        }
        fprintf(fpWrite , "%c" , c);
        //printf("%c" , c);

    }
    return 0;
}


int isInCharSet(char c)//判断字符是否是字符表中字符，主要作字符串成块依据
{
    if(c >= 'a' && c <= 'z')
    {
        return 1;
    }
    else if(c >= 'A' && c <= 'Z')
    {
        return 1;
    }
    else if(c >= '0' && c <= '9')
    {
        return 1;
    }
    else if(c == '_')
    {
        return 1;
    }
    return 0;
}

int getMacroType(char* macroName)//识别预处理语句类别
{
    char elif[5]={'e','l','i','f'};
   // char endif[6]={'e','n','d','i','f'};
    if(strcmp(macroName , "define") == 0)
    {
        return DEFINE;
    }
    else if (strcmp(macroName , "include") == 0)
    {
        return INCLUDE;
    }
    else if (strcmp(macroName , "if") == 0)
    {
        return IF;
    }
    else if (strcmp(macroName , "elif") == 0)
    {
        return ELIF;
    }
    else if (strcmp(macroName , "else") == 0)
    {
        return ELSE;
    }
    else if (strcmp(macroName , "endif") == 0)
    {
        return ENDIF;
    }
    else if (strcmp(macroName , "ifdef") == 0)
    {
        return IFDEF;
    }
    else if (strcmp(macroName , "ifndef") == 0)
    {
        return IFNDEF;
    }
    else
    {
        return ERROR;
    }

}

void annotation_filter(const char* sourceFile , const char* outFile)//将一个文件去注释
{
    int inBlockAnnotation = 0;
    int inLineAnnotation = 0;
    int inAnnotation = 0;
    inAnnotation = inLineAnnotation || inBlockAnnotation;
    FILE* pRead = fopen(sourceFile , "r");
    FILE* pWrite = fopen(outFile , "w+");
    while(1){
        char c = fgetc(pRead);
        if(feof(pRead)){
            break;
        }
        char pre_char;
        int wrong_pre_read = 0;
        if(!inAnnotation)
        {
            if(c == '/')
            {
                pre_char = fgetc(pRead);
                if(pre_char == '*')
                {
                    inBlockAnnotation = 1;
                    inAnnotation = inLineAnnotation || inBlockAnnotation;
                    continue;
                }
                else if(pre_char == '/')
                {
                    inLineAnnotation = 1;
                    inAnnotation = inLineAnnotation || inBlockAnnotation;
                    continue;
                }
                else
                {
                    wrong_pre_read = 1;
                }
            }
        }
        else
        {
            if(inLineAnnotation)
            {
                if(c == '\n')
                {
                    inLineAnnotation = 0;
                    inAnnotation = inLineAnnotation || inBlockAnnotation;
                    continue;
                }
                else
                {
                    continue;
                }
            }
            else if(inBlockAnnotation)
            {
                if(c == '*')
                {
                    pre_char = fgetc(pRead);
                    if(pre_char == '/')
                    {
                        inBlockAnnotation = 0;
                        inAnnotation = inLineAnnotation || inBlockAnnotation;
                        continue;
                    }
                    else
                    {
                        continue;
                    }
                }
                else
                {
                    continue;
                }
            }
        }

        printf("%c" , c);
        fprintf(pWrite , "%c" , c);
        if(wrong_pre_read)
        {
            printf("%c" , pre_char);
            fprintf(pWrite , "%c" , pre_char);
            wrong_pre_read = 0;
        }
    }
    fclose(pRead);
    fclose(pWrite);
}

char* macroReplace(PMACRO pmacro , char** params)//传入参数与具体宏，返回替换字符串
{
    PMYSTRING result = createString(512);
    int i = 0;
    char *pc = pmacro->formats.formatString;
    for( ; i < strlen(pc) ; i++)
    {
        if(pc[i] != '#')
        {
            addChar(result , pc[i]);
        }
        else
        {
            int index = pc[++i] - '0';
            myStrcat(result , params[index]);
        }
    }
    return result->sequence;
}


char* trans(char *line , int length)//将一行字符串扫描一遍，替换'一次'所有满足的宏定义
{
    int i = 0;
    PMYSTRING out = createString(512);
    int bufferEmpty = 1;
    PMYSTRING buffer = createString(512);
    for(; i < length; i++){
        if(isInCharSet(line[i]))
        {
            addChar(buffer , line[i]);
            bufferEmpty = 0;
        }
        else//非表字符
        {
            if(line[i] == '(' && line[i + 1] == ')')
            {//带括号的无参宏
                addChar(buffer , line[i]);
                addChar(buffer , line[i + 1]);
                i += 2;
                bufferEmpty = 0;
            }
            if(!bufferEmpty)
            {//读进来一个表符号串
                if(hasMacroCalled(pms , buffer->sequence))
                {//有这个宏
                    if(line[i] == '(')
                    {//有参数
                        char* params[100];
                        int paramNum = 0;
                        int state = 1;
                        PMYSTRING param = createString(128);
                        for(i = i + 1 ; i < length ; i++)
                        {
                            if(line[i] == ',')
                            {
                                params[paramNum++] = param->sequence;
                                free(param);
                                param = createString(128);
                            }
                            if(line[i] == '(')
                            {
                                state++;
                                addChar(param , line[i]);
                            }
                            else if(line[i] == ')')
                            {
                                state--;
                                if(state == 0)
                                {
                                    params[paramNum++] = param->sequence;
                                    free(param);
                                    int index = findMacroIndex(pms , buffer->sequence , paramNum);
                                    if(index >= 0){
                                        myStrcat(out , macroReplace(pms->list[index] , params));
                                        paramNum = 0;
                                        state = 1;
                                        param = createString(128);
                                        buffer = createString(128);
                                        bufferEmpty = 1;
                                    }
                                    else
                                    {
                                        int realIndex = findMacroIndex(pms , buffer->sequence , 0);
                                        myStrcat(out , pms->list[realIndex]->formats.formatString);
                                        addChar(out , '(');
                                        int iii = 0;
                                        for( ; iii < paramNum - 1; iii++)
                                        {
                                            myStrcat(out , params[iii]);
                                            addChar(out , ',');
                                        }
                                        myStrcat(out , params[paramNum - 1]);
                                        addChar(out , ')');

                                    }
                                    break;
                                }
                                else
                                {
                                    addChar(param , line[i]);
                                }
                            }
                            else
                            {
                                addChar(param , line[i]);
                            }
                        }

                    }
                    else
                    {//无参数宏
                        int index = findMacroIndex(pms , buffer->sequence , 0);
                        if(index >= 0)
                        {
                            myStrcat(out , pms->list[index]->formats.formatString);
                            free(buffer->sequence);
                            free(buffer);
                            buffer = createString(512);
                            bufferEmpty = 1;
                        }

                    }
                }
                else
                {//不是宏
                    myStrcat(out , buffer->sequence);
                    free(buffer->sequence);
                    free(buffer);
                    buffer = createString(512);
                    bufferEmpty = 1;
                }
                addChar(out , line[i]);

            }
            else
            {//buffer空的
                addChar(out , line[i]);
            }
        }
    }
    if(strlen(buffer->sequence) > 0){
        if(hasMacroCalled(pms , buffer->sequence))
        {
            int index = findMacroIndex(pms , buffer->sequence , 0);
            if(index > 0)
            {
                myStrcat(out , pms->list[index]->formats.formatString);
            }
            else
            {
                myStrcat(out , buffer->sequence);
            }

        }
        else
        {
            myStrcat(out , buffer->sequence);
        }

    }
    free(buffer->sequence);
    free(buffer);
    char *ret = out->sequence;
    free(out);
    return ret;

}
void handle(const char* annotation_filter_file , const char* out_file)//主识别与处理函数
{
    FILE* pRead = fopen(annotation_filter_file , "r");
    FILE* pWrite = fopen(out_file , "w+");

    int valids[100];
    int validDeep = 0;
    int valid = 1;
    valids[validDeep] = valid;

    int itr = 0;
    for( ; itr < 100; itr++)
    {
        valids[itr] = 0;
    }

    int inMacro = 0;//是否在#后面
    int metSharp = 0;//是否在#所在行(遇见\则跨行)

    int inBlock = 0;//
    int bufferNotNull = 0;

    PMYSTRING macroName;
    PMYSTRING def;
    PMYSTRING line_buffer;
    PMYSTRING block_buffer;
    while(1)
    {

        char c = fgetc(pRead);

        if(metSharp == 0 && inMacro == 0)//没有进入宏定义
        {
            if(c == '#')
            {
                inMacro = 1;
                macroName = createString(128);
                metSharp = 1;
            }
            else
            {
                if(inBlock == 1)//是否成块
                {
                    if(c == '\n')
                    {
                        addChar(block_buffer , c);

                        char *outter = trans(block_buffer->sequence , strlen(block_buffer->sequence));

                        while(strcmp(outter , trans(outter , strlen(outter))) != 0)
                        {
                            outter = trans(outter , strlen(outter));
                        }

                        fprintf(pWrite , "%s" , outter);
                        //printf("%s\n" , block_buffer->sequence);
                        free(block_buffer->sequence);
                        free(block_buffer);
                        inBlock = 0;
                        bufferNotNull = 0;
                    }
                    else
                    {
                        addChar(block_buffer , c);
                    }

                }
                else//inBlock = 0;
                {
                    if(c != '\n')
                    {
                        block_buffer = createString(512);
                        addChar(block_buffer , c);
                        bufferNotNull = 1;
                        inBlock = 1;
                    }
                    else
                    {
                        fprintf(pWrite , "%c" , c);
                        //printf("*****%c\n" , c);
                    }
                }
            }
        }
        else if(metSharp == 1 && inMacro == 1)
        {
            if(c == '\t' || c == ' ' || c == '\n')
            {
                if(c == '\n')
                {
                    def->sequence = "";
                }
                inMacro = 0;
                //printf("%d\t%s\n" , getMacroType(macroName->sequence) , macroName->sequence);
                int type = getMacroType(macroName->sequence);
                def = createString(512);
                //fprintf(pP , "%d\t%s\n" , getMacroType(pStr->sequence) , pStr->sequence);
                if(type == ENDIF)
                {//弹栈
                    int temp = valid;
                    valid = valids[--validDeep];
                    printf("Match Endif , from %s to %s \n" , temp?"valid":"invalid" , valid?"valid":"invalid");
                }
            }
            else
            {
                addChar(macroName , c);
            }
        }
        else if(metSharp == 1 && inMacro == 0)
        {
            if(c == '\n')
            {
                metSharp = 0;

                if(getMacroType(macroName->sequence) == DEFINE)
                {
                    if(valid)
                    {
                        PMACRO pm = generate_define_macro(def->sequence , strlen(def->sequence));
                        //printf("%s\t\t%d\t\t%s \n" , pm->symbols , pm->formats.paramNum , pm->formats.formatString);
                        printf("***/Valid area define:/***\t#%s %s\n" , macroName->sequence , def->sequence);
                        addMacro(pms , pm);
                        //printf("#%s %s\n" , macroName->sequence , def->sequence);

                    }
                    else
                    {
                        printf("***/Invalid area define:/***\t#%s %s\n" , macroName->sequence , def->sequence);
                    }

                }
                else if(getMacroType(macroName->sequence) == IFNDEF)
                {
                    if(hasMacroCalled(pms , def->sequence))
                    {
                        printf("Already has macro called %s \n" , def->sequence);
                        valid = 0;
                        valids[++validDeep] = valid;
                    }
                    else
                    {
                        printf("No macro called %s , valid area \n" , def->sequence);
                        valid = 1;
                        valids[++validDeep] = valid;
                    }

                }
                else if(getMacroType(macroName->sequence) == IF)
                {
                    //fprintf(pWrite , "#%s %s\n" , macroName->sequence , def->sequence);
                    printf("False bool : invalid area #%s %s\n" , macroName->sequence , trans(def->sequence , strlen(def->sequence)));
                    valid = 0;
                    valids[++validDeep] = valid;
                }
                else if(getMacroType(macroName->sequence) == ERROR)
                {
                    if(valid)
                    {
                        printf("Error message : %s\n" , def->sequence);
                    }
                    else
                    {
                        printf("Cannot output message : %s\n" , def->sequence);
                    }
                }
                else if(getMacroType(macroName->sequence) == ENDIF)
                {
                    int temp = valid;
                    valid = valids[--validDeep];
                    printf("Match Endif , from %s to %s \n" , temp?"valid":"invalid" , valid?"valid":"invalid");
                }


            }
            else if(c == '\\' && (c = fgetc(pRead)) == '\n')
            {
                continue;
            }
            else
            {
                addChar(def , c);
            }
        }
        else
        {
        }

        if(feof(pRead))
        {
            break;
        }

    }

    int i = 0;


    printf("\n\n");
    for(; i < pms->num; i++)
    {
        printf("%s\t%d\t%s\n" , pms->list[i]->symbols , pms->list[i]->formats.paramNum , pms->list[i]->formats.formatString);
    }



    fclose(pRead);
    fclose(pWrite);
}

int include(const char* annotation_filter_file , const char* out_file)//包含关系的识别与处理函数
{
    int num = 0;
    FILE* pRead = fopen(annotation_filter_file , "r+");
    FILE* pWrite = fopen(out_file , "w+");

    int inMacro = 0;//是否在#后面
    int metSharp = 0;//是否在#所在行(遇见\则跨行)

    int inOneReference = 0;
    int inDoubleReference = 0;
    int inLine = inOneReference || inDoubleReference;

    int inBlock = 0;//
    int bufferNotNull = 0;

    PMYSTRING macroName;
    PMYSTRING def;
    PMYSTRING line_buffer;
    PMYSTRING block_buffer;



    while(1)
    {

        char c = fgetc(pRead);
        if(feof(pRead))
        {
            break;
        }
        if(metSharp == 0 && inMacro == 0)//没有进入宏定义
        {
            if(inLine == 0)//没在字符串或字符内
            {
                if(c == '\"')
                {
                    inDoubleReference = 1;
                    inLine = inOneReference || inDoubleReference;
                    line_buffer = createString(50);
                    addChar(line_buffer , c);
                    if(inBlock)
                    {
                        fprintf(pWrite , "%s" , block_buffer->sequence);
                        printf("%s" , block_buffer->sequence);
                        free(block_buffer->sequence);
                        free(block_buffer);
                        inBlock = 0;
                        bufferNotNull = 0;
                    }
                }
                else if(c == '\'')
                {
                    inOneReference = 1;
                    inLine = inOneReference || inDoubleReference;
                    line_buffer = createString(50);
                    addChar(line_buffer , c);
                    if(inBlock)
                    {
                        fprintf(pWrite , "%s" , block_buffer->sequence);
                        printf("%s" , block_buffer->sequence);
                        free(block_buffer->sequence);
                        free(block_buffer);
                        inBlock = 0;
                        bufferNotNull = 0;
                    }
                }
                else if(c == '#')
                {
                    inMacro = 1;
                    macroName = createString(20);
                    metSharp = 1;
                }
                else
                {
                    if(inBlock == 1)//是否成块
                    {
                        if(c == ' ' || c == '\t' || c == '\n')
                        {
                            addChar(block_buffer , c);
                            fprintf(pWrite , "%s" , block_buffer->sequence);
                            printf("%s" , block_buffer->sequence);
                            free(block_buffer->sequence);
                            free(block_buffer);
                            inBlock = 0;
                            bufferNotNull = 0;
                        }
                        else
                        {
                            addChar(block_buffer , c);
                        }

                    }
                    else//inBlock = 0;
                    {
                        if(c != ' ' && c != '\t' && c != '\n')
                        {
                            block_buffer = createString(50);
                            addChar(block_buffer , c);
                            bufferNotNull = 1;
                            inBlock = 1;
                        }
                        else
                        {
                            fprintf(pWrite , "%c" , c);
                            printf("%c" , c);
                        }
                    }
                }
            }
            else
            {
                if(inOneReference)
                {
                    if(c == '\'')
                    {
                        addChar(line_buffer , c);
                        inOneReference = 0;
                        inLine = inOneReference || inDoubleReference;
                        fprintf(pWrite , "%s" , line_buffer->sequence);
                        printf("%s" , line_buffer->sequence);
                        free(line_buffer->sequence);
                        free(line_buffer);
                    }
                    else
                    {
                        addChar(line_buffer , c);
                    }
                }
                if(inDoubleReference)
                {
                    if(c == '\"')
                    {
                        addChar(line_buffer , c);
                        inDoubleReference = 0;
                        inLine = inOneReference || inDoubleReference;
                        fprintf(pWrite , "%s" , line_buffer->sequence);
                        printf("%s" , line_buffer->sequence);
                        free(line_buffer->sequence);
                        free(line_buffer);
                    }
                    else
                    {
                        addChar(line_buffer , c);
                    }
                }
            }

        }
        else if(metSharp == 1 && inMacro == 1)
        {
            if(c == '\t' || c == ' ')
            {
                inMacro = 0;
                //printf("%d\t%s\n" , getMacroType(macroName->sequence) , macroName->sequence);
                //int i = getMacroType(macroName->sequence);
                def = createString(60);
                //fprintf(pP , "%d\t%s\n" , getMacroType(pStr->sequence) , pStr->sequence);
            }
            else if(c == '\n')
            {
                metSharp = 0;
                inMacro = 0;
                /**

                endif
                ifndef
                error
                判断

                */
                fprintf(pWrite , "\n");
                fprintf(pWrite , "#%s\n" , macroName->sequence);
            }
            else
            {
                addChar(macroName , c);
            }
        }
        else if(metSharp == 1 && inMacro == 0)
        {
            if(c == '\n')
            {
                metSharp = 0;
                if(getMacroType(macroName->sequence) == INCLUDE)
                {
                    num++;
                    char root[100] = "D:/test/include/";
                    char *tempFile = "D:/test/temp.c";
                    PMYSTRING son = createString(50);
                    sscanf(def->sequence , "<%s" , son->sequence);
                    PMYSTRING sson = createString(50);
                    strncpy(sson->sequence , son->sequence , strcspn(son->sequence , ">"));
                    strcat(root , sson->sequence);

                    annotation_filter(root , tempFile);

                    FILE* pIn = fopen(tempFile,"r");
                    fprintf(pWrite , "\n");
                    func_include(pIn , pWrite);
                    fprintf(pWrite , "\n");
                    fclose(pIn);
                }
                else
                {
                    fprintf(pWrite , "#%s %s\n" , macroName->sequence , def->sequence);
                    //printf("#%s %s\n" , macroName->sequence , def->sequence);
                }

            }
            else if(c == '\\' && (c = fgetc(pRead)) == '\n')
            {
                continue;
            }
            else
            {
                addChar(def , c);
            }
        }
        else
        {
        }



    }
    fclose(pRead);
    fclose(pWrite);
    return num;
}

PMACRO generate_define_macro(char* def , int length)//宏定义的识别
{
    PMACRO pm = (PMACRO)malloc(sizeof(MACRO));

    PMYSTRING s_symbol = createString(50);
    PMYSTRING s_formats = createString(512);


    int hasContent = 1;
    int hasParam = 1;
    int i = 0 , j = i , k = j;
    for(i = 0; i <= length; i++)
    {
        if(isInCharSet(def[i]))
        {
            addChar(s_symbol , def[i]);
        }
        else
        {

            if(def[i] == '(')
            {
                if(def[i + 1] == ')' )
                {
                    hasParam = 0;
                    addChar(s_symbol , def[i]);
                    addChar(s_symbol , def[i + 1]);
                    j = i + 1;

                    break;
                }
                else
                {
                    hasParam = 1;
                }

            }
            else if(def[i] == ' ' || def[i] == '\t')
            {
                hasParam = 0;
                hasContent = 1;
            }
            else if(def[i] == '\0' || def[i] == '\n')
            {
                hasParam = 0;
                hasContent = 0;
            }
            j = i;
            break;

        }
        j++;
    }

    pm->symbols = s_symbol->sequence;
    free(s_symbol);
    if(hasContent)
    {
        PMYSTRING buffer = createString(128);
        int stop = j + 1;
        if(hasParam)
        {
            int num = 0;
            PMYSTRING* strings = (PMYSTRING*)malloc(10*sizeof(PMYSTRING));

            for(j = j + 1; j <= length; j++)
            {
                k = j;
                if(def[j] == ',')
                {
                    strings[num++] = buffer;
                    buffer = createString(128);
                }
                else if(def[j] == ')')
                {
                    strings[num++] = buffer;
                    k = j;
                    break;
                }
                else
                {
                    addChar(buffer , def[j]);
                }

            }
            int l = k + 1;
            PMYSTRING sDef = createString(512);
            for(; l < length; l++)
            {
                addChar(sDef , def[l]);
            }

            char *rightDef = trans(sDef->sequence , strlen(sDef->sequence));
            free(sDef->sequence);
            free(sDef);

            while(strcmp(rightDef , trans(rightDef , strlen(rightDef))) != 0)
            {
                rightDef = trans(rightDef , strlen(rightDef));
            }


            pm->formats.paramNum = num;
            PMYSTRING formatTemp = createString(512);

            buffer = createString(128);

            int bufferHasContent = 0;

            for(k = 0; k < strlen(rightDef); k++)
            {
                if(isInCharSet(rightDef[k]))//字母表中
                {
                    addChar(buffer , rightDef[k]);
                    bufferHasContent = 1;
                }
                else//非字母标
                {
                    if(bufferHasContent)
                    {
                        int paramIndex = -1;
                        int ii = 0;
                        for(ii = 0; ii < num; ii++){
                            if(strcmp(strings[ii]->sequence , buffer->sequence) == 0)
                            {
                                paramIndex = ii;
                                break;
                            }
                        }
                        if(paramIndex >= 0){
                            addChar(formatTemp , '#');
                            addChar(formatTemp , ('0' + paramIndex));
                        }
                        else
                        {
                            /**
                            */
                            myStrcat(formatTemp , buffer->sequence);

                        }
                        //
                        free(buffer->sequence);
                        free(buffer);
                        buffer = createString(128);

                    }
                    addChar(formatTemp , rightDef[k]);
                }

            }

            pm->formats.formatString = formatTemp->sequence;
            free(formatTemp);
            //printf("\n\n%s ------- %s\n\n" , pm->symbols , pm->formats.formatString);
        }
        else
        {
            free(buffer->sequence);
            free(buffer);
            buffer = createString(512);

            for( ; stop < length; stop++)
            {
                addChar(buffer , def[stop]);
            }
            pm->formats.paramNum = 0;
            pm->formats.formatString = trans(buffer->sequence , strlen(buffer->sequence));
            free(buffer->sequence);
            free(buffer);
        }

    }
    else
    {//没有内容
        pm->formats.paramNum = 0;
        pm->formats.formatString = "";
    }
    return pm;
}

int main()
{
    pms = generate_macro_set(300);
    int i1,i2,i3,i4;

    //printf("");
    //annotation_filter()
//    annotation_filter("/Users/kingsblade/CProjects/c_pre_compiler/test/sched.c" , "/Users/kingsblade/CProjects/c_pre_compiler/test/sched.annotation_processed.c");
//    i1 = include("/Users/kingsblade/CProjects/c_pre_compiler/test/sched.annotation_processed.c" , "/Users/kingsblade/CProjects/c_pre_compiler/test/sched.include1.c");
//    i2 = include("/Users/kingsblade/CProjects/c_pre_compiler/test/sched.include1.c" , "/Users/kingsblade/CProjects/c_pre_compiler/test/sched.include2.c");
//    i3 = include("/Users/kingsblade/CProjects/c_pre_compiler/test/sched.include2.c" , "/Users/kingsblade/CProjects/c_pre_compiler/test/sched.include4.c");
//    printf("引入次数统计：%d %d %d \n" , i1 , i2 , i3);

    handle("/Users/kingsblade/CProjects/c_pre_compiler/test/sched.include4.c" , "/Users/kingsblade/CProjects/c_pre_compiler/test/sched.out.c");
    return 0;
}
