#include "afxwin.h"
#include <string>
CString GetMoudlePath();

///CString to string
std::string CString2String(const CString &str);
//��ȡģ�������ļ�·������exeͬ��Ŀ¼��etc��
CString GetMoudleConfigPath();
//��ȡģ����Դ�ļ�·������exeͬ��Ŀ¼��res��
CString GetMoudleResPath();
CString GetMoudleConfigFileName();
int UTF82Unicode(LPCSTR pInput, int nInputLen, WCHAR **ppOutPut, int* nOutputLen);
CString UTF82Unicode(LPCSTR pUtf8);
std::string GetIPAddress();
CStringA Unicode2UTF8(LPCWSTR pInput);
int Unicode2UTF8(LPCWSTR pInput, int nInputLen, char **ppOutPut, int* nOutputLen);

