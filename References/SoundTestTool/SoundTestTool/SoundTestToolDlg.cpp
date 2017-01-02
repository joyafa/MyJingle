
// SoundTestToolDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "SoundTestTool.h"
#include "SoundTestToolDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// �Ի�������
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CSoundTestToolDlg �Ի���




CSoundTestToolDlg::CSoundTestToolDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CSoundTestToolDlg::IDD, pParent)
	, m_nBlockAlgn(2)
	, m_dwBitsPerSample(16)
	, m_bPlaying(false)
    , m_SoundCapture(NULL)
    , m_SoundPlay(NULL)
	, m_strPlayFilePath(_T("TestRecord.pcm"))
	, m_strRecordFilePath(_T("TestRecord.pcm"))
	, m_Codec_Speex()
	, m_Codec(0)
	, m_pCurrentSelCodec(&m_Codec)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CSoundTestToolDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_PLAYPATH, m_strPlayFilePath);
	DDX_Control(pDX, IDC_COMBO_BPS, m_cbxBps);
	DDX_Control(pDX, IDC_COMBO_FORMAT, m_cbxFormat);
	DDX_Text(pDX, IDC_EDIT_BLOCKALGN, m_nBlockAlgn);
	DDX_Text(pDX, IDC_EDIT_BITPERSAMPLE, m_dwBitsPerSample);
	DDX_Text(pDX, IDC_EDIT_RECORD_FILEPATH, m_strRecordFilePath);
	DDX_Control(pDX, IDC_COMBO3, m_cbxChannels);
	DDX_Control(pDX, IDC_EDIT_PLAYPATH, m_editPlayPath);
	DDX_Control(pDX, IDC_EDIT_RECORD_FILEPATH, m_editRecordPath);
	DDX_Control(pDX, IDC_COMBO_CODEC, m_cbxCodec);
}

BEGIN_MESSAGE_MAP(CSoundTestToolDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BTN_RECORD, &CSoundTestToolDlg::OnBnClickedBtnRecord)
	ON_BN_CLICKED(IDC_BTN_PLAY, &CSoundTestToolDlg::OnBnClickedBtnPlay)
	ON_WM_LBUTTONDBLCLK()
	ON_WM_TIMER()
	ON_MESSAGE(WM_MSG_SEND_RECORDSTATUS, &CSoundTestToolDlg::OnBnClickedRecordStatus)
END_MESSAGE_MAP()


static UINT TIME_EVENT = 1000;

// CSoundTestToolDlg ��Ϣ�������
HWND g_hWnd ;
BOOL CSoundTestToolDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ��������...���˵�����ӵ�ϵͳ�˵��С�

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}
	g_hWnd = m_hWnd;
	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	m_waveFormatex.wFormatTag      = WAVE_FORMAT_PCM;
	m_waveFormatex.nChannels       = 1;//������
	m_waveFormatex.nSamplesPerSec  = 8000;
	m_waveFormatex.nAvgBytesPerSec = m_waveFormatex.nSamplesPerSec * 2;
	m_waveFormatex.nBlockAlign     = m_nBlockAlgn;//(m_waveFormatex.nChannels*m_waveFormatex.wBitsPerSample)/8;
	m_waveFormatex.wBitsPerSample  = m_dwBitsPerSample;
	m_waveFormatex.cbSize          = 0;

	// TODO: �ڴ���Ӷ���ĳ�ʼ������
	SetTimer(TIME_EVENT, 1000, NULL);

	InitCombxBps();
	m_cbxBps.SetCurSel(0);
	InitVoiceFormat();
	m_cbxFormat.SetCurSel(0);
	InitChannle();
	m_cbxChannels.SetCurSel(0);
	InitCombxCodec();
	m_cbxCodec.SetCurSel(0);
	m_JitterBuffer.Init();
	m_Codec_Speex.Init();

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

void  CSoundTestToolDlg::PackFormat()
{
	UpdateData(TRUE);
	m_waveFormatex.wFormatTag      = WAVE_FORMAT_PCM;
	m_waveFormatex.nChannels       = m_cbxChannels.GetCurSel() + 1;//������

	CString str;
	GetDlgItemText(IDC_COMBO_BPS, str);
	m_waveFormatex.nSamplesPerSec  = _wtoi(str.GetBuffer());
	m_waveFormatex.nAvgBytesPerSec = m_waveFormatex.nSamplesPerSec * 2;
	m_waveFormatex.nBlockAlign     = m_nBlockAlgn;//(m_waveFormatex.nChannels*m_waveFormatex.wBitsPerSample)/8;
	m_waveFormatex.wBitsPerSample  = m_dwBitsPerSample;
	m_waveFormatex.cbSize          = 0;
}

void CSoundTestToolDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CSoundTestToolDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}


//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CSoundTestToolDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

Codec *CSoundTestToolDlg::SetCodec()
{
	int nSel = m_cbxCodec.GetCurSel();
	switch(nSel)
	{
	case 0:
		m_pCurrentSelCodec = &m_Codec;
		break;
	case 1:
		m_pCurrentSelCodec = &m_Codec_Speex;
		break;
	case 2:
		m_pCurrentSelCodec = &m_Codec_G711;
		break;
	default:
		m_pCurrentSelCodec = &m_Codec;
		break;
	}
	
	return m_pCurrentSelCodec;
}

void CSoundTestToolDlg::OnBnClickedBtnRecord()
{
	m_bPlaying = false;
	SetDlgItemText(IDC_STATIC_STATUSTIME, _T("¼��ʱ��:"));
	PackFormat();
	CString strText;
	GetDlgItemText(IDC_BTN_RECORD, strText);
	if (strText == _T("��ʼ"))
	{
		SetDlgItemText(IDC_BTN_RECORD, _T("ֹͣ"));
		if (m_SoundCapture)
		{
			m_SoundCapture->Stop();
			m_SoundCapture->Exit();
			delete m_SoundCapture;
			m_SoundCapture = NULL;
		}
		m_SoundCapture = new ::SoundCaptureImpl(m_waveFormatex, m_hWnd, m_strRecordFilePath.GetBuffer());
		m_SoundCapture->Init();
		m_SoundCapture->Encoder((Codec*)m_pCurrentSelCodec);
		m_SoundCapture->Start();
	}
	else
	{
		SetDlgItemText(IDC_BTN_RECORD, _T("��ʼ"));
		if (m_SoundCapture) 
		{
			m_SoundCapture->Stop();
			m_SoundCapture->Exit();
			delete m_SoundCapture;
			m_SoundCapture = NULL;
		}
	}

	SetDlgItemText(IDC_STATIC_STATUSTIME, _T("¼��״̬:"));
}

void CSoundTestToolDlg::ReadPcmData()
{
	SoundDeserializer deSerizer(m_strPlayFilePath.GetBuffer());
	deSerizer.Init();
	deSerizer.SetJitterBuffer(&m_JitterBuffer);
	deSerizer.SetJitterBufferData();
}


void CSoundTestToolDlg::OnBnClickedBtnPlay()
{
	m_bPlaying = true;
	UpdateData(TRUE);
	//�������ò���
	PackFormat();
	SetDlgItemText(IDC_STATIC_STATUSTIME, _T("����״̬:"));
	CString strText;
	GetDlgItemText(IDC_BTN_PLAY, strText);
	if (strText == _T("����"))
	{
		SetDlgItemText(IDC_BTN_PLAY, _T("ֹͣ"));
		if (m_SoundPlay)
		{
			m_SoundPlay->Stop();
			m_SoundPlay->Exit();
			delete m_SoundPlay;
			m_SoundPlay = NULL;				
		}
		m_SoundPlay = new ::SoundPlayImpl(m_waveFormatex, m_hWnd, m_strPlayFilePath.GetBuffer());
		m_SoundPlay->JitterBuf(&m_JitterBuffer);
		m_SoundPlay->Decoder((Codec*)m_pCurrentSelCodec);	
		ReadPcmData();
		m_SoundPlay->Init();
		m_SoundPlay->Start();
	}
	else
	{
		SetDlgItemText(IDC_BTN_PLAY, _T("����"));
		m_SoundPlay->Stop();
		m_SoundPlay->Exit();
		delete m_SoundPlay;
		m_SoundPlay = NULL;
	}
}


LRESULT CSoundTestToolDlg::OnBnClickedRecordStatus( WPARAM wParam, LPARAM lParam )
{
	CString str;
	str.Format(_T("��ǰ����:%d"), *(int *)lParam);
	SetDlgItemText(IDC_STATIC_SHOWTIMESPACE, str);

	return 1;
}

void CSoundTestToolDlg::OnLButtonDblClk(UINT nFlags, CPoint point)
{
    //˫�������ļ�·��
	CDialogEx::OnLButtonDblClk(nFlags, point);
}


void CSoundTestToolDlg::OnTimer(UINT_PTR nIDEvent)
{
	SYSTEMTIME st;
	GetLocalTime(&st);
	CString strCurrentTime;
	strCurrentTime.Format(_T("%04d-%02d-%02d %02d:%02d:%02d.%03d"),st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);
	SetDlgItemText(IDC_STATIC_CURTIME, strCurrentTime);

	CDialogEx::OnTimer(nIDEvent);
}
