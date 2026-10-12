#ifndef __OLEVEL_H__
#define __OLEVEL_H__

#include "kernel/h/context.h"
#include "storage/h/subject.h"
#include "storage/h/Attr.h"
#include "kernel/h/object.h"
#include "sc/h/sc.h"

typedef SimulationContext *SimulationContextPTR;


#define HOWITZER_MAX_NAME  40
#define HOWITZER_MAX_LEVEL 128

typedef struct {
	CFVector3	pos;
	KR_ObjectID occupant;
	char		symbolic[HOWITZER_MAX_NAME];
} HowitzerPlace;


class AttributeLevel : public ct_Attribute
{
 public:
//{{ATTRIBUTE
    ct_AttrItem  m_array[30];
    double          m_minDesire              ;  // Минимальное значение для желания пострелять
    double          m_maxDesire              ;  // Максимальное значение, которое может принимать желание пострелять
    double          m_addDesire              ;  // скорость прироста желания пострелять в секунду
    double          m_subDesire              ;  // На сколько уменьшить в момент выстрела
    double          m_desireAttackThreshold  ;  // Выше этого порога происходит атака врага
    double          m_desireForgetThreshold  ;  // Значение, ниже которого происходит забываие о враге
    double          m_levelAddRatingScale    ;  // 
    double          m_levelSubRatingScale    ;  // 
    double          m_minRating              ;  // 
    double          m_maxRating              ;  // 
    double          m_renegatThreshold       ;  // 
    double          m_unitDamageScale        ;  // 
    double          m_unitDamageScaleFromPlayer;  // 
    int             m_minSecBulletCnt        ;  // 
    int             m_maxSecBulletCnt        ;  // 
    double          m_minDamage              ;  // 
    double          m_fromFriendDamageScale  ;  // 
    double          m_666Time                ;  // 
    int             msInvX                   ;  // 
    int             msInvY                   ;  // 
    double          msSensX                  ;  // 
    double          msSensY                  ;  // 
    int             msUse                    ;  // 
    int             jsInvX                   ;  // 
    int             jsInvY                   ;  // 
    double          jsSensX                  ;  // 
    double          jsSensY                  ;  // 
    int             jsUse                    ;  // 
    int             m_reloadLevel            ;  // Значение уровня для перезагрузки
    double          keySens                  ;  // GREEN

    AttributeLevel()
    {
        m_minDesire          = -1.0;
        m_maxDesire          = 1.0;
        m_addDesire          = 0.03;
        m_subDesire          = 0.55;
        m_desireAttackThreshold = 0.5;
        m_desireForgetThreshold = 0.0;
        m_levelAddRatingScale = 0.5;
        m_levelSubRatingScale = 1.0;
        m_minRating          = -2.4;
        m_maxRating          = 2.4;
        m_renegatThreshold   = -0.4;
        m_unitDamageScale    = 0.8;
        m_unitDamageScaleFromPlayer = 0.6;
        m_minSecBulletCnt    = 10;
        m_maxSecBulletCnt    = 30;
        m_minDamage          = 0.5;
        m_fromFriendDamageScale = 0.2;
        m_666Time            = 6.0;
        msInvX               = 0;
        msInvY               = 0;
        msSensX              = 0.5;
        msSensY              = 0.5;
        msUse                = 0;
        jsInvX               = 0;
        jsInvY               = 0;
        jsSensX              = 0.5;
        jsSensY              = 0.5;
        jsUse                = 0;
        m_reloadLevel        = 0;
        keySens              = 1;

        m_array[0].set("m_minDesire",m_minDesire);
        m_array[1].set("m_maxDesire",m_maxDesire);
        m_array[2].set("m_addDesire",m_addDesire);
        m_array[3].set("m_subDesire",m_subDesire);
        m_array[4].set("m_desireAttackThreshold",m_desireAttackThreshold);
        m_array[5].set("m_desireForgetThreshold",m_desireForgetThreshold);
        m_array[6].set("m_levelAddRatingScale",m_levelAddRatingScale);
        m_array[7].set("m_levelSubRatingScale",m_levelSubRatingScale);
        m_array[8].set("m_minRating",m_minRating);
        m_array[9].set("m_maxRating",m_maxRating);
        m_array[10].set("m_renegatThreshold",m_renegatThreshold);
        m_array[11].set("m_unitDamageScale",m_unitDamageScale);
        m_array[12].set("m_unitDamageScaleFromPlayer",m_unitDamageScaleFromPlayer);
        m_array[13].set("m_minSecBulletCnt",m_minSecBulletCnt);
        m_array[14].set("m_maxSecBulletCnt",m_maxSecBulletCnt);
        m_array[15].set("m_minDamage",m_minDamage);
        m_array[16].set("m_fromFriendDamageScale",m_fromFriendDamageScale);
        m_array[17].set("m_666Time",m_666Time);
        m_array[18].set("msInvX",msInvX);
        m_array[19].set("msInvY",msInvY);
        m_array[20].set("msSensX",msSensX);
        m_array[21].set("msSensY",msSensY);
        m_array[22].set("msUse",msUse);
        m_array[23].set("jsInvX",jsInvX);
        m_array[24].set("jsInvY",jsInvY);
        m_array[25].set("jsSensX",jsSensX);
        m_array[26].set("jsSensY",jsSensY);
        m_array[27].set("jsUse",jsUse);
        m_array[28].set("m_reloadLevel",m_reloadLevel);
        m_array[29].set("keySens",keySens);

        linkTable(m_array,30);
    }
//}}END_OF_ATTRIBUTE
};

extern AttributeLevel g_levelAttr;

#define SAVELOAD_SLOTS_NUM 8

class ol_Level : public KR_Object
 {
 private:
      int      m_curLevel;
 public:
	
    enum {MST_NONE, MST_LOAD, MST_SAVE, MST_RESTART };
    

    static int    m_saveGameStatus;
    static int	  m_levelNumber;
    static int	  m_levelToLoad;
    static char   m_saveGameName[];


    int m_saveSlotUsed[SAVELOAD_SLOTS_NUM];
    int m_saveSlotLevel[SAVELOAD_SLOTS_NUM];


	HowitzerPlace m_howitzerPool[HOWITZER_MAX_LEVEL];
	int			  m_howitzersLoaded;
	bool		  ReadHowitzers();
	
	// Returns index in holders array
	int			  AttachToHowitzerHolder(const char * holderName, KR_ObjectID occupant);
	int			  AttachToHowitzerHolder(int index, KR_ObjectID occupant);
	void		  ReleaseHolder(int index, KR_ObjectID occupant);	

    virtual int receiveEvent(KR_Event &event);
    void addNotify   ();
    void removeNotify();

    void PrepareSaveLoadMenu(const char *flp);
    void DumpSaveLoadCfgFile(const char *flp) const;

      int      openLevel(
                          TSuaCript            &suacript,
                          ct_Arena             &storage,
                          int                   levelNum
                        );
      void     closeLevel();

      void    reliseProject(int prj);

      ol_Level(){ m_curLevel = -1; }
      virtual ~ol_Level(){ closeLevel(); }
      virtual bool shouldDump () { return false; } // we don't dump attributes
 };

#endif
/* End of OLEVEL.H */