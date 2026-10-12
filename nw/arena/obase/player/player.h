#ifndef __PLAYER_H__INCLUDED
#define __PLAYER_H__INCLUDED

#include "i/player.i"
#include "kernel/h/setofid.h"

 /*
  * Класс описания статуса игрока по отношению к одной воюющей стороне.
  */
class PlayerStatus
 {
 public:
    double      m_damageDiff; // разница между повреждениями своих и врагов
    int         m_isRenegat;  // флаг предательства для данной стороны
    KR_ObjectID m_masterID;   // идентификатор командира стороны
    int         m_isMissionSuccess; // флаг выполнения миссии;

    void startInitialize()
    {
        m_damageDiff = 0;
        m_isRenegat  = 0;
        m_masterID   = KR_ObjectID::NUL();
        m_isMissionSuccess = 1;
    }

    PlayerStatus()
    {
        startInitialize();
    }
 };


typedef struct {
    PlayerStatus  m_playerStatus[5];
    int           m_sideQnty;
	PlayerMission m_mission[6];
    int           m_missCnt;
    int           m_total_misCount;
      
    double        m_damage;  

} PlayerData ;


class PIN_SaveFile;
class Player :	public IPlayer,
				public PlayerData
 {
 public:
    
     void          startInitialize()
    {
       m_sideQnty = 0;
       m_damage   = 1;	
       m_missCnt  = 0;
       m_total_misCount   = 0;
    }

    Player()
    {
       startInitialize();
    }
    void constructor();
    void destructor();

   PlayerStatus *searchStatus(KR_ObjectID commander);

	bool	dump(PIN_SaveFile & sf);
	bool	load(PIN_SaveFile & );

	void	loadNotify();


   /*
    * Interface IPlayer
    */
   virtual double &damageDiff      (KR_ObjectID commander);
   virtual int    &isRenegat       (KR_ObjectID commander);
   virtual int    &isMissionSuccess(KR_ObjectID commander);
   virtual double &getDamage       ()                    ;
   virtual void    resetPlayer     ()                    ;
   virtual void    addCommander     (KR_ObjectID commander);
   virtual void    findCommander    ();
   virtual void    attackUnit      (KR_ObjectID commander,double damage);

   virtual int     getMissionCnt() const;
   virtual KR_ObjectID getMission(int i);
   virtual int     addMission( KR_ObjectID mID, KR_ObjectID comID );
   //virtual void    delMission( KR_ObjectID mID );
   virtual void	   CleanupMissionPool();
   virtual bool    doYouWant ( KR_ObjectID mID );
   virtual PlayerMission &getMiss(int i);
   virtual void	   BetrayFor (KR_ObjectID commander) ;   
   virtual void	   SetHostility (KR_ObjectID commander);
 };

#endif

