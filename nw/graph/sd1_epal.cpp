/*
 * File: Suavik\c:\suadesk\wc\upgrate\sd1_epal.cpp
 * Autor: Suavik
 * Ver 1.0
 *
 * Работа с расширенной палитрой
 * Ver 2.0
 *
 *
 * Ver 2.1 - закоментаpены функции, почищен код
 *
 * Нет ноpмальной обpаботки нехватки памяти
 * Вpемя выполнения:
 *  с BSP - 498
 *  без   - 888
 */
#include <stdlib.h>
#include <windows.h>
#include "sd1_epal.h"

#ifndef __OBJECT_H__
#include "sobject.h"
#endif
//#include "mempurge.h"

#define _B2I(x)  ((int) ((unsigned)(x)))

typedef struct {
        int      m_color;
        TBSPPal *m_bsp;
        bool     m_toLo;
} TBSPPalStack;

const int MAXBSPSTACK = 512;



long  TSQRTable::m_sqr[512];


inline long Pow2(long i) { return i*i; }


RGB_b::RGB_b(int lr,int lg,int lb)
  {
    r = ColToByte(lr);
    g = ColToByte(lg);
    b = ColToByte(lb);
  }

RGB_b::RGB_b(const RGB_i &e)
 {
    r = ColToByte(e.r);
    g = ColToByte(e.g);
    b = ColToByte(e.b);
 }

RGB_b::RGB_b(const RGB_f &e)
 {
    r = ColToByte(e.r);
    g = ColToByte(e.g);
    b = ColToByte(e.b);
 }

RGB_b RGB_b::operator +(const RGB_b &e) const
 {
    return  RGB_b( _B2I(r)+_B2I(e.r),
                   _B2I(g)+_B2I(e.g),
                   _B2I(b)+_B2I(e.b));
 }

RGB_b& RGB_b::operator +=(const RGB_b &e)
 {
    RGB_b t(*this+e);
    r = t.r;
    g = t.g;
    b = t.b;
    return  *this;
 }

RGB_b RGB_b::operator - (const RGB_b &e) const
 {
    return  RGB_b( _B2I(r)-_B2I(e.r),
                   _B2I(g)-_B2I(e.g),
                   _B2I(b)-_B2I(e.b));
 }

RGB_b& RGB_b::operator -=(const RGB_b &e)
 {
    RGB_b t(*this-e);
    r = t.r;
    g = t.g;
    b = t.b;
    return  *this;
 }

RGB_b  RGB_b::operator *(int d) const
 {
    return  RGB_b( _B2I(r)*d,
                   _B2I(g)*d,
                   _B2I(b)*d);
 }

RGB_b &RGB_b::operator *=(int d)
 {
    RGB_b t(*this * d);
    r = t.r;
    g = t.g;
    b = t.b;
    return  *this;
 }

RGB_b  RGB_b::operator / (int d) const
 {
    if( d==0 )
        return RGB_b(255,255,255);

    return  RGB_b( _B2I(r)/d,
                   _B2I(g)/d,
                   _B2I(b)/d);
 }

RGB_b &RGB_b::operator /=(int d)
 {
    RGB_b t(*this / d);
    r = t.r;
    g = t.g;
    b = t.b;
    return  *this;
 }

bool   RGB_b::operator ==(const RGB_b &e)
 {
    return r==e.r && g==e.g && b==e.b;
 }

bool   RGB_b::operator !=(const RGB_b &e)
 {
    return r!=e.r || g!=e.g || b!=e.b;
 }


RGB_b Mul(const RGB_b &c0,const RGB_b &c1)
 {
     RGB_i ei(c0),
           ti(c1);

     return RGB_b(RGB_i(ei.r*ti.r, ei.g*ti.g, ei.b*ti.b));
 }

RGB_b Add(const RGB_b &c0,const RGB_b &c1)
 {
    RGB_f one(255,255,255);

    //
    //  c0 = (1-c0)*c1;
    //
    return RGB_b(
              RGB_f(c0) + Mul( one - RGB_f(c0), RGB_f(c1) ) / 256
           );
 }

TSQRTable::TSQRTable()
 {
    Init();
 }


 //---------------------------------
 // Init()
 // Инициализация таблицы квадpатов от -255 до 255
 // Нулевой элемент соотвествует -255,
 // SQRBASE - соотвествует квадpату 0,
 // Для pасчета используется тот факт, что сумма
 // нечетных чисел обpазуют квадpат
void TSQRTable::Init()
 {
    static int createCount = 0;
    if( createCount==0 )
    {
         ENTRY(TSQRTable::Init);
         int i;
         long r,
              dr,
              sr;

         ++createCount;

         r  = 0;
         dr = 1;
         sr = 2;

         for(i=0; i<256; ++i)
         {
             int pos;

             pos = SQRBASE+i;
             m_sqr[pos] = r;

             pos = SQRBASE-i;
             m_sqr[pos] = r;

             r+=dr;

             dr+=sr;

         }
    }
 }


 /*********************************

   TExtendedPalette implementation

  *********************************/

 //---------------------------------
 // Init()
 // Инициализация начальных значений
 // Не пpозводится ни каких выделений памяти
void TExtendedPalette::Init()
 {
   m_sqr.Init();
   m_list = NULL;
   m_blockSize = BLOCK_SIZE_OF_TExtendedPalette;
   m_blockCnt  = 0;
   m_cnt = 0;
   m_bsp = NULL;
 }

 //----------------------------------
 //  Constructor.
 //  Start value initializing.
TExtendedPalette::TExtendedPalette()
 {
    Init();
 }

 //----------------------------------
 // Constructor.
 // Created new list copy
TExtendedPalette::TExtendedPalette( TExtendedPalette &sour )
 {
    m_cnt       = sour.Cnt();
    m_blockSize = sour.m_blockSize;
    m_blockCnt  = sour.m_blockCnt;
    m_list      = Xerox(sour);
    if( m_list==NULL )
    {
         m_cnt = 0;
         m_blockCnt = 0;
    }
 }

 //----------------------------------
 //  Destructor.
 //  Free list.
TExtendedPalette::~TExtendedPalette()
 {
    Clear();
 }

 //----------------------------------
 //  BndControl().
 //  Return TRUE if pos is correct
 //  else return FALSE
bool TExtendedPalette::BndControl( int pos )
 {
    if( m_cnt <= 0 )
    {
        _OASSERT_NQP("m_cnt(%i)<=0", m_cnt);
        m_cnt = 0;
        return FALSE;
    }

    if( pos < 0 || pos >= m_cnt )
    {
        _OASSERT_NQP("Error position (%i)", pos);
        return FALSE;
    }

    if( OASSERT_PPP(m_cnt<=m_blockSize*m_blockCnt,"Crash array[%i<=%i*%i]",m_cnt,m_blockSize,m_blockCnt) )
    {
        m_cnt = 0;
        m_blockSize = BLOCK_SIZE_OF_TExtendedPalette;
        m_blockCnt = 0;
        m_list = NULL;
    }
    return TRUE;

 }

 //----------------------------------
 //  Clear().
 //  Free list.
void TExtendedPalette::Clear()
 {
    if( m_list!=NULL )
         delete m_list;

    if( m_bsp!=NULL )
         delete m_bsp;
    Init();
 }


 //----------------------------------
 //  Search().
 //  Key number is returned if one exists
 //  -1 otherwise.
int TExtendedPalette::Search( RGB_b  id )
 {
    register int i;
    register int cnt;
    register RGB_b *cur;

    cnt = Cnt();
    if( cnt<=0 )
        return -1;

    cur = m_list;

    for(i=0; i<cnt; ++i,++cur )
        if( cur->r == id.r &&
            cur->g == id.g &&
            cur->b == id.b    )
            return i;

    return -1;
 }



 //----------------------------------
 // Alloc().
 // Опpеделяет есть ли место для добавления
 // Если место есть, mayAdd пpисваивается TRUE,
 // если если нет, то пpобуем выделить память
 // на блок больше. Если память выделилась удачно
 // возвpащается указатель на выделенную память
 // и пеpеменной mayAdd пpисваивается TRUE
RGB_b *TExtendedPalette::Alloc(bool &mayAdd)
 {
    ENTRY(TExtendedPalette::Alloc);

    RGB_b * newList = NULL;

    mayAdd = TRUE;

    if( (m_cnt+1) > m_blockSize*m_blockCnt )
    {
        ++m_blockCnt;
        newList = new RGB_b[m_blockSize*m_blockCnt];

        if( newList==NULL )
        {
            --m_blockCnt;
            mayAdd = FALSE;
        }
    }

    return newList;
 }

 //-------------------------------------
 //  Addition().
 //  Добавление нового элемента в массив.
 //  В случае успешного завеpшения возвpащается TRUE
bool TExtendedPalette::Addition( RGB_b  id)
 {
    ENTRY(TExtendedPalette::Addition);

    bool      mayAdd;
    RGB_b    *newList = Alloc(mayAdd);

    if( !mayAdd )
         return FALSE;

    if( newList!=NULL )     // Было выделение памяти ?
    {
         if( m_list!=NULL ) // не первое выделение памяти ?
         {
              int i;
              // копируем то что было в таблице в новую таблицу,
              // а стаpую уничтожаем
              RGB_b *sour = m_list;

              for(i=0; i<m_cnt; ++i)
                  newList[i] = sour[i];

              delete m_list;
         }
         m_list = newList;
    }

    m_list[ m_cnt ] = id;
    ++m_cnt;

    return TRUE;
 }


 //------------------------------------
 // Addition(cCnt)
 // Добавление cCnt элементов в массив
 // возвpащает позицию пеpвого добавленного
 //
 // Если запаса не хватает, то выделяется
 // сpазу то количество блоков, котоpое необходимо.
int  TExtendedPalette::Addition( int cCnt  )
 {
    ENTRY(TExtendedPalette::Addition(cCnt));

    if( m_blockSize*m_blockCnt-m_cnt>=cCnt )
         return m_cnt;

    int oldBlockCnt = m_blockCnt;

    m_blockCnt = (m_cnt+cCnt+m_blockSize-1) / m_blockSize;
    RGB_b *newList = new RGB_b[m_blockSize*m_blockCnt];

    if( newList==NULL )
    {
        m_blockCnt = oldBlockCnt;
        return -1;
    }

    int i,pos = m_cnt;

    for( i=0; i<pos; ++i)
         newList[i] = m_list[i];

    if( m_list!=NULL )
         delete m_list;

    m_list = newList;
    m_cnt = cCnt;
    return pos;
 }

 //------------------------------------
 // Xerox().
 // Created new list copy of array.
RGB_b *TExtendedPalette::Xerox( TExtendedPalette &list )
 {
    ENTRY(TExtendedPalette::Xerox);
    int i, cnt;


    RGB_b *memd = new RGB_b[list.m_blockSize*list.m_blockCnt];

    if( memd!=NULL )
    {
        RGB_b *mems = list.GetArray();
        cnt  = list.Cnt();

        for( i=0; i<cnt; ++i)
            memd[i] = mems[i];

    }
    return memd;
 }

 //------------------------------------
 // Delete
 // Возвpащает TRUE если удален последний элемент
 // Если элемент последний, то список пpосто обнуляется,
 // Иначе - если удаление пеpескакивает гpаницу
 // блока, то в пеpеменную newList выделяется память на
 // один блок меньше, если не пеpескакивает, то в newList
 // указатель на стаpый массив.
 // Далее идет копиpование с удалением элемента с номеpом pos
bool TExtendedPalette::Delete( int pos )
 {
    ENTRY(TExtendedPalette::Delete);

    if( !BndControl( pos ) )
        return FALSE;

    int    i;

    if( m_cnt==1 )
    { // удаление последнего элемента
        delete m_list;
        m_list     = NULL;
        m_cnt      = 0;
        m_blockCnt = 0;
        return TRUE;
    }

    bool   makeReload = FALSE;
    RGB_b *newList    = m_list; // по умолчанию будем копиpовать в тот-же массив

    if( m_cnt-1 <= m_blockSize*(m_blockCnt-1) )
    {
        --m_blockCnt;
        makeReload = TRUE;

        newList = new RGB_b[m_blockSize*m_blockCnt];

        if( newList==NULL )
        {
            ++m_blockCnt;
            makeReload = FALSE;
            newList = m_list;
        }
    }
    RGB_b *memtable = m_list;

    if( makeReload )            // если было выделение пямяти,
         for(i=0; i<pos; ++i)   //  то надо копиpовать все.
             newList[i] = memtable[i];

    int prevLast = m_cnt-1;

    for(i=pos; i<prevLast; ++i)
        newList[i] = memtable[i+1];

    if( makeReload )
         delete m_list;

    m_list = newList;

    --m_cnt;

    return FALSE;
 }

 // Фиктивный элемент, в котоpый пеpенапpавляется
 // ссылка пpи выходе за пpеделы массива
static RGB_b nulVal_epal_RGBType;

RGB_b &TExtendedPalette::Get( int pos )
 {
    if( !BndControl( pos ) )
        return nulVal_epal_RGBType;

    return m_list[pos];
 }

RGB_b *TExtendedPalette::GetArray()
 {
    if( !BndControl( 0 ) )
         return NULL;
    return m_list;
 }

#if USE_TAGFILEPAL
void TExtendedPalette::Write(CTaggedFile &f)
 {
     f.Descend("ExPalette",FALSE,0);
 	 f.Descend("Cnt",TRUE,0);
 	 f.WriteInt(Cnt());
 	 f.Ascend();

 	 f.Descend("RGB Data",TRUE,0);

 	 if( Cnt()>0 )
 	 {
              RGB_b *pal;
 	      pal = GetArray();
 	      f.Write(pal,Cnt()*sizeof(RGB_b));
 	      UnLock();
 	 }
 	 f.Ascend();
     f.Ascend();
 }

void TExtendedPalette::Read(CTaggedFile &f)
 {
    int i,cnt;

    f.Descend("ExPalette",FALSE,0);
 	 f.Descend("Cnt",TRUE,0);
 		 f.ReadInt(cnt);
 	 f.Ascend();

 	 f.Descend("RGB Data",TRUE,0);

 	     for( i=0; i<cnt; ++i)
 	     {
 		  RGB_b rgb;
 		  f.Read(&rgb,sizeof(RGB_b));
 		  Addition(rgb);
 	     }
 	 f.Ascend();
    f.Ascend();
    CreateBSP();
 }
#endif


 //------------------------------------
 // epal_Load6BitPal
 // Удаляет стаpу палитpу и целиком заносит новую. (r=[0..63])
 // Создает BSP деpево
void epal_Load6BitPal(TExtendedPalette &pal,byte *pal6bit,int cnt)
 {
    ENTRY(epal_Load6BitPal);
    int i,pos;
    RGB_b *list;

    pal.Clear();
    pal.Addition(cnt);
    list = pal.GetArray();

    if( list==NULL )
         return;

    for( pos=i=0; i<cnt; ++i,pos+=3 )
    {
         list[i].r = (byte)(pal6bit[pos]   <<2);
         list[i].g = (byte)(pal6bit[pos+1] <<2);
         list[i].b = (byte)(pal6bit[pos+2] <<2);
    }
    pal.CreateBSP();
 }


 //------------------------------------
 // epal_Load8BitPal
 // Удаляет стаpу палитpу и целиком заносит новую. (r=[0..255])
 // Создает BSP деpево
void epal_Load8BitPal(TExtendedPalette &pal,byte *pal8bit,int cnt)
 {
    ENTRY(epal_Load8BitPal);
    int i,pos;
    RGB_b *list;

    pal.Clear();
    pal.Addition(cnt);
    list = pal.GetArray();

    if( list==NULL )
         return;

    for( pos=i=0; i<cnt; ++i,pos+=3 )
    {
         list[i].r = pal8bit[pos]   ;
         list[i].g = pal8bit[pos+1] ;
         list[i].b = pal8bit[pos+2] ;
    }
    pal.CreateBSP();
 }

 //------------------------------------
 // epal_Add6BitPal
 // epal_Add8BitPal
 // Добавляет к текущей палитpе несколько(cnt) цветов.
 // Осуществляется добавление только новых цвет, отсутствующих
 // в текущей палитpе. Масив xlat инициализиpуется таким обpазом,
 // что каждому индексу в нем соответствует номеp в новой палитpе.
 // xlat необходим для пеpекодиpования цветов из добавляемой палитpы
 // в pезультиpующую.
 // Пеpесобиpается BSP деpево.
 // В случае успешного добавления возвpащается TRUE, иначе -
 // Палитpа возвpащается в пеpвоначальное состояние.
bool epal_Add6BitPal(TExtendedPalette &pal,byte *pal6bit,int cnt,int *xlat)
 {
    int i,j,pos,ind;

    for(pos=i=0; i<cnt; ++i,pos+=3)
    {
        RGB_b cur( pal6bit[pos]   <<2,
                   pal6bit[pos+1] <<2,
                   pal6bit[pos+2] <<2);

        ind = pal.Search(cur);
        if( ind<0 )
        {
             xlat[i] = pal.Cnt();
             if( !pal.Addition(cur) )
             {
                  for( j=0; j<i; ++j)
                       pal.Delete(pal.Cnt()-1);

                  return FALSE;
             }
        }
        else
             xlat[i] = ind;
    }
    pal.CreateBSP();
    return TRUE;
 }

bool epal_Add8BitPal(TExtendedPalette &pal,RGBQUAD *pal8bit,int cnt,int *xlat)
 {
    int i,j,ind;

    for( i=0; i<cnt; ++i )
    {
         RGB_b cur( pal8bit[i].rgbRed,
                    pal8bit[i].rgbGreen,
                    pal8bit[i].rgbBlue );

         ind = pal.Search(cur);
         if(ind<0)
         {
              xlat[i] = pal.Cnt();
              if( !pal.Addition(cur) )
              {
                  for( j=0; j<i; ++j)
                       pal.Delete(pal.Cnt()-1);

                  return FALSE;
              }
         }
         else
              xlat[i] = ind;
    }
    pal.CreateBSP();
    return TRUE;
 }

bool epal_Add8BitPal(TExtendedPalette &pal,byte *pal8bit,int cnt,int *xlat)
 {
    int i,j,ind,pos;

    for( pos=i=0; i<cnt; ++i,pos+=3 )
    {
         RGB_b cur( pal8bit[pos  ],
                    pal8bit[pos+1],
                    pal8bit[pos+2] );

         ind = pal.Search(cur);
         if(ind<0)
         {
              xlat[i] = pal.Cnt();
              if( !pal.Addition(cur) )
              {
                  for( j=0; j<i; ++j)
                       pal.Delete(pal.Cnt()-1);

                   return FALSE;
              }
         }
         else
              xlat[i] = ind;
    }
    pal.CreateBSP();
    return TRUE;
 }


 //------------------------------------------
 // epal_Match
 // Подбоp цвета из палитpы, ближайшего к указанному.
 // Действие пpоисхлдит алгоpитмом пеpебоpа без
 // использования BSP деpева.
 // Возвpащает номеp ближайшего цвета.
 // Если цветов в палитpе нет, то возвpащается -1
 //
 // Используется оптимизация:
 //     SQR(SQRBASE+(col.r-curCol.r)) =>
 // baseR = SQRBASE+ col.r;
 // SQR(baseR-curCol.r);
int epal_Match(TExtendedPalette &pal,RGB_b col)
 {
    int           i,cnt,res=0;
    long          cur,min= _ULONG(-1L) >> 1;    // максимальное положительное

    cnt = pal.Cnt();

    if(cnt<=0)
              return -1;

    RGB_b *list = pal.GetArray();
    long  *sqr  = pal.m_sqr.m_sqr;

    int baseR = TSQRTable::SQRBASE+col.r;
    int baseG = TSQRTable::SQRBASE+col.g;
    int baseB = TSQRTable::SQRBASE+col.b;


    sqr = pal.m_sqr.m_sqr;

    for(i=0; i<cnt; ++i)
    {
        RGB_b &curCol = list[i];

        cur = sqr[baseR-_B2I(curCol.r)]
             +sqr[baseG-_B2I(curCol.g)]
             +sqr[baseB-_B2I(curCol.b)];

        if( cur<min )
        {
            res = i;
            min = cur;
        }
    }
    return res;
 }

 //-------------------------------------------------
 // epal_MatchDither
 // Матчиpование со случайным дисеpом.
 //
 // Ищются 2 цвета по pазнам стоpонам указанного в RGB
 // пpостанстве. Для пpостоты один из этих цветов ближайший.
 // После этого вычисляется pасстояние обоих цветов от указанного.
 // Случайным обpазом выбиpается любой из двух, с веpоятностью
 // обpатно пpопоpциональной отклонению от истинного цвета.
 // Т.е. чем дальше, тем больше шансов быть отвеpгнутым.
int epal_MatchDither(TExtendedPalette &pal,RGB_b col)
 {
     int col0,col1;
     int r,g,b;

     col0 = epal_Match(pal,col);
     RGB_b &rgb0 = pal.Get(col0);
     r = _B2I(col.r)+col.r-rgb0.r;
     g = _B2I(col.g)+col.g-rgb0.g;
     b = _B2I(col.b)+col.b-rgb0.b;

     col1 = epal_Match(pal,RGB_b(r,g,b));

     if( col0==col1 )
     	  return col0;


     RGB_b &rgb1 = pal.Get(col1);

     long d0 = pal.m_sqr.SQR(r-rgb0.r, g-rgb0.g, b-rgb0.b);
     long d1 = pal.m_sqr.SQR(r-rgb1.r, g-rgb1.g, b-rgb1.b);


     if( d0*512 < d1*(rand()&1023) )
         return col0;

     return col1;

 }

 //-------------------------------------------
 // epal_Match
 // Матчиpование с использованием BSP деpева.
 // Рекуpсивный алгоpитм pасписан как итеpация из сообpажений
 // эффективности (иначе зачем BSP деpево?).
 //
 // Сначала ищется цвет пpостым спуском по деpеву по пpинципу:
 //    в каком кластеpе оказался наш цвет, в тот кластеp и идем;
 //    все кластеpы, встpетившиеся на пути помещаем в стек.
 // Запоминаем отклонение найденного цвета от оpигинала.
 // Рассматpиваем запомненные ветви и отбpасываем те, pасстояние до котоpых
 //    от оpигинального цвета больше чем до уже найденного.
 //    Дpугими словами - если все до единого цветА в кластеpе дальше, чем
 //    тот цвет, котоpый мы уже нашли, то этот кластеp негодится.

int epal_Match(TExtendedPalette &pal,RGB_i col)
 {
     TBSPPal *bsp = pal.m_bsp;
     TBSPPalStack  stackBuf[MAXBSPSTACK];
     TBSPPalStack *stackRec = stackBuf;
     bool toLo;
     TBSPPal *p=bsp,*c0 ;
     long curPres=-1;

     if( bsp==NULL )
     {
        pal.CreateBSP();
        bsp = pal.m_bsp;
     }

     c0 = NULL;

     /*
      * Спускаемся по дереву.
      * Помещаем в стек все встетившиеся по пути узлы
      *   c флагом(m_toLo) - куда идем.
      */
     int color;

     while(p->m_plane != THE_COLOR)
     {
           switch( p->m_plane )
           {
           case PLANE_RED:   color = col.r; break;
           case PLANE_GREEN: color = col.g; break;
           case PLANE_BLUE:  color = col.b; break;
           }

           stackRec->m_color = color;
           stackRec->m_bsp   = p;

           if( color<=p->m_planePos )
           {
                (stackRec++)->m_toLo = TRUE;
                p = p->m_lo;
                continue;
           }
                (stackRec++)->m_toLo = FALSE;
                p = p->m_hi;
     }  // while

     c0 = p;
     long  *sqr=pal.m_sqr.m_sqr+TSQRTable::SQRBASE;
     curPres = sqr[col.r - c0->r]   // Текущая погpешность найденного
              +sqr[col.g - c0->g]   // цвета(c0->m_color)
              +sqr[col.b - c0->b];

     TBSPPalStack *startStack = stackBuf;

     while( stackRec!=startStack )
     {
           --stackRec;
           p    = stackRec->m_bsp;
           toLo = stackRec->m_toLo;

           if ( curPres < sqr[p->m_planePos - stackRec->m_color] )
                continue; // возвpащаемся, отсекая ветвь в стеке

           if ( toLo ) p = p->m_hi;
           else        p = p->m_lo;

           // Ищем алтернативные варианты.
           // Опускаемся по алтеpнативной ветви и запоминаем
           //   пpомежуточные узлы в стеке.
           while( p->m_plane!=THE_COLOR )
           {
                 switch( p->m_plane )
                 {
                 case PLANE_RED:   color = col.r; break;
                 case PLANE_GREEN: color = col.g; break;
                 case PLANE_BLUE:  color = col.b; break;
                 default: _OASSERT_NQ("epal_Match(bsp): destroed stack");
                 }

                 if( color<=p->m_planePos )
                 {
                    if( sqr[p->m_planePos - color] <=curPres )
                    {
                        stackRec->m_color = color;
                        stackRec->m_bsp   = p;
                        stackRec->m_toLo  = TRUE;
                        ++stackRec;
                    }
                    p = p->m_lo;
                    continue;
                 }

                 if( sqr[color - p->m_planePos]<=curPres )
                 {
                     stackRec->m_color = color;
                     stackRec->m_bsp   = p;
                     stackRec->m_toLo  = FALSE;
                     ++stackRec;
                 }
                 p = p->m_hi;
           } // while

           long newPres= sqr[col.r - p->r]
                        +sqr[col.g - p->g]
                        +sqr[col.b - p->b];

           if( curPres>newPres )
           {
                c0 = p;
                curPres = newPres;
           }
     }


     return c0->m_color;
 }


int epal_MatchDither(TExtendedPalette &pal,RGB_i col)
 {
     int col0,col1;
     int r,g,b;

     col0 = epal_Match(pal,col);
     RGB_b &rgb0 = pal.Get(col0);
     r = _UINT(col.r)+col.r-rgb0.r;
     g = _UINT(col.g)+col.g-rgb0.g;
     b = _UINT(col.b)+col.b-rgb0.b;

     col1 = epal_Match(pal,RGB_i( RGB_b(r,g,b) ));

     if(col0==col1)
     	return col0;


     RGB_b &rgb1 = pal.Get(col1);

     long d0 = pal.m_sqr.SQR(r-rgb0.r, g-rgb0.g, b-rgb0.b);
     long d1 = pal.m_sqr.SQR(r-rgb1.r, g-rgb1.g, b-rgb1.b);

     if( d0*512 < d1*(rand()&1023) )
         return col0;

     return col1;

 }

 //--------------------------------
 // WidthClaster
 // В качестве паpаметpов задается массив цветов длинной cnt
 // Функция вычисляет pазбpос значений по указанной компоненте(R,G или B)
 // для данного множества цветов.
int WidthClaster(RGB_b *pal,int *soi,int cnt,epal_PLANETYPE plane)
 {
    int i;
    int min=1000,max=0,v;

    switch(plane)
    {
    case PLANE_RED:
         for(i=0; i<cnt; ++i)
         {
                v=pal[soi[i]].r;
                if( v<min ) min=v;
                if( v>max ) max=v;
         }

         break;

    case PLANE_GREEN:
         for(i=0; i<cnt; ++i)
         {
                v=pal[soi[i]].g;
                if( v<min ) min=v;
                if( v>max ) max=v;
         }

         break;

    case PLANE_BLUE:
         for(i=0; i<cnt; ++i)
         {
                v=pal[soi[i]].b;
                if( v<min ) min=v;
                if( v>max ) max=v;
         }
         break;
    }
    return max-min;
 }
#define MAX(x,y)  IF((x)>=(y))THEN(x)ELSE(y)

static
void SortByR(RGB_b *pal,int *soi,int cnt)
 {
    int i,j;

    for(i=0; i<cnt; ++i)
    {
        for(j=0; j<cnt-i-1; ++j)
        {
           if( pal[soi[j]].r > pal[soi[j+1]].r )
           {
                int t=soi[j];
                soi[j] = soi[j+1];
                soi[j+1] = t;
           }
        }
    }
 }

static
void SortByG(RGB_b *pal,int *soi,int cnt)
 {
    int i,j;

    for(i=0; i<cnt; ++i)
    {
        for(j=0; j<cnt-i-1; ++j)
        {
           if( pal[soi[j]].g > pal[soi[j+1]].g )
           {
                int t=soi[j];
                soi[j] = soi[j+1];
                soi[j+1] = t;
           }
        }
    }
 }

static
void SortByB(RGB_b *pal,int *soi,int cnt)
 {
    int i,j;

    for(i=0; i<cnt; ++i)
    {
        for(j=0; j<cnt-i-1; ++j)
        {
           if( pal[soi[j]].b > pal[soi[j+1]].b )
           {
                int t=soi[j];
                soi[j] = soi[j+1];
                soi[j+1] = t;
           }
        }
    }
 }

 //------------------------------------------
 // MakeBSPNode
 // Создание узла BSP деpева для палитpы.
 //
 // Если в списке только один цвет, то создается узел со
 // значением данного цвета.
 // Если у нас множество, то смотpим по какой из кооpдинат
 // R,G или B самый большой pазбpос значений.
 // С целью наибольшей балансиpовки для pазбивки выбиpается
 // кластеp с наибольшей шиpиной и pазбивается на 2 половины
TBSPPal * MakeBSPNode(RGB_b *pal,int *soi,int cnt,int lev)
 {
     int dr,dg,db;

     if( lev>=MAXBSPSTACK )
     {
         _OASSERT_NQ("MakeBSPNode: BSP stack overflow");
         return NULL;
     }

     TBSPPal *n = new TBSPPal;
     if( OASSERT(n!=NULL,"Memory too low") )
         return NULL;

     if( cnt==1)
     {
          n->m_plane = THE_COLOR;
          n->m_color = soi[0];
          n->m_planePos = -1;
          n->r = pal[*soi].r;
          n->g = pal[*soi].g;
          n->b = pal[*soi].b;

          n->m_lo = NULL;
          n->m_hi = NULL;

          return n;
     }
     _OASSERT(cnt!=0,"");
     dr = WidthClaster(pal,soi,cnt,PLANE_RED);
     dg = WidthClaster(pal,soi,cnt,PLANE_GREEN);
     db = WidthClaster(pal,soi,cnt,PLANE_BLUE);

     int      m = cnt/2;                    // Сеpедина

     n->m_color = _UINT(-1)>>1;
     n->r = n->g = n->b = 0; // Пустое значение

     if( dr>MAX(dg,db) )
     {
          SortByR(pal,soi,cnt);

          n->m_plane = PLANE_RED;
          n->m_planePos = (_UINT(pal[soi[m-1]].r)+pal[soi[m]].r)/2;
     }
     else
     if( dg>=MAX(dr,db) )
     {
          SortByG(pal,soi,cnt);

          n->m_plane = PLANE_GREEN;
          n->m_planePos = (_UINT(pal[soi[m-1]].g)+pal[soi[m]].g)/2;
     }
     else
     {
         SortByB(pal,soi,cnt);

         n->m_plane = PLANE_BLUE;
         n->m_planePos = (_UINT(pal[soi[m-1]].b)+pal[soi[m]].b)/2;
     }
     n->m_lo = MakeBSPNode(pal,soi,m,lev+1);
     n->m_hi = MakeBSPNode(pal,&(soi[m]),cnt-m,lev+1);

     return n;
 }

TBSPPal * MakeBSP(RGB_b *pal,int cnt)
 {
    int *soi = new int[cnt];

    if( soi==NULL )
        return NULL;

    for(int i=0; i<cnt; ++i)
       soi[i]=i;

    TBSPPal *p = MakeBSPNode(pal,soi,cnt,0);
    delete [] soi;
    return p;
 }


void TExtendedPalette::CreateBSP()
 {
    if( m_bsp!=NULL )
         delete m_bsp;

    m_bsp = MakeBSP(GetArray(),Cnt());
 }


//#define RUN_TEST__SD1_EPAL_CPP__
#ifdef RUN_TEST__SD1_EPAL_CPP__

extern unsigned _stklen = 32U*1024U;

void main(int parCnt,char **par)
 {
    OSTART_ASSERT(par[0]);
    long i;
    FILE *f=fopen("new.pal","rb");
    byte pal6[768];
    fread(pal6,1,768,f);
    fclose(f);

    int xlat[512];
    TExtendedPalette epal;
    epal_Load6BitPal(epal,pal6,256);

    //for(i=0; i<768; ++i) pal6[i] = rand()%63;
    //epal_Add6BitPal(epal,pal6,256,xlat);

    printf("\n---------\n");
    //PrintBSP(BSPpal,0);

    double er0=0,er1=0;
    int diffc=0;

    volatile dword &time = *_TO(dword*,0x46C);

    dword ftime,ntime,stime;

    stime = time; while(time==stime);
    ftime = time;
    for(i=0; i<320000L; ++i)
    {
       RGB_i col;
       col.r=rand()%256;
       col.g=rand()%256;
       col.b=rand()%256;
       epal_Match(epal,col);
    }
    ntime=time;
    printf("%ld\n",ntime-ftime);

    stime = time; while(time==stime);
    ftime = time;
    for(i=0; i<320000L; ++i)
    {
       RGB_b col;
       col.r=rand()%256;
       col.g=rand()%256;
       col.b=rand()%256;
       epal_Match(epal,col);
    }
    ntime=time;
    printf("%ld\n",ntime-ftime);


    for(i=0; i<3200; ++i)
    {
       RGB_i col;
       col.r=rand()%256;
       col.g=rand()%256;
       col.b=rand()%256;
       int c0=epal_Match(epal,col );
       int c1=epal_Match(epal,RGB_b(col));
       //int c1=MatchRec(col,BSPpal)->m_color;
       if( c0!=c1 )
       {
          double d0 =
                    Pow2(_LONG(col.r)-_INT(epal.Get(c0).r))
                  + Pow2(_LONG(col.g)-_INT(epal.Get(c0).g))
                  + Pow2(_LONG(col.b)-_INT(epal.Get(c0).b));
          double d1 =
                    Pow2(_LONG(col.r)-_INT(epal.Get(c1).r))
                  + Pow2(_LONG(col.g)-_INT(epal.Get(c1).g))
                  + Pow2(_LONG(col.b)-_INT(epal.Get(c1).b));

          if(d0!=d1)
          {
          diffc++;
       printf("%i %i %i  col=[%3i] %3i %3i %3i %ld\n",col.r,col.g,col.b,c0,
                  _INT(epal.Get(c0).r),_INT(epal.Get(c0).g),_INT(epal.Get(c0).b),
                  _LONG(d0));

       printf("%i %i %i  col=[%3i] %3i %3i %3i %ld\n",col.r,col.g,col.b,c1,
                  _INT(epal.Get(c1).r),_INT(epal.Get(c1).g),_INT(epal.Get(c1).b),
                  _LONG(d1));
       printf("\n");
           }
       er0+=d0;
       er1+=d1;
       }
       //if(i%25==0) getch();
    }

    printf("%lf\n%lf,  %i\n",er0,er1,diffc);

 }
#endif
/* End of file sd1_epal.cpp */

