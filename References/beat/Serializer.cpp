#include "StdAfx.h"
#include "Serializer.h"
#include "SString.h"
#include <vector>
#include "Packet.h"
using namespace std;

//只能序列化Packet结构体的数据
//调用顺序:
//1: int ObjectsInFile(); 获取文件中包含数据库的条数
//2: bool Deserialize(void **ppOut, int *pnLength, int nIndex); 送数据块序号,将数据库读取出来缓存到数组中


//格式: 数据块 + 长度
//总块数
Serializer::Serializer(const SCHAR* filename, int mode )
{
	m_nOpenMode = mode;
	if (filename)
	{
#ifdef _UNICODE
		//UNICODE转换为多字节
		vector<char> vc;
		UnicodeToMultiBytes(vc, filename, SSystem::strlen(filename));
		m_strFileName.assign(vc.begin(), vc.end());
#else
		m_strFileName = filename;
#endif
	}
}

bool Serializer::Serialize( const char  *pVoid, int nLength )
{
	//写入文件的格式:  块长度, 块数据,块个数
	//TODO:读,写 都同时打开同一个文件,有问题吗?
	if (NULL == pVoid || 0 == nLength)
	{
		return false;
	}

	//检查文件是否已经打开
	if (!m_fout.is_open())
	{
		//将快条数和每块长度读取出来
		m_fout.open(m_strFileName.c_str(), m_nOpenMode);
		if (!IsGood())
		{
			//TRACE("file is not opened !!");
			return false;
		}
	}

	//数据个数: 长度 +　数据块　

	//写长度
	//m_fout.write((char *)&nLength, 4);
	//写数据
	m_fout.write((char *)pVoid, nLength);
	
	return true;
}


int Serializer::ObjectsInFile()
{
	//数据块个数
	int nObjects = 0;

	//检查文件是否已经打开
	if (!m_fin.is_open())
	{
		m_fin.open(m_strFileName.c_str(), std::ios::binary | std::ios::in);
	}

	//文件末尾四个字节为个数
	if (!IsGood())
	{
		//TRACE("file is not opened !!");

		return 0;
	}
	//文件末尾四个字节为块个数
	if (!m_fin.eof())
	{
		m_fin.seekg(-4, ios::end);
		m_fin.read((char *)&nObjects, 4);
	}

	m_fin.close();

	return nObjects;
}

bool Serializer::Deserialize(void **ppOut, int *pnLength, int nIndex)
{
	if (!m_fin.is_open())
	{
		m_fin.open(m_strFileName.c_str(), std::ios::binary | std::ios::in);
		if (m_fin.bad())
		{
			OutputDebugString(_T("打开文件失败!\n"));
			return false;
		}
	}

	//读四个字节长度
	m_fin.read((char *)pnLength, 4);
	*ppOut = new char[sizeof(Packet)];
	memset(*ppOut, 0, sizeof(Packet));

	//申请内存存放实际数据
	unsigned char *pData = new unsigned char[*pnLength];
	//读取数据
	m_fin.read((char *)pData, *pnLength);
	
	//存内存的地址
	*(unsigned int *)(*ppOut) = (unsigned int)pData;
	//存长度
	*(unsigned int *)((char *)*ppOut + 4) = *pnLength;

	return true;
}

//获取文件长度
int Serializer::GetFileLength()
{
	if (!m_fin.is_open())
	{
		m_fin.open(m_strFileName.c_str(), std::ios::binary | std::ios::in);
		if (m_fin.bad())
		{
			OutputDebugString(_T("打开文件失败!\n"));
			return 0;
		}
	}

	m_fin.seekg(0, ios::end);

	return m_fin.tellg();
}