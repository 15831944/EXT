#pragma once
#include "d:\objectarx 2013\inc\dbjig.h"
#define _PI 3.141592653
struct lastState
{
	lastState()
	{
	};
	lastState(const lastState &state)
	{
		id = state.id;
		ptSt = state.ptSt;
		ptEd = state.ptEd;
	};
	lastState(AcDbObjectId aid, AcGePoint3d aptSt, AcGePoint3d aptEd)
	{
		id = aid;
		ptSt = aptSt;
		ptEd = aptEd;
	};
	lastState & operator = (const lastState &state)
	{
		id = state.id;
		ptSt = state.ptSt;
		ptEd = state.ptEd;
		return *this;
	}
	AcDbObjectId id;
	AcGePoint3d ptSt;
	AcGePoint3d ptEd;
};

class CJigEXT :public AcEdJig
{
public:
	CJigEXT();
	virtual ~CJigEXT();

	BOOL doIt();
	virtual AcDbEntity* entity() const;
	virtual DragStatus sampler();
	virtual Adesk::Boolean update();

protected:
	AcDbLine* m_pLine;
	int m_nVec;
	AcGePoint3d m_ptCur;
	AcGePoint3d m_ptSel;
	AcGeLine3d m_GeLine;

	vector<lastState> m_stateArr;
};

