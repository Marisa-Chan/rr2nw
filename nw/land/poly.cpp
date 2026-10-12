#define __POLY_CPP__

#include <stdlib.h>
#include <string.h>

#include "stdtypes.h"
#define LAST_H__VIEW
//#include "game.h"
#include "_land.h"

#include "poly.h"

#define ADD_STEP	8


CPolygon::CPolygon(int VertN, SVertex **v) { // create convex polygon from
                                            // a set of vertices
      int i;

        ASSERT(VertN);
        m_vtx = new SVertex*[m_nVtxs = PO_MAX_VERTICES];
        ASSERT(m_vtx);
        for (i = 0; i < m_nVtxs; i++) {
            m_vtx[i] = new SVertex;
            ASSERT(m_vtx[i]);
        }

        m_verticesCnt = 0;

        for (i = 0; i < VertN; i++)
            AddVertex(*v[i]);
}


/*****************************************
 *
 *
 *
 */
void CPolygon::AddVertex(const SVertex &v)  // enlarge convex polygon by
                                            // adding a vertex
{
  SVertex *vp;
  static int flags[PO_MAX_VERTICES];
  int i,cnt,p2;
  int *f = flags;
    // special cases: 0, 1, 2 points exists

    if (m_verticesCnt == 0) {
        *m_vtx[0] = v;
        m_verticesCnt = 1;
        return;
    }

    if (m_verticesCnt == 1) {
        *m_vtx[1] = v;
        m_verticesCnt = 2;
        return;
    }

    if (m_verticesCnt == 2) {
//        if (CheckPos(v.pnt, 0, 1) >= 0) {
        if (CheckPos(v.pnt, 0, 1) >= 0.5) {
            *m_vtx[2] = *m_vtx[1];
            *m_vtx[1] = v;
        } else {
            *m_vtx[2] = v;
        }
        m_verticesCnt = 3;
        return;
    }

    for (i = 0; i < m_verticesCnt; i++)
        f[i] = 0;

    for(i = 0, cnt = 0; i < m_verticesCnt; i++) {
        p2 = (i + 1) % m_verticesCnt;
//        if (CheckPos(v.pnt, i, p2) >= 0) {
        if (CheckPos(v.pnt, i, p2) >= 0.5) {
            ++f[i]; ++f[p2];
        }
    }
    for(i = 0; i < m_verticesCnt; i++) {
        if (f[i] == 2) {    // substitute covered vertex
            *m_vtx[i] = v;
            f[i] = 0;
            for (i = 0; i < m_verticesCnt;) {   // drop other covered vertices
                if (f[i] == 2) {
                    vp = m_vtx[i];
                    if (i < m_verticesCnt - 1) {
                         memmove(&m_vtx[i], &m_vtx[i+1], (m_verticesCnt - i - 1) * sizeof(m_vtx[0]));
                         m_vtx[m_verticesCnt - 1] = vp;
                         memmove(&f[i], &f[i+1], (m_verticesCnt - i - 1) * sizeof(f[0]));
                    }
                    --m_verticesCnt;
                } else
                    ++i;
            }
            return;
        }
    }

    // comes here only if vertex should be added or dropped

    for (i = 0; i < m_verticesCnt; i++) {
        p2 = (i + 1) % m_verticesCnt;
        if ((f[i] == 1) && (f[p2] == 1)) {  // insert vertex
            if (m_verticesCnt == m_nVtxs) { // no more free space
                ASSERT(1);
/*              SVertex **tmpVtx;
                tmpVtx = new SVertex*[m_nVtxs + ADD_STEP];
                ASSERT(tmpVtx);
                memcpy(&tmpVtx[0], &m_vtx[0], sizeof(tmpVtx[0]) * m_nVtxs);
                for (j = m_nVtxs; j < m_nVtxs + ADD_STEP; j++)
                    tmpVtx[j] = NULL;
                delete m_vtx;
                m_vtx = tmpVtx;
                m_nVtxs += ADD_STEP;
 */
            }
            if (i < p2) {  // no wrapping -> insertion
                vp = m_vtx[i + 1 + m_verticesCnt - p2];
                memmove(&m_vtx[i + 2], &m_vtx[i + 1], sizeof(m_vtx[0]) * (m_verticesCnt - p2));
                m_vtx[i + 1] = vp;
            }   // else -> appending
            *m_vtx[i + 1] = v;
            ++m_verticesCnt;
            return;
        }
    }
    return; // ignore vertex as internal for polygon
}


void CPolygon::RemoveVertex(int n)
{
  SVertex *vp;
    if (m_verticesCnt <= 0) {
        return;
    }
    ASSERT(n < m_verticesCnt);
    ASSERT(m_vtx[n]);
    vp = m_vtx[n];
    if (n != m_verticesCnt - 1) {
        memmove(&m_vtx[n], &m_vtx[n+1], (m_verticesCnt - n - 1) * sizeof(m_vtx[0]));
        m_vtx[m_verticesCnt - 1] = vp;
    }
    --m_verticesCnt;
}

int CPolygon::GetClosestVertexN(const CFVector3 &pnt)
{
  int i;
  double d, minD;
  int n;
    for (i = 0, minD = 9e30 ,n = -1; i < m_verticesCnt; i++) {
        d = Abs(m_vtx[i]->pnt - pnt);
        if (d < minD) {
            minD = d;
            n = i;
        }
    }
    return n;
}


bool CPolygon::Belong(const CFVector3 &pnt)
{
  int i, p2;
  int cnt;

    for(i = 0, cnt = 0; i < m_verticesCnt; i++) {
        p2 = (i + 1) % m_verticesCnt;
//        if (CheckPos(pnt, i, p2) > 0)
        if (CheckPos(pnt, i, p2) >= 0.5)
            return FALSE;
    }
    return TRUE;
}

CFVector3 &CPolygon::Center()
{
  int i;
    m_center = CFVector3(0, 0, 0);
    for(i = 0; i < m_verticesCnt; i++) {
        m_center += m_vtx[i]->pnt;
    }
    m_center = m_center / m_verticesCnt;
    return m_center;
}