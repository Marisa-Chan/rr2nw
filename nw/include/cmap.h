inline bool IsPowerof2(int n) { return (n|(n-1)) == n*2-1; }


template<class T> class CMap {
private:
        T *m_p;
        int m_w, m_h;
public:
        CMap() { m_p = NULL; }
        ~CMap() { Destroy(); }
        void Destroy() { delete [] m_p; m_p = NULL;}
        void Create(int w,int h) {
                ASSERT( IsPowerof2(w) && IsPowerof2(h) );
                m_w = w; m_h = h;
                m_p = new T[m_w*m_h];
                RTCHECK(m_p,"No memory");
                for(int i=0;i<w*h;i++) m_p[i] = 0;
        }
        int GetH() const {return(m_h);}
        int GetW() const {return(m_w);}
        void Read(TCchar *name) {
                FILE *f = fopen(name,"rb");
                RTCHECK(f,name);
                m_w = m_h = 0;
                fread(&m_w,2,1,f);
                fread(&m_h,2,1,f);
                ASSERT( IsPowerof2(m_w) && IsPowerof2(m_h) );
                byte c;
                fread(&c,1,1,f);
                m_p = new T[m_w*m_h];
                RTCHECK(m_p,"No memory");
                fread(m_p,m_w*m_h,sizeof(T),f);
                fclose(f);
        }
        void Write(TCchar *name) {
                FILE *f = fopen(name,"wb");
                RTCHECK(f,name);
                fwrite(&m_w,2,1,f);
                fwrite(&m_h,2,1,f);
                byte c=0;
                fwrite(&c,1,1,f);
                fwrite(m_p,m_w*m_h,sizeof(T),f);
                fclose(f);
        }
        T &operator()(int x,int y) {
                return m_p[(y&(m_h-1))*m_w+(x&(m_w-1))];
        }
};
