#pragma once

/// @brief ����ö��
enum ETOOLTYPE
{
	eUnknow = 0,		//	δ֪
	eDrawRect,			//	���ƾ���
	eDrawEllipse,		//	��Բ
	eDrawArrow,			//	��ͷ
	eDrawBrush,			//	��ˢ
	eDrawFont,			//	����
	eUndo,				//	����
	eSave,				//	����
	eCancel,			//	ȡ��
	eFinish				//	���
};

/// @brief ���ƴ�С
enum EPAINTSIZE
{
	eDrawSmall = 0,		//	С
	eDrawMid,			//	��
	eDrawBig			//	��
};

/// @struct DRAWDATA
/// @brief DRAWDATA ��ͼ����

struct DRAWDATA
{
	DRAWDATA() : eToolType(eUnknow), ePaintSize(eDrawSmall), nFontSize(12), color(RGB(255, 0, 0)){}
	std::vector<CPoint>		vecPt;				//	�㼯��
	CString					strText;			//	����
	COLORREF				color;				//	��ɫ
	ETOOLTYPE				eToolType;			//	����Ԫ��
	EPAINTSIZE				ePaintSize;			//	���ƴ�С
	int						nFontSize;			//	���ִ�С
};

/// @class CAjScreenShotObserver
/// @brief ��ͼ���߹۲�����

class CWGScreenShotToolObserver
{
public:
	CWGScreenShotToolObserver(){}
	virtual ~CWGScreenShotToolObserver() {}
public:
	/// @brief ������Ϣ
	/// @param [in ] type ����ö��
	virtual void DisposeTool(const ETOOLTYPE type) = 0;
};

/// @class CAjColorPanelObserver
/// @brief ��ɫ���۲���

class CWGColorPanelObserver
{
public:
	CWGColorPanelObserver(){}
	virtual ~CWGColorPanelObserver() {}
public:
	/// @brief ���ݸ���
	virtual void ModifyData(void) = 0;
};