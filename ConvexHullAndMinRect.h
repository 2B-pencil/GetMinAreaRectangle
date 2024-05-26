#pragma once

#ifdef TAO_OBJECTARX
#define TAO_CAD_SHARE_API __declspec(dllexport)
#else
#define TAO_CAD_SHARE_API 
#endif


class TAO_CAD_SHARE_API CConvexHullAndMinRect
{
public:
	struct MinAreaRectangleInfo
	{
		double area; // 面积
		double width; // 宽度（长度）
		double height; // 高度（宽度）
		AcGePoint2dArray pointArray; // 四个点位
		MinAreaRectangleInfo()
		{
			area = 0;
			width = 0;
			height = 0;
			pointArray.removeAll();
		}
		~MinAreaRectangleInfo()
		{
			area = 0;
			width = 0;
			height = 0;
			pointArray.removeAll();
		}
	};

	static bool GetMinAreaRectangle(const AcDbCurve* pCurve, MinAreaRectangleInfo& marInfo);
	static bool GetMinAreaRectangle(const AcDbObjectId& curveId, MinAreaRectangleInfo& marInfo);

};


