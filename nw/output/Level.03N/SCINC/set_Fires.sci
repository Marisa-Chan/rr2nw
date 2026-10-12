func void CreateVolcano(int ctID,vector pos)
{
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
    CreateVolcano(ctSmokerID,[3410,135,-450]);
    CreateFireField(ctSmokerID,[1848,107.5,-4293]);
}