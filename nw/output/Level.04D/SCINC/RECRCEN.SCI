func void SetRecruitCenter()
 var int ctID;
{
  ctID := s_AddClassTable("RecruitCenter",4);

  CreateRecruitCenter(ctID,"A.Recr0",[2371.099,66.546, -2622.395],"Actek","brief\defaztec.txt");
  CreateRecruitCenter(ctID,"C.Recr0",[2750.203,80.552, -2663.0],"Colony","brief\defcolony.txt");
}
