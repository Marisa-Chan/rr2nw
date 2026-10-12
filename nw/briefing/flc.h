#include <string.h>

typedef struct {
  dword Size;
  word  Magic;
  word  CadrCnt;
  word  Width;
  word  Height;
  word  Bits;
  word  Flags3;
  dword Speed;
  word  Zero;
  dword CreatedData;
  dword Creator;
  dword UpDated;
  dword UpDator;
  word  AspectX;
  word  AspectY;
  byte  Reserved0[38];
  dword OffsetFirst;
  dword OffsetNext;
  byte  Reserved1[40];
} FLC_Header;

typedef struct {
  dword  Size;
  word   Magic;
  word   Chunks;
  byte   Reserved2[8];
} FrameHeader;

typedef struct {
  dword Size;
  word  Type;
} ChunkHeader;

enum {
	FLI_COLOR256 = 4,
	FLI_WORD_LC  = 7,
	FLI_COLOR    = 11,
	FLI_LC       = 12,

	FLI_BLACK    = 13,
	FLI_BRUN     = 15,

	FLI_COPY     = 16,
	FLI_PREVIEW  = 18
};