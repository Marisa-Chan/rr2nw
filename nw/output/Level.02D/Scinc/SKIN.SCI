	
/**********************************

     Animate dragon anl_02.vbc

 **********************************/

const int dr_RW0    = 0;
const int dr_RW1    = 1;
const int dr_RW2    = 2;
const int dr_RW3    = 3;
const int dr_RW4    = 4;
const int dr_RW5    = 5;
const int dr_TAIL1  = 6;
const int dr_TAIL2  = 7;
const int dr_TAIL3  = 8;
const int dr_HEAD   = 9;
const int dr_ZOB    = 10;
const int dr_LW0    = 11;
const int dr_LW1    = 12;
const int dr_LW2    = 13;
const int dr_LW3    = 14;
const int dr_LW4    = 15;
const int dr_LW5    = 16;
const int dr_TOTAL  = 17;
const int dr_RFOOT  = 18;
const int dr_LFOOT  = 19;

const int drt_RCHEST = 20;
const int drt_LCHEST = 21;
const int drt_BODY   = 22;
const int dr_TAIL4   = 23;
const int dr_NECK    = 24;

func void drt_AnimateHead(int oID, int cachePos, float rsp, float F)
{
    skin_SetAnimProg_LOADIDENTITY (oID, cachePos, dr_HEAD);
    skin_SetAnimProg_ROTATEOZC    (oID, cachePos, dr_HEAD, [0,0,0], 5~, rsp*5/7, 87~+F);
    skin_SetAnimProg_ROTATEOXC    (oID, cachePos, dr_HEAD, [-0.78,1.8,0], 5~, rsp,  90~+F);
    skin_SetAnimProg_ROTATEOXC    (oID, cachePos, dr_HEAD, [-0.78,1.8,0], 5~, rsp*0.7, 90~+F);

    skin_SetAnimProg_LOADIDENTITY (oID, cachePos, dr_ZOB);
    skin_SetAnimProg_MOVE(oID, cachePos, dr_ZOB, [0,0,0], [0,1,0], 0.07, rsp*3, 0);

    skin_SetAnimProg_LOADIDENTITY (oID, cachePos, dr_NECK);
    skin_SetAnimProg_ROTATEOZC    (oID, cachePos, dr_NECK, [0,0,0], 3~, rsp*5/7, 87~+F);
    skin_SetAnimProg_ROTATEOXC    (oID, cachePos, dr_NECK, [-0.78,1.8,0], 3~, rsp,  90~+F);
    skin_SetAnimProg_ROTATEOXC    (oID, cachePos, dr_NECK, [-0.78,1.8,0], 3~, rsp*0.7, 90~+F);
   
}

func void drt_AnimateWing(int oID, int cachePos,int num, 
                         vector ax, vector ax1,float A, float A1,float rsp, float F)
{
    //skin_SetAnimProg_LOADIDENTITY (oID, cachePos, num+dr_RW0);
    //skin_SetAnimProg_ROTATEOYC    (oID, cachePos, num+dr_RW0, ax,  A, rsp, F);
    //skin_SetAnimProg_ROTATEOZC    (oID, cachePos, num+dr_RW0, ax1, A1, rsp, F);

    //skin_SetAnimProg_LOADIDENTITY (oID, cachePos, num+dr_RW1);
    //skin_SetAnimProg_ROTATEOYC    (oID, cachePos, num+dr_RW1, ax,  A, rsp, F-20~);
    //skin_SetAnimProg_ROTATEOZC    (oID, cachePos, num+dr_RW1, ax1, A1, rsp, F-20~-15~);

    skin_SetAnimProg_LOADIDENTITY (oID, cachePos, num+dr_RW2);
    skin_SetAnimProg_ROTATEOYC    (oID, cachePos, num+dr_RW2, ax,  A, rsp, F-10~);
    skin_SetAnimProg_ROTATEOZC    (oID, cachePos, num+dr_RW2, ax1, A1, rsp, F-10~);

    skin_SetAnimProg_LOADIDENTITY (oID, cachePos, num+dr_RW3);
    skin_SetAnimProg_ROTATEOYC    (oID, cachePos, num+dr_RW3, ax,  A, rsp, F-30~);
    skin_SetAnimProg_ROTATEOZC    (oID, cachePos, num+dr_RW3, ax1, A1, rsp, F-30~-25~);

    skin_SetAnimProg_LOADIDENTITY (oID, cachePos, num+dr_RW4);
    skin_SetAnimProg_ROTATEOYC    (oID, cachePos, num+dr_RW4, ax,  A, rsp, F-40~);
    skin_SetAnimProg_ROTATEOZC    (oID, cachePos, num+dr_RW4, ax1, A1, rsp, F-40~-15~);

    skin_SetAnimProg_LOADIDENTITY (oID, cachePos, num+dr_RW5);
    skin_SetAnimProg_ROTATEOYC    (oID, cachePos, num+dr_RW5, ax,  A*0.5,      rsp, F-30~);
    skin_SetAnimProg_ROTATEOZC    (oID, cachePos, num+dr_RW5, ax1*1.2, A1*0.5, rsp, F-30~);

}

func void CreateAnimation_anl_02t()
 var int oID, cachePos;
 var float rsp, ox, oy;
{
    CreateAnimSetsAuto(oID,cachePos, "sk.taxi.anl_02", 0, 25, 100 );
    //SetAnimateBlock(oID,cachePos,0,  dr_RW0,     "RightWing0");
    //SetAnimateBlock(oID,cachePos,0,  dr_RW1,     "RightWing1");
    SetAnimateBlock(oID,cachePos,0,  dr_RW2,     "RightWing2");
    SetAnimateBlock(oID,cachePos,0,  dr_RW3,     "RightWing3");
    SetAnimateBlock(oID,cachePos,0,  dr_RW4,     "RightWing4");
    SetAnimateBlock(oID,cachePos,0,  dr_RW5,     "RightWing5");
    SetAnimateBlock(oID,cachePos,0,  dr_TAIL1,   "Tail1"    );
    SetAnimateBlock(oID,cachePos,0,  dr_TAIL2,   "Tail2"    );
    SetAnimateBlock(oID,cachePos,0,  dr_TAIL3,   "Tail3"    );
    SetAnimateBlock(oID,cachePos,0,  dr_TAIL4,   "Tail4"    );
    SetAnimateBlock(oID,cachePos,0,  dr_HEAD,    "Head"     );
    SetAnimateBlock(oID,cachePos,0,  dr_ZOB,     "Zob"      );
    SetAnimateBlock(oID,cachePos,0,  dr_NECK,    "Neck"     );
   // SetAnimateBlock(oID,cachePos,0,  dr_LW0,     "LeftWing0");
   // SetAnimateBlock(oID,cachePos,0,  dr_LW1,     "LeftWing1");
    SetAnimateBlock(oID,cachePos,0,  dr_LW2,     "LeftWing2");
    SetAnimateBlock(oID,cachePos,0,  dr_LW3,     "LeftWing3");
    SetAnimateBlock(oID,cachePos,0,  dr_LW4,     "LeftWing4");
    SetAnimateBlock(oID,cachePos,0,  dr_LW5,     "LeftWing5");
    SetAnimateBlock(oID,cachePos,0,  dr_TOTAL,   "Total"    );
   // SetAnimateBlock(oID,cachePos,0,  dr_RFOOT,   "RightFoot");
   // SetAnimateBlock(oID,cachePos,0,  dr_LFOOT,   "LeftFoot" );
    SetAnimateBlock(oID,cachePos,0,  drt_RCHEST, "RightChest");
    SetAnimateBlock(oID,cachePos,0,  drt_LCHEST, "LeftChest" );
    SetAnimateBlock(oID,cachePos,0,  drt_BODY,   "Body" );

    rsp := 3;
    ox := 10*0.2;
    oy := 14*0.2;

//==================Body animation==========================

    skin_SetAnimProg_LOADIDENTITY (oID, cachePos, drt_BODY);
    skin_SetAnimProg_MOVE   	  (oID, cachePos, drt_BODY, [0,0.25,0], [0,1,0], 0.25, rsp/2, 0);

    skin_SetAnimProg_LOADIDENTITY (oID, cachePos, drt_RCHEST);
    skin_SetAnimProg_MOVE   	  (oID, cachePos, drt_RCHEST, [0,0.25,0], [0,1,0], 0.25, rsp/2, 0);
    skin_SetAnimProg_MOVE   	  (oID, cachePos, drt_RCHEST, [0.1,0,-0.1], [1,0,-1.5], 0.1, rsp/2, 0);

    skin_SetAnimProg_LOADIDENTITY (oID, cachePos, drt_LCHEST);
    skin_SetAnimProg_MOVE   	  (oID, cachePos, drt_LCHEST, [0,0.25,0], [0,1,0], 0.25, rsp/2, 0);
    skin_SetAnimProg_MOVE   	  (oID, cachePos, drt_LCHEST, [-0.1,0,-0.1], [-1,0,-1.5], 0.1, rsp/2, 0);

//==================Tail animation==========================

    skin_SetAnimProg_LOADIDENTITY (oID, cachePos, dr_TAIL1);
    skin_SetAnimProg_ROTATEOYC    (oID, cachePos, dr_TAIL1, [0.0 ,1.2,6.62], 0.25/2, oy,  0);

    skin_SetAnimProg_LOADIDENTITY (oID, cachePos, dr_TAIL2);
    skin_SetAnimProg_ROTATEOYC    (oID, cachePos, dr_TAIL2, [0.0 ,1.2,6.62], 0.4/2, oy, -20~);

    skin_SetAnimProg_LOADIDENTITY (oID, cachePos, dr_TAIL3);
    skin_SetAnimProg_ROTATEOYC    (oID, cachePos, dr_TAIL3, [0.0 ,1.2,6.62], 0.48/2, oy, -45~);

    skin_SetAnimProg_LOADIDENTITY (oID, cachePos, dr_TAIL4);
    skin_SetAnimProg_ROTATEOYC    (oID, cachePos, dr_TAIL4, [0.0 ,1.2,6.62], 0.48/2, oy, -75~);

//==================Head animation==========================

   // drt_AnimateHead(oID, cachePos, rsp/3, 270~);
    
    skin_SetAnimProg_LOADIDENTITY (oID, cachePos, dr_HEAD);
    skin_SetAnimProg_ROTATEOZC    (oID, cachePos, dr_HEAD, [0,0,0], 5~, rsp*5/7, 87~);
    skin_SetAnimProg_ROTATEOXC    (oID, cachePos, dr_HEAD, [-0.78,1.8,0], 5~, rsp,  90~);
    skin_SetAnimProg_ROTATEOXC    (oID, cachePos, dr_HEAD, [-0.78,1.8,0], 5~, rsp*0.7, 90~);
    skin_SetAnimProg_ROTATEOYC    (oID, cachePos, dr_HEAD, [0,0,-2], 7~, rsp*2.0, 0);
    skin_SetAnimProg_ROTATEOYC    (oID, cachePos, dr_HEAD, [0,0,-2], 7~, rsp*2.2, 0);

    skin_SetAnimProg_LOADIDENTITY (oID, cachePos, dr_ZOB);
    skin_SetAnimProg_MOVE(oID, cachePos, dr_ZOB, [0,0,0], [0,1,0], 0.07, rsp/2, 0);

    skin_SetAnimProg_LOADIDENTITY (oID, cachePos, dr_NECK);
    skin_SetAnimProg_ROTATEOZC    (oID, cachePos, dr_NECK, [0,0,0], 3~, rsp*5/7, 87~);
    skin_SetAnimProg_ROTATEOXC    (oID, cachePos, dr_NECK, [-0.78,1.8,0], 3~, rsp, 90~);
    skin_SetAnimProg_ROTATEOXC    (oID, cachePos, dr_NECK, [-0.78,1.8,0], 3~, rsp*0.7, 90~);

//=========================================================
    
    

    drt_AnimateWing(oID, cachePos, dr_RW0, [ 0.92,1.37,-3],[0,0,0], 0.1, 0.1, rsp*0.4, 0);
    drt_AnimateWing(oID, cachePos, dr_LW0, [-0.92,1.37,-3],[0,0,0],-0.1,-0.1, rsp*0.4, 0);

//    skin_SetAnimProg_LOADIDENTITY (oID, cachePos, drt_BODY);
//    skin_SetAnimProg_ROTATEOXC    (oID, cachePos, drt_BODY, [0.92,1.2,8.62], 4~, rsp*0.2, 90~);

    skin_SetAnimProg_UPDATE       (oID, cachePos, dr_TOTAL);
}




func void dr_AnimateWing(int oID, int cachePos,int num, 
                         vector ax, vector ax1,float A, float A1,float rsp, float F)
{
    skin_SetAnimProg_LOADIDENTITY (oID, cachePos, num+dr_RW0);
    skin_SetAnimProg_ROTATEOYC    (oID, cachePos, num+dr_RW0, ax,  A, rsp, F);
    skin_SetAnimProg_ROTATEOZC    (oID, cachePos, num+dr_RW0, ax1, A1, rsp, F);

    skin_SetAnimProg_LOADIDENTITY (oID, cachePos, num+dr_RW1);
    skin_SetAnimProg_ROTATEOYC    (oID, cachePos, num+dr_RW1, ax,  A, rsp, F-10~);
    skin_SetAnimProg_ROTATEOZC    (oID, cachePos, num+dr_RW1, ax1, A1, rsp, F-10~-15~);

    skin_SetAnimProg_LOADIDENTITY (oID, cachePos, num+dr_RW2);
    skin_SetAnimProg_ROTATEOYC    (oID, cachePos, num+dr_RW2, ax,  A, rsp, F-5~);
    skin_SetAnimProg_ROTATEOZC    (oID, cachePos, num+dr_RW2, ax1, A1, rsp, F-5~);

    skin_SetAnimProg_LOADIDENTITY (oID, cachePos, num+dr_RW3);
    skin_SetAnimProg_ROTATEOYC    (oID, cachePos, num+dr_RW3, ax,  A, rsp, F-15~);
    skin_SetAnimProg_ROTATEOZC    (oID, cachePos, num+dr_RW3, ax1, A1, rsp, F-15~-25~);

    skin_SetAnimProg_LOADIDENTITY (oID, cachePos, num+dr_RW4);
    skin_SetAnimProg_ROTATEOYC    (oID, cachePos, num+dr_RW4, ax,  A, rsp, F-20~);
    skin_SetAnimProg_ROTATEOZC    (oID, cachePos, num+dr_RW4, ax1, A1, rsp, F-20~-15~);

    skin_SetAnimProg_LOADIDENTITY (oID, cachePos, num+dr_RW5);
    skin_SetAnimProg_ROTATEOYC    (oID, cachePos, num+dr_RW5, ax,  A,      rsp, F-15~);
    skin_SetAnimProg_ROTATEOZC    (oID, cachePos, num+dr_RW5, ax1*1.2, A1, rsp, F-15~);

}

func void CreateAnimation_anl_02()
 var int   oID, cachePos;
 var float rsp, ox,oy;
{
    CreateAnimSetsAuto(oID,cachePos, "sk.peop.anl_02", 0, 20, 60 );
    SetAnimateBlock(oID,cachePos,0,  dr_RW0,     "RightWing0");
    SetAnimateBlock(oID,cachePos,0,  dr_RW1,     "RightWing1");
    SetAnimateBlock(oID,cachePos,0,  dr_RW2,     "RightWing2");
    SetAnimateBlock(oID,cachePos,0,  dr_RW3,     "RightWing3");
    SetAnimateBlock(oID,cachePos,0,  dr_RW4,     "RightWing4");
    SetAnimateBlock(oID,cachePos,0,  dr_RW5,     "RightWing5");
    SetAnimateBlock(oID,cachePos,0,  dr_TAIL1,   "Tail1"    );
    SetAnimateBlock(oID,cachePos,0,  dr_TAIL2,   "Tail2"    );
    SetAnimateBlock(oID,cachePos,0,  dr_TAIL3,   "Tail3"    );
    SetAnimateBlock(oID,cachePos,0,  dr_HEAD,    "Head"     );
    SetAnimateBlock(oID,cachePos,0,  dr_ZOB,     "Zob"      );
    SetAnimateBlock(oID,cachePos,0,  dr_LW0,     "LeftWing0");
    SetAnimateBlock(oID,cachePos,0,  dr_LW1,     "LeftWing1");
    SetAnimateBlock(oID,cachePos,0,  dr_LW2,     "LeftWing2");
    SetAnimateBlock(oID,cachePos,0,  dr_LW3,     "LeftWing3");
    SetAnimateBlock(oID,cachePos,0,  dr_LW4,     "LeftWing4");
    SetAnimateBlock(oID,cachePos,0,  dr_LW5,     "LeftWing5");
    SetAnimateBlock(oID,cachePos,0,  dr_TOTAL,   "Total"    );
    SetAnimateBlock(oID,cachePos,0,  dr_RFOOT,   "RightFoot");
    SetAnimateBlock(oID,cachePos,0,  dr_LFOOT,   "LeftFoot" );

    //-------------------------------------------------------
    rsp := 5.5;
    ox := 10*0.6;
    oy := 14*0.6;
    skin_SetAnimProg_LOADIDENTITY (oID, cachePos, dr_TAIL1);
    skin_SetAnimProg_ROTATEOXC    (oID, cachePos, dr_TAIL1, [0.92,1.2,6.62], 0.25/3, ox, 0);
    skin_SetAnimProg_ROTATEOYC    (oID, cachePos, dr_TAIL1, [0.0 ,1.2,6.62], 0.25,   oy,  0);

    skin_SetAnimProg_LOADIDENTITY (oID, cachePos, dr_TAIL2);
    skin_SetAnimProg_ROTATEOXC    (oID, cachePos, dr_TAIL2, [0.92,1.2,6.62], 0.4/3, ox, -20~);
    skin_SetAnimProg_ROTATEOYC    (oID, cachePos, dr_TAIL2, [0.0 ,1.2,6.62], 0.4,   oy, -20~);

    skin_SetAnimProg_LOADIDENTITY (oID, cachePos, dr_TAIL3);
    skin_SetAnimProg_ROTATEOXC    (oID, cachePos, dr_TAIL3, [0.92,1.2,6.62], 0.48/3, ox, -55~);
    skin_SetAnimProg_ROTATEOYC    (oID, cachePos, dr_TAIL3, [0.0 ,1.2,6.62], 0.48,   oy, -55~);

    dr_AnimateWing(oID, cachePos, dr_RW0, [ 0.92,1.37,-3],[0,0,0], 0.8, 0.9, rsp, 0);
    dr_AnimateWing(oID, cachePos, dr_LW0, [-0.92,1.37,-3],[0,0,0],-0.8,-0.9, rsp, 0);

    skin_SetAnimProg_LOADIDENTITY (oID, cachePos, dr_ZOB);
    skin_SetAnimProg_ROTATEOXC    (oID, cachePos, dr_ZOB, [-0.78,1.8,0], 20~, rsp, 90~);
    skin_SetAnimProg_MOVE         (oID, cachePos, dr_ZOB, [0,0,0], [0,1,0], 0.2, rsp*2.7, 45~);

    skin_SetAnimProg_LOADIDENTITY (oID, cachePos, dr_HEAD);
    skin_SetAnimProg_ROTATEOZC    (oID, cachePos, dr_HEAD, [0,0,0], 30~, rsp*5/7, 87~);
    skin_SetAnimProg_ROTATEOXC    (oID, cachePos, dr_HEAD, [-0.78,1.8,0], 20~, rsp, 90~);


    skin_SetAnimProg_LOADIDENTITY (oID, cachePos, dr_RFOOT);
    skin_SetAnimProg_MOVE         (oID, cachePos, dr_RFOOT, [0,0,0], [0,1,0], 0.8, rsp*2.7, 45~);

    skin_SetAnimProg_LOADIDENTITY (oID, cachePos, dr_LFOOT);
    skin_SetAnimProg_MOVE         (oID, cachePos, dr_LFOOT, [0,0,0], [0,1,0], 0.8, rsp*2.7, 30~);

    skin_SetAnimProg_UPDATE       (oID, cachePos, dr_TOTAL);
}


const int kg_TAIL1 = 4;
const int kg_TAIL2 = 5;
const int kg_LEFT  = 6;
const int kg_LEFT1 = 7;
const int kg_LEFT2 = 8;
const int kg_HEAD  = 9;
const int kg_RSIDE = 10;
const int kg_LSIDE = 11;

func void kg_AnimateFoot(int oID, int cachePos, int num, vector ax0, float F)
 var float rsp;
{
    rsp := 12;
    skin_SetAnimProg_LOADIDENTITY (oID, cachePos, num);
    skin_SetAnimProg_ROTATEOXC    (oID, cachePos, num, ax0, 0.7, rsp, F);

    skin_SetAnimProg_LOADIDENTITY (oID, cachePos, num+1);
    skin_SetAnimProg_ROTATEOXC    (oID, cachePos, num+1, [1.4,1.3,0.4], 0.7, rsp, F-45~);
    skin_SetAnimProg_ROTATEOXC    (oID, cachePos, num+1, ax0, 0.7, rsp, F);

    skin_SetAnimProg_LOADIDENTITY (oID, cachePos, num+2);
    skin_SetAnimProg_ROTATEOXC    (oID, cachePos, num+2, [1.3,0.8,1.0], 0.8, rsp, F-180~);
    skin_SetAnimProg_ROTATEOXC    (oID, cachePos, num+2, ax0, 0.7, rsp, F);
}

func void CreateAnimation_anl_00()
 var vector ax0;
 var float  rsp, phase;
 var int oID, cachePos;
{
    CreateAnimSetsAuto(oID,cachePos, "sk.tank.anl_00", 0, 12, 28 );
    SetAnimateBlock(oID,cachePos,0,  0,        "Right");
    SetAnimateBlock(oID,cachePos,0,  1,        "Right1");
    SetAnimateBlock(oID,cachePos,0,  2,        "Right2");
    SetAnimateBlock(oID,cachePos,0,  3,        "Total");
    SetAnimateBlock(oID,cachePos,0,  kg_TAIL1, "Tail1");
    SetAnimateBlock(oID,cachePos,0,  kg_TAIL2, "Tail2");
    SetAnimateBlock(oID,cachePos,0,  kg_LEFT,  "Left");
    SetAnimateBlock(oID,cachePos,0,  kg_LEFT1, "Left1");
    SetAnimateBlock(oID,cachePos,0,  kg_LEFT2, "Left2");
    SetAnimateBlock(oID,cachePos,0,  kg_HEAD,  "Head");
    SetAnimateBlock(oID,cachePos,0,  kg_RSIDE, "RSide");
    SetAnimateBlock(oID,cachePos,0,  kg_LSIDE, "LSide");

    ax0 := [1.5,2.1,0.77];
    rsp := 12;
    phase := 180~;

    kg_AnimateFoot(oID, cachePos, 0,       ax0, 0~);
    kg_AnimateFoot(oID, cachePos, kg_LEFT, ax0, 180~);
    skin_SetAnimProg_LOADIDENTITY (oID, cachePos, kg_TAIL1);
    skin_SetAnimProg_ROTATEOYC    (oID, cachePos, kg_TAIL1, [0,0,0], 0.2, rsp, 0);

    skin_SetAnimProg_LOADIDENTITY (oID, cachePos, kg_TAIL2);
    skin_SetAnimProg_ROTATEOYC    (oID, cachePos, kg_TAIL2, [0,0,0], 0.25,rsp, -60~);

    skin_SetAnimProg_LOADIDENTITY (oID, cachePos, kg_HEAD);
    skin_SetAnimProg_MOVE         (oID, cachePos, kg_HEAD, [0,0,0], [0,1,0], 0.2, rsp, 90~);

    skin_SetAnimProg_LOADIDENTITY (oID, cachePos, kg_RSIDE);
    skin_SetAnimProg_MOVE         (oID, cachePos, kg_RSIDE, [0,0,0], [0,1,0], 0.1, rsp, 0~);

    skin_SetAnimProg_LOADIDENTITY (oID, cachePos, kg_LSIDE);
    skin_SetAnimProg_MOVE         (oID, cachePos, kg_LSIDE, [0,0,0], [0,1,0], 0.1, rsp, 0~-180~);

    skin_SetAnimProg_UPDATE       (oID, cachePos, 3);
}

func void CreateAnimation_Bowman()
 var vector ax0;
 var vector ax1;
 var float  rsp;
 var float  amp;	
 var int oID, cachePos;
{
    CreateAnimSetsAuto(oID,cachePos, "sk.peop.Bowman", 0, 7, 21 );
    SetAnimateBlock(oID,cachePos,0,  0,        "RLeg");
    SetAnimateBlock(oID,cachePos,0,  1,        "RFoot");
    SetAnimateBlock(oID,cachePos,0,  2,        "LLeg");
    SetAnimateBlock(oID,cachePos,0,  3,        "LFoot");
    SetAnimateBlock(oID,cachePos,0,  4,	       "Body");
    SetAnimateBlock(oID,cachePos,0,  5,	       "Waist");
    SetAnimateBlock(oID,cachePos,0,  6,	       "Total");

    ax0 := [0.0,1.1,0.04];
    ax1 := [0.0,0.6,-0.03];
    rsp := 10;
    amp := 5~;

    skin_SetAnimProg_LOADIDENTITY (oID, cachePos, 0);
    skin_SetAnimProg_MOVE	  (oID, cachePos, 0, [0,0,0], [0,1,0], amp, rsp*2, 90~); 
    skin_SetAnimProg_ROTATEOXC    (oID, cachePos, 0, ax0, 15~, rsp, 180~);

    skin_SetAnimProg_LOADIDENTITY (oID, cachePos, 1);
    skin_SetAnimProg_MOVE	  (oID, cachePos, 1, [0,0,0], [0,1,0], amp, rsp*2, 90~); 
    skin_SetAnimProg_ROTATEOXC    (oID, cachePos, 1, ax1, 15~, rsp, 180~);
    skin_SetAnimProg_ROTATEOXC    (oID, cachePos, 1, ax0, 15~, rsp, 180~);

    skin_SetAnimProg_LOADIDENTITY (oID, cachePos, 2);
    skin_SetAnimProg_MOVE	  (oID, cachePos, 2, [0,0,0], [0,1,0], amp, rsp*2, 90~); 
    skin_SetAnimProg_ROTATEOXC    (oID, cachePos, 2, ax0, 15~, rsp, 0);

    skin_SetAnimProg_LOADIDENTITY (oID, cachePos, 3);
    skin_SetAnimProg_MOVE	  (oID, cachePos, 3, [0,0,0], [0,1,0], amp, rsp*2, 90~); 
    skin_SetAnimProg_ROTATEOXC    (oID, cachePos, 3, ax1, 15~, rsp, 0);
    skin_SetAnimProg_ROTATEOXC    (oID, cachePos, 3, ax0, 15~, rsp, 0);

    skin_SetAnimProg_LOADIDENTITY (oID, cachePos, 4);
    skin_SetAnimProg_MOVE	  (oID, cachePos, 4, [0,0,0], [0,1,0], amp, rsp*2, 90~);
    skin_SetAnimProg_ROTATEOXC    (oID, cachePos, 4, ax0, 4~, rsp*2, 180~);
    skin_SetAnimProg_ROTATEOZC    (oID, cachePos, 4, [0,1,0], 4~, rsp*2, 270~);

    skin_SetAnimProg_LOADIDENTITY (oID, cachePos, 5);
    skin_SetAnimProg_MOVE	  (oID, cachePos, 5, [0,0,0], [0,1,0], amp, rsp*2, 90~); 

    skin_SetAnimProg_UPDATE       (oID, cachePos, 6);
}

func void CreateAnimation_Helic()
 var float  rsp;
 var vector ax0;
 var int oID, cachePos;
{
    CreateAnimSetsAuto(oID,cachePos, "sk.peop.vhl_hel", 0, 3, 8);
    SetAnimateBlock(oID,cachePos,0,  0, "Propeller");
    SetAnimateBlock(oID,cachePos,0,  1, "Body");
    SetAnimateBlock(oID,cachePos,0,  2, "Total");

    ax0 := [0.0,2.0,0.0];
    rsp := 10;

    skin_SetAnimProg_LOADIDENTITY (oID, cachePos, 0);
    skin_SetAnimProg_ROTATEOY     (oID, cachePos, 0, [0,0,0], rsp, 0);
    skin_SetAnimProg_ROTATEOXC    (oID, cachePos, 0, ax0, 3~, rsp, 0);
    skin_SetAnimProg_ROTATEOZC    (oID, cachePos, 0, ax0, 3~, rsp, 90~);
    
    skin_SetAnimProg_LOADIDENTITY (oID, cachePos, 1);
    skin_SetAnimProg_ROTATEOXC    (oID, cachePos, 1, ax0, 3~, rsp, 0);
    skin_SetAnimProg_ROTATEOZC    (oID, cachePos, 1, ax0, 3~, rsp, 90~);

    skin_SetAnimProg_UPDATE       (oID, cachePos, 2);
}

func void CreateAnimation_Mill()
 var float  rsp;
 var int oID, cachePos;
{
    CreateAnimSetsAuto(oID,cachePos, "sk.peop.vhl_ml", 0, 6, 21);
    SetAnimateBlock(oID,cachePos,0,  0, "Propeller");
    SetAnimateBlock(oID,cachePos,0,  1, "Most");
    SetAnimateBlock(oID,cachePos,0,  2, "FWheels");
    SetAnimateBlock(oID,cachePos,0,  3, "BWheels");
    SetAnimateBlock(oID,cachePos,0,  4, "Body");
    SetAnimateBlock(oID,cachePos,0,  5, "Total");

    rsp := 10;

    skin_SetAnimProg_LOADIDENTITY (oID, cachePos, 0);
    skin_SetAnimProg_ROTATEOZ     (oID, cachePos, 0, [0,5.8,0], 3 * rsp, 0);
    skin_SetAnimProg_ROTATEOZC    (oID, cachePos, 0, [0,1,0], 1~, rsp, 0);
    skin_SetAnimProg_ROTATEOXC    (oID, cachePos, 0, [0,1,0], 1~, rsp, 90~);

    skin_SetAnimProg_LOADIDENTITY (oID, cachePos, 1);
    skin_SetAnimProg_ROTATEOY     (oID, cachePos, 1, [0,0,0], rsp, 0);
    skin_SetAnimProg_ROTATEOZC    (oID, cachePos, 1, [0,1,0], 1~, rsp, 0);
    skin_SetAnimProg_ROTATEOXC    (oID, cachePos, 1, [0,1,0], 1~, rsp, 90~);

    skin_SetAnimProg_LOADIDENTITY (oID, cachePos, 2);
    skin_SetAnimProg_ROTATEOX     (oID, cachePos, 2, [0,1,-2.14], -0.5 * rsp, 0);
    skin_SetAnimProg_ROTATEOZC    (oID, cachePos, 2, [0,1,0], 1~, rsp, 0);
    skin_SetAnimProg_ROTATEOXC    (oID, cachePos, 2, [0,1,0], 1~, rsp, 90~);

    skin_SetAnimProg_LOADIDENTITY (oID, cachePos, 3);
    skin_SetAnimProg_ROTATEOX     (oID, cachePos, 3, [0,1,2.14], -0.5 * rsp, 0);
    skin_SetAnimProg_ROTATEOZC    (oID, cachePos, 3, [0,1,0], 1~, rsp, 0);
    skin_SetAnimProg_ROTATEOXC    (oID, cachePos, 3, [0,1,0], 1~, rsp, 90~);

    skin_SetAnimProg_LOADIDENTITY (oID, cachePos, 4);
    skin_SetAnimProg_MOVE	  (oID, cachePos, 4, [0,0,0], [0,1,0], 3~, 2 * rsp, 0);
    skin_SetAnimProg_ROTATEOZC    (oID, cachePos, 4, [0,1,0], 1~, rsp, 0);
    skin_SetAnimProg_ROTATEOXC    (oID, cachePos, 4, [0,1,0], 1~, rsp, 90~);

    skin_SetAnimProg_UPDATE       (oID, cachePos, 5);
}

func void CreateAnimation_Wagon()
 var float  rsp;
 var int oID, cachePos;
{
    CreateAnimSetsAuto(oID,cachePos, "sk.peop.vhl_wag", 0, 5, 21);
    SetAnimateBlock(oID,cachePos,0,  0, "Wheels");
    SetAnimateBlock(oID,cachePos,0,  1, "Wagon");
    SetAnimateBlock(oID,cachePos,0,  2, "Head");
    SetAnimateBlock(oID,cachePos,0,  3, "Legs");
    SetAnimateBlock(oID,cachePos,0,  4, "Total");

    rsp := 6;

    skin_SetAnimProg_LOADIDENTITY (oID, cachePos, 0);
    skin_SetAnimProg_ROTATEOX     (oID, cachePos, 0, [0,0.75,5.5], -0.5 * rsp, 0);
    skin_SetAnimProg_ROTATEOXC    (oID, cachePos, 0, [0,1.5,-0.5], 1~, rsp, 0);
    skin_SetAnimProg_ROTATEOZC    (oID, cachePos, 0, [0,0.75,0], 1.5~, 0.5 * rsp, 90~);

    skin_SetAnimProg_LOADIDENTITY (oID, cachePos, 1);
    skin_SetAnimProg_ROTATEOXC    (oID, cachePos, 1, [0,1.5,-0.5], 1~, rsp, 0);
    skin_SetAnimProg_ROTATEOZC    (oID, cachePos, 1, [0,0.75,0], 1.5~, 0.5 * rsp, 90~);

    skin_SetAnimProg_LOADIDENTITY (oID, cachePos, 2);
    skin_SetAnimProg_ROTATEOXC    (oID, cachePos, 2, [0,1.5,-0.5], 5~, rsp, 0);

    skin_SetAnimProg_LOADIDENTITY (oID, cachePos, 3);
    skin_SetAnimProg_MOVE	  (oID, cachePos, 3, [0,0,0], [1,0,0], 2~, rsp, 0);
    skin_SetAnimProg_MOVE	  (oID, cachePos, 3, [0,0,0], [0,0,1], 4~, rsp, 90~);

    skin_SetAnimProg_UPDATE       (oID, cachePos, 4);
}

func void CreateAnimation_Ship()
 var float  rsp;
 var int oID, cachePos;
{
    CreateAnimSetsAuto(oID,cachePos, "sk.peop.vhl_gal", 0, 11, 28);
    SetAnimateBlock(oID,cachePos,0,  0, "Gun0");
    SetAnimateBlock(oID,cachePos,0,  1, "Gun1");
    SetAnimateBlock(oID,cachePos,0,  2, "Gun2");
    SetAnimateBlock(oID,cachePos,0,  3, "LOar1");
    SetAnimateBlock(oID,cachePos,0,  4, "LOar2");
    SetAnimateBlock(oID,cachePos,0,  5, "LOar3");
    SetAnimateBlock(oID,cachePos,0,  6, "ROar1");
    SetAnimateBlock(oID,cachePos,0,  7, "ROar2");
    SetAnimateBlock(oID,cachePos,0,  8, "ROar3");
    SetAnimateBlock(oID,cachePos,0,  9, "Flag");
    SetAnimateBlock(oID,cachePos,0,  10, "Total");

    rsp := 2;

    skin_SetAnimProg_LOADIDENTITY (oID, cachePos, 0);
    skin_SetAnimProg_ROTATEOY     (oID, cachePos, 0, [0,0,-18], 0.5 * rsp, 0);
    
    skin_SetAnimProg_LOADIDENTITY (oID, cachePos, 1);
    skin_SetAnimProg_ROTATEOY     (oID, cachePos, 1, [0,0,6], 0.5 * rsp, 0);

    skin_SetAnimProg_LOADIDENTITY (oID, cachePos, 2);
    skin_SetAnimProg_ROTATEOY     (oID, cachePos, 2, [0,0,27.4], 0.5 * rsp, 0);

    skin_SetAnimProg_LOADIDENTITY (oID, cachePos, 3);
    skin_SetAnimProg_ROTATEOZC    (oID, cachePos, 3, [-8,5.5,-7], 30~,rsp, 90~);
    skin_SetAnimProg_ROTATEOYC    (oID, cachePos, 3, [-8,5.5,-7], 50~,rsp, 0);

    skin_SetAnimProg_LOADIDENTITY (oID, cachePos, 4);
    skin_SetAnimProg_ROTATEOZC    (oID, cachePos, 4, [-8,5.5,0], 30~,rsp, 90~);
    skin_SetAnimProg_ROTATEOYC    (oID, cachePos, 4, [-8,5.5,0], 50~,rsp, 0);

    skin_SetAnimProg_LOADIDENTITY (oID, cachePos, 5);
    skin_SetAnimProg_ROTATEOZC    (oID, cachePos, 5, [-8,5.5,7], 30~,rsp, 90~);
    skin_SetAnimProg_ROTATEOYC    (oID, cachePos, 5, [-8,5.5,7], 50~,rsp, 0);

    skin_SetAnimProg_LOADIDENTITY (oID, cachePos, 6);
    skin_SetAnimProg_ROTATEOZC    (oID, cachePos, 6, [8,5.5,-7], 30~,rsp, 270~);
    skin_SetAnimProg_ROTATEOYC    (oID, cachePos, 6, [8,5.5,-7], 50~,rsp, 180~);

    skin_SetAnimProg_LOADIDENTITY (oID, cachePos, 7);
    skin_SetAnimProg_ROTATEOZC    (oID, cachePos, 7, [8,5.5,0], 30~,rsp, 270~);
    skin_SetAnimProg_ROTATEOYC    (oID, cachePos, 7, [8,5.5,0], 50~,rsp, 180~);

    skin_SetAnimProg_LOADIDENTITY (oID, cachePos, 8);
    skin_SetAnimProg_ROTATEOZC    (oID, cachePos, 8, [8,5.5,7], 30~,rsp, 270~);
    skin_SetAnimProg_ROTATEOYC    (oID, cachePos, 8, [8,5.5,7], 50~,rsp, 180~);

    skin_SetAnimProg_LOADIDENTITY (oID, cachePos, 9);
    skin_SetAnimProg_ROTATEOXC    (oID, cachePos, 9, [0,20,39], 10~,10 * rsp, 0);
    skin_SetAnimProg_MOVE	  (oID, cachePos, 9, [0,-0.5,0], [1,0,0], 1, 5 * rsp, 0);

    skin_SetAnimProg_UPDATE       (oID, cachePos, 10);
}









const int ch_HEAD   = 0;
const int ch_TAIL0  = 1;
const int ch_TAIL1  = 2;
const int ch_RFOOT0 = 3;
const int ch_RFOOT1 = 4;
const int ch_RFOOT2 = 5;
const int ch_LFOOT0 = 6;
const int ch_LFOOT1 = 7;
const int ch_LFOOT2 = 8;
const int ch_LEFT   = 9;
const int ch_RIGHT  = 10;
const int ch_TOTAL  = 11;

const float ch_ASP = 0.7;


func void ch_AnimateFoot(int oID, int cachePos,int num, vector ax, vector ax1,float A, float rsp, float F, float A1)
{
    skin_SetAnimProg_LOADIDENTITY (oID, cachePos, num);
    skin_SetAnimProg_ROTATEOXC    (oID, cachePos, num, ax,  A, rsp, F);
    skin_SetAnimProg_MOVE         (oID, cachePos, num,  [0,0,0], [0,1,0], A1, rsp,F);

    skin_SetAnimProg_LOADIDENTITY (oID, cachePos, num+1);
    skin_SetAnimProg_ROTATEOXC    (oID, cachePos, num+1, ax1, A*0.2, rsp, F);
    skin_SetAnimProg_ROTATEOXC    (oID, cachePos, num+1, ax,  A, rsp, F-5~);

    skin_SetAnimProg_LOADIDENTITY (oID, cachePos, num+2);
    skin_SetAnimProg_ROTATEOXC    (oID, cachePos, num+2, ax1,  A*0.2, rsp, F);
    skin_SetAnimProg_ROTATEOXC    (oID, cachePos, num+2, ax, A, rsp, F-10~);
}

func void ch_AnimateFoott(int oID, int cachePos,int num, vector ax, vector ax1,float A, float rsp, float F, float A1)
{
    skin_SetAnimProg_LOADIDENTITY (oID, cachePos, num);
    skin_SetAnimProg_ROTATEOXC    (oID, cachePos, num, ax,  A, rsp, F);
    skin_SetAnimProg_MOVE         (oID, cachePos, num,  [0,0,0], [0,1,0], A1, rsp,F);
    skin_SetAnimProg_MOVE         (oID, cachePos, num,  [0,0,0], [0,1,0], A1, rsp*ch_ASP,F);

    skin_SetAnimProg_LOADIDENTITY (oID, cachePos, num+1);
    skin_SetAnimProg_MOVE         (oID, cachePos, num+1,  [0,0,0], [0,1,0], A1, rsp,F);
    skin_SetAnimProg_MOVE         (oID, cachePos, num+1,  [0,0,0], [0,1,0], A1, rsp*ch_ASP,F);
}

func void CreateAnimation_anl_01()
 var int oID, cachePos;
 var float rsp;
{
    CreateAnimSetsAuto(oID,cachePos, "sk.tank.anl_01", 0, 12, 30 );
    SetAnimateBlock(oID,cachePos,0,  ch_HEAD,    "Head");
    SetAnimateBlock(oID,cachePos,0,  ch_TAIL0,   "Tail0");
    SetAnimateBlock(oID,cachePos,0,  ch_TAIL1,   "Tail1");
    SetAnimateBlock(oID,cachePos,0,  ch_RFOOT0,  "RFoot0");
    SetAnimateBlock(oID,cachePos,0,  ch_RFOOT1,  "RFoot1");
    SetAnimateBlock(oID,cachePos,0,  ch_RFOOT2,  "RFoot2");
    SetAnimateBlock(oID,cachePos,0,  ch_LFOOT0,  "LFoot0");
    SetAnimateBlock(oID,cachePos,0,  ch_LFOOT1,  "LFoot1");
    SetAnimateBlock(oID,cachePos,0,  ch_LFOOT2,  "LFoot2");
    SetAnimateBlock(oID,cachePos,0,  ch_LEFT,    "Left");
    SetAnimateBlock(oID,cachePos,0,  ch_RIGHT,   "Right");
    SetAnimateBlock(oID,cachePos,0,  ch_TOTAL,   "Total");

    rsp := 10;

    ch_AnimateFoot(oID,cachePos,ch_RFOOT0, [1.52, 2.16, 0.77],[1.52, 1, 0], 30~, rsp, 0   , 0.1);
    ch_AnimateFoot(oID,cachePos,ch_LFOOT0, [1.52, 2.16, 0.77],[1.52, 1, 0], 30~, rsp, 180~, 0.1);

    skin_SetAnimProg_LOADIDENTITY (oID, cachePos, ch_LEFT);
    skin_SetAnimProg_MOVE         (oID, cachePos, ch_LEFT,  [0,0,0], [0,1,0], 0.2, rsp, 180~);
    skin_SetAnimProg_LOADIDENTITY (oID, cachePos, ch_RIGHT);
    skin_SetAnimProg_MOVE         (oID, cachePos, ch_RIGHT, [0,0,0], [0,1,0], 0.2, rsp, 0~);


    skin_SetAnimProg_LOADIDENTITY (oID, cachePos, ch_TAIL0);
    skin_SetAnimProg_MOVE         (oID, cachePos, ch_TAIL0, [0,0,0], [0,1,0], 0.2, rsp, 90~);
    skin_SetAnimProg_LOADIDENTITY (oID, cachePos, ch_TAIL1);
    skin_SetAnimProg_MOVE         (oID, cachePos, ch_TAIL1, [0,0,0], [0,1,0], 0.4, rsp, 90~-35~);

    skin_SetAnimProg_LOADIDENTITY (oID, cachePos, ch_HEAD);
    skin_SetAnimProg_ROTATEOXC    (oID, cachePos, ch_HEAD, [1.52, 2.36, -0.77], 10~, rsp, 90~);


    skin_SetAnimProg_UPDATE       (oID, cachePos, ch_TOTAL);

}
func void CreateAnimation_anl_01t()
 var int oID, cachePos;
 var float rsp;
{
    CreateAnimSetsAuto(oID,cachePos, "sk.taxi.anl_01", 0, 12, 30 );
    SetAnimateBlock(oID,cachePos,0,  ch_HEAD,    "Head");
    SetAnimateBlock(oID,cachePos,0,  ch_TAIL0,   "Tail0");
    SetAnimateBlock(oID,cachePos,0,  ch_TAIL1,   "Tail1");
    SetAnimateBlock(oID,cachePos,0,  ch_RFOOT0,  "RFoot0");
    SetAnimateBlock(oID,cachePos,0,  ch_RFOOT1,  "RFoot1");
    SetAnimateBlock(oID,cachePos,0,  ch_RFOOT2,  "RFoot2");
    SetAnimateBlock(oID,cachePos,0,  ch_LFOOT0,  "LFoot0");
    SetAnimateBlock(oID,cachePos,0,  ch_LFOOT1,  "LFoot1");
    SetAnimateBlock(oID,cachePos,0,  ch_LFOOT2,  "LFoot2");
    SetAnimateBlock(oID,cachePos,0,  ch_LEFT,    "Left");
    SetAnimateBlock(oID,cachePos,0,  ch_RIGHT,   "Right");
    SetAnimateBlock(oID,cachePos,0,  ch_TOTAL,   "Total");

    rsp := 2;

    ch_AnimateFoott(oID,cachePos,ch_RFOOT0, [1.52, 2.16, 0.77],[1.52, 1, 0], 5~, rsp, 0   , 0.05);
    ch_AnimateFoott(oID,cachePos,ch_LFOOT0, [1.52, 2.16, 0.77],[1.52, 1, 0], 5~, rsp, 180~, 0.05);

    skin_SetAnimProg_LOADIDENTITY (oID, cachePos, ch_LEFT);
    skin_SetAnimProg_MOVE         (oID, cachePos, ch_LEFT,  [0,0,0], [0,1,0], 0.05, rsp,     180~);
    skin_SetAnimProg_MOVE         (oID, cachePos, ch_LEFT,  [0,0,0], [0,1,0], 0.05, rsp*ch_ASP, 180~);
    skin_SetAnimProg_LOADIDENTITY (oID, cachePos, ch_RIGHT);
    skin_SetAnimProg_MOVE         (oID, cachePos, ch_RIGHT, [0,0,0], [0,1,0], 0.05, rsp,     0);
    skin_SetAnimProg_MOVE         (oID, cachePos, ch_RIGHT, [0,0,0], [0,1,0], 0.05, rsp*ch_ASP,     0);


    skin_SetAnimProg_LOADIDENTITY (oID, cachePos, ch_TAIL0);
    skin_SetAnimProg_MOVE         (oID, cachePos, ch_TAIL0, [0,0,0], [0,1,0], 0.1, rsp, 90~);
    skin_SetAnimProg_LOADIDENTITY (oID, cachePos, ch_TAIL1);
    skin_SetAnimProg_MOVE         (oID, cachePos, ch_TAIL1, [0,-0.4,0], [0,1,0], 0.2, 8, 90~+35~);
    skin_SetAnimProg_MOVE         (oID, cachePos, ch_TAIL1, [0,0,0],    [0,1,0], 0.2, 7, 90~+35~);

    skin_SetAnimProg_LOADIDENTITY (oID, cachePos, ch_HEAD);
    skin_SetAnimProg_ROTATEOXC    (oID, cachePos, ch_HEAD, [1.52, 2.36, -0.77], 5~, rsp, 90~);


    skin_SetAnimProg_UPDATE       (oID, cachePos, ch_TOTAL);

}




func void CreateAnimation_Peasant()
 var vector ax0;
 var vector ax1;
 var vector ax2;
 var vector ax3;
 var float  rsp;
 var float  amp;	
 var int oID, cachePos;
{
    CreateAnimSetsAuto(oID,cachePos, "sk.peop.Peasant", 0, 11, 38 );
    SetAnimateBlock(oID,cachePos,0,  0,        "RLeg");
    SetAnimateBlock(oID,cachePos,0,  1,        "RFoot");
    SetAnimateBlock(oID,cachePos,0,  2,        "LLeg");
    SetAnimateBlock(oID,cachePos,0,  3,        "LFoot");
    SetAnimateBlock(oID,cachePos,0,  4,	       "Body");
    SetAnimateBlock(oID,cachePos,0,  5,	       "Waist");
    SetAnimateBlock(oID,cachePos,0,  6,	       "RArm");
    SetAnimateBlock(oID,cachePos,0,  7,        "RHand");
    SetAnimateBlock(oID,cachePos,0,  8,	       "LArm");
    SetAnimateBlock(oID,cachePos,0,  9,	       "LHand");
    SetAnimateBlock(oID,cachePos,0,  10,       "Total");


    ax0 := [0.0,1.1,0.04];
    ax1 := [0.0,0.6,-0.03];
    ax2 := [0.0,1.8,0];
    ax3 := [0.0,1.48,0.08];
    
    rsp := 6;
    amp := 5~;

    skin_SetAnimProg_LOADIDENTITY (oID, cachePos, 0);
    skin_SetAnimProg_MOVE	  (oID, cachePos, 0, [0,0,0], [0,1,0], amp, rsp*2, 90~); 
    skin_SetAnimProg_ROTATEOXC    (oID, cachePos, 0, ax0, 20~, rsp, 180~);

    skin_SetAnimProg_LOADIDENTITY (oID, cachePos, 1);
    skin_SetAnimProg_MOVE	  (oID, cachePos, 1, [0,0,0], [0,1,0], amp, rsp*2, 90~); 
    skin_SetAnimProg_ROTATEOXC    (oID, cachePos, 1, ax1, 20~, rsp, 180~);
    skin_SetAnimProg_ROTATEOXC    (oID, cachePos, 1, ax0, 20~, rsp, 180~);

    skin_SetAnimProg_LOADIDENTITY (oID, cachePos, 2);
    skin_SetAnimProg_MOVE	  (oID, cachePos, 2, [0,0,0], [0,1,0], amp, rsp*2, 90~); 
    skin_SetAnimProg_ROTATEOXC    (oID, cachePos, 2, ax0, 20~, rsp, 0);

    skin_SetAnimProg_LOADIDENTITY (oID, cachePos, 3);
    skin_SetAnimProg_MOVE	  (oID, cachePos, 3, [0,0,0], [0,1,0], amp, rsp*2, 90~); 
    skin_SetAnimProg_ROTATEOXC    (oID, cachePos, 3, ax1, 20~, rsp, 0);
    skin_SetAnimProg_ROTATEOXC    (oID, cachePos, 3, ax0, 20~, rsp, 0);

    skin_SetAnimProg_LOADIDENTITY (oID, cachePos, 4);
    skin_SetAnimProg_MOVE	  (oID, cachePos, 4, [0,0,0], [0,1,0], amp, rsp*2, 90~); 
    skin_SetAnimProg_ROTATEOZC    (oID, cachePos, 4, [0,1,0], 4~, rsp*2, 90~);

    skin_SetAnimProg_LOADIDENTITY (oID, cachePos, 5);
    skin_SetAnimProg_MOVE	  (oID, cachePos, 5, [0,0,0], [0,1,0], amp, rsp*2, 90~); 

    skin_SetAnimProg_LOADIDENTITY (oID, cachePos, 6);
    skin_SetAnimProg_MOVE	  (oID, cachePos, 6, [0,0,0], [0,1,0], amp, rsp*2, 90~); 
    skin_SetAnimProg_ROTATEOZC    (oID, cachePos, 6, [0,1,0], 4~, rsp*2, 90~);
    skin_SetAnimProg_ROTATEOXC    (oID, cachePos, 6, ax2, 20~, rsp, 0);

    skin_SetAnimProg_LOADIDENTITY (oID, cachePos, 7);
    skin_SetAnimProg_MOVE	  (oID, cachePos, 7, [0,0,0], [0,1,0], amp, rsp*2, 90~); 
    skin_SetAnimProg_ROTATEOZC    (oID, cachePos, 7, [0,1,0], 4~, rsp*2, 90~);
    skin_SetAnimProg_ROTATEOXC    (oID, cachePos, 7, ax3, 20~, rsp, 0);
    skin_SetAnimProg_ROTATEOXC    (oID, cachePos, 7, ax2, 20~, rsp, 0);

    skin_SetAnimProg_LOADIDENTITY (oID, cachePos, 8);
    skin_SetAnimProg_MOVE	  (oID, cachePos, 8, [0,0,0], [0,1,0], amp, rsp*2, 90~); 
    skin_SetAnimProg_ROTATEOZC    (oID, cachePos, 8, [0,1,0], 4~, rsp*2, 90~);
    skin_SetAnimProg_ROTATEOXC    (oID, cachePos, 8, ax2, 20~, rsp, 180~);

    skin_SetAnimProg_LOADIDENTITY (oID, cachePos, 9);
    skin_SetAnimProg_MOVE	  (oID, cachePos, 9, [0,0,0], [0,1,0], amp, rsp*2, 90~); 
    skin_SetAnimProg_ROTATEOZC    (oID, cachePos, 9, [0,1,0], 4~, rsp*2, 90~);
    skin_SetAnimProg_ROTATEOXC    (oID, cachePos, 9, ax3, 20~, rsp, 180~);
    skin_SetAnimProg_ROTATEOXC    (oID, cachePos, 9, ax2, 20~, rsp, 180~);
    
    skin_SetAnimProg_UPDATE       (oID, cachePos, 10);
}

/**************************************************************************
 *
 *                    Skins
 *
 **************************************************************************/

func void main_LoadSkin()
 var int ctIDSkin;
{
    ctIDSkin := s_AddClassTable("Skin",35);
    LoadSkin( ctIDSkin, "dcross.vbc"    , "sk.DebugCross" );
    LoadSkin( ctIDSkin, "piece.vbc"   , "Expl.Piece" );
    LoadSkin( ctIDSkin, "ast_csh1.vbc", "sk.corpse.default");
    LoadSkin( ctIDSkin, "ast_csh2.vbc", "sk.corpse.other");
    LoadSkin( ctIDSkin, "corpse.vbc",   "sk.corpse.final");

    LoadSkin(ctIDSkin,"anl_00.vbc", "sk.tank.anl_00");
    LoadSkin(ctIDSkin,"anl_01.vbc", "sk.tank.anl_01");
    LoadSkin(ctIDSkin,"anl_01.vbc", "sk.taxi.anl_01");
    LoadSkin(ctIDSkin,"anl_02.vbc", "sk.peop.anl_02");
    LoadSkin(ctIDSkin,"anl_02t.vbc","sk.taxi.anl_02");
    LoadSkin(ctIDSkin,"vhl_hel.vbc","sk.peop.vhl_hel");
    LoadSkin(ctIDSkin,"man_knht.vbc","sk.peop.man_knht");
    LoadSkin(ctIDSkin,"man_mknt.vbc","sk.peop.man_mknt");
    LoadSkin(ctIDSkin,"man_peas.vbc","sk.peop.man_peas");
    LoadSkin(ctIDSkin,"vhl_wag.vbc","sk.peop.vhl_wag");

    LoadSkin(ctIDSkin,"bld_a00.vbc","sk.mortar.bld_a00");
    LoadSkin(ctIDSkin,"bld_a01.vbc","sk.ballsender.bld_a01");
    LoadSkin(ctIDSkin,"bld_a02.vbc","sk.ballsender.bld_a02");
    LoadSkin(ctIDSkin,"bld_a03.vbc","sk.mortar.bld_a03");
    LoadSkin(ctIDSkin,"vhl_wag.vbc","sk.tank.vhl_wag");
    LoadSkin(ctIDSkin,"vhl_ml.vbc","sk.peop.vhl_ml");
    LoadSkin( ctIDSkin, "man1_a1.vbc", "sk.peop.Bowman");
    LoadSkin( ctIDSkin, "peasant.vbc", "sk.peop.Peasant");

    LoadSkin( ctIDSkin, "bird.vbc"    , "sk.Bird.0" );
    LoadSkin( ctIDSkin, "art1.vbc", "sk.Artefact.0");
    LoadSkin(ctIDSkin,"vhl_gal.vbc", "sk.peop.vhl_gal");


/*
    LoadSkin(ctIDSkin,"anl_00.vbc","sk.tank.anl_00");
    LoadSkin(ctIDSkin,"anl_00.vbc","sk.taxi.anl_00");

    LoadSkin(ctIDSkin,"anl_01.vbc","sk.tank.anl_01");
    LoadSkin(ctIDSkin,"anl_01.vbc","sk.taxi.anl_01");

    LoadSkin(ctIDSkin,"anl_02.vbc","sk.peop.anl_02");
    LoadSkin(ctIDSkin,"anl_02t.vbc","sk.taxi.anl_02");

    LoadSkin(ctIDSkin,"vhl_hel.vbc","sk.peop.vhl_hel");

    LoadSkin(ctIDSkin,/* "man_mknt.vbc" */ "man1_a1.vbc", "sk.peop.man_mknt");
    LoadSkin(ctIDSkin,/* "man_peas.vbc" */ "man1_a1.vbc" ,"sk.peop.man_peas");
    LoadSkin(ctIDSkin,"vhl_wag.vbc","sk.tank.vhl_wag");

    LoadSkin(ctIDSkin,"bld_a00.vbc","sk.mortar.bld_a00");
    LoadSkin(ctIDSkin,"bld_a01.vbc","sk.ballsender.bld_a01");
    LoadSkin(ctIDSkin,"bld_a02.vbc","sk.ballsender.bld_a02");
    LoadSkin(ctIDSkin,"bld_a03.vbc","sk.mortar.bld_a03");

    LoadSkin( ctIDSkin, "bird.vbc"    , "sk.Bird.0" );
    LoadSkin( ctIDSkin, "art1.vbc", "sk.Artefact.0");
 */

    LoadSkin(ctIDSkin,"vhl_gal.vbc","sk.ship.gal");
    LoadSkin(ctIDSkin,"vhl_gal2.vbc","sk.ship.gal.king");
    LoadSkin(ctIDSkin,"vhl_shp.vbc","sk.ship.vhl_shp");
    LoadSkin(ctIDSkin,"anl_03.vbc","sk.rat");
    LoadSkin(ctIDSkin,"vhl_shp.vbc","sk.ship.stay");






    CreateAnimation_anl_00 ();
    CreateAnimation_anl_01 ();
    CreateAnimation_anl_01t();
    CreateAnimation_anl_02 ();
    CreateAnimation_anl_02t();

    CreateAnimation_Bowman();
    CreateAnimation_Helic();
    CreateAnimation_Mill();
    CreateAnimation_Wagon();
    CreateAnimation_Ship();
    CreateAnimation_Peasant();

    ctIDSkin := s_AddClassTable("SkinSpr",1);
    LoadSkin( ctIDSkin, "fusion.txr", "sk.Fusion.0"  );
}


