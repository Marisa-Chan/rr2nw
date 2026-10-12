func void main_CreateFountainAttr()
 var int ctFountAttr;
{
    ctFountAttr := s_AddClassTable("FountainAttr",7);

    CreateVolcanoAttr       (ctFountAttr);
    CreateArabeskAttr       (ctFountAttr);
    CreateFountainAttr      (ctFountAttr);
    CreateFountain2Attr     (ctFountAttr);
    CreateFountainBloodAttr (ctFountAttr);
    CreateFountainB2Attr    (ctFountAttr);
    CreateBlobAttr          (ctFountAttr);
}


