// SoundTest.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "SoundCapture.h"
#include "Serializer.h"
#include "SoundRecorder.h"
#include "Packet.h"
#include <iostream>
#include "SoundPlay.h"
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


int _tmain111(int argc, _TCHAR* argv[])
{
	//TestSerializer();
	//TestDeSerializer();
	//return 1;
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


#include <windows.h>
#include   "mmsystem.h" 
#include <tchar.h>


LRESULT CALLBACK WndProc(HWND,UINT,WPARAM,LPARAM);

int APIENTRY WinMain(HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR     lpCmdLine,
	int       iCmdShow)
{
	// TODO: Place code here.
	static   TCHAR szAppName[]=TEXT("HelloWin");
	HWND     hwnd;
	MSG      msg;
	WNDCLASS wndclass;
	wndclass.style         =CS_HREDRAW|CS_VREDRAW;
	wndclass.lpfnWndProc   =WndProc;
	wndclass.cbClsExtra    =0;
	wndclass.cbWndExtra    =0;
	wndclass.hInstance     =hInstance;
	wndclass.hIcon         =LoadIcon(NULL,IDI_APPLICATION);
	wndclass.hCursor       =LoadCursor(NULL,IDC_ARROW);
	wndclass.hbrBackground =(HBRUSH)GetStockObject(WHITE_BRUSH);
	wndclass.lpszMenuName  =NULL;
	wndclass.lpszClassName =szAppName;

	if(!RegisterClass(&wndclass))
	{
		MessageBox(NULL,TEXT("This program requires Windows NT!"),szAppName,MB_ICONERROR);
		return 0;
	}

	hwnd=CreateWindow(szAppName,                  //window class name
		TEXT("The Hello Program"),  //window caption
		WS_OVERLAPPEDWINDOW,        //window style
		CW_USEDEFAULT,              //initial x position
		CW_USEDEFAULT,              //initial y position
		CW_USEDEFAULT,              //initial x size
		CW_USEDEFAULT,              //initial y size
		NULL,                       //parent window handle
		NULL,                       //window menu handle
		hInstance,                  //program instance handle
		NULL);                      //creation parameters
	ShowWindow(hwnd,SW_SHOWNORMAL);
	UpdateWindow(hwnd);

	while(GetMessage(&msg,NULL,0,0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return 0;
}

WAVEFORMATEX format;
void InitFormat();
void ReadPcmData();

::SoundCaptureImpl* m_SoundCapture;
::SoundPlayImpl* m_SoundPlay;
::JitterBuffer m_JitterBuffer;

LRESULT CALLBACK WndProc(HWND hwnd,UINT message,WPARAM wParam,LPARAM lParam)
{
	HDC          hdc;
	PAINTSTRUCT  ps;
	RECT         rect;

	switch(message) 
	{
	case WM_LBUTTONDOWN:
		{
			//读文件中的数据到jittbuffer中
			ReadPcmData();
			m_SoundPlay = new ::SoundPlayImpl(format, hwnd);
			m_SoundPlay->JitterBuf(&m_JitterBuffer);
			m_SoundPlay->Init();
			m_SoundPlay->Start();
		}
		break;
	case WM_RBUTTONDOWN:
		{
			//读文件中的数据到jittbuffer中
			m_SoundCapture = new ::SoundCaptureImpl(format, hwnd);
			m_SoundCapture->Init();
			m_SoundCapture->Start();
		}
		break;
	case WM_CREATE:
		InitFormat();
		m_JitterBuffer.Init();
		return 0;
	case WM_PAINT:
		hdc=BeginPaint(hwnd,&ps);
		GetClientRect(hwnd,&rect);
		DrawText(hdc,TEXT("声音测试！"),-1,&rect,DT_SINGLELINE|DT_CENTER|DT_VCENTER);
		rect.top += 40;
		DrawText(hdc,TEXT("左键按下开始播放！"),-1,&rect,DT_SINGLELINE|DT_CENTER|DT_VCENTER);
		rect.top += 40;
		DrawText(hdc,TEXT("右键按下开始录音！"),-1,&rect,DT_SINGLELINE|DT_CENTER|DT_VCENTER);
		EndPaint(hwnd,&ps);

		return 0;
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}

	return DefWindowProc(hwnd,message,wParam,lParam);

}

void ReadPcmData()
{
	SoundDeserializer deSerizer(_S("TestRecord.pcm"));
	deSerizer.Init();
	deSerizer.SetJitterBuffer(&m_JitterBuffer);
	deSerizer.SetJitterBufferData();
}

void InitFormat()
{
	format.wFormatTag = WAVE_FORMAT_PCM;
	format.nChannels = 1;//立体声
	format.nSamplesPerSec = 8000;
	format.nAvgBytesPerSec = 16000;
	format.nBlockAlign = 2;
	format.wBitsPerSample = 16;
	format.cbSize = 0;
}

