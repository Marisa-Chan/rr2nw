//    ct_AttrStr      m_skinName               ;  // Ежику понятно
//    ct_AttrStr      m_corpseAttrName         ;  // Ежику понятно
//    double          m_initialDamage          ;  // Ежику понятно
//    double          m_fireSpeed              ;  // Время между выстрелами
//    double          m_turnSpeed              ;  // Скорость поворота
//    double          m_addRoll                ;  // Начальный поворот пушки. Бить тех, кто скины ориентирует неправильно
//    double          m_shootAngle             ;  // Минимальное скалярное произведение нормированных векторов
//						     точного и фактического направления на цель
//						     при котором пушечка начинает стрелять
//					             0.95 по умолчанию
//    ct_AttrStr      m_bulletAttrName         ;  // Ежику понятно
//    double          m_dx                     ;  // Координата точки ортносительно центра, из которой вылетают пульки
//    double          m_dy                     ;  // 
//    double          m_dz                     ;  // 
//    int             m_dumbness               ;  // "Тупость" пушки. 0 - совсем тупая, 1 - стреляет с упреждением
//    double          m_deflectionXMax         ;  // Случайное отклонение при стрельбе. Пушка можеит мазать
//    double          m_deflectionXMin         ;  // 
//    double          m_deflectionYMax         ;  // 
//    double          m_deflectionYMin         ;  // 
//    double          m_deflectionZMax         ;  // 
//    double          m_deflectionZMin         ;  // 
//
//    double          m_maxBulletFlyTime       ;     Тщательно прицеливаетмся и стреляем точно чтобы попасть. 
//						     Если пуля в течении m_maxBulletFlyTime секунд не попадает  
//                                                   в цель или бампится об какой-то объект, не являющийся целью
//						     то есть цель загорожена, то ищем новую цель


/**************************************************************************
 *
 *                            Howitzer
 *
 ******************************************************************`********/

func void CreateHowitzerAttrDefault( int ctID )
 var int oID, cachePos;
{

   New(ctID, "Howitzer.Attr.Default", oID, cachePos);   
// ПОСТАВТЕ ЗДЕСЬ СКИН, СООТВЕТСТВУЮЩИЙ УРОВНЮ!!!!!!!!!!!!!!
   SetAttribute_s(oID,cachePos,"m_skinName","sk.corpse.default");
   SetAttribute_s(oID,cachePos,"m_bulletAttrName","Bullet.Sec");
   SetAttribute_f(oID,cachePos,"m_dx",0.5);
   SetAttribute_f(oID,cachePos,"m_dy",0.5);
}


func void main_CreateHowitzerAttrs()
var int ctID;
{
   ctID := s_AddClassTable( "HowitzerAttr", 2);  
   CreateHowitzerAttrDefault( ctID );
}
