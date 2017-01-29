#pragma once
#include "poco\Util\XMLConfiguration.h"
#include <string>
#include "Poco\AutoPtr.h"
using Poco ::Util:: XMLConfiguration;
using Poco ::AutoPtr;

enum TypeLoadStatus
{
        UNLOAD      = 0,   //未导入,初始状态
        LOADED_FAILED  ,   //导入失败
        LOADED_SUCCESS    //导入成功
};

class CConfig
{
public:
        /********************************************************************
       *      @Param      :
       *      @CreateTime : 2017-1-20 19:41:13
       *      @Author     : Joyafa
       *   @Return     : CConfig &
       *      @Description: 获取配置引用单例
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
       *      @Description: 根据输入的key查找string值
       *********************************************************************/
        std::string GetString( const std ::string &strKey);
       
        /********************************************************************
       *      @Param      : const std::string &strKey: key
       *      @CreateTime : 2017-1-20 19:41:13
       *      @Author     : Joyafa
       *   @Return     : int
       *      @Description: 根据输入的key查找int值
       *********************************************************************/
        int GetInt (const std::string &strKey );
       
        /********************************************************************
       *      @Param      : void
       *   @Param      : const char *pXmlFileName:XML配置文件名称
       *      @CreateTime : 2017-1-20 19:41:13
       *      @Author     : Joyafa
       *   @Return     : TypeLoadStatus
       *      @Description: 加载配置文件
       *********************************************************************/
        TypeLoadStatus LoadConfigFile (const char * pXmlFileName = NULL);

        /********************************************************************
       *      @Param      :
       *      @CreateTime : 2017-1-20 19:41:13
       *      @Author     : Joyafa
       *   @Return     : void
       *      @Description: 设置黑白名单文件完整路径
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
       *      @Description: 根据输入的key查找string值
       *********************************************************************/
        bool SetString (const std::string &strKey , const std:: string & strValue);
       
       
        //************************************
        // Method:    SetInt
        // FullName:  CConfig::SetInt
        // Access:    public
        // Returns:   bool
        // Parameter: const std::string & strKey:key
        // Parameter: int nValue:需要设置的value
        // Description:将int值设置到配置文件中
        // Author:    gfjie
        // Modify Time:2017/01/20
        //************************************
        bool SetInt (const std::string &strKey , int nValue);
private:
        CConfig(void );
        virtual ~ CConfig(void );
        std::string m_strErrMsg;
        AutoPtr<XMLConfiguration > m_pXmlConfigure ;
        //配置文件导入状态
        TypeLoadStatus m_eLoadCongfileStatus ;
        //XML配置文件路径
		std::string m_strConfigurePath;
};
