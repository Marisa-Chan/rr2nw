
func void SetRecruitCenter()
 var int nRecCenterCTID;
{nRecCenterCTID:=s_AddClassTable("RecruitCenter",4);
 CreateRecruitCenter(
  nRecCenterCTID,"Kingdom.Recruit.0",[2639.793,57.110,-2789.498],"Kingdom","brief\defking.txt");
 CreateRecruitCenter(
  nRecCenterCTID,"Magician.Recruit.0",[3109.075,58.008,-2776.970],"Magician","brief\defking.txt");
}
