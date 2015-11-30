typedef struct {
    char *ch;
    int len;
}HString;

//生成一个字符串，值等于str
int StrAssign(HString *hstr, const char *str);

//由src复制得到dst
int StrCopy(HString *dst, HString *src);

//串hstr为空，返回1，否则返回0
int StrEmpty(HString *hstr);

//如果hstr1>hstr2返回值>0,如果hstr1=hstr2返回值=0,如果hstr1<hstr2则返回值<0
int StrCompare(HString *hstr1, HString *hstr2);

//返回hstr长度
int StrLength(HString *hstr);

//清为空串
int ClearString(HString *hstr);

//连接hstr1和hstr2,由dst返回
int Concat(HString *dst,HString *hstr1,HString *hstr2);

//由sub返回src从pos开始len个长度的字串
int SubString(HString *sub, HString *src, int pos, int len);

//返回主串hstr中第pos个字符之后第1次出现的位置，否则返回0
int Index(HString *hstr, HString *sub, int pos);
