#ifndef __FONT_I__
#define __FONT_I__

enum
{
    IFixedFontIID = 12
};

class IFixedFont{
public:
      virtual int RecreateFont() = 0;
      virtual int PrintAt(long x, long y, const char *str) = 0;
      virtual int PrintColorAt(long x, long y,const char * str, unsigned long color) = 0;
      virtual int PrintClipAt(long x, long y,const char *str) = 0;
      virtual int PrintClipColorAt(long x, long y,const char * str, unsigned long color) = 0;
      virtual int LUPrintAt(long x, long y, const char *str) = 0;
      virtual int LUPrintColorAt(long x, long y,const char * str, unsigned long color) = 0;
      virtual int LUPrintClipAt(long x, long y,const char *str) = 0;
      virtual int LUPrintClipColorAt(long x, long y,const char * str, unsigned long color) = 0;
      virtual long StringWidth(TCchar *str) const = 0;
      virtual int CharWidth(char ch) const = 0;
      virtual int Height() const = 0;
};

#endif // __FONT_I__