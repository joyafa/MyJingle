
// SoundTestToolDlg.h : 头文件
//

#pragma once
#include "afxwin.h"
#include <mmsystem.h>
#include "SoundCapture.h"
#include "SoundPlay.h"
#include "JitterBuffer.h"
#include "ActionEdit.h"
#include "Codec.h"


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

	void InitCombxCodec()
	{
		m_cbxCodec.AddString(_T("CODEC"));
		m_cbxCodec.AddString(_T("SPEEX"));
		m_cbxCodec.AddString(_T("G711"));

	}
	void InitVoiceFormat()
	{
		m_cbxFormat.AddString(_T("WAVE_FORMAT_PCM"));
	}

	void InitChannle()
	{
		m_cbxChannels.AddString(_T("单声道"));
		m_cbxChannels.AddString(_T("立体声"));
	}


	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
	Codec *SetCodec();
public:
	CString m_strPlayFilePath;
	CComboBox m_cbxBps;
	CComboBox m_cbxFormat;
	int m_nBlockAlgn;
	DWORD m_dwBitsPerSample;
	CString m_strRecordFilePath;
	afx_msg void OnBnClickedBtnRecord();
	void ReadPcmData();
	afx_msg void OnBnClickedBtnPlay();
	afx_msg LRESULT OnBnClickedRecordStatus(WPARAM wParam, LPARAM lParam);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	void PackFormat();
private:
	bool m_bPlaying;
	WAVEFORMATEX m_waveFormatex;
	::SoundCaptureImpl* m_SoundCapture;
	::SoundPlayImpl* m_SoundPlay;
	::JitterBuffer m_JitterBuffer;
	::SpeexCodec m_Codec_Speex;
	::Codec m_Codec;
	::G711Codec m_Codec_G711;
	//当前选中生效的codec编解码器
	::Codec *m_pCurrentSelCodec;

public:
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	CComboBox m_cbxChannels;
	CActionEdit m_editPlayPath;
	CActionEdit m_editRecordPath;
	CComboBox m_cbxCodec;
};
