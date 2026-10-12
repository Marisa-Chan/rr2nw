#ifndef __DYNOBJ_H__INCLUDED
#define __DYNOBJ_H__INCLUDED

class CViewObjectRef;

class s_ViewDynamicObject : public  CViewSphericDynamic
 {
        CViewObjectRef &m_ref;

 public:

        s_ViewDynamicObject( CViewObjectRef &ref )
                : m_ref(ref)
        {
        }

	virtual void Draw();
        void         prepareToRender();
 };

#endif

