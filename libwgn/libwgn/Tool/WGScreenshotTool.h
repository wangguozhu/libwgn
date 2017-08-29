#pragma once

#include "../resource.h"
#include "WGScreenShotHead.h"
#include "ScreenShotColorPanel.h"
#include "WGEdit.h"

/// @class AjScreenshotTool
/// @brief 截屏工具条

class CWGScreenshotTool : public CWGColorPanelObserver
{
	/// @struct RECORDDATA
	/// @brief 记录数据
	struct RECORDDATA
	{
		RECORDDATA() : color(RGB(255, 0, 0)), ePaintSize(eDrawSmall), nFontSize(12) {}
		COLORREF		color;				//	颜色
		EPAINTSIZE		ePaintSize;			//	绘制大小
		int				nFontSize;			//	文字大小
	};
public:
	explicit CWGScreenshotTool();
	~CWGScreenshotTool();
public:
	/// @brief 创建工具
	/// @param [in ] pParentWnd 父窗口
	void Create(CWnd* pParentWnd);

	/// @brief 设置是否可以绘制
	/// @param [in ] bTrue 是否可以绘制
	/// @param [in ] orgRect 原始区域
	/// @param [in ] showRect 可视区域
	void setIsCanDraw(bool bTrue, const CRect& orgRect, const CRect& showRect);

	/// @brief 设置观察者
	/// @param [in ] pOberver 观察者
	void SetObserver(CWGScreenShotToolObserver* pOberver);

	/// @brief 触发功能
	/// @param [in ] eToolType 功能的枚举
	void TriggerFunc(const ETOOLTYPE eToolType);

	/// @brief 重置
	void Reset(void);

	/// @brief 绘制
	/// @param [in ] bIsShowPanel 是否显示面板
	bool Draw(CDC* pDC, const bool bIsShowPanel = true);

	/// @brief 鼠标移动
	void OnMouseMove(UINT nFlags, CPoint point);

	/// @brief 鼠标按下左键
	void OnLbButtonDown(UINT nFlags, CPoint point);

	/// @brief 鼠标左键弹起
	void OnLbButtonUp(UINT nFlags, CPoint point);

	/// @brief 修改控件颜色
	void OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);

	/// @brief 数据更改
	virtual void ModifyData(void);

	/// @brief 获取位置
	/// @return 1 底部 2 顶部 3 可视区域内
	int GetPos(void) const;

	/// @brief 获取实际宽度
	int GetActualWidth(void) const;

	/// @brief 获取实际高度
	int GetActualHeight(void) const;

	/// @brief 刷新子控件
	void RedrawChildWnd(void);

	/// @brief 获取ToolTip信息
	/// @param [out] mapToolTipRel ToolTip信息
	void GetToolTipInfo(std::map<CRect, CString>& mapToolTipRel) const;
private:
	/// @brief 初始化
	void Init(void);

	/// @brief 显示颜色栏
	void ShowColorPanel();

	/// @brief 坐标转换
	/// @param [out] pt 转换前后的坐标
	void ScreenToClient(CPoint& pt) const;

	/// @brief 坐标转换
	/// @param [out] rc 转换前后的矩形
	void ClientToScreen(CRect& rc) const;

	/// @brief 计算起始点
	void CalcStartPos(void);

	/// @brief 获取功能图片的选中区域
	/// @param [in ] nWidth 图片的宽度
	CRect GetSelectRect(const int nWidth = 24) const;

	/// @brief 获取功能图片的原始区域
	/// @param [in ] nWidth 图片的宽度
	CRect GetOrgRect(const int nWidth = 24) const;

	/// @brief 获取功能图片的移动区域
	/// @param [in ] nWidth 图片的宽度
	CRect GetMoveRect(const int nWidth = 24) const;

	/// 释放内存
	void Release(void);

	/// @brief 获取可视区域时的最近点
	/// @param [in ] pt 点
	CPoint GetClosePt(const CPoint& pt);
private:
	/// @brief 绘制记录
	/// @param [out] 绘制DC
	/// @param [in ] drawData 绘制数据
	void Draw(CDC* pDC, const DRAWDATA& drawData) const;

	/// @brief 绘制历史记录
	/// @param [out] 绘制DC
	void DrawHistory(CDC* pDC);

	/// @brief 绘制矩形
	/// @param [out] 绘制DC
	/// @param [in ] drawData 绘制数据
	void DrawRect(CDC* pDC, const DRAWDATA& drawData) const;

	/// @brief 绘制椭圆
	/// @param [out] 绘制DC
	/// @param [in ] drawData 绘制数据
	void DrawEllipse(CDC* pDC, const DRAWDATA& drawData) const;

	/// @brief 绘制箭头
	/// @param [out] 绘制DC
	/// @param [in ] drawData 绘制数据
	void DrawArrow(CDC* pDC, const DRAWDATA& drawData) const;

	/// @brief 绘制画刷
	/// @param [out] 绘制DC
	/// @param [in ] drawData 绘制数据
	void DrawReBrush(CDC* pDC, const DRAWDATA& drawData) const;

	/// @brief 绘制文字
	/// @param [out] 绘制DC
	/// @param [in ] drawData 绘制数据
	void DrawFont(CDC* pDC, const DRAWDATA& drawData) const;

	/// @brief 回退
	void Undo(void);

	/// @brief 显示隐藏字体编辑框
	/// @param [in ] bShow 是否显示
	void ShowFontEdit(const bool bShow);
private:
	CScreenShotColorPanel					m_screenShotColorPanel;		//	颜色面板
	CAjEdit									m_FontEdit;					//	文本
	CFont									m_Font;						//	字体
	CRectTracker							m_FontTracker;				//	文字橡皮筋
	CPoint									m_startPt;					//	起始位置
	CPoint									m_movePt;					//	鼠标移动的点
	CImage									m_pngBk;					//	背景图片
	DRAWDATA								m_curDrawData;				//	当前绘制数据
	std::map<ETOOLTYPE, RECORDDATA>			m_mapRecordData;			//	记录数据
	std::vector<CImage*>					m_vecFuncPng;				//	功能图片集合
	std::vector<CRect>						m_vecFuncRect;				//	功能按钮区域
	std::list<DRAWDATA>						m_lsDrawData;				//	绘制数据
	CRect									m_orgRect;					//	原始区域
	CRect									m_showRect;					//	可视区域
	CWGScreenShotToolObserver*				m_pObserver;				//	观察者
	CWnd*									m_pWnd;						//  父窗口
	int										m_nWidth;					//	宽度
	int										m_nHeight;					//	高度
	int										m_nFontProcess;				//	-1 没有 0 选位置 1输入文字
	int										m_nPos;						//	位置 1 底部 2 顶部 3 可视区域内
	bool									m_bIsCanDraw;				//	是否可以绘制
	bool									m_bIsPosOnTop;				//	是否在顶部
};
