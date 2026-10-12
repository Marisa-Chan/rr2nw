#ifndef __VESA_T_H__
#define __VESA_T_H__

#pragma pack(1)

typedef struct{
	char             VESASignature[4];      //4 signature bytes
	unsigned short   VESAVersion;	        //VESA version number
	unsigned long    OEMStringPtr;          //Pointer to OEM string
	char             Capabilities[4];       //capabilities of the video environment
	unsigned short   *VideoModePtr;          //pointer to supported Super VGA modes
	unsigned short   TotalMemory;           //Number of 64kb blocks on board

	char             Reserved1[236];         //Remainder of
} VgaInfoBlock;

typedef struct {
    short	    ModeAttributes;	/* mode attributes */
    char	    WinAAttributes;	/* Window A attributes */
    char	    WinBAttributes;	/* Window B attributes */
    short	    WinGranularity;	/* window granularity */
    short	    WinSize;		/* window size */
    unsigned short  WinASegment;	/* Window A start segment */
    unsigned short  WinBSegment;	/* Window B start segment */
    void            (*WinFuncPtr)();	/* pointer to window function */
    short	    BytesPerScanLine;	/* bytes per scan line */
    /*
     * extended information
     * optional information
     */
    short	    XResolution;	/* horizontal resolution */
    short	    YResolution;	/* vertical resolution */
    char	    XCharSize;		/* character cell width */
    char	    YCharSize;		/* character cell height */
    char	    NumberOfPlanes;	/* number of memory planes */
    char	    BitsPerPixel;	/* bits per pixel */
    char	    NumberOfBanks;	/* number of banks */
    char	    MemoryModel;	/* memory model type */
    char	    BankSize;		/* bank size in K */
    char	    NumImagePages;	/* number of image pages */
    char	    reserved[1];
    /*
     * VESA 1.2 and later
     */
    char	    RedMaskSize;	/* number of bits in red mask */
    char	    RedMaskPos;		/* starting bit for red mask */
    char	    GreenMaskSize;
    char	    GreenMaskPos;
    char	    BlueMaskSize;
    char	    BlueMaskPos;
    char	    ReservedMaskSize;	/* reserved bits in pixel */
    char	    ReservedMaskPos;
    char	    DirectScreenMode;
} ModeInfoBlock;

#pragma pack(0)

#endif /* __VESA_T_H__ */
