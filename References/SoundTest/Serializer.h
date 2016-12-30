#pragma once
#include "SSystem.h"
#include <fstream>
#include <string>
//TODO:序列化 反序列化,操作都是这个类,怎么实现
class Serializer
{
public:
	Serializer(const SCHAR* filename, int mode = std::ios::trunc | std::ios::binary | std::ios::out );
	virtual ~Serializer(void)
	{
		m_fout.close();
	}

	bool Serialize(const char *pVoid, int nLength);

	int ObjectsInFile();
	bool Deserialize(void **ppOut, int *pnLength, int nIndex);

	int GetFileLength();

	bool IsGood()
	{
		return m_fout.good();
	}
private:
	//序列化时文件输出对象
	std::ofstream m_fout;
	//反序列化时文件输入对象
	std::ifstream m_fin;
	//序列化文件名
	std::string m_strFileName;
	//写时打开文件方式
	int m_nOpenMode;
};

