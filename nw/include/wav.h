#ifndef _WAV_H_
#define _WAV_H_

#define MIN(a,b) ((a)<=(b)?(a):(b))

#define DID "data"

typedef struct WaveDataHeadTag {
	char          dId[4];
	unsigned long dLen;
}WaveDataHead;

#define WID "WAVE"
#define FID "fmt "

typedef struct WaveDataTag {
	char          wId[4];
	char          fId[4];
	unsigned long fLen;
	unsigned short  wFormat;
	unsigned short  nChannels;
	unsigned short  nFreq;
	unsigned short  nAvgBytesPerSec;
	unsigned short  nBlockAlign;
	unsigned short  formatSpecific;
	unsigned short  tFormat;
	unsigned short  nBitsPerSample;
	WaveDataHead  wData;
}WaveData;

#define RID "RIFF"

typedef struct RiffTag {
	char          rId[4];
	unsigned long rLen;
	WaveData      rData;
}Riff;

class wav_Wave {
public:

    wav_Wave()
        { isValid = 0; buf =0; }
    wav_Wave(char const *filename)
        { OpenWave(filename);  }
    ~wav_Wave()
        {  delete [] buf; }

    int           OpenWave(char const *filename);
    int           isValid;

	Riff          riff;
	void          *buf;
	long          buf_size;
	long          file_off;
};


#endif

