#pragma once
#include "SSystem.h"
#include <fstream>

class Serializer
{
public:
	Serializer(const char* filename, int mode = std::ios::app | std::ios::binary | std::ios::out );
	virtual ~Serializer(void){m_fout.close();}

	int Serialize(const char *pVoid, int nLength);
	int Deserialize(void **ppOut, int nLength, int nIndex);

	int ObjectsInFile();
	bool IsGood()
	{
		return m_fout.good();
	}
private:
	std::ofstream m_fout;
};

