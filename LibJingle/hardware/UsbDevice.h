#pragma once
#include <stdio.h>
#include <string>
#include <concrt.h>
#include <winbase.h>
#include <windef.h>
using namespace std;

#define WM_PHONE (WM_USER + 4)

enum HardwareEventType
{
	UNKNOWN_KEY,   //δ֪
	LEFT_KEY,  //���,һ��Ϊ����
	RIGHT_KEY  //�Ҽ�,һ��Ϊ�Ҷϻ�ܾ�����
};

class CUsbDevice
{
public:
    CUsbDevice(int vid, int pid);
	~CUsbDevice();
	//�����豸
	bool ConnectDevice();
	//���ִ�
	void ReadDeviceString();
	//�Ͽ�����
	bool Disconnect();
	//��ȡ����
	bool ReadData();
	//��������
	bool SendData();

	//************************************
	// Method:    IsConnectDevice
	// Access:    public 
	// Returns:   bool
	// Qualifier: const
	// Description:����豸�Ƿ�������
	// CreateTime: 2017/2/13
	//************************************
	bool IsConnectDevice() const 
	{
		return (m_hidHandle != INVALID_HANDLE_VALUE);
	}

	void SetOwner(HWND hWnd)
	{
		m_hWnd = hWnd; 
	}

	//���������ڴ��ھ��
	HWND m_hWnd;
	//PID
	int m_usbPid;
    //VID
	int m_usbVid;
	//�豸���
	HANDLE m_hidHandle;
	//TODO:�豸·��
	//Ӧ����Ҫָ���豸·������Ϊ���ж���豸·����vid pid�Ǹ������һ�µģ���Щ��ʵ����������Ҫ�ģ�ֻҪkeyboard��������Ҫ�ģ������Ķ���������

	//�����߳�
	HANDLE m_hSendThread;

	//�����߳�
	HANDLE m_hRecvThread;

	//�˳������߳�
	bool m_bExitRecvThread;
	//�˳������߳�
	bool m_bExitSendThread;
	//�豸·��
	CString m_strDevicePath;
};