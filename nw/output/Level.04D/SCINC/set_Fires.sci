func void CreateVolcano(int ctID,vector pos)
{
    CreateFountain("LMD","Fount.Attr.Volcano",pos);
    CreateSmoker(ctID, "Smoker.Attr", pos );
    CreateSmoker(ctID, "Smoker.Attr.Fire", pos );
}


func void main_CreateFires()
var int ctSmokerID;
{
 ctSmokerID := s_SearchSeanceClassTable("Smoker");
 CreateVolcano(ctSmokerID,[3010,169,-2771]);
}