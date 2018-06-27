#include "stdafx.h"
#include "JigEXT.h"


CJigEXT::CJigEXT():m_pLine(NULL),m_nVec(0)
{
}


CJigEXT::~CJigEXT()
{
}


BOOL CJigEXT::doIt()
{
	
	ads_name ename;
	ads_point pt;
	int res = 1;
	Acad::ErrorStatus es;
	AcDbObjectId id;
	while ( 1 )
	{
		acedInitGet(RSG_NONULL,_T("U"));
	    res = acedEntSel(_T("\n<选择要修改的对象>/放弃(U)"),ename,pt);
		if ( res == RTNORM )
		{
			es = acdbGetObjectId(id,ename);
			if ( es != Acad::eOk )
		        continue;
			AcDbEntity *pEnt = NULL;
			es = acdbOpenObject(pEnt,id,AcDb::kForWrite);
	        if ( es != Acad::eOk )
		        continue;
			pEnt->close();
			if ( pEnt->isKindOf(AcDbLine::desc()) )
			    break;
			else
			{
				acutPrintf(_T("\n所选对象无效，请重新选择!"));
				continue;
			}
		}
		if ( res == RTCAN )
			return FALSE;
		if ( res == RTKWORD )
		{
			if ( m_stateArr.size() > 0 )
			{
			    //acedCommand(RTSTR,_T("Undo"),RTSTR,_T("1"),RTNONE);
				vector<lastState>::iterator ite = m_stateArr.end()-1;
				AcDbObjectPointer<AcDbLine> pLast(ite->id,AcDb::kForWrite);
				if ( pLast.openStatus() == Acad::eOk )
				{
					pLast->setStartPoint(ite->ptSt);
					pLast->setEndPoint(ite->ptEd);
					m_stateArr.erase(ite);
				}
			}
			return TRUE;
		}
	}
	
	
	

	AcDbLine *pLine = NULL;
	es = acdbOpenObject(pLine,id,AcDb::kForWrite);
	if ( es != Acad::eOk )
		return FALSE;

	

	m_pLine = new AcDbLine();
	m_pLine = (AcDbLine*)pLine->clone();

	pLine->setVisibility(AcDb::kInvisible);
	//pLine->erase();
	pLine->close();
	
	AcGePoint3d ptSt = m_pLine->startPoint();
	AcGePoint3d ptEd = m_pLine->endPoint();
	m_GeLine.set(ptSt,ptEd);

	m_ptSel = asPnt3d(pt);
	(m_ptSel.distanceTo(ptSt) < m_ptSel.distanceTo(ptEd) )?m_nVec = 0:m_nVec = 1;

	setDispPrompt(_T("\n输入新的终点:"));
	setUserInputControls((UserInputControls)(kNullResponseAccepted));

	AcEdJig::DragStatus stat = drag();

	if (kNormal == stat )//|| kNull == stat)
	{
		//AcDbObjectId id = append();
		//Adesk::Int32 i = id.asOldId();
		es = acdbOpenObject(pLine,id,AcDb::kForWrite);
	    if ( es == Acad::eOk )
		{
		    m_nVec?pLine->setEndPoint(m_ptCur):pLine->setStartPoint(m_ptCur);
		    pLine->setVisibility(AcDb::kVisible);
		    pLine->close();
		}
		m_pLine->erase();
		m_pLine->close();
		//AcDbObjectId idApp = append();
		m_stateArr.push_back(lastState(id,ptSt,ptEd));
		return TRUE;
	}
	else 
	{
		es = acdbOpenObject(pLine,id,AcDb::kForWrite);
	    if ( es == Acad::eOk )
		{
		    pLine->setVisibility(AcDb::kVisible);
		    pLine->close();
		}
		//m_nVec?m_pLine->setEndPoint(ptPre):m_pLine->setStartPoint(ptPre);
		m_pLine->erase();
		m_pLine->close();
		//append();
		return FALSE;
	}
}

AcDbEntity* CJigEXT::entity() const
{
	return m_pLine;
}

AcEdJig::DragStatus CJigEXT::sampler()
{
	setUserInputControls((UserInputControls)
		(kNullResponseAccepted | kAccept3dCoordinates |
		 kDontUpdateLastPoint));

	AcGePoint3d ptCur;
	DragStatus stat = acquirePoint(ptCur,m_ptSel);
	//ptCur.z = 0;
	AcGePoint3d pt1 = ptCur + m_GeLine.direction().rotateBy(_PI/2,AcGeVector3d(0,0,1));
	AcGeLine3d Ln(ptCur,pt1);
	Ln.intersectWith(m_GeLine,ptCur);
    if (stat == kNormal)
    {
		if( ptCur.distanceTo(m_ptCur) < 10e-6 )
			return kNoChange;
		m_ptCur = ptCur;
    }
	else if (stat == kNull || stat == kCancel)
	{
	}
	else
	{
		stat = kNoChange;
	}
	
	return stat;
}

Adesk::Boolean CJigEXT::update()
{
	m_nVec?m_pLine->setEndPoint(m_ptCur):m_pLine->setStartPoint(m_ptCur);
	return Adesk::kTrue;
}