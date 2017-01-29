#include "afxwin.h"
#include <string>
CString GetMoudlePath();

std::string CString2String(const CString &str);
CString GetMoudleConfigIniPath();

int UTF82Unicode(LPCSTR pInput, int nInputLen, WCHAR **ppOutPut, int* nOutputLen);
CString UTF82Unicode(LPCSTR pUtf8);
std::string GetIPAddress();
