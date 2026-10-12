#ifndef __POLY_HPP__
#define __POLY_HPP__

#ifdef __POLY_CPP__
#define GLOBAL
#define VALUE(n) = n
#else
#define GLOBAL extern
#define VALUE(n)
#endif


struct SVertex {
    CFVector3 pnt;
};

#define PO_MAX_VERTICES    50

class CPolygon {    // convex polygon
private:
#define PO_MIN_VERTICES  4  // minimum allocated vertices required
    int m_nVtxs;            // total allocated
    int m_verticesCnt;      // used
    SVertex **m_vtx;        // vertices (dynamically allocated)
    SRGB m_color;           // color
    CFVector3 m_center;

    double CheckPos(const CFVector3 &p, const int p1, const int p2) {
        return p.x * (m_vtx[p1]->pnt.y - m_vtx[p2]->pnt.y) +
            p.y * (m_vtx[p2]->pnt.x - m_vtx[p1]->pnt.x) +
            m_vtx[p1]->pnt.x * m_vtx[p2]->pnt.y -
            m_vtx[p1]->pnt.y * m_vtx[p2]->pnt.x;
    }

public:
    CPolygon(int VertN = PO_MAX_VERTICES) {   // create empty polygon pattern
      int i;
        m_vtx = new SVertex*[m_nVtxs = VertN > PO_MIN_VERTICES ? VertN : PO_MIN_VERTICES];
        ASSERT(m_vtx);
        for (i = 0; i < m_nVtxs; i++) {
            m_vtx[i] = new SVertex;
            ASSERT(m_vtx[i]);
        }
        m_verticesCnt = 0;
    }
    CPolygon(int VertN, SVertex **v);   // create convex polygon from
                                        //  a set of vertices
    CPolygon(CPolygon &p) {
      int i;
        m_vtx = new SVertex*[m_nVtxs = PO_MAX_VERTICES];
        ASSERT(m_vtx);
        for (i = 0; i < p.m_verticesCnt; i++) {
            m_vtx[i] = new SVertex;
            ASSERT(m_vtx[i]);
            *m_vtx[i] = *p.m_vtx[i];
        }
        for (; i < m_nVtxs; i++) {
            m_vtx[i] = new SVertex;
            ASSERT(m_vtx[i]);
        }
        m_verticesCnt = p.m_verticesCnt;
    }

    ~CPolygon() {
      int i;
        if (m_vtx) {
            for (i = 0; i < m_nVtxs; i++) {
                if (m_vtx[i] != NULL) {
                    delete m_vtx[i];
                    m_vtx[i] = NULL;
                }
            }
        }
        delete m_vtx;
        m_vtx = NULL;
    }

    void Clear(void) {  //  remove all vertices from polygon
        /*
        for (i = 0; i < m_verticesCnt; i++) {
            if (m_vtx[i] != NULL) {
                delete m_vtx[i];
                m_vtx[i] = NULL;
            }
        }
        for (; i < m_nVtxs; i++) {
            m_vtx[i] = NULL;
        }
        */
        m_verticesCnt = 0;
    }

    int GetNVertices(void) { return m_verticesCnt; }

    SVertex &GetVertex(int i) {
        ASSERT(i < m_verticesCnt);
        ASSERT(m_vtx[i]);
        return *m_vtx[i];
    }

    void AddVertex(const SVertex &v);
    void PlaceVertex(const SVertex &v);
    void RemoveVertex(int n);
    int GetClosestVertexN(const CFVector3 &pnt);
    bool Belong(const CFVector3 &pnt);
    CFVector3 &Center();

    const SRGB GetColor() { return m_color; }
    void SetColor(const int r,const int g,const int b) {
        m_color.r = (byte)r; m_color.g = (byte)g; m_color.b = (byte)b;
    }
    void SetColor(SRGB color) { m_color = color; }

};

#undef GLOBAL
#undef VALUE

#endif
