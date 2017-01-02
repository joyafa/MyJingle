#include "stdafx.h"
#include "SpeexEC.h"

CSpeexEC::CSpeexEC()
{
	m_bHasInit   = false;
	m_pState   = NULL;
	m_pPreprocessorState  = NULL;
	m_nFrameSize  = 320;//short类型数量 ？？ 640
	m_nFilterLen  = 320*8;
	m_nSampleRate = 16000;
	//m_pfNoise   = NULL;
}

CSpeexEC::~CSpeexEC()
{
	Reset();
}

void CSpeexEC::Init(int frame_size, int filter_length, int sampling_rate)
{
	Reset(); 

	//if (frame_size<=0 || filter_length<=0 || sampling_rate<=0)
	{
		m_nFrameSize  = 320;//short类型数量 ？？ 640
		m_nFilterLen  = 320*8;
		m_nSampleRate = 16000;
	}
// 	else
// 	{
// 		m_nFrameSize  =frame_size;
// 		m_nFilterLen  = filter_length;
// 		m_nSampleRate = sampling_rate;
// 	}

	//m_pState = speex_echo_state_init(m_nFrameSize, m_nFilterLen);
	//m_pPreprocessorState = speex_preprocess_state_init(m_nFrameSize, m_nSampleRate);
	//m_pfNoise = new spx_int32_t[m_nFrameSize+1];
	m_bHasInit = true;
}

void CSpeexEC::Reset()
{
	if (m_pState != NULL)
	{
		//speex_echo_state_destroy(m_pState);
		m_pState = NULL;
	}
	if (m_pPreprocessorState != NULL)
	{
		//speex_preprocess_state_destroy(m_pPreprocessorState);
		m_pPreprocessorState = NULL;
	}
	/*if (m_pfNoise != NULL)
	{
		delete []m_pfNoise;
		m_pfNoise = NULL;
	}*/
	m_bHasInit = false;
}


void CSpeexEC::DoAEC(short* mic, short* ref, short* out)
{
	if (!m_bHasInit)
		return;

	/*1.1.9版本中所使用的函数*/
	//speex_echo_cancel(m_pState, mic, ref, out, m_pfNoise);
	//speex_preprocess(m_pPreprocessorState, (__int16 *)out, m_pfNoise);
	/*1.2beta3-win32版本中使用的函数，从参数可以看出最新版本没有参数m_pfNoise,所以CSpeex中可以删除数据成员m_pfNoise*/
	/*本文依然保留，是为了测试两个版本的差别，从结果来看，至少人耳似乎听不出有多大差别*/
	//speex_echo_cancellation(m_pState, mic, ref, out);
	//int nRet = speex_preprocess_run(m_pPreprocessorState, (__int16 *)out);
	//TRACE("speex_preprocess_run: ret = %d \n", nRet);

}