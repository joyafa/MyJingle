#pragma once
#include "poco\Util\XMLConfiguration.h"
#include <string>
#include "Poco\AutoPtr.h"
using Poco ::Util:: XMLConfiguration;
using Poco ::AutoPtr;

enum TypeLoadStatus
{
        UNLOAD      = 0,   //δ����,��ʼ״̬
        LOADED_FAILED  ,   //����ʧ��
        LOADED_SUCCESS    //����ɹ�
};

class CConfig
{
public:
        /********************************************************************
       *      @Param      :
       *      @CreateTime : 2017-1-20 19:41:13
       *      @Author     : Joyafa
       *   @Return     : CConfig &
       *      @Description: ��ȡ�������õ���
       *********************************************************************/
        static CConfig &GetInstance ()
        {
               static CConfig theInstance;
               return theInstance ;
        }

        /********************************************************************
       *      @Param      : const std::string &strKey: key
       *      @CreateTime : 2017-1-20 19:41:13
       *      @Author     : Joyafa
       *   @Return     : int
       *      @Description: ���������key����stringֵ
       *********************************************************************/
        std::string GetString( const std ::string &strKey);
       
        /********************************************************************
       *      @Param      : const std::string &strKey: key
       *      @CreateTime : 2017-1-20 19:41:13
       *      @Author     : Joyafa
       *   @Return     : int
       *      @Description: ���������key����intֵ
       *********************************************************************/
        int GetInt (const std::string &strKey );
       
        /********************************************************************
       *      @Param      : void
       *   @Param      : const char *pXmlFileName:XML�����ļ�����
       *      @CreateTime : 2017-1-20 19:41:13
       *      @Author     : Joyafa
       *   @Return     : TypeLoadStatus
       *      @Description: ���������ļ�
       *********************************************************************/
        TypeLoadStatus LoadConfigFile (const char * pXmlFileName = NULL);

        /********************************************************************
       *      @Param      :
       *      @CreateTime : 2017-1-20 19:41:13
       *      @Author     : Joyafa
       *   @Return     : void
       *      @Description: ���úڰ������ļ�����·��
       *********************************************************************/
        _inline std ::string GetLastError()
        {
               return m_strErrMsg ;
        }

        /********************************************************************
       *      @Param      : const std::string &strKey: key
       *      @Param      : const std::string &strValue
       *      @CreateTime : 2017-1-20 19:41:13
       *      @Author     : Joyafa
       *   @Return     : bool
       *      @Description: ���������key����stringֵ
       *********************************************************************/
        bool SetString (const std::string &strKey , const std:: string & strValue);
       
       
        //************************************
        // Method:    SetInt
        // FullName:  CConfig::SetInt
        // Access:    public
        // Returns:   bool
        // Parameter: const std::string & strKey:key
        // Parameter: int nValue:��Ҫ���õ�value
        // Description:��intֵ���õ������ļ���
        // Author:    gfjie
        // Modify Time:2017/01/20
        //************************************
        bool SetInt (const std::string &strKey , int nValue);
private:
        CConfig(void );
        virtual ~ CConfig(void );
        std::string m_strErrMsg;
        AutoPtr<XMLConfiguration > m_pXmlConfigure ;
        //�����ļ�����״̬
        TypeLoadStatus m_eLoadCongfileStatus ;
        //XML�����ļ�·��
		std::string m_strConfigurePath;
};
