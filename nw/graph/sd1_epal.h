/**************************
 * File: sd1_epal.h
 * Autor: Suavik
 * Ver 1.0 (auto)
 * Ver 1.1 add type epal_RGBType
 */

#ifndef __SD1_EPAL_H__
#define __SD1_EPAL_H__

#define USE_TAGFILEPAL 0


#ifndef __STYPES_H__
#include "stypes.h"
#endif

#if USE_TAGFILEPAL
#include "filesys.h"
#endif

//#ifndef __BITMAP_H__
//#include ".h"
//#endif

const int BLOCK_SIZE_OF_TExtendedPalette =16;

typedef byte epal_ColorType;

enum epal_PLANETYPE {
  PLANE_RED,
  PLANE_GREEN,
  PLANE_BLUE,
  THE_COLOR
};


#define epal_ColorType_(x) ((epal_ColorType)(x))

class RGB_b;
class RGB_i;
class RGB_f;

class RGB_i {
 public:
    int r,g,b;
    RGB_i() {}
    RGB_i(int lr,int lg,int lb);
    RGB_i(const RGB_i &e);
    RGB_i(const RGB_b &e);
    RGB_i(const RGB_f &e);
    RGB_i  operator + (const RGB_i &e) const;
    RGB_i &operator +=(const RGB_i &e);
    RGB_i  operator - (const RGB_i &e) const;
    RGB_i &operator -=(const RGB_i &e);
    RGB_i  operator * (int d) const;
    RGB_i &operator *=(int d);
    RGB_i  operator / (int d) const;
    RGB_i &operator /=(int d);
    friend RGB_i  Mul(const RGB_i &c0,const RGB_i &c1);
};



class RGB_f {
 public:
    double r,g,b;
    RGB_f();
    RGB_f(double lr,double lg,double lb);
    RGB_f(const RGB_f &e);
    RGB_f(const RGB_b &e);
    RGB_f(const RGB_i &e);
    RGB_f  operator + (const RGB_f &e) const;
    RGB_f &operator +=(const RGB_f &e);
    RGB_f  operator - (const RGB_f &e) const;
    RGB_f &operator -=(const RGB_f &e);
    RGB_f  operator * (double d) const;
    RGB_f &operator *=(double d);
    RGB_f  operator / (double d) const;
    RGB_f &operator /=(double d);
    friend RGB_f  Mul(const RGB_f &c0,const RGB_f &c1);
};


class RGB_b{
 public:
    byte r,g,b,m_align00;
    RGB_b() {}
    RGB_b(const RGB_b &e);
    RGB_b(const RGB_i &e);
    RGB_b(const RGB_f &e);
    RGB_b(int lr,int lg,int lb);
    RGB_b  operator + (const RGB_b &e) const;
    RGB_b &operator +=(const RGB_b &e);
    RGB_b  operator - (const RGB_b &e) const;
    RGB_b &operator -=(const RGB_b &e);
    RGB_b  operator * (int d) const;
    RGB_b &operator *=(int d);
    RGB_b  operator / (int d) const;
    RGB_b &operator /=(int d);
    friend RGB_b  Mul(const RGB_b &c0,const RGB_b &c1);

    bool   operator ==(const RGB_b &e);
    bool   operator !=(const RGB_b &e);
    friend RGB_b Add(const RGB_b &c0,const RGB_b &c1);
};



class TBSPPal {
 public:
    epal_PLANETYPE   m_plane;
    int              m_planePos;
    int              r,g,b;
    int              m_color;
    TBSPPal         *m_lo, *m_hi;

     TBSPPal();
    ~TBSPPal();
};


class TSQRTable {
 public:
  enum { SQRBASE = 255 };
  static long     m_sqr[512];

  static long SQR(int r,int g,int b);
  void Init();
  TSQRTable();
};

class TExtendedPalette{
 private:
    int
           m_blockSize,
           m_blockCnt,
           m_cnt;
    RGB_b *m_list;

    RGB_b *Alloc(bool &mayAdd);
    bool    BndControl( int num );

 protected:

    TSQRTable  m_sqr;
    TBSPPal   *m_bsp;
    friend int epal_Match(TExtendedPalette &pal,RGB_b col);
    friend int epal_MatchDither(TExtendedPalette &pal,RGB_b col);
    friend int epal_Match(TExtendedPalette &pal,RGB_i col);
    friend int epal_MatchDither(TExtendedPalette &pal,RGB_i col);
 public:

    bool Addition( RGB_b  id );
    int  Addition( int cCnt  ); // Выделение гpуппы элементов
                                // возвpащает индекс пеpвого
    bool Delete( int num );
    int  Cnt() const                   { return m_cnt; }
    void Init();
    TExtendedPalette();
    TExtendedPalette(TExtendedPalette &);
    ~TExtendedPalette();

    static RGB_b * Xerox( TExtendedPalette &list );
    RGB_b &Get( int pos );
    void   Clear();
    int    Search( RGB_b  id );

    RGB_b *GetArray();
    void   CreateBSP();
#if USE_TAGFILEPAL
    void   Write(CTaggedFile &f);
    void   Read(CTaggedFile &f);
#endif
};

inline byte ColToByte(int x)
 {
        if(x<0) return 0;
        if(x>255) return 255;
        return (byte)x;
 }

inline byte ColToByte(double x)
 {
        if(x<0) return 0;
        if(x>255) return 255;
        return (byte)x;
 }


 /***********************

   RGB_i implementation

  ***********************/



inline RGB_i::RGB_i(int lr,int lg,int lb)
 {
    r = lr;
    g = lg;
    b = lb;
 }

inline RGB_i::RGB_i(const RGB_i &e)
 {
    *this = e;
 }


inline RGB_i::RGB_i(const RGB_b &e)
 {
    r = e.r;
    g = e.g;
    b = e.b;
 }

inline RGB_i::RGB_i(const RGB_f &e)
 {
    r = (int)e.r;
    g = (int)e.g;
    b = (int)e.b;
 }


inline RGB_i RGB_i::operator +(const RGB_i &e) const
 {
    return  RGB_i(r+e.r,g+e.g,b+e.b);
 }

inline RGB_i& RGB_i::operator +=(const RGB_i &e)
 {
    r += e.r;
    g += e.g;
    b += e.b;
    return  *this;
 }

inline RGB_i RGB_i::operator - (const RGB_i &e) const
 {
    return  RGB_i(r-e.r, g-e.g, b-e.b);
 }

inline RGB_i& RGB_i::operator -=(const RGB_i &e)
 {
    r -= e.r;
    g -= e.g;
    b -= e.b;
    return  *this;
 }

inline RGB_i  RGB_i::operator *(int d) const
 {
    return RGB_i(r*d,g*d,b*d);
 }

inline RGB_i &RGB_i::operator *=(int d)
 {
    r *= d;
    g *= d;
    b *= d;
    return *this;
 }

inline RGB_i  RGB_i::operator / (int d) const
 {
    return RGB_i(r/d, g/d, b/d);
 }

inline RGB_i &RGB_i::operator /=(int d)
 {
    r /= d;
    g /= d;
    b /= d;
    return *this;
 }

inline RGB_i Mul(const RGB_i &c0,const RGB_i &c1)
 {
     return RGB_i(c0.r*c1.r, c0.g*c1.g, c0.b*c1.b);
 }



inline TBSPPal::TBSPPal()
 {
    m_lo = 0;
    m_hi = 0;
 }

inline TBSPPal::~TBSPPal()
 {
    if( m_plane!=THE_COLOR )
    {
         delete m_lo;
         delete m_hi;
    }
 }

 /***********************

   RGB_f implementation

  ***********************/

inline RGB_f::RGB_f()
 {
    r = 0.0;
    g = 0.0;
    b = 0.0;
 }

inline RGB_f::RGB_f(double lr,double lg,double lb)
 {
    r = lr;
    g = lg;
    b = lb;
 }

inline RGB_f::RGB_f(const RGB_f &e)
 {
    *this = e;
 }

inline RGB_f::RGB_f(const RGB_i &e)
 {
    r = e.r;
    g = e.g;
    b = e.b;
 }

inline RGB_f::RGB_f(const RGB_b &e)
 {
    r = e.r;
    g = e.g;
    b = e.b;
 }

inline RGB_f RGB_f::operator +(const RGB_f &e) const
 {
    return  RGB_f(r+e.r,g+e.g,b+e.b);
 }

inline RGB_f& RGB_f::operator +=(const RGB_f &e)
 {
    r += e.r;
    g += e.g;
    b += e.b;
    return  *this;
 }

inline RGB_f RGB_f::operator - (const RGB_f &e) const
 {
    return  RGB_f(r-e.r, g-e.g, b-e.b);
 }

inline RGB_f& RGB_f::operator -=(const RGB_f &e)
 {
    r -= e.r;
    g -= e.g;
    b -= e.b;
    return  *this;
 }

inline RGB_f  RGB_f::operator *(double d) const
 {
    return RGB_f(r*d,g*d,b*d);
 }

inline RGB_f &RGB_f::operator *=(double d)
 {
    r *= d;
    g *= d;
    b *= d;
    return *this;
 }

inline RGB_f  RGB_f::operator / (double d) const
 {
    return RGB_f(r/d, g/d, b/d);
 }

inline RGB_f &RGB_f::operator /=(double d)
 {
    r /= d;
    g /= d;
    b /= d;
    return *this;
 }

inline RGB_f Mul(const RGB_f &c0,const RGB_f &c1)
 {
     return RGB_f(c0.r*c1.r, c0.g*c1.g, c0.b*c1.b);
 }

 /***********************

   RGB_b implementation

  ***********************/
inline RGB_b::RGB_b(const RGB_b &p)
 {
     *this=p;
 }


inline long TSQRTable::SQR(int r,int g,int b)
 {
     return  m_sqr[SQRBASE+r]
            +m_sqr[SQRBASE+g]
            +m_sqr[SQRBASE+b];
 }


void epal_Load6BitPal(TExtendedPalette &pal,byte *pal6bit,int cnt);
void epal_Load8BitPal(TExtendedPalette &pal,byte *pal8bit,int cnt);
bool epal_Add6BitPal (TExtendedPalette &pal,byte *pal6bit,int cnt,int *xlat);
bool epal_Add8BitPal (TExtendedPalette &pal,RGBQUAD *pal8bit,int cnt,int *xlat);
bool epal_Add8BitPal (TExtendedPalette &pal,byte *pal8bit,int cnt,int *xlat);

int epal_Match(TExtendedPalette &pal,RGB_b col);
int epal_Match(TExtendedPalette &pal,RGB_i col);
int epal_MatchDither(TExtendedPalette &pal,RGB_b col);
int epal_MatchDither(TExtendedPalette &pal,RGB_i col);

#endif
/* End of file sd1_epal.h */
