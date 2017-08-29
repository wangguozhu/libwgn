#pragma once


/// @class CAjEdit
/// @brief 自定义编辑框

class CAjEdit : public CEdit
{
	DECLARE_DYNAMIC(CAjEdit)

public:
	CAjEdit();
	virtual ~CAjEdit();
public:
	/// @brief 设置可视区域
	/// @param [in ] rc 可视区域
	void SetShowRect(const CRect& rc);

	/// @brief 更新父DC
	void UpDateDC(void);
protected:
	DECLARE_MESSAGE_MAP()

	/// @brief 背景画刷
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);

	/// @brief 鼠标左键
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);

	/// @brief 鼠标移动
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);

	/// @brief 画刷
	afx_msg HBRUSH CtlColor(CDC* /*pDC*/, UINT /*nCtlColor*/);

	afx_msg UINT OnGetDlgCode();

	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);

	afx_msg void OnEnChange();

	afx_msg void OnPaint();
private:
	/// @brief 计算位置
	void CalcPos(void);

	/// @brief 释放
	void Release(void);
private:
	CDC				m_dcParent;			//	父资源的DC
	CBitmap			m_dcBmp;			//	资源图片
	CBitmap*		m_pOldBmp;			//	父资源的旧的图片
	CRect			m_ShowRect;			//	可视区域
	CPoint			m_LBtnPt;			//	左键按钮
};