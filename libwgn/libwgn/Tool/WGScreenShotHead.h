#pragma once

/// @brief 工具枚举
enum ETOOLTYPE
{
	eUnknow = 0,		//	未知
	eDrawRect,			//	绘制矩形
	eDrawEllipse,		//	椭圆
	eDrawArrow,			//	箭头
	eDrawBrush,			//	画刷
	eDrawFont,			//	文字
	eUndo,				//	回退
	eSave,				//	保存
	eCancel,			//	取消
	eFinish				//	完成
};

/// @brief 绘制大小
enum EPAINTSIZE
{
	eDrawSmall = 0,		//	小
	eDrawMid,			//	中
	eDrawBig			//	大
};

/// @struct DRAWDATA
/// @brief DRAWDATA 绘图数据

struct DRAWDATA
{
	DRAWDATA() : eToolType(eUnknow), ePaintSize(eDrawSmall), nFontSize(12), color(RGB(255, 0, 0)){}
	std::vector<CPoint>		vecPt;				//	点集合
	CString					strText;			//	文字
	COLORREF				color;				//	颜色
	ETOOLTYPE				eToolType;			//	绘制元素
	EPAINTSIZE				ePaintSize;			//	绘制大小
	int						nFontSize;			//	文字大小
};

/// @class CAjScreenShotObserver
/// @brief 截图工具观察者类

class CWGScreenShotToolObserver
{
public:
	CWGScreenShotToolObserver(){}
	virtual ~CWGScreenShotToolObserver() {}
public:
	/// @brief 处理消息
	/// @param [in ] type 工具枚举
	virtual void DisposeTool(const ETOOLTYPE type) = 0;
};

/// @class CAjColorPanelObserver
/// @brief 颜色面板观察者

class CWGColorPanelObserver
{
public:
	CWGColorPanelObserver(){}
	virtual ~CWGColorPanelObserver() {}
public:
	/// @brief 数据更改
	virtual void ModifyData(void) = 0;
};