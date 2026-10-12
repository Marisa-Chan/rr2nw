#ifndef	__PATTERNS_CPP__
#include"_pattern.h"
#endif

CFont *CControl::m_pFigs5x3 = NULL;
void CControl::Init()
{
  if (m_pFigs5x3) delete m_pFigs5x3;
  m_pFigs5x3 = new CFont("figs5x3.fnt");
}

//================ MsgBoard ===================
//CFont CMessageControl::m_figs5x3("figs5x3.fnt");
void CMessageControl::Init(byte *szMsgPtr, byte *szMsg, int len)
{
  //m_szMsg = szMsgPtr;
  //m_szMsg = (byte*)
  (void)szMsgPtr;
  strncpy((char*)m_szMsg, (char*)szMsg, Min(len,MAX_MSG_LEN));
  //int rlen = strlen((char const*)szMsg);
  //m_szMsg[rlen] = 0;
  m_fDelta = 0;
}

void CMessageControl::Print(int x, int y)
{
    ASSERT(m_pFigs5x3);
    x -= m_pFigs5x3->StringWidth((char*)m_szMsg)/2;
    m_pFigs5x3->PrintAtC(x, y, (char*)m_szMsg);
}

CMsgBoardControl::CMsgBoardControl(int nLeft, int nRight, int nTop,
                                   int nStep, int nMsgLen)
{
  m_nLeft = nLeft;
  m_nRight = nRight;
  m_nTop = nTop;

  m_nStep = nStep;
  m_nCurrLine = 0;
  //for(int i=0; i<MAX_MSG_NUM; i++)
  //   m_ppMsgStrings[i] = new byte[nMsgLen+1];
  m_nMsgLen = nMsgLen;
  m_fLifeTime = 0;
  m_nLineNum = 5;
}


void CMsgBoardControl::DeleteMsg(int nMsgNum)
{
       byte *ptr = m_ppMessages[nMsgNum].GetMessagePtr();
	   *ptr = 0;
       //memset(ptr, 0, m_nMsgLen);
       //delete m_ppMessages[nMsgNum];
       for(int i=nMsgNum; i<m_nCurrLine-1; i++)
          m_ppMessages[i] = m_ppMessages[i+1];
       m_nCurrLine--;
}

void CMsgBoardControl::InsertMsg(const char *szMsg)
{
    if (m_nCurrLine == m_nLineNum)   //Scrolling
       DeleteMsg(0);

    int n = m_nCurrLine;
    for (int i=0; i<m_nCurrLine; i++)
       if (strlen((char*)m_ppMessages[i].GetMessagePtr()) == 0)
          {  n = i; break; }

    //m_ppMessages[m_nCurrLine] = new CMessageControl(m_ppMsgStrings[n],(byte*)szMsg,
    //                                                m_nMsgLen);
	m_ppMessages[m_nCurrLine].Init((byte*)m_ppMessages[n].GetMessagePtr(),(byte*)szMsg,
                                                    m_nMsgLen);
    m_nCurrLine++;
}

void CMsgBoardControl::Draw()
{
    m_nBottom = m_nTop + m_nLineNum*m_nStep;
    m_nBoardW = m_nRight - m_nLeft - 2;
    GRSetClip(TCCRect2(m_nLeft, m_nTop, m_nRight, m_nBottom));
    for(int i=0; i<m_nCurrLine; i++)
         m_ppMessages[i].Print(m_nBoardW/2+m_nLeft, m_nTop+i*m_nStep);
}

void CMsgBoardControl::Step(double fDelta)
{
   for (int i=0; i<m_nCurrLine; i++)
   {
      m_ppMessages[i].SetDelta(fDelta);
      if (m_ppMessages[i].GetDelta() > m_fLifeTime)
         DeleteMsg(i);
   }
}

//=================== Speed ===================

CSpeedBarControl::CSpeedBarControl(TCCRect2 clip, int nSpeedExtra, int nSpeedDY)
{
     m_clip = clip;
     m_nSpeedExtra = nSpeedExtra;
     m_nSpeedDY = nSpeedDY;
     m_nSpeedH = m_clip.bottom - m_clip.top - 2*m_nSpeedDY;
     m_nSpeedW = m_clip.right - m_clip.left - 2*m_nSpeedExtra;
     m_nOffset = GRGetScreenWidth() * m_clip.top + m_clip.left;
     m_fMaxSpeed = 1;
     m_fMinSpeed = 0;
}


void CSpeedBarControl::Draw(double fSpeed, double fEffSpeed)
{
        int   nScrWidth = GRGetScreenWidth();

        int speed01 = Round(m_nSpeedW/(m_fMaxSpeed-m_fMinSpeed));
        int speed0 = /*speedL+*/ m_nSpeedExtra-Round(speed01*m_fMinSpeed);
        byte *p0 = GRGetScreenPtr() + m_nOffset;
        byte *p1, *p;
        int  i, j, jl, jr;
        if( fEffSpeed < 0 )
          jl = Max(0,(jr=speed0)+Round(speed01*fEffSpeed));
        else
          jr = Min((int)m_clip.right-(int)m_clip.left,
               (jl=speed0)+Round(speed01*fEffSpeed));
        p1 = p0+nScrWidth*m_nSpeedDY+jl;
        for( i = 0 ; i < m_nSpeedH ; i++, p1 += nScrWidth )
           for( j = jl, p = p1 ; j <= jr ; j++ ) *p++ = 2;
        p = p0+speed0;
        for( i = 0 ; i < m_nSpeedDY ; i++, p+=nScrWidth )  *p = 2;
        for( i = 0 ; i < m_nSpeedH ; i++, p+=nScrWidth )   *p = 15;
        for( i = 0 ; i < m_nSpeedDY ; i++, p+=nScrWidth )  *p = 2;
        p = p0+Max(0,Min((int)m_clip.right-(int)m_clip.left,
                      speed0+Round(speed01*fSpeed)));
        for( i = 0 ; i < m_clip.bottom-m_clip.top ; i++, p+=nScrWidth ) *p = 15;
}


CSpeedScaleControlV::CSpeedScaleControlV(TCCRect2 clip,
                   int nPixEtch, double fStepEtch, int nStepFigs)
{
   m_clip = clip;
   m_nSpeedY = (m_clip.top + m_clip.bottom)/2;
   m_nPixEtch = nPixEtch;
   m_fStepEtch = fStepEtch;
   m_nStepFigs = nStepFigs;
}

CSpeedScaleControlH::CSpeedScaleControlH(TCCRect2 clip,
                   int nPixEtch, double fStepEtch, int nStepFigs)
{
   m_clip = clip;
   m_nSpeedX = (m_clip.left + m_clip.right)/2;
   m_nPixEtch = nPixEtch;
   m_fStepEtch = fStepEtch;
   m_nStepFigs = nStepFigs;
}

//CFont CSpeedControl::m_figs5x3("figs5x3.fnt");

void CSpeedScaleControlV::Draw(double fEffSpeed, double fSpeed)
{
    int  nScrWidth = GRGetScreenWidth();
    GRSetClip(m_clip);
    byte *p;
    int i;

    ASSERT(m_pFigs5x3);

    double  fScale0 = (fEffSpeed/m_fStepEtch)*m_nPixEtch+(m_nSpeedY-m_clip.top);
    int nScale0 = Round(fScale0),
        y = m_clip.top + nScale0%m_nPixEtch,
        nStepPtr = m_nPixEtch*nScrWidth;
     p = GRGetPixelPtr(m_clip.right,y);
     int nFigs = Round(fScale0 / m_nPixEtch);
     int nCnt,
         nEffFig = Round(fSpeed / m_fStepEtch);
     double fnSpeed;

     while( y < m_clip.bottom + 2) {
       fnSpeed = m_fStepEtch * nFigs;
       nCnt = nFigs%m_nStepFigs;
       if (nEffFig == nFigs)
         {
            int j = m_clip.left - m_clip.right;
            for(i=0; i<2; i++)
               p[j+i] = 15;
         }

       if( nCnt == 0 ) {
           if( y >= m_clip.top && y < m_clip.bottom )
                for( int i = -3 ; i < 0 ; i++ ) p[i] = 2;
           nCnt = 0;
           char  s[20];
           sprintf(s,"%d",Round(fnSpeed));
           m_pFigs5x3->PrintAtC(m_clip.right-4-m_pFigs5x3->StringWidth(s),y-2,s);
       } else
          if( y >= m_clip.top && y < m_clip.bottom )
             for( int i = -2 ; i < 0 ; i++ ) p[i] = 2;
       y += m_nPixEtch;
       nFigs--;
       p += nStepPtr;
    }
    p = GRGetPixelPtr(m_clip.left,m_nSpeedY);
    for( i = 0 ; i < m_clip.right-m_clip.left ; i++ )  *p++=15;
}


void CSpeedScaleControlH::Draw(double fEffSpeed, double fSpeed)
{
    int   nScrWidth = GRGetScreenWidth();
    GRSetClip(m_clip);
    byte *p;
    int i;

    ASSERT(m_pFigs5x3);

    double fScale0 = (fEffSpeed/m_fStepEtch)*m_nPixEtch+(m_nSpeedX-m_clip.left);
    int nScale0 = Round(fScale0),
        y = m_clip.right - nScale0%m_nPixEtch,
        nStepPtr = m_nPixEtch; //*nScrWidth;
     p = GRGetPixelPtr(y, m_clip.bottom);
     int nFigs = Round(fScale0 / m_nPixEtch);
     int nCnt,
         nEffFig = Round(fSpeed / m_fStepEtch);
     double fnSpeed;

     while( y > m_clip.left + 2) {
       fnSpeed = m_fStepEtch * nFigs;
       nCnt = (int)(fnSpeed/m_fStepEtch)%m_nStepFigs;
       if (nEffFig == nFigs)
         {
            int j = m_clip.top - m_clip.bottom;
            for(i=0; i<2; i++)
               p[(j+i)*nScrWidth] = 15;
         }

       if( nCnt == 0 ) {
           if( y >= m_clip.left && y < m_clip.right )
                for( int i = -3 ; i < 0 ; i++ ) p[i*nScrWidth] = 2;
           nCnt = 0;
           //char  s[20];
//           sprintf(s,"%d",Round(fSpeed*10),Round(fEffSpeed*10));
//           m_pFigs5x3->PrintAtC(m_clip.bottom-4-m_pFigs5x3->StringWidth(s),y-2,s);
//           m_pFigs5x3->PrintAtC(m_clip.left,m_clip.top,s);
       } else
          if( y >= m_clip.left && y < m_clip.right )
             for( int i = -2 ; i < 0 ; i++ ) p[i*nScrWidth] = 2;
       y -= m_nPixEtch;
       nFigs--;
       p -= nStepPtr;
    }
    p = GRGetPixelPtr(m_nSpeedX,m_clip.top);
    for( i = 0 ; i < m_clip.bottom-m_clip.top ; i++ )  p[i*nScrWidth]=15;
}




//================ Compass ===================

CFMatrix3x4 CCompassDialControl::m_inv;
CCompassDialControl::CCompassDialControl(TCCRect2 clip, TCchar *szCompass,
                                 CFVector3 transVector, double fAspectRatio)
{
   FILE *f;

   m_clip = clip;
   m_transVector = transVector;

//{ {24, 186}, {50, 168}, {50, 145}, {24, 145} }
   m_vert[0].x = m_clip.left; m_vert[0].y = m_clip.bottom;
   m_vert[1].x = m_clip.right; m_vert[1].y = m_clip.bottom;
   m_vert[2].x = m_clip.right; m_vert[2].y = m_clip.top;
   m_vert[3].x = m_clip.left; m_vert[3].y = m_clip.top;

   m_ppVerts[0] = m_vert;
   m_ppVerts[1] = m_vert+1;
   m_ppVerts[2] = m_vert+2;
   m_ppVerts[3] = m_vert+3;

   f = CFileResource::FOpenCurrent(szCompass);
   RTCHECK(f,szCompass);
   m_nCompassX = m_nCompassY = 0;
   fread(&m_nCompassX,2,1,f);
   fread(&m_nCompassY,2,1,f);
   fseek(f,1,SEEK_CUR);
   m_pCompass = new byte[m_nCompassX*m_nCompassY];
   fread(m_pCompass,m_nCompassX*m_nCompassY,1,f);
   fclose(f);

   m_fAspectRatio = fAspectRatio;
}



#if 0
void CCompassDialControl::TexturedCallBack(SGRTexturedCallBackData &tcbd)
{
	const double FACTOR = 65536;
	tcbd.signx1 = tcbd.signx2 = tcbd.a = m_inv.m[0][0]*FACTOR;
	tcbd.signy1 = tcbd.signy2 = tcbd.c = m_inv.m[1][0]*FACTOR;
	tcbd.f = 0;
	tcbd.b1 = (m_inv.m[0][1]*tcbd.y1+m_inv.m[0][3])*FACTOR;
	tcbd.b2 = (m_inv.m[0][1]*tcbd.y2+m_inv.m[0][3])*FACTOR;
	tcbd.d1 = (m_inv.m[1][1]*tcbd.y1+m_inv.m[1][3])*FACTOR;
	tcbd.d2 = (m_inv.m[1][1]*tcbd.y2+m_inv.m[1][3])*FACTOR;
	tcbd.g1 = tcbd.g2 = FACTOR;
}
#endif

void CCompassDialControl::Draw(double fDir)
{
	(void) fDir;
	#if 0
    GRSetClip(m_clip);
    m_inv.LoadIdentity().TranslateL(m_transVector).
             ScaleL(CFVector3(1,1/m_fAspectRatio,1)).RotateOzL(fDir).
             ScaleL(CFVector3(1,m_fAspectRatio,1)).
             TranslateL(CFVector3(m_nCompassX/2.,m_nCompassY/2.,0));

    GRCFillTexturedPolyPCCW((TCCVector2**)m_ppVerts,4,TexturedCallBack,
                            m_nCompassX, m_nCompassY, m_pCompass,NULL);
	#endif
}


//--------- Scale Compass -----------------//
//CFont CCompassScaleControl::m_figs5x3("figs5x3.fnt");
CCompassScaleControl::CCompassScaleControl (TCCRect2 clip, int nPixEtch,
                                            int nStepEtch, int nStepFigs)
{
   m_clip = clip;
   m_nPixEtch = nPixEtch;
   m_nStepEtch = nStepEtch;
   m_nStepFigs = nStepFigs;
   m_nCompassX = (clip.right + clip.left) / 2;
   m_fRadius = nPixEtch * 180 / (nStepEtch * M_PI);
}

void CCompassScaleControl::Draw(double fDir)
{
    int   nScrWidth = GRGetScreenWidth();
    GRSetClip(m_clip);
    byte *p;
    //int i;
    int nOff;
    int nColor = 15;

    ASSERT(m_pFigs5x3);

    while (fDir > 2*M_PI) fDir -= 2*M_PI;
    while (fDir < -2*M_PI) fDir += 2*M_PI;

    nOff = Round(m_fRadius * fDir);

    int nFigCount = 360 / m_nStepEtch;
    int nFigN = 0,
        nFigW = -nFigCount / 4,
        nFigW2 = nFigCount * 3 / 4,
        nFigS = nFigCount / 2,
        nFigE = nFigCount / 4,
		nFigE2 = -nFigCount * 3 / 4;

    double fScale0 = nOff+(m_nCompassX-m_clip.left);
    int nScale0 = Round(fScale0),
        y = m_clip.right + 5 - nScale0%m_nPixEtch,
        nStepPtr = m_nPixEtch;
     p = GRGetPixelPtr(y, m_clip.bottom);
     int nFigs = nScale0 / m_nPixEtch;
     int nCnt;
     int nCompass;

     while( y >= m_clip.left-5) {
       nCompass = m_nStepEtch * nFigs;
       nCnt = nFigs%m_nStepFigs;
       if( nCnt == 0 ) {
           if( y >= m_clip.left - 5 && y < m_clip.right + 5)
           {
			 if ( y >= m_clip.left  && y < m_clip.right)
			    for( int i = -3 ; i < 0 ; i++ ) p[i*nScrWidth] = (byte)nColor;

             char flag = 0;
             char s[20];

             if (nFigs == nFigN)
               { sprintf(s, "‘"); flag = 1; }
             else if ((nFigs == nFigW) || (nFigs == nFigW2))
               { sprintf(s, "‡"); flag = 1; }
             else if ((nFigs == nFigS) || (nFigs == -nFigS))
               { sprintf(s, "ž"); flag = 1; }
             else if ((nFigs == nFigE) || (nFigs == nFigE2))
               { sprintf(s, "‚"); flag = 1; }
             if ((flag) && (y >= m_clip.left - 5) && (y<m_clip.right+5))
               m_pFigs5x3->PrintAtC(y-m_pFigs5x3->StringWidth(s)/2, m_clip.bottom-9,s);
           }
       } else
          if( y >= m_clip.left && y < m_clip.right )
             for( int i = -2 ; i < 0 ; i++ ) p[i*nScrWidth] = (byte)nColor;
       y -= m_nPixEtch;
       nFigs--;
       p -= nStepPtr;
    }
    p = GRGetPixelPtr(m_nCompassX,m_clip.top);
    *(p-2) = (byte)nColor; *(p+2) = (byte)nColor;
    p += nScrWidth;
    *(p-1) = (byte)nColor; *(p+1) = (byte)nColor;
    *(p+nScrWidth) = (byte)nColor;

}


//================= Alt ====================

CAltLineControl::CAltLineControl(TCCRect2 clip, int nPixEtch, int nStepEtch, int nStepFigs)
{
   m_clip = clip;
   m_nAltY = (m_clip.top + m_clip.bottom)/2 + 3;
   m_nPixEtch = nPixEtch;
   m_nStepEtch = nStepEtch;
   m_nStepFigs = nStepFigs;
}

//CFont CAltControl::m_figs5x3("figs5x3.fnt");

void CAltLineControl::Draw(double fAlt)
{
    ASSERT(m_pFigs5x3);

    int nColor = 15;

    int   nScrWidth = GRGetScreenWidth();
    GRSetClip(m_clip);
    double  fScale0 = (fAlt/m_nStepEtch)*m_nPixEtch+(m_nAltY-m_clip.top+2);
    int nScale0 = Round(fScale0),
        y = m_clip.top-2+nScale0%m_nPixEtch,
        nHeight = nScale0/m_nPixEtch*m_nStepEtch,
        nStepPtr = m_nPixEtch*nScrWidth;
        byte *p = GRGetPixelPtr(m_clip.right,y);
     while( y < m_clip.bottom+2 ) {
       if( nHeight%m_nStepFigs == 0 ) {
           if( y >= m_clip.top && y < m_clip.bottom )
                for( int i = -3 ; i < 0 ; i++ ) p[i] = (byte)nColor;
           char  s[20];
           sprintf(s,"%d",nHeight);
           m_pFigs5x3->PrintAtC(m_clip.right-4-m_pFigs5x3->StringWidth(s),y-2,s);
       } else
          if( y >= m_clip.top && y < m_clip.bottom )
             for( int i = -2 ; i < 0 ; i++ ) p[i] = (byte)nColor;
       y += m_nPixEtch;
       nHeight -= m_nStepEtch;
       p += nStepPtr;
    }
    p = GRGetPixelPtr(m_clip.left,m_nAltY);
    for( int i = 0 ; i < m_clip.right-m_clip.left ; i++ )  *p++=15;
}

CAltTrControl::CAltTrControl(TCCRect2 clip, int nPixEtch, int nStepEtch, int nStepFigs)
{
   m_clip = clip;
   m_nAltY = (m_clip.top + m_clip.bottom)/2 + 3;
   m_nPixEtch = nPixEtch;
   m_nStepEtch = nStepEtch;
   m_nStepFigs = nStepFigs;
}

void CAltTrControl::Draw(double fAlt)
{
    ASSERT(m_pFigs5x3);
    int nColor = 15;

    int   nScrWidth = GRGetScreenWidth();
    GRSetClip(m_clip);
    double  fScale0 = (fAlt/m_nStepEtch)*m_nPixEtch+(m_nAltY-m_clip.top+2);
    int nScale0 = Round(fScale0),
        y = m_clip.top-2+nScale0%m_nPixEtch,
        nHeight = nScale0/m_nPixEtch*m_nStepEtch,
        nStepPtr = m_nPixEtch*nScrWidth;
        byte *p = GRGetPixelPtr(m_clip.right,y);
     while( y < m_clip.bottom+2 ) {
       if( nHeight%m_nStepFigs == 0 ) {
           if( y >= m_clip.top && y < m_clip.bottom )
           {
                for( int i = -3 ; i < 0 ; i++ ) p[i] = (byte)nColor;
                char  s[20];
                sprintf(s,"%d",nHeight);
                m_pFigs5x3->PrintAtC(m_clip.right-4-m_pFigs5x3->StringWidth(s),y-2,s);
           }
       } else
          if( y >= m_clip.top && y < m_clip.bottom )
             for( int i = -2 ; i < 0 ; i++ ) p[i] = (byte)nColor;
       y += m_nPixEtch;
       nHeight -= m_nStepEtch;
       p += nStepPtr;
    }
    p = GRGetPixelPtr(m_clip.left,m_nAltY);
    *(p+nScrWidth*2) = (byte)nColor; *(p-nScrWidth*2) = (byte)nColor;
    *(p+nScrWidth+1) = (byte)nColor; *(p-nScrWidth+1) = (byte)nColor;
    *(p+2) = (byte)nColor;
}


//================== Shield =================

CShieldControlV::CShieldControlV(double fRedShield, TCCRect2 clip,
                               int nShieldDX)
{
   m_fRedShield = fRedShield;
   m_clip = clip;
   m_nShieldDX = nShieldDX;
   m_nShieldW = m_clip.right - m_clip.left - m_nShieldDX*2;
   m_nNumShields = (m_clip.bottom - m_clip.top - 1)/2;
   m_nNumReds = m_nNumShields * m_fRedShield + 0.5;
   m_nOffset = GRGetScreenWidth() * (m_clip.bottom-2) +
               (m_clip.left+m_nShieldDX);
}


void CShieldControlV::Draw(double fShield)
{
       int   nScrWidth = GRGetScreenWidth();

       byte  *p = GRGetScreenPtr() + m_nOffset;
       int  n = Round(m_nNumShields*fShield), nr = Min(n,m_nNumReds),
            w2 = nScrWidth*2+m_nShieldW;
       for( int  i = 0 ; i < nr ; i++, p-=w2 )
            for( int j = 0 ; j < m_nShieldW ; j++ )    *p++=1;
       for( ; i < n ; i++, p-=w2 )
            for( int j = 0 ; j < m_nShieldW ; j++ )    *p++=2;
}

CShieldControlH::CShieldControlH(double fRedShield, TCCRect2 clip,
                               int nShieldDX)
{
   m_fRedShield = fRedShield;
   m_clip = clip;
   m_nShieldDX = nShieldDX;
   m_nShieldW = m_clip.bottom - m_clip.top - m_nShieldDX*2;
   m_nNumShields = (m_clip.right - m_clip.left - 1)/2;
   m_nNumReds = m_nNumShields * m_fRedShield + 0.5;
   m_nOffset = GRGetScreenWidth() * (m_clip.top+m_nShieldDX) +
               (m_clip.left+2);
}

void CShieldControlH::Draw(double fShield)
{
       int   nScrWidth = GRGetScreenWidth();

       byte  *p = GRGetScreenPtr() + m_nOffset;
       int  n = Round(m_nNumShields*fShield), nr = Min(n,m_nNumReds);

       for( int  i = 0 ; i < nr ; i++, p+=2 )
            for( int j = 0 ; j < m_nShieldW ; j++ )    p[nScrWidth*j]=1;
       for( ; i < n ; i++, p+=2 )
            for( int j = 0 ; j < m_nShieldW ; j++ )    p[nScrWidth*j]=2;
}

// ==================== Hit ===================

CHitDrawControl::CHitDrawControl(TCCRect2 hit0, TCCRect2 hit1, TCCRect2 hit2,
                         int nHit01, int nHit12, byte nHitColor)
{
  m_hit0 = hit0;
  m_hit1 = hit1;
  m_hit2 = hit2;
  m_nHit01 = nHit01; m_nHit12 = nHit12;
  m_nHitColor = nHitColor;
}

void CHitDrawControl::Draw(bool *bHits)
{
   int   nScrWidth = GRGetScreenWidth();

   if( bHits[0] ) {
           byte *p = GRGetPixelPtr(m_hit0.left,m_hit0.top);
           int  nDY = nScrWidth-(m_hit0.right-m_hit0.left);
           for( int  i = 0 ; i < m_hit0.bottom-m_hit0.top ; i++ ) {
                   for( int  j = 0 ; j < m_hit0.right-m_hit0.left ; j++ )
                        *p++ = m_nHitColor;
                   p+=nDY;
           }
    }
    if( bHits[1] ) {
           byte *p = GRGetPixelPtr(m_hit1.left,m_hit1.top+1);
           int  nDY = nScrWidth-1;
           for( int  i = 0 ; i < m_nHit01-1 ; i++ ) {
              for( int  j = 0 ; j <= i ; j++ )   *p++ = m_nHitColor;
              p+=nDY; nDY--;
           }
           for( i = 0 ; i < m_hit0.bottom-m_hit0.top+1 ; i++ ) {
              for( int  j = 0 ; j < m_nHit01 ; j++ )  *p++ = m_nHitColor;
              p+=nDY;
           }
           for( i = m_nHit01 ; i > 0 ; i-- ) {
              for( int  j = 0 ; j < i ; j++ ) *p++ = m_nHitColor;
              p+=nDY; nDY++;
           }
    }
    if( bHits[3] ) {
        byte *p = GRGetPixelPtr(m_hit1.right-1,m_hit1.top+1);
        int  nDY = nScrWidth-1;
        for( int  i = 0 ; i < m_nHit01-1 ; i++ ) {
           for( int  j = 0 ; j <= i ; j++ )        *p++ = m_nHitColor;
           nDY--; p+=nDY;
        }
        for( i = 0 ; i < m_hit0.bottom-m_hit0.top+1 ; i++ ) {
           for( int  j = 0 ; j < m_nHit01 ; j++ )     *p++ = m_nHitColor;
           p+=nDY;
        }
        for( i = m_nHit01 ; i > 0 ; i-- ) {
           for( int  j = 0 ; j < i ; j++ ) *p++ = m_nHitColor;
           nDY++; p+=nDY;
        }
    }
    if( bHits[2] ) {
        byte *p = GRGetPixelPtr(m_hit1.left+1,m_hit1.top);
        int  nDY = nScrWidth-(m_hit1.right-m_hit1.left-3);
        for( int  i = m_hit1.right-m_hit1.left-3 ; i >= m_hit0.right-m_hit0.left+1; i-=2 ) {
            for( int  j = 0 ; j <= i ; j++ )   *p++ = m_nHitColor;
            p+=nDY; nDY+=2;
        }
    }
    if( bHits[4] ) {
       byte *p = GRGetPixelPtr(m_hit0.left-1,m_hit0.bottom+1);
       int  nDY = nScrWidth-(m_hit0.right-m_hit0.left+1);
       for( int  i = m_hit0.right-m_hit0.left+1 ; i <= m_hit1.right-m_hit1.left-3; i+=2 ) {
           for( int  j = 0 ; j <= i ; j++ )        *p++ = m_nHitColor;
           nDY-=2; p+=nDY;
       }
    }
    if( bHits[5] ) {
        byte *p = GRGetPixelPtr(m_hit2.left,m_hit2.top);
        int  nDY = nScrWidth-(m_hit2.right-m_hit2.left);
        for( int  i = 0; i < m_nHit12; i++ ) {
           for( int  j = 0 ; j < m_hit2.right-m_hit2.left ; j++ ) *p++ = m_nHitColor;
           p+=nDY;
        }
        for( i = 0; i < m_hit1.bottom-m_hit1.top+2 ; i++ ) {
            for( int  j = 0 ; j < m_nHit12 ; j++ )     *p++ = m_nHitColor;
            p+=m_hit1.right-m_hit1.left+2;
            for( j = 0 ; j < m_nHit12 ; j++ )  *p++ = m_nHitColor;
            p+=nDY;
        }
        for( i = 0; i < m_nHit12; i++ ) {
            for( int  j = 0 ; j < m_hit2.right-m_hit2.left ; j++ ) *p++ = m_nHitColor;
            p+=nDY;
        }
   }
}

CHitSprControl::CHitSprControl(TCchar *szHitPatt)
{
   int i;
   for (i=0; i<6; i++)
   {
       char fname[20];
       sprintf(fname,"%s%d.pnl",szHitPatt,i);
       m_hit[i].Read(fname);
   }
}
void CHitSprControl::Draw(bool *bHits)
{
     if (bHits[0]) m_hit[0].Draw(0,0);
     if (bHits[1]) m_hit[1].Draw(0,0);
     if (bHits[2]) m_hit[2].Draw(0,0);
     if (bHits[3]) m_hit[3].Draw(0,0);
     if (bHits[4]) m_hit[4].Draw(0,0);
     if (bHits[5]) m_hit[5].Draw(0,0);
}

//====================== Charge ===================

//CFont CChargeControl::m_figs5x3("figs5x3.fnt");
CChargeControl::CChargeControl(TCchar *szChargePatt, int nX, int nY) {
    m_nX = nX;
    m_nY = nY;

    int i;
    for (i=0; i<WEAPON_NUM; i++)
    {
        char fname[20];
        sprintf(fname,"%s%d.pnl",szChargePatt,i);
        m_charge[i].Read(fname);
    }

}

void CChargeControl::Draw(int nCharge, int nWeapon)
{
    ASSERT(m_pFigs5x3);
    m_charge[nWeapon].Draw(0,0);

    char    s[20];

    sprintf(s,"%d",nCharge);
    m_pFigs5x3->PrintAt(m_nX-m_pFigs5x3->StringWidth(s),m_nY,s);
}

//==============  RADAR =======================//
CRadarControl::CRadarControl(TCCRect2 back, int nHalfRadius,
								double fAspectRatio, double fDeltaY )
{
   m_back = back;
   m_nBackX = (m_back.left + m_back.right /*- 1*/) / 2;
   m_nBackY = (m_back.top + m_back.bottom /*- 1*/) / 2;
   int dX = m_back.right - m_back.left,
	   dY = m_back.bottom - m_back.top;
   m_nRadius = Min(dX, dY) / 2 - 3;
   m_nHalfRadius = nHalfRadius;
   m_fAspectRatio = fAspectRatio;
   m_fDeltaY = fDeltaY;
   //m_fShelterRadius = fShelterRadius;
}

void CRadarControl::BeginDraw()
{
    GRSetOrigin(0,0);
    GRSetClip(m_back);
    GRClearClipRect(0);
    byte *pRadar = GRGetPixelPtr(m_nBackX,m_nBackY);
    *pRadar = 2;    // green
}

void CRadarControl::Draw (double fRealRadius, double fDir, double fHeight)
{
    //double fScale = m_fShelterRadius / m_nRadius;
    double fRadius = m_nRadius *
               (1.0 - exp(-fRealRadius * M_LN2 / m_nHalfRadius));
    int nX = Round(-fRadius * sin(fDir)),
        nZ = Round(-fRadius * cos(fDir) * m_fAspectRatio);

    //DrawItem(CFVector3(x, fHeight, z));
    byte    *pRadar = GRGetPixelPtr(m_nBackX, m_nBackY);
    int     nWidth = GRGetScreenWidth();
    int i;

    if (fHeight > m_fDeltaY)
        for (i=-1; i<=1; i++)
        {
        pRadar[nX+i + nZ * nWidth] = 1;
        pRadar[nX + (nZ+i) * nWidth] = 1;
        }
    else if (fHeight < -m_fDeltaY)
            for (i=-1; i<=1; i++)
                pRadar[nX+i + nZ * nWidth] = 1;
    else
            for(i=-1; i<=1; i++)
            {
                pRadar[nX+i + (nZ-1) * nWidth] = 1;
                pRadar[nX+i + (nZ+1) * nWidth] = 1;
            }

}

//============= KillPoint====================
CKillPointControl::CKillPointControl(TCCRect2 clip, int nBorderWidth)
{
   m_clip = clip;
   m_nBorderWidth = nBorderWidth;
   m_nClipW = clip.right - clip.left;
   m_nClipH = clip.bottom - clip.top;
}

void CKillPointControl::Draw(int nColor, int nKillPoints)
{
   int i, j;
   byte *p;
   int nScrWidth = GRGetScreenWidth();

   GRSetClip(m_clip);
   p = GRGetPixelPtr(m_clip.left, m_clip.top);
   TCbyte *pt = CPaletteTranslator::Transparency();
	for( i = 0 ; i < m_nClipH ; i++ ) {
		for( j = 0 ; j < m_nClipW ; j++,p++ )	*p = pt[*p];
		p+=nScrWidth-m_nClipW;
	}


   p = GRGetPixelPtr(m_clip.left, m_clip.top);
   for(j=0; j<m_nBorderWidth; j++)
   {
      for(i=0; i<m_nClipW; i++)
      {
         p[i+j*nScrWidth] = (byte)nColor;
         p[i+(m_nClipH-j)*nScrWidth] = (byte)nColor;
      }
      for(i=0; i<=m_nClipH; i++)
      {
          p[i*nScrWidth+j] = (byte)nColor;
          p[i*nScrWidth+m_nClipW-j] = (byte)nColor;
      }
   }

   ASSERT(m_pFigs5x3);
   char s[20];
   sprintf(s, "%d", nKillPoints);
   int len = m_pFigs5x3->StringWidth(s);
   int x = m_clip.right - m_nBorderWidth - 2 - len;
   int y = m_clip.top + Round(m_nClipH / 2) - 2;
   m_pFigs5x3->PrintAtC(x, y, s);
}
