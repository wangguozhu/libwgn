#include "StdAfx.h"
#include "WGScreenshotMgr.h"
#include "WGScreenshotUI.h"

//-------------------------------------------------------------------------
CWGScreenshotMgr::CWGScreenshotMgr(void)
{
	Gdiplus::GdiplusStartupInput gdiplusStartupInput;
	Gdiplus::GdiplusStartup(&m_gdiplusToken, &gdiplusStartupInput, NULL);

	m_pWScreenshotUI = new CWGScreenshotUI();
}

//-------------------------------------------------------------------------
CWGScreenshotMgr::~CWGScreenshotMgr(void)
{
	delete m_pWScreenshotUI;
	m_pWScreenshotUI = NULL;

	Gdiplus::GdiplusShutdown(m_gdiplusToken);
}

//-------------------------------------------------------------------------
void CWGScreenshotMgr::DoIt(void)
{
	if (NULL != m_pWScreenshotUI)
	{
		m_pWScreenshotUI->DoModal();
	}
}

//-------------------------------------------------------------------------
ECLOSEUICODE CWGScreenshotMgr::GetCloseCode() const
{
	if (NULL != m_pWScreenshotUI)
	{
		return m_pWScreenshotUI->GetCloseCode();
	}

	return eUIUnKnow;
}

//-------------------------------------------------------------------------
HBITMAP CWGScreenshotMgr::GetScreenshotBitmap(void) const
{
	if (NULL != m_pWScreenshotUI)
	{
		return m_pWScreenshotUI->GetScreenshotBitmap();
	}

	return NULL;
}

//-------------------------------------------------------------------------
void CWGScreenshotMgr::SetSaveFilePrefix(const char* strSaveFilePrefix)
{
	if (NULL != m_pWScreenshotUI)
	{
		return m_pWScreenshotUI->SetSaveFilePrefix(strSaveFilePrefix);
	}
}
