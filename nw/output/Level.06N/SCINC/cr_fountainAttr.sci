func void main_CreateFountainAttr()
 var int ctFountAttr;
{
    ctFountAttr := s_AddClassTable("FountainAttr",4);

    CreateVolcanoAttr       (ctFountAttr);
    CreateArabeskAttr       (ctFountAttr);
    CreateFountainAttr      (ctFountAttr);
}


