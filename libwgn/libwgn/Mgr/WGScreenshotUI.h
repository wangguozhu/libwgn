#pragma once

#include "WGScreenshotUIHead.h"
#include "../Tool/WGScreenshotTool.h"
#include "../Tool/WGScreenShotHead.h"
#include "../Tool/RectColorTracker.h"
#include "../Tool/PngImage.h"

/// @class CAjScreenshotUI
/// @brief 屏幕截图界面

class CWGScreenshotUI : public CDialog, public CWGScreenShotToolObserver
{
	DECLARE_DYNAMIC(CWGScreenshotUI)
private:
	struct CAjWndData
	{
		HWND						hwnd;
		CRect						rc;
		std::vector<CAjWndData>		children;
	};
public:
	CWGScreenshotUI(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CWGScreenshotUI();

// 对话框数据
	enum { IDD = IDD_SCREENSHOT_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	/// @brief 设置保存文件的前缀
	/// @param [in ] strSaveFilePrefix 文件前缀
	void SetSaveFilePrefix(const char* strSaveFilePrefix);

	/// @brief 截图结束后的返回值 
	ECLOSEUICODE GetCloseCode() const;

	/// @brief 获取图片
	HBITMAP GetScreenshotBitmap(void) const;
private:
	/// @brief 添加窗口和位置
	/// @param [in ] hwnd 窗口
	bool AddWndPos(const HWND hwnd);

	/// @brief 处理消息
	/// @param [in ] type 工具枚举
	virtual void DisposeTool(const ETOOLTYPE type);
private:
	/// @brief 更新显示
	void UpDataUI(void);

	/// @brief 设置绘制矩形
	/// @param [in ] 是否绘制了矩形
	void SetIsDrawRect(const bool bTrue);

	/// @brief 绘制框架
	/// @param [out] 资源文件
	/// @param [in ] bIsShowRect 是否绘制矩形橡皮筋
	void DrawFrame(CDC* pDC = NULL, const bool bIsShowRect = true);

	/// @brief 绘制背景图片
	/// @param [out] pDC 绘图资源
	bool DrawBK(CDC* pDC);

	/// @brief 绘制截图
	/// @param [out] pDC 绘图资源
	/// @param [in ] bIsShowRect 是否绘制矩形橡皮筋
	bool DrawScreenShot(CDC* pDC, const bool bIsShowRect = true);

	/// @brief 绘制工具
	/// @param [out] pDC 绘图资源
	bool DrawTool(CDC* pDC) const;

	/// @brief 保存位图
	/// @param [in ] bSaveToFile 是否保存成文件
	/// @param [in ] strFilePath 文件全路径
	bool SavePic(const bool bSaveToFile = false, const CString& strFilePath = _T(""));

	/// @brief 获取所有窗口
	void GetAllWindow(void);

	/// @brief 根据位置获取区域
	/// @param [in ] pt 位置
	/// @param [in ] rc 区域
	bool FindWndByPt(const CPoint& pt, CRect& rc) const;

	/// @brief 修改光标
	/// @param [in ] point 点信息
	void ModifyCusor(const CPoint& point);

	/// @brief 绘制放大镜
	/// @param [in ] bDrawBk 是否显示背景
	void DrawZoom(CDC* pDC);

	/// @brief 绘制放大镜
	/// @param [in ] pBKDC 背景DC
	void DrawZoom(CDC* pDC, CDC* pBKDC);

	/// @brief 枚举子窗口数据
	/// @param [in ] data 窗口数据
	/// @param [in ] pt 位置
	/// @param [in ] rc 区域
	bool EnumChildWndData(const CAjWndData& data, const CPoint& pt, CRect& rc) const;

	/// @brief 遍历子窗口
	static BOOL CALLBACK EnumChildProc(HWND hwnd,LPARAM lParam);

	/// @brief 遍历窗口
	static BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam);

	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();
	virtual BOOL OnInitDialog();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);

	// 菜单命令
	afx_msg void OnMenuRect();
	afx_msg void OnMenuEllipse();
	afx_msg void OnMenuArrow();
	afx_msg void OnMenuBrush();
	afx_msg void OnMenuFont();
	afx_msg void OnMenuUndo();
	afx_msg void OnMenuReselect();
	afx_msg void OnMenuFinnish();
	afx_msg void OnMenuSave();
	afx_msg void OnMenuCancel();
private:
	std::vector<CAjWndData>				m_vecWndData;			//	窗口数据
	std::map<HWND, CAjWndData*>			m_mapWnd;				//	窗口间的关系
	CString								m_strSaveFilePrefix;	//	保存的文件前缀
	CWGScreenshotTool					m_WGScreenShotTool;		//	截图工具栏					
	CWGImage							m_pngMask;				//	遮盖图片
	CBitmap								m_retBitMap;			//	返回的图片
	CToolTipCtrl						m_ToolTip;				//	文字帮助
	CRectColorTracker					m_rectTracker;			//	矩形橡皮筋
	CRect								m_rectOrg;				//	原矩形
	CPoint								m_ptLeftBtn;			//	按下的左键的点
	int									m_xScreen;				//	屏幕的宽度
	int									m_yScreen;				//	屏幕的高度
	int									m_nRectTrackHit;		//	单击矩形框的位置
	HBITMAP								m_screenBmp;			//	全屏的图片
	HCURSOR								m_hColorCursor;			//	有颜色的光标
	HCURSOR								m_hDrawCursor;			//	绘制的光标
	ECLOSEUICODE						m_eCloseUICode;			//	对话框结束返回值
	bool								m_bIsLeftBtnDown;		//	左键是否按下
	bool								m_bIsDrawRect;			//	是否绘制矩形
	bool								m_bIsBanModifyRect;		//	是否禁止修改矩形
};
