#include "stdafx.h"
#include "Config.h"
#include <string>
#include "Poco\Exception.h"
#include "Common.h"
using namespace std;
using Poco::NotFoundException;

CConfig::CConfig (void)
        : m_pXmlConfigure (new XMLConfiguration())
        , m_eLoadCongfileStatus (UNLOAD)
        , m_strConfigurePath ("")
{
}


CConfig::~CConfig (void)
{
}


TypeLoadStatus CConfig ::LoadConfigFile( const char *pXmlFileName )
{
        if ( UNLOAD != m_eLoadCongfileStatus)
        {
               return m_eLoadCongfileStatus ;
        }

        m_eLoadCongfileStatus = LOADED_FAILED;

        using std ::string;
        char chErrMsg [512] = {0};

        //加载配置文件
		CString strIniPath;
        if ( NULL == pXmlFileName)
        {
               strIniPath = GetMoudlePath() + _T("VoiceChat.xml");
        }
        else
        {
               strIniPath = pXmlFileName;
        }
		m_strConfigurePath  = CString2String(strIniPath);

        using Poco ::NotFoundException;
        using Poco ::NullPointerException;

        try
        {
               m_pXmlConfigure->load (m_strConfigurePath);
               m_eLoadCongfileStatus = LOADED_SUCCESS;
        }
        catch ( NullPointerException & e)
        {
               OutputDebugStringA(e .what());
               sprintf(chErrMsg , "解析配置文件:[%s]失败,原因:[%s]" , m_strConfigurePath.c_str(), e .what());
               m_strErrMsg = chErrMsg;
        }
        catch ( std::exception &e )
        {
               OutputDebugStringA(e .what());
               sprintf(chErrMsg , "解析配置文件:[%s]失败,原因:[%s]" , m_strConfigurePath.c_str(), e .what());
               m_strErrMsg = chErrMsg;
        }

        return m_eLoadCongfileStatus ;
}


bool CConfig ::SetString( const std ::string &strKey, const std::string &strValue )
{
        if ( LOADED_SUCCESS == m_eLoadCongfileStatus && m_pXmlConfigure )
        {
               m_pXmlConfigure->setString (strKey, strValue);
               m_pXmlConfigure->save (m_strConfigurePath);

               return true ;
        }

        return false ;
}


bool CConfig ::SetInt( const std ::string &strKey, int nValue)
{
        if ( LOADED_SUCCESS == m_eLoadCongfileStatus && m_pXmlConfigure )
        {
               m_pXmlConfigure->setInt (strKey, nValue);
               m_pXmlConfigure->save (m_strConfigurePath);

               return true ;
        }

        return false ;
}


std::string CConfig:: GetString(const std:: string & strKey)
{
        string strValue = "";

        if ( LOADED_SUCCESS == m_eLoadCongfileStatus && m_pXmlConfigure )
        {
               m_strErrMsg = "";

               try
               {
                      strValue = m_pXmlConfigure-> getString(strKey );
               }
               catch ( NotFoundException & e)
               {
                      char chErrMsg [512] = {0};
                      sprintf(chErrMsg , "参数[%s]未设置:%s" , strKey .c_str(), e.what ());
                      m_strErrMsg = chErrMsg;
               }
			   catch (std::exception &e)
			   {
				   char chErrMsg [512] = {0};
				   sprintf(chErrMsg , "参数[%s]未设置:%s" , strKey .c_str(), e.what ());
				   m_strErrMsg = chErrMsg;
			   }
        }

        return strValue ;
}


int CConfig ::GetInt( const std ::string &strKey)
{
        int nValue = 0;

        if ( LOADED_SUCCESS == m_eLoadCongfileStatus && m_pXmlConfigure )
        {
               m_strErrMsg = "";
               try
               {
                      nValue = m_pXmlConfigure-> getInt(strKey );
               }
			   catch ( Poco::NotFoundException & e)
               {
                      char chErrMsg [512] = {0};
                      sprintf(chErrMsg , "参数[%s]未设置:%s" , strKey .c_str(), e.what ());
                      m_strErrMsg = chErrMsg;
               }
			   catch (std::exception &e)
			   {
				   char chErrMsg [512] = {0};
				   sprintf(chErrMsg , "参数[%s]未设置:%s" , strKey .c_str(), e.what ());
				   m_strErrMsg = chErrMsg;
			   }
        }

        return nValue ;
}

