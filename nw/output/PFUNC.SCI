// ------------------------- Project functions ---------------------------
func int p_AddPlayBriefingMsg(int nPrevNode,str szFile, int nMsg)
var int nNode;
{nNode:=s_NewPNode(COM_PLAY_BRIEFING_MSG,s_PNodeNULL(),nPrevNode);
 s_OpenProjectData(nNode);
 s_ProjectWriteStr(nNode,szFile);
 s_ProjectWriteInt(nNode,nMsg);
 s_CloseProjectData(nNode);
 return nNode;
}


func int p_AddPlayBriefing(int nPrevNode,str szFile)
var int nNode;
{nNode:=s_NewPNode(COM_PLAY_BRIEFING,s_PNodeNULL(),nPrevNode);
 s_OpenProjectData(nNode);
 s_ProjectWriteStr(nNode,szFile);
 s_CloseProjectData(nNode);
 return nNode;
}

func int p_AddRunScript(int nPrevNode,str szFile)
var int nNode;
{nNode:=s_NewPNode(COM_RUN_SCRIPT,s_PNodeNULL(),nPrevNode);
 s_OpenProjectData(nNode);
 s_ProjectWriteStr(nNode,szFile);
 s_CloseProjectData(nNode);
 return nNode;
}

func int p_AddMissionInfo(int nPrevNode,int nNumber)
var int nNode;
{nNode:=s_NewPNode(COM_MISSIONINFO,s_PNodeNULL(),nPrevNode);
 s_OpenProjectData(nNode);
 s_ProjectWriteInt(nNode,nNumber);
 s_CloseProjectData(nNode);
 return nNode;
}

func int p_AddSuccessKill(int nPrevNode,str szUnit)
var int nNode;
{nNode:=s_NewPNode(COM_SUCCESS_KILL,s_PNodeNULL(),nPrevNode);
 s_OpenProjectData(nNode);
 s_ProjectWriteStr(nNode,szUnit);
 s_CloseProjectData(nNode);
 return nNode;
}

func int p_AddSuccessLive(int nPrevNode,str szUnit)
var int nNode;
{nNode:=s_NewPNode(COM_SUCCESS_LIVE,s_PNodeNULL(),nPrevNode);
 s_OpenProjectData(nNode);
 s_ProjectWriteStr(nNode,szUnit);
 s_CloseProjectData(nNode);
 return nNode;
}

func int p_AddSuccessReached(int nPrevNode,str szUnit,
                             float fX,float fZ,float fRadius)
var int nNode;
{nNode:=s_NewPNode(COM_SUCCESS_REACHED,s_PNodeNULL(),nPrevNode);
 s_OpenProjectData(nNode);
 s_ProjectWriteStr(nNode,szUnit);
 s_ProjectWriteFloat(nNode,fX);
 s_ProjectWriteFloat(nNode,fZ);
 s_ProjectWriteFloat(nNode,fRadius);
 s_CloseProjectData(nNode);
 return nNode;
} 

func int p_AddFiledKill(int nPrevNode,str szUnit)
var int nNode;
{nNode:=s_NewPNode(COM_FILED_KILL,s_PNodeNULL(),nPrevNode);
 s_OpenProjectData(nNode);
 s_ProjectWriteStr(nNode,szUnit);
 s_CloseProjectData(nNode);
 return nNode;
}

func int p_AddFiledLive(int nPrevNode,str szUnit)
var int nNode;
{nNode:=s_NewPNode(COM_FILED_LIVE,s_PNodeNULL(),nPrevNode);
 s_OpenProjectData(nNode);
 s_ProjectWriteStr(nNode,szUnit);
 s_CloseProjectData(nNode);
 return nNode;
}

func int p_AddFiledReached(int nPrevNode,str szUnit,
                           float fX,float fZ,float fRadius)
var int nNode;
{nNode:=s_NewPNode(COM_FILED_REACHED,s_PNodeNULL(),nPrevNode);
 s_OpenProjectData(nNode);
 s_ProjectWriteStr(nNode,szUnit);
 s_ProjectWriteFloat(nNode,fX);
 s_ProjectWriteFloat(nNode,fZ);
 s_ProjectWriteFloat(nNode,fRadius);
 s_CloseProjectData(nNode);
 return nNode;
} 

func int p_AddSuccessFiled(int nPrevNode)
var int nNode;
{nNode:=s_NewPNode(COM_SUCCESS_FILED,s_PNodeNULL(),nPrevNode);
 return nNode;
}

func int p_AddFiledSuccess(int nPrevNode)
var int nNode;
{nNode:=s_NewPNode(COM_FILED_SUCCESS,s_PNodeNULL(),nPrevNode);
 return nNode;
}

func int p_AddCommander(int nPrevNode,str szCommander)
var int nNode;
{nNode:=s_NewPNode(COM_0COMMANDER,s_PNodeNULL(),nPrevNode);
 s_OpenProjectData(nNode);
 s_ProjectWriteStr(nNode,szCommander);
 s_CloseProjectData(nNode);
 return nNode;
}

func int p_SetMissionSummary(int nPrevNode,str szName, str szSummary, 
			     str szRoute, int RGB, float sw,float ew)
var int nNode;
{nNode:=s_NewPNode(COM_SET_MISSION_SUMMARY,s_PNodeNULL(),nPrevNode);
 s_OpenProjectData  (nNode);
 s_ProjectWriteStr  (nNode,szName);
 s_ProjectWriteStr  (nNode,szSummary);
 s_ProjectWriteStr  (nNode,szRoute);
 s_ProjectWriteInt  (nNode,RGB);
 s_ProjectWriteFloat(nNode,sw);
 s_ProjectWriteFloat(nNode,ew);
 s_CloseProjectData(nNode);
 return nNode;
}

// ---------------------- End of project functions -----------------------
