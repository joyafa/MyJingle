#include "StdAfx.h"
#include "Serializer.h"
using namespace std;

Serializer::Serializer(const char* filename, int mode )
{
	m_fout.open(filename, mode);
}

int Serializer::Serialize( const char  *pVoid, int nLength )
{
	if (NULL == pVoid || 0 == nLength)
	{
		return ;
	}
	if (!IsGood())
	{
		//TRACE("file is not opened !!");
		return 0;
	}
	//m_fout.seekp(
	m_fout.write((char *)pVoid, nLength);

	return 1;
}


int Serializer::ObjectsInFile()
{
	//�ļ�ĩβ�ĸ��ֽ�Ϊ����
	if (!IsGood())
	{
		//TRACE("file is not opened !!");
		return 0;
	}
	std::ofstream  fin();

	m_fout.seekp(4, ios::end);
	int nObjects = 0;
	//m_fout.read(&nObjects, sizeof(int));
	//������ô����

    m_fout.seekp(0, ios::end);

	return nObjects;
}

int Serializer::Deserialize(void **ppOut, int nLength, int nIndex)
{


	return 1;

}