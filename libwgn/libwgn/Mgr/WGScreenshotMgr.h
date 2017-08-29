#pragma once

#include "WGScreenshotUIHead.h"

class CWGScreenshotUI;

/// @class CWGScreenshotMgr
/// @brief 截图管理类

class LIBWGN_EXPORT CWGScreenshotMgr
{
public:
	CWGScreenshotMgr(void);
	~CWGScreenshotMgr(void);
public:
	/// @brief 设置保存文件的前缀
	/// @param [in ] strSaveFilePrefix 文件前缀
	void SetSaveFilePrefix(const char* strSaveFilePrefix);

	/// @brief 执行截图
	void DoIt(void);

	/// @brief 截图结束后的返回值 
	ECLOSEUICODE GetCloseCode() const;

	/// @brief 获取图片
	HBITMAP GetScreenshotBitmap(void) const;
private:
	ULONG_PTR				m_gdiplusToken;
	CWGScreenshotUI*		m_pWScreenshotUI;		// 截图UI
};

