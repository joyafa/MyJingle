#include "afxwin.h"
#include <string>
CString GetMoudlePath();

///CString to string
std::string CString2String(const CString &str);
//获取模块配置文件路径，在exe同级目录的etc下
CString GetMoudleConfigPath();
//获取模块资源文件路径，在exe同级目录的res下
CString GetMoudleResPath();
CString GetMoudleConfigFileName();
int UTF82Unicode(LPCSTR pInput, int nInputLen, WCHAR **ppOutPut, int* nOutputLen);
CString UTF82Unicode(LPCSTR pUtf8);
std::string GetIPAddress();
CStringA Unicode2UTF8(LPCWSTR pInput);
int Unicode2UTF8(LPCWSTR pInput, int nInputLen, char **ppOutPut, int* nOutputLen);

