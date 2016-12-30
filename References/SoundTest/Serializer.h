#pragma once
#include "SSystem.h"
#include <fstream>
#include <string>
//TODO:���л� �����л�,�������������,��ôʵ��
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
	//���л�ʱ�ļ��������
	std::ofstream m_fout;
	//�����л�ʱ�ļ��������
	std::ifstream m_fin;
	//���л��ļ���
	std::string m_strFileName;
	//дʱ���ļ���ʽ
	int m_nOpenMode;
};

