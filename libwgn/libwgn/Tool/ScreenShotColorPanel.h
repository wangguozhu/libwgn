#pragma once

#include "../resource.h"
#include "afxwin.h"
#include "WGScreenShotHead.h"

/// @class CScreenShotColorPanel
/// @brief 颜色面板

class CScreenShotColorPanel : public CDialog
{
	DECLARE_DYNAMIC(CScreenShotColorPanel)

public:
	CScreenShotColorPanel(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CScreenShotColorPanel();

// 对话框数据
	enum { IDD = IDD_DIALOG_COLOR };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	/// @brief 设置绘制数据
	/// @param [out] pDrawData 绘制数据
	void SetDrawData(DRAWDATA* pDrawData);

	/// @brief 设置观察者
	/// @param [in ] pObserver 观察者
	void SetObserver(CWGColorPanelObserver* pObserver);

	/// @brief 更新界面
	void UpdataUI(void);
protected:
	/// @brief 初始化
	BOOL OnInitDialog();

	/// @brief 绘制
	afx_msg void OnPaint();

	/// @brief 绘制背景
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);

	/// @brief 鼠标移动
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);

	/// @brief 按下左键
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);

	/// @brief 下拉框的修改
	afx_msg void OnCbnSelchangeComboFont();

	/// @brief 设置光标
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);

	/// @brief 消息处理函数
	virtual BOOL PreTranslateMessage(MSG* pMsg);
private:
	/// @brief 绘制颜色面板
	/// @param [out] pDC 绘制上下文
	/// @param [in ] nStartPos 起始位置
	void DrawColorBar(CDC* pDC, const int nStartPos);

	/// @brief 绘制颜色矩形
	/// @param [out] pDC 绘制上下文
	/// @param [in ] nX 左边起始位置
	/// @param [in ] nY 顶部起始位置
	/// @param [in ] nWidth 矩形宽度
	/// @param [in ] nColor 矩形颜色
	void DrawColor(CDC* pDC, const int nX, const int nY, const int nWidth, const COLORREF& nColor);

	/// @brief 绘制文字控件
	/// @param [out] pDC 绘制上下文
	/// @param [out] nStartPos 起始位置
	void DrawFontBar(CDC* pDC, int& nStartPos);

	/// @brief 绘制大小控件
	/// @param [out] pDC 绘制上下文
	/// @param [out] nStartPos 起始位置
	void DrawSizeBar(CDC* pDC, int& nStartPos);

	/// @brief 获取当前鼠标在哪个颜色上
	/// @param [in ] point 鼠标位置
	int GetColorIndex(const CPoint& point) const;

	/// @brief 获取鼠标在哪个大小控件上
	/// @param [in ] point 鼠标位置
	int GetSizeIndex(const CPoint& point) const;

	/// @brief 获取默认的区域
	CRect GetNormalRect(void) const;

	/// @brief 获取选中的区域
	CRect GetSelectRect(void) const;
	
	/// @brief 获取移动的区域
	CRect GetMoveRect(void) const;

	/// 释放内存
	void Release(void);
private:
	CComboBox								m_comboFont;		//	字体大小					
	CImage									m_ImageFont;		//	文字图片
	std::vector<COLORREF>					m_vecColor;			//	颜色集合
	std::vector<CRect>						m_vecColorRect;		//	颜色几何的矩形
	std::vector<CImage*>					m_vecFuncPng;		//	功能图片集合
	std::vector<CRect>						m_vecFuncRect;		//	功能按钮区域
	COLORREF								m_nBKColor;			//	背景颜色
	COLORREF								m_nPenColor;		//	笔的颜色
	COLORREF								m_nSpliteColor;		//	分割符的颜色
	HCURSOR									m_hColorCursor;		//	有颜色的光标
	int										m_nMoveIndex;		//	鼠标移动的下标
	DRAWDATA*								m_pCurDrawData;		//	当前绘制数据
	CWGColorPanelObserver*					m_pObserver;		//	观察者
	int										m_nCurSelect;		//	当前选中的是哪一个
};
