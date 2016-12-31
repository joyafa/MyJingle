
// SoundTestToolDlg.h : 头文件
//

#pragma once
#include "afxwin.h"
#include <mmsystem.h>
#include "SoundCapture.h"
#include "SoundPlay.h"
#include "JitterBuffer.h"


// CSoundTestToolDlg 对话框
class CSoundTestToolDlg : public CDialogEx
{
// 构造
public:
	CSoundTestToolDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_SOUNDTESTTOOL_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
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
