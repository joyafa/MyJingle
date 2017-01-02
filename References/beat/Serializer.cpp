#include "StdAfx.h"
#include "Serializer.h"
#include "SString.h"
#include <vector>
#include "Packet.h"
using namespace std;

//ֻ�����л�Packet�ṹ�������
//����˳��:
//1: int ObjectsInFile(); ��ȡ�ļ��а������ݿ������
//2: bool Deserialize(void **ppOut, int *pnLength, int nIndex); �����ݿ����,�����ݿ��ȡ�������浽������


//��ʽ: ���ݿ� + ����
//�ܿ���
Serializer::Serializer(const SCHAR* filename, int mode )
{
	m_nOpenMode = mode;
	if (filename)
	{
#ifdef _UNICODE
		//UNICODEת��Ϊ���ֽ�
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
	//д���ļ��ĸ�ʽ:  �鳤��, ������,�����
	//TODO:��,д ��ͬʱ��ͬһ���ļ�,��������?
	if (NULL == pVoid || 0 == nLength)
	{
		return false;
	}

	//����ļ��Ƿ��Ѿ���
	if (!m_fout.is_open())
	{
		//����������ÿ�鳤�ȶ�ȡ����
		m_fout.open(m_strFileName.c_str(), m_nOpenMode);
		if (!IsGood())
		{
			//TRACE("file is not opened !!");
			return false;
		}
	}

	//���ݸ���: ���� +�����ݿ顡

	//д����
	//m_fout.write((char *)&nLength, 4);
	//д����
	m_fout.write((char *)pVoid, nLength);
	
	return true;
}


int Serializer::ObjectsInFile()
{
	//���ݿ����
	int nObjects = 0;

	//����ļ��Ƿ��Ѿ���
	if (!m_fin.is_open())
	{
		m_fin.open(m_strFileName.c_str(), std::ios::binary | std::ios::in);
	}

	//�ļ�ĩβ�ĸ��ֽ�Ϊ����
	if (!IsGood())
	{
		//TRACE("file is not opened !!");

		return 0;
	}
	//�ļ�ĩβ�ĸ��ֽ�Ϊ�����
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
			OutputDebugString(_T("���ļ�ʧ��!\n"));
			return false;
		}
	}

	//���ĸ��ֽڳ���
	m_fin.read((char *)pnLength, 4);
	*ppOut = new char[sizeof(Packet)];
	memset(*ppOut, 0, sizeof(Packet));

	//�����ڴ���ʵ������
	unsigned char *pData = new unsigned char[*pnLength];
	//��ȡ����
	m_fin.read((char *)pData, *pnLength);
	
	//���ڴ�ĵ�ַ
	*(unsigned int *)(*ppOut) = (unsigned int)pData;
	//�泤��
	*(unsigned int *)((char *)*ppOut + 4) = *pnLength;

	return true;
}

//��ȡ�ļ�����
int Serializer::GetFileLength()
{
	if (!m_fin.is_open())
	{
		m_fin.open(m_strFileName.c_str(), std::ios::binary | std::ios::in);
		if (m_fin.bad())
		{
			OutputDebugString(_T("���ļ�ʧ��!\n"));
			return 0;
		}
	}

	m_fin.seekg(0, ios::end);

	return m_fin.tellg();
}