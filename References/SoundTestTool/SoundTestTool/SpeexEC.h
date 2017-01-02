#ifndef SPEEX_EC_H
#define SPEEX_EC_H

#include <stdio.h>
#include <stdlib.h>
/*在项目属性里设置VC++目录的包含目录和库目录分别为speex库中的include和lib，我用的是speex-1.2beta3-win32*/
#include "speex/speex_echo.h"
#include "speex/speex_preprocess.h"

class CSpeexEC
{
public:
	CSpeexEC();
	~CSpeexEC();
	void Init(int frame_size=320, int filter_length=5000, int sampling_rate=8000); 
	void DoAEC(short *mic, short *ref, short *out);

protected:
	void Reset();

private:
	bool      m_bHasInit;
	SpeexEchoState*   m_pState;
	SpeexPreprocessState* m_pPreprocessorState;
	int      m_nFrameSize;
	int      m_nFilterLen;
	int      m_nSampleRate;
	//spx_int32_t*      m_pfNoise;
};

#endif