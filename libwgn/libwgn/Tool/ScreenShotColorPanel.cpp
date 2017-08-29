// ScreenShotColorPanel.cpp : 实现文件
//

#include "stdafx.h"
#include "ScreenShotColorPanel.h"
#include "afxdialogex.h"

extern HINSTANCE g_resourceInstance;

//-------------------------------------------------------------------------
IMPLEMENT_DYNAMIC(CScreenShotColorPanel, CDialog)

//-------------------------------------------------------------------------
CScreenShotColorPanel::CScreenShotColorPanel(CWnd* pParent /*=NULL*/)
	: CDialog(CScreenShotColorPanel::IDD, pParent)
	, m_nBKColor(RGB(216, 241, 255))
	, m_nPenColor(RGB(80, 143, 191))
	, m_nSpliteColor(RGB(80, 143, 191))
	, m_pCurDrawData(NULL)
	, m_nCurSelect(-1)
	, m_nMoveIndex(-1)
	, m_pObserver(NULL)
{
	// 从上到下 从左到右
	m_vecColor.push_back(RGB(0, 0, 0));
	m_vecColor.push_back(RGB(255, 255, 255));
	m_vecColor.push_back(RGB(128, 128, 128));
	m_vecColor.push_back(RGB(192, 192, 192));
	m_vecColor.push_back(RGB(128, 0, 0));
	m_vecColor.push_back(RGB(255, 0, 0));
	m_vecColor.push_back(RGB(128, 128, 0));
	m_vecColor.push_back(RGB(255, 255, 0));
	m_vecColor.push_back(RGB(0, 128, 0));
	m_vecColor.push_back(RGB(0, 255, 0));
	m_vecColor.push_back(RGB(0, 0, 128));
	m_vecColor.push_back(RGB(0, 0, 255));
	m_vecColor.push_back(RGB(128, 0, 128));
	m_vecColor.push_back(RGB(255, 0, 255));
	m_vecColor.push_back(RGB(0, 128, 128));
	m_vecColor.push_back(RGB(0, 255, 255));

	// 加载图片
	m_hColorCursor = LoadCursor(g_resourceInstance, MAKEINTRESOURCE(IDC_COLOR_ARROW));
	m_ImageFont.LoadFromResource(g_resourceInstance, MAKEINTRESOURCE(IDB_BITMAP_FONT));

	int nId[] = {IDB_BITMAP_SMALL, IDB_BITMAP_MID, IDB_BITMAP_BIG};

	const int nWidth = 24;
	const int nHeight = 29;
	const int nGap = 6;
	int nStartPos = 2;
	int len = sizeof(nId) / sizeof(int);
	for (int i = 0; i < len; ++i)
	{
		CImage* pImage = new CImage();
		pImage->LoadFromResource(g_resourceInstance, MAKEINTRESOURCE(nId[i]));
		m_vecFuncPng.push_back(pImage);

		m_vecFuncRect.push_back(CRect(nStartPos, 3, nStartPos + nWidth, nHeight + 3));

		nStartPos += nWidth + nGap;
	}
}

//-------------------------------------------------------------------------
CScreenShotColorPanel::~CScreenShotColorPanel()
{
	DeleteObject(m_hColorCursor);

	int len = (int)m_vecFuncPng.size();
	for (int i = 0; i < len; ++i)
	{
		delete m_vecFuncPng[i];
		m_vecFuncPng[i] = NULL;
	}
}

//-------------------------------------------------------------------------
void CScreenShotColorPanel::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_FONT, m_comboFont);
}

//-------------------------------------------------------------------------
BEGIN_MESSAGE_MAP(CScreenShotColorPanel, CDialog)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_SETCURSOR()
	ON_CBN_SELCHANGE(IDC_COMBO_FONT, OnCbnSelchangeComboFont)
END_MESSAGE_MAP()

//-------------------------------------------------------------------------
BOOL CScreenShotColorPanel::OnInitDialog()
{
	__super::OnInitDialog();

	CRect rc;
	GetClientRect(rc);

	SetWindowPos(NULL, 0, 0, rc.Width(), 37, SWP_SHOWWINDOW);

	int nIndex = 0;
	m_comboFont.InsertString(nIndex++, _T("8"));
	m_comboFont.InsertString(nIndex++, _T("9"));
	m_comboFont.InsertString(nIndex++, _T("10"));
	m_comboFont.InsertString(nIndex++, _T("11"));
	m_comboFont.InsertString(nIndex++, _T("12"));
	m_comboFont.InsertString(nIndex++, _T("14"));
	m_comboFont.InsertString(nIndex++, _T("16"));
	m_comboFont.InsertString(nIndex++, _T("18"));
	m_comboFont.InsertString(nIndex++, _T("20"));

	m_comboFont.SetCurSel(4);

	m_comboFont.SetWindowPos(NULL, 24, 4, 0, 0, SWP_NOSIZE);

	return TRUE;
}

//-------------------------------------------------------------------------
void CScreenShotColorPanel::OnPaint()
{
	CPaintDC dc(this);

	if (NULL == m_pCurDrawData)
	{
		return;
	}

	CRect rc;
	GetClientRect(rc);

	CDC dcCompatible;
	dcCompatible.CreateCompatibleDC(&dc);

	CBitmap mCompatibleBmp;
	mCompatibleBmp.CreateCompatibleBitmap(&dc, rc.Width(), rc.Height());

	HBITMAP hOldBitmap = (HBITMAP)dcCompatible.SelectObject(&mCompatibleBmp);

	CBrush brush(m_nBKColor);
	dcCompatible.FillRect(rc, &brush);
	brush.DeleteObject();

	int nStartPos = 0;
	switch (m_pCurDrawData->eToolType)
	{
	case eDrawFont:
		DrawFontBar(&dcCompatible, nStartPos);
		break;
	default:
		DrawSizeBar(&dcCompatible, nStartPos);
		break;
	}

	DrawColorBar(&dcCompatible, nStartPos);

	dc.BitBlt(0, 0, rc.Width(), rc.Height(), &dcCompatible, 0, 0, SRCCOPY);

	dcCompatible.SelectObject(hOldBitmap);

	mCompatibleBmp.DeleteObject();

	dcCompatible.DeleteDC();
}

//-------------------------------------------------------------------------
BOOL CScreenShotColorPanel::OnEraseBkgnd(CDC* pDC)
{
	return TRUE;
}

//-------------------------------------------------------------------------
void CScreenShotColorPanel::DrawColorBar(CDC* pDC, const int nStartPos)
{// width 168
	if (NULL == pDC
		|| NULL == m_pCurDrawData)
	{
		return;
	}

	int nDrawStartPos = nStartPos;
	const int nGap1 = 4, nTopGap = 2, nGap2 = 1, nPenWidth = 1;
	const int nBigRectWidth = 28, nNormalRect = 13, nSelectrRect = 15, nTop = 4;
	// 绘制大矩形
	CPen pen;
	pen.CreatePen(PS_SOLID, nPenWidth, m_nPenColor);
	CPen* pOldPen = pDC->SelectObject(&pen);

	// 空白画刷
	CBrush *pBrush = CBrush::FromHandle((HBRUSH)GetStockObject(NULL_BRUSH));
	CBrush *pOldBrush = pDC->SelectObject(pBrush);

	pDC->Rectangle(CRect(nDrawStartPos, nTop, nDrawStartPos + nBigRectWidth, nTop + nBigRectWidth));

	DrawColor(pDC, nDrawStartPos + nGap2 * 2, nTop + nGap2 * 2, nBigRectWidth - nGap2 * 4, m_pCurDrawData->color);

	nDrawStartPos += nBigRectWidth + nGap1;

	// 绘制面板
	bool bAddRect = (0 == m_vecColorRect.size());
	int len = (int)m_vecColor.size();
	const int nColorRect = nNormalRect - nGap2 * 2;
	for (int i = 0; i < len; i += 2)
	{
		// 上
		int nTempTop = nTop;
		CRect rc(nDrawStartPos, nTempTop, nDrawStartPos + nNormalRect, nTempTop + nNormalRect);
		if (bAddRect)
		{
			m_vecColorRect.push_back(rc);
		}

		if (i == m_nCurSelect)
		{
			rc = CRect(nDrawStartPos - 1, nTempTop - 1, nDrawStartPos + nSelectrRect - 1, nTempTop + nSelectrRect - 1);
		}

		pDC->Rectangle(rc);
		DrawColor(pDC, nDrawStartPos + nGap2, nTempTop + nGap2, nColorRect, m_vecColor[i]);

		// 下
		nTempTop += nNormalRect + nTopGap;
		rc = CRect(nDrawStartPos, nTempTop, nDrawStartPos + nNormalRect, nTempTop + nNormalRect);
		if (bAddRect)
		{
			m_vecColorRect.push_back(rc);
		}

		if (i + 1 == m_nCurSelect)
		{
			rc = CRect(nDrawStartPos - 1, nTempTop - 1, nDrawStartPos + nSelectrRect - 1, nTempTop + nSelectrRect - 1);
		}

		pDC->Rectangle(rc);
		DrawColor(pDC, nDrawStartPos + nGap2, nTempTop + nGap2, nColorRect, m_vecColor[i + 1]);

		nDrawStartPos += nNormalRect + nGap1;
	}

	pDC->SelectObject(pOldBrush);
	pBrush->DeleteObject();

	pDC->SelectObject(pOldPen);
	pen.DeleteObject();
}

//-------------------------------------------------------------------------
void CScreenShotColorPanel::DrawColor(CDC* pDC, const int nX, const int nY, const int nWidth, const COLORREF& nColor)
{
	if (NULL == pDC)
	{
		return;
	}

	CPen pen;
	pen.CreatePen(PS_SOLID, 1, nColor);
	CPen* pOldPen = pDC->SelectObject(&pen);

	CBrush brush;
	brush.CreateSolidBrush(nColor);
	CBrush *pOldBrush = pDC->SelectObject(&brush);

	pDC->Rectangle(CRect(nX, nY, nX + nWidth, nY + nWidth));

	pDC->SelectObject(pOldBrush);
	brush.DeleteObject();
	
	pDC->SelectObject(pOldPen);
	pen.DeleteObject();
}

//-------------------------------------------------------------------------
void CScreenShotColorPanel::OnMouseMove(UINT nFlags, CPoint point)
{
	m_nCurSelect = GetColorIndex(point);

	m_nMoveIndex = GetSizeIndex(point);

	Invalidate();

	return __super::OnMouseMove(nFlags, point);
}

//-------------------------------------------------------------------------
void CScreenShotColorPanel::OnLButtonDown(UINT nFlags, CPoint point)
{
	if (-1 != m_nCurSelect
		&& NULL != m_pCurDrawData)
	{
		m_pCurDrawData->color = m_vecColor[m_nCurSelect];
	}
	else if (NULL != m_pCurDrawData)
	{
		int nIndex = GetSizeIndex(point);
		if (-1 != nIndex)
		{
			m_pCurDrawData->ePaintSize = (EPAINTSIZE)nIndex;
		}
	}

	Invalidate();

	return __super::OnLButtonDown(nFlags, point);
}

//-------------------------------------------------------------------------
int CScreenShotColorPanel::GetColorIndex(const CPoint& point) const
{
	int len = (int)m_vecColorRect.size();
	for (int i = 0; i < len; ++i)
	{
		if (m_vecColorRect[i].PtInRect(point))
		{
			return i;
		}
	}

	return -1;
}

//-------------------------------------------------------------------------
void CScreenShotColorPanel::Release(void)
{
	int len = (int)m_vecFuncPng.size();
	for (int i = 0; i < len; ++i)
	{
		delete m_vecFuncPng[i];
		m_vecFuncPng[i] = NULL;
	}
}

//-------------------------------------------------------------------------
void CScreenShotColorPanel::DrawFontBar(CDC* pDC, int& nStartPos)
{// width 74
	if (NULL == pDC)
	{
		return;
	}

	m_ImageFont.Draw(pDC->GetSafeHdc(), CRect(nStartPos, 4, nStartPos + 24, 25), CRect(24, 0, 48, 22));

	CRect rc;
	m_comboFont.GetWindowRect(rc);
	ScreenToClient(rc);

	CDC* pFontDC = m_comboFont.GetDC();
	pDC->BitBlt(rc.left, rc.top, rc.Width(), rc.Height(), pFontDC, 0, 0, SRCCOPY);

	ReleaseDC(pFontDC);

	nStartPos += 24 + rc.Width() + 5;
}

//-------------------------------------------------------------------------
CRect CScreenShotColorPanel::GetNormalRect(void) const
{
	return CRect(24, 0, 48, 29);
}

//-------------------------------------------------------------------------
CRect CScreenShotColorPanel::GetSelectRect(void) const
{
	return CRect(72, 0, 96, 29);
}

//-------------------------------------------------------------------------
CRect CScreenShotColorPanel::GetMoveRect(void) const
{
	return CRect(48, 0, 72, 29);
}

//-------------------------------------------------------------------------
void CScreenShotColorPanel::SetDrawData(DRAWDATA* pDrawData)
{
	m_pCurDrawData = pDrawData;
}

//-------------------------------------------------------------------------
void CScreenShotColorPanel::UpdataUI(void)
{
	if (NULL == m_pCurDrawData)
	{
		return;
	}

	switch (m_pCurDrawData->eToolType)
	{
	case eDrawFont:
		m_comboFont.ShowWindow(SW_SHOW);
		SetWindowPos(NULL, 0, 0, 242, 37, SWP_SHOWWINDOW | SWP_NOMOVE);
		break;
	default:
		m_comboFont.ShowWindow(SW_HIDE);
		SetWindowPos(NULL, 0, 0, 267, 37, SWP_SHOWWINDOW | SWP_NOMOVE);
		break;
	}

	Invalidate();
}

//-------------------------------------------------------------------------
void CScreenShotColorPanel::DrawSizeBar(CDC* pDC, int& nStartPos)
{// width 99
	if (NULL == pDC
		|| NULL == m_pCurDrawData)
	{
		return;
	}

	ASSERT(m_vecFuncPng.size() == m_vecFuncRect.size());

	const int nTop = 3, nHeight = 29, nGap = 6;
	int len = (int)m_vecFuncPng.size();
	for (int i = 0; i < len; ++i)
	{
		CRect rcSelect = GetNormalRect();
		if (m_pCurDrawData->ePaintSize == i)
		{
			rcSelect = GetSelectRect();
		}
		else if (m_nMoveIndex == i)
		{
			rcSelect = GetMoveRect();
		}

		CRect rcPos = m_vecFuncRect[i];
		m_vecFuncPng[i]->Draw(pDC->GetSafeHdc(), rcPos, rcSelect);

		nStartPos = rcPos.right;
	}

	// 绘制分割符
	nStartPos += nGap;
	
	CPen pen;
	pen.CreatePen(PS_SOLID, 1, m_nSpliteColor);

	CPen* pOldPen = pDC->SelectObject(&pen);

	pDC->MoveTo(CPoint(nStartPos, nTop));
	pDC->LineTo(CPoint(nStartPos, nHeight + nTop));

	pDC->SelectObject(pOldPen);

	pen.DeleteObject();

	nStartPos += nGap;
}

//-------------------------------------------------------------------------
int CScreenShotColorPanel::GetSizeIndex(const CPoint& point) const
{
	if (NULL != m_pCurDrawData
		&& eDrawFont != m_pCurDrawData->eToolType)
	{
		int len = (int)m_vecFuncRect.size();
		for (int i = 0; i < len; ++i)
		{
			if (m_vecFuncRect[i].PtInRect(point))
			{
				return i;
			}
		}
	}
	return -1;
}

//-------------------------------------------------------------------------
void CScreenShotColorPanel::OnCbnSelchangeComboFont()
{
	if (NULL == m_pCurDrawData)
	{
		return;
	}

	CString strValue;
	m_comboFont.GetLBText(m_comboFont.GetCurSel(), strValue);

	m_pCurDrawData->nFontSize = _tstoi(strValue);

	if (NULL != m_pObserver)
	{
		m_pObserver->ModifyData();
	}
}

//-------------------------------------------------------------------------
BOOL CScreenShotColorPanel::PreTranslateMessage(MSG* pMsg)
{
	CWnd* pParentWnd = GetParent();
	if (NULL != pParentWnd)
	{
		pParentWnd->PreTranslateMessage(pMsg);
	}

	return __super::PreTranslateMessage(pMsg);
}

//-------------------------------------------------------------------------
void CScreenShotColorPanel::SetObserver(CWGColorPanelObserver* pObserver)
{
	m_pObserver = pObserver;
}

//-------------------------------------------------------------------------
BOOL CScreenShotColorPanel::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
	//设置彩色光标
	SetCursor(m_hColorCursor);
	return TRUE;
}
