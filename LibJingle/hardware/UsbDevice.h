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
	UNKNOWN_KEY,   //未知
	LEFT_KEY,  //左键,一般为呼叫
	RIGHT_KEY  //右键,一般为挂断或拒绝接听
};

class CUsbDevice
{
public:
    CUsbDevice(int vid, int pid);
	~CUsbDevice();
	//连接设备
	bool ConnectDevice();
	//读字串
	void ReadDeviceString();
	//断开连接
	bool Disconnect();
	//读取数据
	bool ReadData();
	//发送数据
	bool SendData();

	//************************************
	// Method:    IsConnectDevice
	// Access:    public 
	// Returns:   bool
	// Qualifier: const
	// Description:检查设备是否已连接
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

	//主界面所在窗口句柄
	HWND m_hWnd;
	//PID
	int m_usbPid;
    //VID
	int m_usbVid;
	//设备句柄
	HANDLE m_hidHandle;
	//TODO:设备路径
	//应该需要指定设备路径，因为会有多个设备路径的vid pid是跟输入的一致的，这些其实都不是我需要的，只要keyboard才是我需要的，其他的读不到数据

	//发送线程
	HANDLE m_hSendThread;

	//接收线程
	HANDLE m_hRecvThread;

	//退出接收线程
	bool m_bExitRecvThread;
	//退出发送线程
	bool m_bExitSendThread;
	//设备路径
	CString m_strDevicePath;
};