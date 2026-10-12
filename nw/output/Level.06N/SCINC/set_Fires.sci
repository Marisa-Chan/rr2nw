/* Вулканизация сцены + огненные поля от Кленина А.И. */

func void CreateVolcano(int ctID,vector pos)
{
    //CreateFountain(,"Fount.Attr.Volcano",pos);
    CreateSmoker(ctID, "Smoker.Attr", pos );
    CreateSmoker(ctID, "Smoker.Attr.Fire", pos );
}

func void CreateFireField(int ctID,vector pos)
{
    CreateSmoker(ctID, "Smoker.Attr.FireArea", pos );
}



func void main_CreateFires()
var int ctSmokerID;
{
    ctSmokerID := s_SearchSeanceClassTable("Smoker");
    CreateVolcano(ctSmokerID,[710,135,-2850]);
    CreateVolcano(ctSmokerID,[800,195,-2735]);
    CreateVolcano(ctSmokerID,[1010,198,-2890]);

    CreateVolcano(ctSmokerID,[2120,169,-2200]);
    CreateVolcano(ctSmokerID,[4140,237.5,-4330]);
    CreateVolcano(ctSmokerID,[2930,241.5,-1060]);

    CreateFireField(ctSmokerID,[1415,30,-3560]);
}
