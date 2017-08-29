#pragma once
#include "afxext.h"
class CRectColorTracker : public CRectTracker
{
public:
	CRectColorTracker(void);
	~CRectColorTracker(void);
public:
	/// @brief 获取线的颜色
	const DWORD& GetLineCorlor(void) const;

	/// @brief 设置线颜色
	/// @param [in ] nColor 颜色
	void SetLineColor(const DWORD& nColor);

	/// @brief 获取矩形的颜色
	const DWORD& GetResizeColor(void) const;

	/// @brief 设置矩形颜色
	/// @param [in ] nColor 颜色
	void SetResizeColor(const DWORD& nColor);

	/// @brief 绘制
	/// @param [out] pDC 绘图的上下文 
	void Draw(CDC* pDC) const;
private:
	DWORD			m_nLineColor;					//	线颜色
	DWORD			m_nResizeClolor;				//	小矩形框的颜色
};

