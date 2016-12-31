
// SoundTestToolDlg.h : ͷ�ļ�
//

#pragma once
#include "afxwin.h"
#include <mmsystem.h>
#include "SoundCapture.h"
#include "SoundPlay.h"
#include "JitterBuffer.h"


// CSoundTestToolDlg �Ի���
class CSoundTestToolDlg : public CDialogEx
{
// ����
public:
	CSoundTestToolDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_SOUNDTESTTOOL_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();

	void InitCombxBps()
	{
		m_cbxBps.AddString(_T("8000"));
		m_cbxBps.AddString(_T("11025"));
		m_cbxBps.AddString(_T("16000"));
		m_cbxBps.AddString(_T("22050"));
		m_cbxBps.AddString(_T("32000"));
		m_cbxBps.AddString(_T("44100"));
		m_cbxBps.AddString(_T("48000"));
		m_cbxBps.AddString(_T("96000"));
	}

	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	CString m_strPlayFilePath;
	CComboBox m_cbxBps;
	CComboBox m_cbsFormat;
	int m_nAvgBytesPerSec;
	int m_nBlockAlgn;
	DWORD m_dwBitsPerSample;
	CString m_strRecordFilePath;
	afx_msg void OnBnClickedBtnRecord();
	afx_msg void OnBnClickedBtnPlay();
	afx_msg LRESULT OnBnClickedRecordStatus(WPARAM wParam, LPARAM lParam);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	
private:
	bool m_bPlaying;
	WAVEFORMATEX m_waveFormatex;
	::SoundCaptureImpl* m_SoundCapture;
	::SoundPlayImpl* m_SoundPlay;
	::JitterBuffer m_JitterBuffer;
public:
	afx_msg void OnTimer(UINT_PTR nIDEvent);
};
