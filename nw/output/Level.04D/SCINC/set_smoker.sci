func void main_CreateSmoker()
 var int attrID, attrCP, ctID;
{
   ctID := s_AddClassTable( "Smoker",50+12 );


    CreateSmoker(ctID,"Smoker.Attr.White", [2474, 100, -2300] );//create place
//    CreateSmoker(ctID,"Smoker.Attr.Tower", [2474+15, 100, -2300] );//create
    CreateSmoker(ctID,"Smoker.Attr.Huge", [4335,112.5,-3563] );//заводская труба
    CreateSmoker(ctID,"Smoker.Attr.Tower",[4383.5, 85,-3529] );//труба маленькая
    CreateSmoker(ctID,"Smoker.Attr.Tower",[4339, 92.5,-3543.5] );//труба средняя

    CreateSmoker(ctID,"Smoker.Attr.Tower",[3370,90,-3276] ); //труба крематория
    CreateSmoker(ctID,"Smoker.Attr.Tower",[3382,90,-3282] );


    CreateSmoker(ctID,"Smoker.Attr.Tower",[3328,119.5,-3134] );// завод у концлагеря
    CreateSmoker(ctID,"Smoker.Attr.Huge", [3355,113,-2074] );//завод на полуострове (большая труба)
    CreateSmoker(ctID,"Smoker.Attr.Tower",[3357,92.5,-2052.5] );// завод на полуострове (малая трубы)
    CreateSmoker(ctID,"Smoker.Attr.Tower",[3134,80,-1992] );//  средняя труба около полуострова
    CreateSmoker(ctID,"Smoker.Attr.Tower",[3130,85,-1976] );// большая труба -..-..-..-
    CreateSmoker(ctID,"Smoker.Attr.Tower",[3191,55.5,-1983] );// труба на домике
    CreateSmoker(ctID,"Smoker.Attr.Tower",[1419,58,-3172] );//труба паровой машины на базе подлодок
    CreateSmoker(ctID,"Smoker.Attr.Tower",[754,130, -2568] );//  труба заводика на ацтекской фабрике
    CreateSmoker(ctID,"Smoker.Attr.Tower",[771,120.75, -2567] );// труба ацтекского двигателя на той же фабрике
    CreateSmoker(ctID,"Smoker.Attr.Tower",[1164,83.75,-2398] );//труба ацтекского двигателя в большом городе

    CreateSmoker(ctID,"Smoker.Attr.Tower",[1345,107.5,-2494] );//труба №1 ацтекского завода в большом городе
    CreateSmoker(ctID,"Smoker.Attr.Tower",[1345,107.5,-2474] );//
    CreateSmoker(ctID,"Smoker.Attr.Huge", [1345,107.5,-2453] );//
    CreateSmoker(ctID,"Smoker.Attr.White",[1640,111,-2021] );//
    CreateSmoker(ctID,"Smoker.Attr.White",[1711,111,-2094] );//

    CreateSmoker(ctID,"Smoker.Attr.White",[1429,50,-3862] );// гейзер1 на дальнем острове
    CreateSmoker(ctID,"Smoker.Attr.White",[1408,50,-3799] );// гейзер2 на дальнем острове
    CreateSmoker(ctID,"Smoker.Attr.White",[634,212.5,-4814] );// гейзер в дальнем лесу
    CreateSmoker(ctID,"Smoker.Attr.White",[762,212.5,-4768] );// гейзер в дальнем лесу
    CreateSmoker(ctID,"Smoker.Attr.White",[1664,173,-1689] );// гейзер на горе
    CreateSmoker(ctID,"Smoker.Attr.White",[1671,148,-1638] );// гейзер на горе
    CreateSmoker(ctID,"Smoker.Attr.White",[2194,42,-1394] );// на бережку

    CreateSmoker(ctID,"Smoker.Attr.White",[1910,39.8,-510] );// гейзер на озере
    CreateSmoker(ctID,"Smoker.Attr.White",[1895,39.8,-405] );// гейзер на озере
    CreateSmoker(ctID,"Smoker.Attr.White",[1795,39.8,-470] );// гейзер на озере

//---------test
    CreateSmoker(ctID,"Smoker.Attr.FireMd",[2304, 97.95, -3345] );// Fakel
    CreateSmoker(ctID,"Smoker.Attr.FireMd",[2304, 97.95, -3395] );// Fakel
    CreateSmoker(ctID,"Smoker.Attr.FireMd",[2365, 97.95, -3345] );// Fakel
    CreateSmoker(ctID,"Smoker.Attr.FireMd",[2365, 97.95, -3395] );// Fakel

    CreateSmoker(ctID,"Smoker.Attr.Tower",[2304, 98.5,  -3345] );
    CreateSmoker(ctID,"Smoker.Attr.Tower",[2304, 98.5, -3395] );
    CreateSmoker(ctID,"Smoker.Attr.Tower",[2365, 98.5, -3345] );
    CreateSmoker(ctID,"Smoker.Attr.Tower",[2365, 97.95, -3395] );// Fakel
}
