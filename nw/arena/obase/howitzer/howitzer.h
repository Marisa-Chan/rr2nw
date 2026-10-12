/*
 * File  : C:\WinGame\OBASE\Howitzer\Howitzer.h
 * Autor :
 * Ver   1.0 
 */
#ifndef __Howitzer_H__INCLUDED
#define __Howitzer_H__INCLUDED



class AttributeHowitzer;

typedef struct {
    
	KR_ObjectID            m_HowitzerAttrID;
	int					   m_HolderIndex;
	double				   m_damage;
    KR_ObjectID            m_commanderID;
	double                 m_rotateOy;
	double				   m_hAngle;
	KR_ObjectID            m_enemyID;

	double                 m_lastActionTime;
	double                 m_lastEnemyScanTime;
	bool				   m_shootThisBastard;
	double				   m_lastShootTime;
	

} HowitzerData;

class Howitzer :	public ct_Subject,
					public IDynamicObject,
					public IUnit,
					public HowitzerData
{
 void setHowitzerAttr();
 void CheckBlockUp();



 public:

    CViewObjectRef  m_skin; 
    s_ViewDynamicObject    m_viewDynObj;                      
    virtual void render   ( CViewDynamicList &list, double ); 
    virtual void endRender( CViewScene *scene );              
    virtual CFVector3     realPosition();

    const AttributeHowitzer   *m_attr;
   	
             Howitzer();
    virtual ~Howitzer();
    virtual int  receiveEvent( KR_Event &event );
    virtual void addNotify   ();
    virtual void removeNotify();

	virtual void      *queryInterface( int interNum );
    


	virtual bool	shouldDump () { return true; } 
	virtual bool	dump(PIN_SaveFile & sf);
	virtual bool	load(PIN_SaveFile & );
	virtual void	loadNotify();

	

// IDynamicObject
	CFVector3  getPos      ();
    double     getHAngle   ();
    CFVector3  getUpVector ();
    CFVector3  getCenter   ();
    double     getRadius   ();
    double     getRadius0  ();
    CFVector3  getMoveDir  ();
    double     getMoveSpeed();
    void       getMatrix   ( CFMatrix3x4 &m);    
    TCCFMatrix3x4 &GetDir  () {return m_skin.GetDir(); }
    void	   SetDir(TCSFMatrix3x4 &dir) { m_skin.GetDirModify() = dir; }
	double 	   getMass	   () {return 1;}	// Масса
// IUnit

	double		getPower   (); 
    int			isFriend   (const KR_ObjectID &commanderID );
    double		getDamage  ();
    void		setDamage  ( double d, const CFVector3 &pos, double ts,
						KR_ObjectID fromID );
    double		desireShoot();
    KR_ObjectID getCommander();
    void        setCommander(KR_ObjectID oID);

};

#endif // ifndef __Howitzer_H__INCLUDED
/* End of file C:\WinGame\OBASE\Howitzer\Howitzer.h */