#ifndef __IMAGE_H__
#define __IMAGE_H__

#include "common.h"
#include "bitmap_t.h"

typedef enum {
   IMG_NONE,
   IMG_RGB,
   IMG_GRAYSCALE,
   IMG_MULTIPLANE,
}TImageType;

class TAbstrImage{
public:
   TAbstrImage(const char *fName, BOOL autoDestroyPlanes = FALSE);
   virtual ~TAbstrImage();

   virtual BOOL LoadImage() = 0;
   virtual BOOL SaveImage(const BYTE *gray, int len, int h) = 0;
   virtual BOOL SaveImage(const BYTE *r, const BYTE *g, const BYTE *b,
                          int len, int h) = 0;
   inline void SetDestroyPlaneStatus(BOOL status) { m_autoDestroyPlanes = status; }

   inline void GetImgFileName(char *dest) const{ strcpy(dest, m_fName); }
   inline TImageType GetType() const         { return(m_imgType); }
   inline BOOL GetDestroyPlaneStatus() const { return(m_autoDestroyPlanes); }

   inline BYTE *GetGrayPlane() const         { return(m_gray); }
   inline BYTE *GetRedPlane() const          { return(m_r); }
   inline BYTE *GetGreenPlane() const        { return(m_g); }
   inline BYTE *GetBluePlane() const         { return(m_b); }

   inline int GetPlaneSize() const           { return(m_planeSize); }
   inline int GetPlaneLen() const            { return(m_imgLen); }
   inline int GetPlaneH() const              { return(m_imgH); }
protected:
   BYTE *m_r, *m_g, *m_b;
   BYTE *m_gray;
   int   m_planeSize;
   int   m_imgLen, m_imgH;
   BOOL  m_autoDestroyPlanes;
   char  m_fName[MAXPATH];
   TImageType m_imgType;
};

class TBMP : public TAbstrImage{
public:
   TBMP(const char *fName, BOOL autoDestroyPlanes = FALSE)
        : TAbstrImage(fName, autoDestroyPlanes){}
   BOOL LoadImage();
   BOOL SaveImage(const BYTE *gray, int len, int h);
   BOOL SaveImage(const BYTE *r, const BYTE *g, const BYTE *b,
                  int len, int h);
private:
   BITMAPFILEHEADER m_bmpFileInfoH;
   BITMAPINFOHEADER m_bmpInfoH;
};

void TestImage();

#endif //__IMAGE_H__
