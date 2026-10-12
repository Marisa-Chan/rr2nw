#ifndef __FONT_H__
#define __FONT_H__

#include "graph.h"
#include "kernel\h\object.h"
#include "i\font.i"

class FixedFontOBJ : public KR_Object, public IFixedFont{
public:
    FixedFontOBJ(const char *fName) : m_font(fName) {}
    ~FixedFontOBJ(){}

    virtual void *queryInterface(int iNum){    
        switch( iNum ){
		    case IUnknownIID:   return (KR_Object*)this;
		    case IFixedFontIID: return (IFixedFont*)this;
        }
        return NULL;
    }
    void addNotify() { KR_Object::addNotify(); }
    void removeNotify() { KR_Object::removeNotify(); }
    int  receiveEvent(KR_Event &) { return(1); }

    int RecreateFont() { return(m_font.RecreateFont()); }
    int PrintAt(long x, long y, const char *str) {return(m_font.PrintAt(x, y, str)); }
    int PrintColorAt(long x, long y,const char * str, unsigned long color) {return(m_font.PrintColorAt(x, y, str, color)); }
    int PrintClipAt(long x, long y,const char *str) {return(m_font.PrintClipAt(x, y, str)); }
    int PrintClipColorAt(long x, long y,const char * str, unsigned long color) {return(m_font.PrintClipColorAt(x, y, str, color)); }
    int LUPrintAt(long x, long y, const char *str) {return(m_font.LUPrintAt(x, y, str)); }
    int LUPrintColorAt(long x, long y,const char * str, unsigned long color) {return(m_font.LUPrintColorAt(x, y, str, color)); }
    int LUPrintClipAt(long x, long y,const char *str) {return(m_font.LUPrintClipAt(x, y, str)); }
    int LUPrintClipColorAt(long x, long y,const char * str, unsigned long color) {return(m_font.LUPrintClipColorAt(x, y, str, color)); }

    long StringWidth(TCchar *str) const {return(m_font.StringWidth(str)); }
    int CharWidth(char ch) const { return(m_font.CharWidth(ch)); }
    int Height() const {return (m_font.Height()); }
    virtual bool	shouldDump () { return false; } 
private:
    CFixedColorFont m_font;
};

#endif // __FONT_H__
