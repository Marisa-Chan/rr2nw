/*
 * File  : D:\GAME\OBASE\Bullet\bullet.h
 * Autor :
 * Ver   1.0 
 */
#ifndef __BULLET_H__INCLUDED
#define __BULLET_H__INCLUDED


#include "..\DynObj\DynObj.h"

#include "storage/h/subject.h"
#include "storage/h/attr.h"
#include "storage/h/strgdefs.h"
#include "kernel/h/active.h"
#include "../dynobj/dynpartln.h"
#include "../dynobj/dyntrace.h"

#include "Sound.h"
#include "obase/sound/wavobj.h"


#define COLOR_GRAD 16


class CDC;


 //===========================================================================
class AttributeBullet : public ct_Attribute
{
 public:

    GR_HTEXTURE    m_cacheImage;
    GR_HTEXTURE    m_cacheImageFront;

    
	WAVObj            * m_wav;	
    ct_ClassTableID     m_ctsndID;


    virtual void    update(double ts);
    virtual int     receiveEvent( KR_Event &event );
    unsigned long   m_colorGrad[COLOR_GRAD];
    ct_ClassTableID m_smokeTableID;
    KR_ObjectID     m_smokeAttrID;
    int             m_cacheSparkAttrTable    ;  // 
    int             m_cacheSparkTable        ;  // 
    int             m_cacheColor             ;  // Цвет из таблицы, расчитывается в момент создания класса
    int             m_cacheSparkAttr         ;  // Объект со вспышкой от попадания в неживой объект
    int             m_cacheOutSparkAttr      ;  // 
    int             m_cacheSplashAttr        ;  // 
    int             m_cacheExplAttr;
    int             m_cacheExplosionTable;
    CViewObjectModel*   m_cacheSkin              ;  // 

//{{ATTRIBUTE
    ct_AttrItem  m_array[41];
    int             m_type                   ;  // тип пули, уникален для каждого вида пули
    int             m_RGB                    ;  // Упакованное RGB значение [00bbggrr]
    int             m_RGB0                   ;  // 
    double          m_moveTimeIncrement      ;  // Через сколько времени пересчитывать координаты
    double          m_chkClzTimeIncrement    ;  // Через сколько времени
    ct_AttrStr      m_sparkAttr              ;  // Тип вспышки при попадании в неживые объекты
    ct_AttrStr      m_outSparkAttr           ;  // 
    ct_AttrStr      m_splashAttr             ;  // Объект вспышки от попадания в живой объект
    double          m_massa                  ;  // Вес пули
    double          m_startSpeed             ;  // m/s
    ct_AttrStr      m_sparkTable             ;  // 
    ct_AttrStr      m_sparkAttrTable         ;  // 
    double          m_radius0                ;  // 
    double          m_radius1                ;  // 
    double          m_length                 ;  // Длина визуализируемой трассы
    double          m_step0                  ;  // 
    double          m_step                   ;  // 
    int             m_useLight               ;  // 
    double          m_lightRadius            ;  // 
    int             m_lightBrightness        ;  // 
    int             m_lightColor             ;  // 
    ct_AttrStr      m_smokeTableName         ;  // 
    ct_AttrStr      m_smokeAttrName          ;  // 
    ct_AttrStr      m_explAttrName           ;  // 
    ct_AttrStr      m_trace                  ;  // Текстура трассы
    int             m_hasTrace               ;  // Есть ли трасса
    double          m_traceMinDist           ;  // Минимальное расстояние от нас, на котором трасса видна
    ct_AttrStr      m_traceWidthString       ;  // Строка, описывающая ширину. Ширина каждого сегмента равна соответствующей букве - "A"
    double          m_traceFlatRatio         ;  // Коэфициент увеличения жопы трассы
    double          m_traceAllFlatDist       ;  // Расстояние на котором не происходит проверки на рисование жопы
    ct_AttrStr      m_traceAss               ;  // Текстура для жопы
    double          m_traceSegmentLength     ;  // Длина сегмента трассы
    ct_AttrStr      m_shootSndName           ;  // 
    int             m_traceExist             ;  // 
    ct_AttrStr      m_traceTexture           ;  // 
    int             m_useBarellSmoke         ;  // Есть ли дым из дула при выстреле
    int             m_useSkin                ;  // 
    ct_AttrStr      m_skinName               ;  // 
    double          m_rotSpeedOx             ;  // 
    double          m_rotSpeedOy             ;  // 
    double          m_rotSpeedOz             ;  // 

    AttributeBullet()
    {
        m_type               = 0;
        m_RGB                = 0x000000FFD050UL;
        m_RGB0               = 0xFFFFFF;
        m_moveTimeIncrement  = 0.02;
        m_chkClzTimeIncrement = 0.1;
        strncpy(m_sparkAttr,"Spark.Flash", sizeof( ct_AttrStr )-1 );
        strncpy(m_outSparkAttr,"Spark.Flash", sizeof( ct_AttrStr )-1 );
        strncpy(m_splashAttr,"BulletSplash", sizeof( ct_AttrStr )-1 );
        m_massa              = 0.009;
        m_startSpeed         = 40.0;
        strncpy(m_sparkTable,"Spark", sizeof( ct_AttrStr )-1 );
        strncpy(m_sparkAttrTable,"SparkAttr", sizeof( ct_AttrStr )-1 );
        m_radius0            = 0.2;
        m_radius1            = 0.6;
        m_length             = 6.0;
        m_step0              = 0.1;
        m_step               = 0.08;
        m_useLight           = 0;
        m_lightRadius        = 2.0;
        m_lightBrightness    = 250;
        m_lightColor         = 7;
        strncpy(m_smokeTableName,"Smoke", sizeof( ct_AttrStr )-1 );
        strncpy(m_smokeAttrName,"Smoke.Attr.Led", sizeof( ct_AttrStr )-1 );
        strncpy(m_explAttrName,"Expl.Attr.Default", sizeof( ct_AttrStr )-1 );
        strncpy(m_trace,"trace.spr", sizeof( ct_AttrStr )-1 );
        m_hasTrace           = 1;
        m_traceMinDist       = 15.0;
        strncpy(m_traceWidthString,"BEFKLMMLKFEB", sizeof( ct_AttrStr )-1 );
        m_traceFlatRatio     = 0.8;
        m_traceAllFlatDist   = 90.0;
        strncpy(m_traceAss,"smoke.spr", sizeof( ct_AttrStr )-1 );
        m_traceSegmentLength = 10.0;
        strncpy(m_shootSndName,"wav.Shoot.Gun2", sizeof( ct_AttrStr )-1 );
        m_traceExist         = 0;
        strncpy(m_traceTexture,"trace.spr", sizeof( ct_AttrStr )-1 );
        m_useBarellSmoke     = 1;
        m_useSkin            = 0;
        strncpy(m_skinName,"", sizeof( ct_AttrStr )-1 );
        m_rotSpeedOx         = 0;
        m_rotSpeedOy         = 0;
        m_rotSpeedOz         = 0;

        m_array[0].set("m_type",m_type);
        m_array[1].set("m_RGB",m_RGB);
        m_array[2].set("m_RGB0",m_RGB0);
        m_array[3].set("m_moveTimeIncrement",m_moveTimeIncrement);
        m_array[4].set("m_chkClzTimeIncrement",m_chkClzTimeIncrement);
        m_array[5].set("m_sparkAttr",m_sparkAttr);
        m_array[6].set("m_outSparkAttr",m_outSparkAttr);
        m_array[7].set("m_splashAttr",m_splashAttr);
        m_array[8].set("m_massa",m_massa);
        m_array[9].set("m_startSpeed",m_startSpeed);
        m_array[10].set("m_sparkTable",m_sparkTable);
        m_array[11].set("m_sparkAttrTable",m_sparkAttrTable);
        m_array[12].set("m_radius0",m_radius0);
        m_array[13].set("m_radius1",m_radius1);
        m_array[14].set("m_length",m_length);
        m_array[15].set("m_step0",m_step0);
        m_array[16].set("m_step",m_step);
        m_array[17].set("m_useLight",m_useLight);
        m_array[18].set("m_lightRadius",m_lightRadius);
        m_array[19].set("m_lightBrightness",m_lightBrightness);
        m_array[20].set("m_lightColor",m_lightColor);
        m_array[21].set("m_smokeTableName",m_smokeTableName);
        m_array[22].set("m_smokeAttrName",m_smokeAttrName);
        m_array[23].set("m_explAttrName",m_explAttrName);
        m_array[24].set("m_trace",m_trace);
        m_array[25].set("m_hasTrace",m_hasTrace);
        m_array[26].set("m_traceMinDist",m_traceMinDist);
        m_array[27].set("m_traceWidthString",m_traceWidthString);
        m_array[28].set("m_traceFlatRatio",m_traceFlatRatio);
        m_array[29].set("m_traceAllFlatDist",m_traceAllFlatDist);
        m_array[30].set("m_traceAss",m_traceAss);
        m_array[31].set("m_traceSegmentLength",m_traceSegmentLength);
        m_array[32].set("m_shootSndName",m_shootSndName);
        m_array[33].set("m_traceExist",m_traceExist);
        m_array[34].set("m_traceTexture",m_traceTexture);
        m_array[35].set("m_useBarellSmoke",m_useBarellSmoke);
        m_array[36].set("m_useSkin",m_useSkin);
        m_array[37].set("m_skinName",m_skinName);
        m_array[38].set("m_rotSpeedOx",m_rotSpeedOx);
        m_array[39].set("m_rotSpeedOy",m_rotSpeedOy);
        m_array[40].set("m_rotSpeedOz",m_rotSpeedOz);

        linkTable(m_array,41);
    }
//}}END_OF_ATTRIBUTE
};



 //===========================================================================
class AttributeTableBullet : public ct_AttributeTable
{
 protected:
    AttributeBullet *m_table;

 public:
    AttributeTableBullet()
    {
       m_table = NULL;
       registerClass( "BulletAttr" );
    }

    virtual void       allocObjects( int objectQnty );
    virtual void       freeObjects ();
    virtual ct_Object *getObjectPTR( int index );
};

extern AttributeTableBullet __bulletAttrTable;



typedef struct {
    int                m_attrIndex;
    int	               m_traceLength;
    double             m_wline;
	
    CFVector3          m_bulletPosition;
    CFVector3          m_dir;
    CFVector3          m_prevPos;
    CFVector3          m_initialPos;
    CFVector3          m_initialDir;
	
    double             m_traceCurrentSegmentLength;
	
    KR_ObjectID        m_bulletMaster;
    KR_ObjectID        m_snd;
	
    int                m_traceCurrentLength;
    int                m_traceCurrentTextureOffset;
    int                m_validNext;
	
} BulletData;



class Bullet :	public ct_Subject,
                public BulletData
{
 void initTrace();

 public:
    AttributeBullet   *m_attr;
    
	s_ViewDynParticleLine m_viewPart;
	s_ViewDynamicTrace	  m_viewTrace[MAX_TRACE_LENGTH];	



             Bullet();
    virtual ~Bullet();
    virtual int  receiveEvent( KR_Event &event );
    virtual void draw        ( CDC &gc );
    virtual void addNotify   ();
    virtual void removeNotify();
    //virtual void render( CViewDynamicList &list, double ts );
    //virtual void endRender( CViewScene *scene );
    strg_SUBJECT_DYNVIEW_DECLARE

    //virtual CFVector3     realPosition();
    void         createSpark ( int sparkAttrIndex, double timeStamp );

    void         createSmoke( const CFVector3 &p, const CFVector3 &dir, double t );
    int          checkCollision( double &clzTime );

	void		 traceStep(double deltaT);

	virtual bool	dump(PIN_SaveFile & sf);
	virtual bool	load(PIN_SaveFile & );
	virtual void	loadNotify();
	virtual bool	shouldDump () { return true; } 
};


 //===========================================================================
class BulletTable : public ct_SubjectTable
{
 private:
    Bullet *m_table;
 public:
    BulletTable()
    {
        m_table = NULL;
        registerClass( "Bullet" );
    }
    ~BulletTable()
    {
        delete [] m_table;
        m_table = NULL;
    }

    virtual void       allocObjects( int objectQnty );
    virtual void       freeObjects ();
    virtual ct_Object *getObjectPTR( int index );
    virtual  bool      isRendering();
	virtual bool       isAudible();	
};


#endif // ifndef __BULLET_H__INCLUDED
/* End of file D:\GAME\OBASE\Bullet\bullet.h */
