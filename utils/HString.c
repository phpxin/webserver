#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "HString.h"


static int StrInit(HString *hstr, int len){
    ClearString(hstr);

    hstr->len = len;
    hstr->ch = (char *)malloc(hstr->len);
    memset(hstr->ch, '\0', hstr->len);

    return 1;
}

int StrAssign(HString *hstr, const char *str)
{
    int slen = strlen(str);

    StrInit(hstr, slen);

    memcpy(hstr->ch, str, slen);
    return 1;
}


int StrCopy(HString *dst, HString *src)
{
    if(StrEmpty(src)){ 
        ClearString(dst);
        return 1; 
    }
    
    StrInit(dst, src->len);

    memcpy(dst->ch, src->ch, src->len);
    return 1;
}

int StrEmpty(HString *hstr)
{
    if(hstr->len<=0){
        return 1;
    }

    return 0;
}

int StrCompare(HString *hstr1, HString *hstr2)
{
    int minlen = hstr1->len < hstr2->len ? hstr1->len : hstr2->len ;
    if(minlen <= 0){
        return hstr1->len < hstr2->len ? -1 : 1 ;
    }

    int i=0;
    int ret = 0;
    for(i=0; i<minlen; i++){
        if(*(hstr1->ch + i) > *(hstr2->ch + i))
            ret = 1;
        else if(*(hstr1->ch + i) < *(hstr2->ch + i))
            ret = -1;
        if(ret!=0)
            break;
    }

    return ret;
}

int StrLength(HString *hstr){
    return hstr->len;
}

int ClearString(HString *hstr){
    if(hstr->ch!=NULL){
        free(hstr->ch);
        hstr->ch = NULL;
    }
    hstr->len = 0;

    return 1;
}

int Concat(HString *dst,HString *hstr1,HString *hstr2){
    int _len = hstr1->len + hstr2->len;

    StrInit(dst, _len);

    int i=0;
    if(!StrEmpty(hstr1)){
        for(i=0; i<hstr1->len; i++){
            *( dst->ch + i ) = *( hstr1->ch + i ) ;
        }
    }
    if(!StrEmpty(hstr2)){
        for(i=0; i<hstr2->len; i++){
            *( dst->ch + hstr1->len + i ) = *( hstr2->ch + i ) ;
        }
    }

    return 1;
}

int SubString(HString *sub, HString *src, int pos, int len)
{
    if(pos>=src->len || pos+len>src->len)   return 0;

    StrInit(sub, len);

    int i = 0, j = 0;
    for(i=pos,j=0; j<len; i++,j++){
        *(sub->ch+j) = *(src->ch+i);
    }

    return 1;
}

int Index(HString *hstr, HString *sub, int pos){
    return 1;
}
