#include "SQLConnector.h"
#include "Poco/Tuple.h"
#include "Poco/NamedTuple.h"
#include "Poco/Data/LOB.h"
#include "Poco/Data/StatementImpl.h"
#include "Poco/Data/MySQL/Connector.h"
#include "Poco/Data/MySQL/Utility.h"
#include "Poco/Data/MySQL/MySQLException.h"
#include "Poco/Nullable.h"
#include "Poco/Data/DataException.h"
#include "beat/beatLog_.h"
using namespace Poco::Data;
using namespace Poco::Data::Keywords;
using Poco::Data::MySQL::ConnectionException;
using Poco::Data::MySQL::Utility;
using Poco::Data::MySQL::StatementException;
using Poco::format;
using Poco::Environment;
using Poco::NotFoundException;
using Poco::Int32;
using Poco::Nullable;
using Poco::Tuple;
using Poco::NamedTuple;
using namespace std;

Poco::SharedPtr<Poco::Data::Session> CSQLConnector::_pSession = nullptr;

CSQLConnector::CSQLConnector(void)
{
}


CSQLConnector::~CSQLConnector(void)
{
}

bool CSQLConnector::Connect(const std::string &strHost, int nPort, const std::string &strDbName, const std::string &strUsrName, const std::string &strPassword)
{
	MySQL::Connector::registerConnector();
	string _dbConnString;
	_dbConnString = "host=" + strHost;
	_dbConnString += ";user=" + strUsrName;
	_dbConnString += ";password=" + strPassword;
	_dbConnString += ";db=" + strDbName;
	_dbConnString += ";compress=true";
	_dbConnString += ";auto-reconnect=true";
	_dbConnString += ";secure-auth=true";

	try
	{
		_pSession = new Session(MySQL::Connector::KEY, _dbConnString);
	}
	catch (ConnectionFailedException& ex)
	{
		beatLog_Error(("CSQLConnector", __FUNCDNAME__, "An Exception has occured while connecting the mysql server:%s!", ex.displayText() ));

		return false;
	}


	return true;
}


AccountInfo CSQLConnector::GetAccountInfo( const std::string &strIpAddress )
{
	if (!_pSession) 
	{
		beatLog_Info(("CSQLConnector", __FUNCDNAME__, "Session is not valid"));

		return AccountInfo();
	}

	Nullable<std::string> resUserName;
	Nullable<std::string> resPassword;
	
	//通过ip地址获取登录名和密码
	Nullable<std::string> address(strIpAddress);
	*_pSession << "SELECT username, Plainpassword FROM ofuser WHERE ip = ?", into(resUserName), into(resPassword), use(address), now;
	AccountInfo info;
	info.strAccount  = resUserName;
	info.strPassword = resPassword;

	return info;
}

void CSQLConnector::dbInfo(Session& session)
{
	beatLog_Info(("CSQLConnector", __FUNCDNAME__, "Server Info: %s",     Utility::serverInfo(session).c_str() ));
	beatLog_Info(("CSQLConnector", __FUNCDNAME__, "Server Version: %ul" ,Utility::serverVersion(session)));
	beatLog_Info(("CSQLConnector", __FUNCDNAME__, "Host Info: %s",       Utility::hostInfo(session).c_str()));
}