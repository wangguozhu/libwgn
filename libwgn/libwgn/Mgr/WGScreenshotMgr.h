#pragma once

#include "WGScreenshotUIHead.h"

class CWGScreenshotUI;

/// @class CWGScreenshotMgr
/// @brief ��ͼ������

class LIBWGN_EXPORT CWGScreenshotMgr
{
public:
	CWGScreenshotMgr(void);
	~CWGScreenshotMgr(void);
public:
	/// @brief ���ñ����ļ���ǰ׺
	/// @param [in ] strSaveFilePrefix �ļ�ǰ׺
	void SetSaveFilePrefix(const char* strSaveFilePrefix);

	/// @brief ִ�н�ͼ
	void DoIt(void);

	/// @brief ��ͼ������ķ���ֵ 
	ECLOSEUICODE GetCloseCode() const;

	/// @brief ��ȡͼƬ
	HBITMAP GetScreenshotBitmap(void) const;
private:
	ULONG_PTR				m_gdiplusToken;
	CWGScreenshotUI*		m_pWScreenshotUI;		// ��ͼUI
};

