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
		double area; // ���
		double width; // ��ȣ����ȣ�
		double height; // �߶ȣ���ȣ�
		AcGePoint2dArray pointArray; // �ĸ���λ
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


