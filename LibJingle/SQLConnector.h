#pragma once
#include <string>
#include "Poco/Data/MySQL/MySQL.h"
#include "Poco/Data/Session.h"
#include "Poco/SharedPtr.h"
struct AccountInfo
{
	std::string strAccount;
	std::string strPassword;
	std::string strDomain;//����
};

class CSQLConnector
{
public:
	CSQLConnector(void);
	virtual ~CSQLConnector(void);

	bool Connect(const std::string &strHost, int nPort, const std::string &strDbName, const std::string &strUsrName, const std::string &strPassword);

	//************************************
	// Method:    GetAccountInfo
	// FullName:  CSQLConnector::GetAccountInfo
	// Access:    public 
	// Returns:   AccountInfo
	// Qualifier: ͨ��IP��ַ��ȡ��¼�˺ź�����
	// Parameter: const std::string & strIpAddress
	//************************************
	AccountInfo GetAccountInfo(const std::string &strIpAddress);
private:
	void dbInfo(Poco::Data::Session& session);
private:
    static Poco::SharedPtr<Poco::Data::Session> _pSession;
};

