/*
 * File  : D:\GAME\OBASE\DCross\DCross.h
 * Autor :
 * Ver   1.0 
 */
#ifndef __DCROSS_H__INCLUDED
#define __DCROSS_H__INCLUDED


class CDC;

class DCross : public ct_Subject
{
 public:
    int           m_type;
    int           x0,y0,x1,y1,
                  xText,yText;
    unsigned long m_color;
    char          text[100];
    double        removeTime;
    CFVector3     m_position;

    CViewObjectRef         m_skin;
    s_ViewDynamicObject    m_viewDynObj;


    void startInitialize()
    {
         m_type  = dc_CROSS;
         m_color = 0;
         text[0] = 0;
         x0 = y0 = x1 = y1 = 0;
         xText = yText = 0;
    }

             DCross();
    virtual ~DCross();
    virtual int  receiveEvent( KR_Event &event );
    virtual void draw        ( CDC & );
    virtual void addNotify   ();
    virtual void removeNotify();
    virtual CFVector3     realPosition();

    void    render   ( CViewDynamicList &list, double ts );
    void    endRender( CViewScene *scene );

    virtual bool shouldDump () { return true; } // we don't save debug stuff
						// just eliminate them
};

#endif // ifndef __DCROSS_H__INCLUDED
/* End of file D:\GAME\OBASE\DCross\DCross.h */