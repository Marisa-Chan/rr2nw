func void main_CreateFarterAttrs()
var int ctID;
{
   ctID := s_AddClassTable( "FarterAttr", 10);  
   CreateFarterAttr (ctID, "Farter.Attr.Factory", "wav.Factory");
   CreateFarterAttr (ctID, "Farter.Attr.Factory2", "wav.Factory2");
   CreateFarterAttr (ctID, "Farter.Attr.Windmill", "wav.Windmill");
   CreateFarterAttr (ctID, "Farter.Attr.Steam1", "wav.Steam1");
}
