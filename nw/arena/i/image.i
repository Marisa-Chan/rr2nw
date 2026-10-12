#ifndef __IMAGE_I__
#define __IMAGE_I__

enum
{
    IImageIID = 13
};

class IImage{
public:
        virtual int Height() const = 0;
        virtual int Width() const = 0;

        virtual void SetWidthHeight(int w, int h) = 0;

        virtual void SetPalette(unsigned char *pal8) = 0;

        virtual int LoadFromSPRFile(const char *sprName, const unsigned char *pal8, 
		                            int x, int y) = 0;

        virtual void LoadPalImage(unsigned char *mem,
                          int x, int y,
                          int x0, int y0, int x1, int y1, int iW) = 0;
        virtual void LoadRGBImage(unsigned long *mem,
                          int x, int y,
                          int x0, int y0, int x1, int y1, int iW) = 0;
        virtual void LoadFromImage(CGRImage &image,
                           int x, int y,
                           int x0, int y0, int x1, int y1) = 0;

        virtual int Pset(int x, int y, int r, int g, int b) = 0;
        virtual int Pset(int x, int y, unsigned long color) = 0; //GRFillColor
        virtual int Pset(int x, int y, unsigned char color) = 0;

        virtual int Draw(int xs, int ys) const = 0;
        virtual int Draw(int xs, int ys, int x0, int y0, int x1, int y1) const = 0;
        virtual int DrawSprite(int xs, int ys) const = 0;
        virtual int DrawSprite(int xs, int ys, int x0, int y0, int x1, int y1) const = 0;
};

#endif // __IMAGE_I__