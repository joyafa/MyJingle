// SoundTest.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "SoundCapture.h"
#include "Serializer.h"
#include "SoundRecorder.h"
#include "Packet.h"
#include <iostream>
using namespace std;

void TestSerializer()
{
	SoundSerializer serizer(_S("SoundCapture.pcm"));
	int nLength = 256;
	
	for (int i=0;i<100;++i)
	{
		char *buffer = new char[nLength];

		for (int j=0;j<nLength;++j)
		{
			srand(time(NULL));
			//Sleep(10);
			buffer[j] = i;
			cout << hex << (unsigned char )buffer[j] << " ";
			if (j % 50 == 0 ) cout << endl;
		}
		Packet *pPacket = Packet::TmpGetPacketFromBuffer(buffer, nLength);
		serizer.AddPacket(pPacket, 0);
		delete [] buffer;
	}
}

void TestDeSerializer()
{
	SoundDeserializer deSerizer(_S("SoundCapture.pcm"));
	deSerizer.Init();
	deSerizer.Exit();
}
int _tmain(int argc, _TCHAR* argv[])
{
	TestSerializer();
	TestDeSerializer();
	return 1;
	WAVEFORMATEX format;
	format.wFormatTag = WAVE_FORMAT_PCM;
	format.nChannels = 1;//立体声
	format.nSamplesPerSec = 8000;
	format.nAvgBytesPerSec = 16000;
	format.nBlockAlign = 2;
	format.wBitsPerSample = 16;
	format.cbSize = 0;

	DirectXFullDuplexAudioDevice* pDevice = GetDirectXFullDuplexAudioDeviceInstance(format, NULL);
	pDevice->Init();

	SoundCapture *pCapture = new SoundCaptureImpl(pDevice->Format(), NULL);
	pCapture->Init();

	return 0;
}

