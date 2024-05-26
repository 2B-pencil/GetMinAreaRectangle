// (C) Copyright 2002-2012 by Autodesk, Inc. 
//
// Permission to use, copy, modify, and distribute this software in
// object code form for any purpose and without fee is hereby granted, 
// provided that the above copyright notice appears in all copies and 
// that both that copyright notice and the limited warranty and
// restricted rights notice below appear in all supporting 
// documentation.
//
// AUTODESK PROVIDES THIS PROGRAM "AS IS" AND WITH ALL FAULTS. 
// AUTODESK SPECIFICALLY DISCLAIMS ANY IMPLIED WARRANTY OF
// MERCHANTABILITY OR FITNESS FOR A PARTICULAR USE.  AUTODESK, INC. 
// DOES NOT WARRANT THAT THE OPERATION OF THE PROGRAM WILL BE
// UNINTERRUPTED OR ERROR FREE.
//
// Use, duplication, or disclosure by the U.S. Government is subject to 
// restrictions set forth in FAR 52.227-19 (Commercial Computer
// Software - Restricted Rights) and DFAR 252.227-7013(c)(1)(ii)
// (Rights in Technical Data and Computer Software), as applicable.
//

//-----------------------------------------------------------------------------
//----- acrxEntryPoint.cpp
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "resource.h"
#include "ConvexHullAndMinRect.h"

//-----------------------------------------------------------------------------
#define szRDS _RXST("TaoGetMAR")

//-----------------------------------------------------------------------------
//----- ObjectARX EntryPoint
class CGetMinAreaRectangleApp : public AcRxArxApp {

public:
	CGetMinAreaRectangleApp () : AcRxArxApp () {}

	virtual AcRx::AppRetCode On_kInitAppMsg (void *pkt) {
		// TODO: Load dependencies here

		// You *must* call On_kInitAppMsg here
		AcRx::AppRetCode retCode =AcRxArxApp::On_kInitAppMsg (pkt) ;
		
		// TODO: Add your initialization code here

		return (retCode) ;
	}

	virtual AcRx::AppRetCode On_kUnloadAppMsg (void *pkt) {
		// TODO: Add your code here

		// You *must* call On_kUnloadAppMsg here
		AcRx::AppRetCode retCode =AcRxArxApp::On_kUnloadAppMsg (pkt) ;

		// TODO: Unload dependencies here

		return (retCode) ;
	}

	virtual void RegisterServerComponents () {
	}
	
	static AcDbObjectId PostToModelSpace(AcDbEntity* pEnt, AcDbDatabase* pDb = acdbHostApplicationServices()->workingDatabase()) 
	{
		if (pEnt)
		{
			if (!pDb) pDb = acdbHostApplicationServices()->workingDatabase();
			if (pDb)
			{
				// 获得当前图形数据库的块表
				AcDbBlockTable* pBlkTbl = NULL;
				if (Acad::eOk == pDb->getBlockTable(pBlkTbl, AcDb::kForRead))
				{
					pBlkTbl->close(); // 可以直接关闭
					AcDbObjectId entId = AcDbObjectId::kNull;
					AcDbBlockTableRecord* pBlkTblRcd = NULL;
					if (Acad::eOk == pBlkTbl->getAt(ACDB_MODEL_SPACE, pBlkTblRcd, AcDb::kForWrite))
					{
						if (Acad::eOk == pBlkTblRcd->appendAcDbEntity(entId, pEnt)) 
							pEnt->close();
						else
						{
							delete pEnt;	// 添加失败时，要delete
							pEnt = NULL;
						}
						pBlkTblRcd->close();
					}
					return entId;
				}
			}
		}
		return AcDbObjectId::kNull;
	}

	static bool PromptSelectEnts(
		const ACHAR* prompt,
		AcRxClass* classDesc,
		AcDbObjectIdArray& entIds,
		const bool& isKindOf)
	{
		std::vector<AcRxClass*> vecClassDesc;
		vecClassDesc.emplace_back(classDesc);
		return PromptSelectEnts(prompt, vecClassDesc, entIds, isKindOf);
	}

	static bool PromptSelectEnts(
		const ACHAR* prompt,
		std::vector<AcRxClass*>& classDescs,
		AcDbObjectIdArray& entIds,
		const bool& isKindOf)
	{
		entIds.setLogicalLength(0); // 初始化
		ads_name sset;
		int result = 0;
		acutPrintf(prompt); // 提示用户选择实体
		if (RTNORM == acedSSGet(NULL, NULL, NULL, NULL, sset))
		{
			// 取得选择集的长度
			Adesk::Int32 length = 0;
			acedSSLength(sset, &length);
			// 遍历选择集
			AcDbObjectId curEntId = AcDbObjectId::kNull;
			ads_name ename;
			int size = (int)classDescs.size();
			if (isKindOf)
			{
				for (long i = 0; i < length; ++i)
				{
					if (RTNORM == acedSSName(sset, i, ename) && Acad::eOk == acdbGetObjectId(curEntId, ename))
					{
						for (int j = 0; j < size; ++j)
						{
							if (curEntId.objectClass()->isDerivedFrom(classDescs[j]))
							{
								entIds.append(curEntId);
								break;
							}
						}
					}
				}
			}
			else
			{
				for (long i = 0; i < length; ++i)
				{
					if (RTNORM == acedSSName(sset, i, ename) && Acad::eOk == acdbGetObjectId(curEntId, ename))
					{
						for (int j = 0; j < size; ++j)
						{
							if (curEntId.objectClass()->isEqualTo(classDescs[j]))
							{
								entIds.append(curEntId);
								break;
							}
						}
					}
				}
			}
		}
		acedSSFree(sset);	// 释放选择集
		return (entIds.length() > 0);
	}


	static void MyGroupGetMinAreaRectangle() {
		AcDbObjectIdArray curveIdArray;
		if (PromptSelectEnts(_T("请拾取需要求最小包围盒得曲线："), AcDbCurve::desc(), curveIdArray, true))
		{
			clock_t start, end;

			start = clock();
			CConvexHullAndMinRect::MinAreaRectangleInfo marInfo;
			AcDbPolyline* pPoly = NULL;
			int index = 0;
			for (int i = 0, length = curveIdArray.length(); i < length; ++i)
			{
				if (CConvexHullAndMinRect::GetMinAreaRectangle(curveIdArray[i], marInfo))
				{
					pPoly = new AcDbPolyline();
					for (int j = 0; j < 4; ++j)
					{
						pPoly->addVertexAt(j, marInfo.pointArray[j]);
					}
					pPoly->setColorIndex(6);
					pPoly->setClosed(true);
					pPoly->setConstantWidth(0.1);
					if (PostToModelSpace(pPoly)) ++index;
				}
			}
			end = clock();

			acutPrintf(_T("求解并绘制 %d 个最小包围盒用时：%d .\n"), index, end - start);

		}
	}

} ;

//-----------------------------------------------------------------------------
IMPLEMENT_ARX_ENTRYPOINT(CGetMinAreaRectangleApp)

ACED_ARXCOMMAND_ENTRY_AUTO(CGetMinAreaRectangleApp, MyGroup, GetMinAreaRectangle, MAR, ACRX_CMD_MODAL, NULL)
