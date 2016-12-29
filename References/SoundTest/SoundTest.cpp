// SoundTest.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "SoundCapture.h"

int _tmain(int argc, _TCHAR* argv[])
{
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

