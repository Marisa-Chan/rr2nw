#ifndef __IMAGE_H__
#define __IMAGE_H__

#include "graph.h"
#include "kernel\h\object.h"
#include "i\image.i"

class ImageOBJ : public KR_Object, public IImage{
public:
    ImageOBJ(const char *fName) : m_image(fName) {}
    ~ImageOBJ(){}

    virtual void *queryInterface(int iNum){    
        switch( iNum ){
		    case IUnknownIID:return (KR_Object*)this;
		    case IImageIID:  return (IImage*)this;
        }
        return NULL;
    }
    void addNotify() { KR_Object::addNotify(); }
    void removeNotify() { KR_Object::removeNotify(); }
    int  receiveEvent(KR_Event &) { return(1); }

    int Height() const {return m_image.Height();}
    int Width() const  {return m_image.Width();}
    void SetWidthHeight(int w, int h) {m_image.SetWidthHeight(w, h); }
    void SetPalette(unsigned char *pal8) {m_image.SetPalette(pal8); }

    int LoadFromSPRFile(const char *sprName, const unsigned char *pal8, int x, int y)
        {return(m_image.LoadFromSPRFile(sprName, pal8, x, y)); }

    int LoadFromBMPFile(const char *fName, int x, int y)
        {return(m_image.LoadFromBMPFile(fName, x, y)); }
    void LoadPalImage(unsigned char *mem, int x, int y, int x0, int y0, int x1, int y1, int iW)
        {m_image.LoadPalImage(mem, x, y, x0, y0, x1, y1, iW); }
    void LoadRGBImage(unsigned long *mem, int x, int y, int x0, int y0, int x1, int y1, int iW)
        {m_image.LoadRGBImage(mem, x, y, x0, y0, x1, y1, iW); }
    void LoadFromImage(CGRImage &image, int x, int y, int x0, int y0, int x1, int y1)
        {m_image.LoadFromImage(image, x, y, x0, y0, x1, y1); }


    int Pset(int x, int y, int r, int g, int b) {return(m_image.Pset(x, y,   r, g, b)); }
    int Pset(int x, int y, unsigned long color) {return(m_image.Pset(x, y, color)); }
    int Pset(int x, int y, unsigned char color) {return(m_image.Pset(x, y, color)); }

    int Draw(int xs, int ys) const {return(m_image.Draw(xs, ys)); }
    int Draw(int xs, int ys, int x0, int y0, int x1, int y1) const {return(m_image.Draw(xs, ys, x0, y0, x1, y1)); }
    int DrawSprite(int xs, int ys) const {return(m_image.DrawSprite(xs, ys)); }
    int DrawSprite(int xs, int ys, int x0, int y0, int x1, int y1) const {return(m_image.DrawSprite(xs, ys, x0, y0, x1, y1)); }
    virtual bool	shouldDump () { return false; } 
private:
    CGRImage m_image;
};

#endif // __IMAGE_H__
