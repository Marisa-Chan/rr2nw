func void main_CreateBirds()
 var int ctID, oID, cachePos, i;
{

    ctID := s_AddClassTable("Bird" ,550);
    s_SearchObjectID( oID, cachePos, "Bird.Attr.0" );

    CreateSomeBirs(ctID, oID, cachePos, 425,259,430,264,121,  20, 10); //радио на острове
    CreateSomeBirs(ctID, oID, cachePos, 407,293,409,313,115 ,20,  20); //большой город с пристанью
    CreateSomeBirs(ctID, oID, cachePos, 432,336,442,352,68 ,20,   15); //городок около базы подлодок
    CreateSomeBirs(ctID, oID, cachePos, 399,350,410,340,65 ,20,   10); //островок базы подлодок
    CreateSomeBirs(ctID, oID, cachePos, 355,340,367,347,121 ,20,  10); //рощиц
    CreateSomeBirs(ctID, oID, cachePos, 338,366,351,381,126 ,20,  40); //лес
    CreateSomeBirs(ctID, oID, cachePos, 331,320,344,335,90 ,20,   30); //концлагерь
    CreateSomeBirs(ctID, oID, cachePos, 316,320,342,297,119 ,20,  10); //лес и завод у концлагеря
    CreateSomeBirs(ctID, oID, cachePos, 296,333,305,344,165 ,20,  30); //храм ацтеков (2822,3422,2863,3379)
    CreateSomeBirs(ctID, oID, cachePos, 251,336,262,324,125 ,20,   5); //заброшенная ацтексая пирамидка
    CreateSomeBirs(ctID, oID, cachePos, 264,358,271,362,135 ,20,  20); //ацтекская деревня
    CreateSomeBirs(ctID, oID, cachePos, 229,333,238,341,128 ,20,   5); //мужицкая статуя на подставке
    CreateSomeBirs(ctID, oID, cachePos, 193,333,213,316,104 ,20,  10); //ацтекская сборочная фабрика с ангаром
    CreateSomeBirs(ctID, oID, cachePos, 189,375,196,366,163 ,20,  10); //ацтекская здоровая пирамида (верхушка)
    CreateSomeBirs(ctID, oID, cachePos, 248,237,254,228,100 ,20,  20); //место рождения
    CreateSomeBirs(ctID, oID, cachePos, 234,259,240,268,161 ,20,  10); //ацтекский военкомат
    CreateSomeBirs(ctID, oID, cachePos, 271,264,278,272,134 ,20,  10); //военкомат колонизаторов
    CreateSomeBirs(ctID, oID, cachePos, 289,252,298,239,80 ,20,   15); //пограничный гарнизон
    CreateSomeBirs(ctID, oID, cachePos, 333,188,340,183,85 ,20,   10); //лесок на полуострове
    CreateSomeBirs(ctID, oID, cachePos, 266,204,275,210,63 ,20,    5); //малый мост около места рождения (дальняя сторона)
    CreateSomeBirs(ctID, oID, cachePos, 266,226,273,223,64 ,20,    5); //малый мост около места рождения (ближняя сторона)
    CreateSomeBirs(ctID, oID, cachePos, 232,241,241,236,156 ,20,   8); //большой мост (ближняя сторона)
    CreateSomeBirs(ctID, oID, cachePos, 235,216,238,218,157 ,20,   3); //-..- (центр)
    CreateSomeBirs(ctID, oID, cachePos, 233,194,240,192,160 ,20,   5); //-..- (дальняя часть)
    CreateSomeBirs(ctID, oID, cachePos, 197,310,230,271,130 ,20,  70); //разреженный лес (здоровый участок)
    CreateSomeBirs(ctID, oID, cachePos, 136,310,143,320,103 ,20,  10); //база подлодок ацтеков
    CreateSomeBirs(ctID, oID, cachePos, 95,330,104,313,63 ,20,    15); //деревенька ацтеков
    CreateSomeBirs(ctID, oID, cachePos,  78,250, 81,250,167,20,    5); //ацтекский кран на вертолётной фабрике
    CreateSomeBirs(ctID, oID, cachePos, 11,230,140,260,138 ,20,   30); //здоровый ацтекский город
    CreateSomeBirs(ctID, oID, cachePos, 106,198,113,207,300 ,20,  10); //ацтекский храм в горах
    CreateSomeBirs(ctID, oID, cachePos, 156,201,172,212,160 ,20,  12); //ацтекская вертолётная фабрика
    CreateSomeBirs(ctID, oID, cachePos, 50,493,114,452,260 ,20,   50); //   лес в скришотовских горах
    CreateSomeBirs(ctID, oID, cachePos, 85,441, 97,452,226 ,20,   10); //-..-..-..-
    //AddBird( ctID, oID, cachePos,  [2060,70,-2390],[2060+20,70+20,-2390-20] );
}
