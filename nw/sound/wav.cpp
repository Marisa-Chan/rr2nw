#include <string.h>
#include <stdio.h>

#include "wav.h"

int wav_Wave::OpenWave(char const *filename)
{
  FILE *file;

  unsigned long time,speed8;

  file = fopen(filename,"rb");

  if (!file)
    return 0;

  fread(&(riff),1,sizeof(Riff),file);

   if (strncmp(RID,riff.rId,4) || strncmp(WID,riff.rData.wId,4)) {
     fclose(file);
     return 0;
  }

  speed8 = (long) riff.rData.nBitsPerSample * (long) riff.rData.nChannels *
           (long) riff.rData.nFreq;

  time = (riff.rData.wData.dLen * 8) / speed8;
  buf_size = riff.rData.wData.dLen;

  /*
    if (riff.rData.nBitsPerSample == 8)
	   wav->buf = (unsigned char*) malloc(wav->buf_size);
  else
  	   wav->buf = (unsigned short*) malloc(wav->buf_size);
    */

  if (riff.rData.nBitsPerSample == 8)
       buf = new unsigned char [buf_size];
  else
       buf = new unsigned short [buf_size];

  file_off = buf_size;
  fread(buf,1,buf_size,file);
  fclose(file);
  return isValid = 1;
}
