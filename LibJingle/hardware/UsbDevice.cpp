#include "stdafx.h"
#include "UsbDevice.h"
using namespace std;

extern "C"
{
#include "setupapi.h"
#include "hidsdi.h"
}   

#pragma	comment(lib,"setupapi")
#pragma	comment(lib,"hid")
#define MAX_DATA_LEN  1024
#define RECV_DATA_LEN 1024

void RecvThreadFunction(HANDLE m_hidHandle);
void SendThreadFunction(HANDLE m_hidHandle);

void  RecvThreadFunction(LPVOID lpParameter   )
{
	CUsbDevice *pUsbDevice = (CUsbDevice *)lpParameter;
	char *recvDataBuf=new char[RECV_DATA_LEN];
	DWORD recvdBytes,Result;
	HANDLE hEventObject;
	PHIDP_PREPARSED_DATA PreparsedData;
	HIDP_CAPS Capabilities;
	BOOL i =1;
	bool flag=FALSE;
	OVERLAPPED HIDOverlapped;
	hEventObject=CreateEvent(NULL,TRUE,TRUE,NULL);
	HIDOverlapped.hEvent=hEventObject;
	HIDOverlapped.Offset=0;
	HIDOverlapped.OffsetHigh=0;
	if(pUsbDevice->m_hidHandle==INVALID_HANDLE_VALUE)
	{
		//"�豸��δ����!");
		return;
	}
	if(!HidD_GetPreparsedData(pUsbDevice->m_hidHandle,&PreparsedData))
	{
	    ULONG  tang=GetLastError();
		/*char   tem[100];
		sprintf(tem,"Could not get HID preparsed data!");*/
	}

	NTSTATUS status=HidP_GetCaps(PreparsedData, &Capabilities);
	
	while(!pUsbDevice->m_bExitRecvThread)
	{
		ResetEvent(hEventObject);
		ZeroMemory(recvDataBuf,RECV_DATA_LEN);
		Result=ReadFile(pUsbDevice->m_hidHandle, recvDataBuf,Capabilities.InputReportByteLength,
			&recvdBytes,(LPOVERLAPPED)&HIDOverlapped); //��������	(LPOVERLAPPED)&HIDOverlapped
		ULONG  tang=GetLastError();
		Result=WaitForSingleObject(hEventObject,5000);
		if(Result==WAIT_TIMEOUT||Result==WAIT_ABANDONED)//�糬ʱ������ȡ������
		{
			CancelIo(pUsbDevice->m_hidHandle);
			i=0;
		}
		if(i==1)//���ǳ�ʱ��������ʾ����
		{
			CHAR tem[180000]="",tem2[512]="";
			/*CHAR tem4[3];
			if(HidP_GetUsageValueArray(HidP_Input,0,0,2,tem4,3,PreparsedData,recvDataBuf,Capabilities.InputReportByteLength)==HIDP_STATUS_SUCCESS)
			{
				sprintf(&tem2[0],"%02X %02X %02X ",(UCHAR)tem4[0],(UCHAR)tem4[1],(UCHAR)tem4[2]);				
			}
			else
				sprintf(&tem2[0],"%02X %02X %02X ",(UCHAR)5,(UCHAR)5,(UCHAR)5);
			*/
			//TODO: �������̣߳������� a5 a6���ͱ�ʾ�������ݣ��¼�֪ͨ׼��������Ҷ�
			for(int i=0,j=0;i<Capabilities.InputReportByteLength;i++,j+=3)
			{
				sprintf(&tem[j],"%02X ",(UCHAR)recvDataBuf[i]);
			}
			sprintf(tem2,"%02X %02X %02X %02X %02X %02X \n",(UCHAR)recvDataBuf[2],(UCHAR)recvDataBuf[3],(UCHAR)recvDataBuf[4],(UCHAR)recvDataBuf[5],(UCHAR)recvDataBuf[6],(UCHAR)recvDataBuf[7]);
			OutputDebugStringA(tem2);
			strcat(tem,tem2);
			OutputDebugStringA(tem);
			ZeroMemory(tem,180000);
			ZeroMemory(tem2,512);
		}
		i=1;
		Sleep(1);
	}
	delete [] recvDataBuf;
	return;
}				


void  SendThreadFunction(LPVOID lpParameter)
{
	CUsbDevice *pUsbDevice = (CUsbDevice *)lpParameter;
	ULONG OutputReportLen=0;
	ULONG UsageLength = 0;
	PHIDP_PREPARSED_DATA PreparsedData;
	HANDLE hEventObject;
	DWORD SendBytes=0,Result;
	HIDP_CAPS Capabilities;
	NTSTATUS status;
	BOOL i=1;
	bool flag=FALSE;
	OVERLAPPED HIDOverlapped;
	hEventObject=CreateEvent(NULL,TRUE,TRUE,NULL);
	HIDOverlapped.hEvent=hEventObject;
	HIDOverlapped.Offset=0;
	HIDOverlapped.OffsetHigh=0;  
	USAGE UsageList[3];
    UsageList[0]=0;
    UsageList[1]=0;
	UsageList[2]=0;
	if(pUsbDevice->m_hidHandle==INVALID_HANDLE_VALUE)
	{
		/*char   tem[100];
		sprintf(tem,"�豸��δ����!");*/
		return;
	}
	if(!HidD_GetPreparsedData(pUsbDevice->m_hidHandle,&PreparsedData))
	{
		/*  ULONG  tang=GetLastError();
		char   tem[100];
		sprintf(tem,"Could not get HID preparsed data!");*/
	}
	status=HidP_GetCaps(PreparsedData, &Capabilities);
	OutputReportLen=Capabilities.OutputReportByteLength;
	char * OutputReport = new char[OutputReportLen];
	ZeroMemory(OutputReport,OutputReportLen);
    OutputReport[0]=0x00; 
    OutputReport[1]=0x56; 
    OutputReport[2]=0x08;
    status = HidP_SetButtons( HidP_Output, HID_USAGE_PAGE_GENERIC, 0,
			UsageList, &UsageLength, PreparsedData, OutputReport, OutputReportLen);
 	if( status!=HIDP_STATUS_SUCCESS)
	{
		delete OutputReport;
		return;
	}
	while(!pUsbDevice->m_bExitSendThread)
	{
		ResetEvent(hEventObject);
		Result=GetLastError();
		Result=WriteFile(pUsbDevice->m_hidHandle, OutputReport,OutputReportLen,
			&SendBytes, (LPOVERLAPPED)&HIDOverlapped); //��������
		Result=GetLastError();
		Result=WaitForSingleObject(hEventObject,1000);
		if(Result==WAIT_TIMEOUT||Result==WAIT_ABANDONED)//�糬ʱ������ȡ������
		{
			CancelIo(pUsbDevice->m_hidHandle);
			i=0;
		}		
		if(i==1)//���ǳ�ʱ��������ʾ����
		{
			CHAR tem[180000]="",tem2[20]="";
			sprintf(tem2,"%02X %02X %02X %02X %02X %02X ",(UCHAR)OutputReport[2],(UCHAR)OutputReport[3],(UCHAR)OutputReport[4],(UCHAR)OutputReport[5],(UCHAR)OutputReport[6],(UCHAR)OutputReport[7]);
			//strcat(tem,tem2);
			OutputDebugStringA(tem2);
			//ZeroMemory(tem,180000);
		}
		i=1;
		if(OutputReport[2]==0xff)
          OutputReport[2]=0x00;	
		else if(OutputReport[3]==0xff)
          OutputReport[3]=0x00;	
		else if(OutputReport[4]==0xff)
          OutputReport[4]=0x00;	
		else if(OutputReport[5]==0xff)
          OutputReport[5]=0x00;	
		else if(OutputReport[6]==0xff)
          OutputReport[6]=0x00;	
		else if(OutputReport[7]==0xff)
          OutputReport[7]=0x00;	
		OutputReport[2]++;
		OutputReport[3]++;
		OutputReport[4]++;
		OutputReport[5]++;
		OutputReport[6]++;
		OutputReport[7]++;
	}
	delete OutputReport;
	return;
}				



CUsbDevice::CUsbDevice( int vid, int pid )
	: m_usbPid(pid)
	, m_usbVid(vid)
	, m_hidHandle(INVALID_HANDLE_VALUE)
	, m_bExitRecvThread(false)
	, m_bExitSendThread(false)
{

}

CUsbDevice::~CUsbDevice()
{
	m_bExitRecvThread = true;
	m_bExitSendThread = true;
	if (INVALID_HANDLE_VALUE != m_hRecvThread)
	{
		CloseHandle(m_hRecvThread);
	}
	if (INVALID_HANDLE_VALUE != m_hSendThread)
	{
		CloseHandle(m_hSendThread);
	}
	if (INVALID_HANDLE_VALUE != m_hidHandle)
	{
		CloseHandle(m_hidHandle);
	}
}

bool CUsbDevice::ConnectDevice()
{
	int deviceNo=0;
	ULONG m_MaxDataLen;
	char tem[100]="";
	char szText[64];

	if(m_hidHandle!=INVALID_HANDLE_VALUE)
	{
		 TRACE("�豸������! \n");
		 return true;
	}

	GUID hidGuid;
	HidD_GetHidGuid(&hidGuid);//ȡ��HID����GUID

	HDEVINFO hDevInfo=SetupDiGetClassDevs(&hidGuid,NULL,NULL,
		(DIGCF_PRESENT|DIGCF_DEVICEINTERFACE));//��ȡHID�豸��Ϣ
	if(!hDevInfo)//ʧ���򱨴�
	{
         TRACE("��ȡHID�豸��Ϣʧ��!");
		 return false;
	}

	SP_DEVICE_INTERFACE_DATA devInfoData;
	devInfoData.cbSize = sizeof(SP_DEVICE_INTERFACE_DATA);
	SetLastError(NO_ERROR);
	while(GetLastError()!=ERROR_NO_MORE_ITEMS)
	{
		if(SetupDiEnumInterfaceDevice  (hDevInfo,0,&hidGuid,deviceNo,&devInfoData))//ʶ��HID�ӿ�
		{
			ULONG requireLength=0;
			SetupDiGetInterfaceDeviceDetail(hDevInfo,&devInfoData,NULL,0,&requireLength,NULL);
			PSP_INTERFACE_DEVICE_DETAIL_DATA devDetail=
				(SP_INTERFACE_DEVICE_DETAIL_DATA*) malloc (requireLength);
			devDetail->cbSize=sizeof(SP_INTERFACE_DEVICE_DETAIL_DATA);
			//��ȡ�豸·��
			if(!SetupDiGetInterfaceDeviceDetail(hDevInfo,&devInfoData,devDetail,requireLength,NULL,NULL))
			{
				MessageBoxA(NULL,"��ȡHID�豸ϸ����Ϣʧ��!","Greetings",0);

				free(devDetail);

				return false;
			}

			OutputDebugStringA("DvicePath: ");
			OutputDebugString(devDetail->DevicePath);
			OutputDebugStringA("\n");
			/*OutputDebugString("DviceGUID: ");
			OutputDebugString(devInfoData.InterfaceClassGuid.);*/
			//���豸,������豸���
			 m_hidHandle=CreateFile(devDetail->DevicePath,
				GENERIC_READ|GENERIC_WRITE,
				FILE_SHARE_READ|FILE_SHARE_WRITE,
				NULL,OPEN_EXISTING,FILE_FLAG_OVERLAPPED,NULL);
			ULONG i = GetLastError();
			//ѭ��ֱ���ҵ����Է��ʵ��豸
			if(m_hidHandle==INVALID_HANDLE_VALUE)
			{
				char buf[128];
				sprintf(buf, "%s CreateFile failed: %d \n", devDetail->DevicePath, GetLastError());
				OutputDebugStringA(buf);
				deviceNo++;
				continue;
			}
			_HIDD_ATTRIBUTES hidAttributes;
			//��ȡ����ID���豸ID
			if(!HidD_GetAttributes(m_hidHandle,&hidAttributes))
			{
				CloseHandle(m_hidHandle);
				deviceNo++;
				continue;
			}
			//����Ҫ�ҵ��豸,����ʾ�豸������
			if(m_usbVid==hidAttributes.VendorID && m_usbPid==hidAttributes.ProductID)
			{
				char version[100]="";
				sprintf(version,"�豸�ҵ���HANDLE: 0x%p, VID:%04X, PID:%04X, ver:%X\r\n���ǡ����ӣ����񡿼�������һ��",m_hidHandle, hidAttributes.VendorID, hidAttributes.ProductID, hidAttributes.VersionNumber);
				m_MaxDataLen=MAX_DATA_LEN;	
				OutputDebugStringA(version);
				OutputDebugStringA("\n");
				deviceNo++;
				if (IDYES == MessageBoxA(NULL,version,"test", MB_YESNO))
				{
					break;
				}
			}
			else//�粻��Ҫ�ҵ��豸,��رմ��豸,��ѭ�������豸
			{
				CloseHandle(m_hidHandle);
				deviceNo++;
			}
		}
	}
	
	SetupDiDestroyDeviceInfoList(hDevInfo);

	//�豸δ����,����ʾ
	return (m_hidHandle != INVALID_HANDLE_VALUE);
}


void CUsbDevice::ReadDeviceString()
{
	if(m_hidHandle == INVALID_HANDLE_VALUE)
	{
		TRACE("�豸��δ����!\n");

		return ;
	}

	//��ȡӲ��������Ϣ����
	char chManufacturerString[256]="";
	if(HidD_GetManufacturerString(m_hidHandle, chManufacturerString,sizeof(chManufacturerString)))  
	{
		for(int i=0,j=0;i<61;i++,j+=3)//��λ���ﶨ����61���ַ�
		{
			sprintf(&chManufacturerString[j],("%02X "),(UCHAR)chManufacturerString[i]);
		}
	}
	else
	{
		TRACE(_T("����HidD_GetManufacturerStringִ��ʧ��"));
	}
}


bool CUsbDevice::Disconnect()
{
	char tem[100]="";
	if(m_hRecvThread != INVALID_HANDLE_VALUE)
	{  //�رս����߳̾��
		CloseHandle(m_hRecvThread);
		m_hRecvThread = INVALID_HANDLE_VALUE;
	}
	if(m_hidHandle!=INVALID_HANDLE_VALUE)  //�ر��豸
	{
		CloseHandle(m_hidHandle);
		m_hidHandle=INVALID_HANDLE_VALUE;
		/*sprintf(tem,"�豸�ѶϿ�!");
		SetWindowText(GetDlgItem(MainHwnd,IDC_Status),tem);*/
	}
	else
	{
		/*	sprintf(tem,"�豸δ����!");
		SetWindowText(GetDlgItem(MainHwnd,IDC_Status),tem);*/
	}

	return true;
}

//TODO:˼·	�����̲߳�ͣ��أ�������Ҫ�����ݱ��յ�ʱ��������Ӧ�Ķ���������������
bool CUsbDevice::ReadData()
{
	m_hRecvThread = CreateThread(NULL,NULL,(LPTHREAD_START_ROUTINE)RecvThreadFunction,this,0,NULL);

	return 1;
}

bool CUsbDevice::SendData()
{
	m_hSendThread = CreateThread(NULL,NULL,(LPTHREAD_START_ROUTINE)SendThreadFunction,this,0,NULL);

	return 1;
}
