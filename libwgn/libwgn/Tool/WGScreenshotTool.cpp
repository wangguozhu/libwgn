// AjScreenshotTool.cpp : 实现文件
//

#include "stdafx.h"
#include "WGScreenshotTool.h"
#include "WGGlobalTool.h"
#include <math.h>

extern HINSTANCE g_resourceInstance;

//-------------------------------------------------------------------------
CWGScreenshotTool::CWGScreenshotTool()
	: m_nWidth(285)
	, m_nHeight(28)
	, m_orgRect(0, 0, 0, 0)
	, m_showRect(0, 0, 0, 0)
	, m_startPt(0, 0)
	, m_bIsCanDraw(false)
	, m_pObserver(NULL)
	, m_nFontProcess(-1)
	, m_pWnd(NULL)
	, m_bIsPosOnTop(false)
	, m_nPos(1)
{
	m_FontTracker.m_nStyle = CRectTracker::dottedLine;
	m_FontTracker.m_rect.SetRect(-1, -1, -1, -1);
}

//-------------------------------------------------------------------------
CWGScreenshotTool::~CWGScreenshotTool()
{
	Release();
}

//-------------------------------------------------------------------------
void CWGScreenshotTool::Init(void)
{
	m_pngBk.LoadFromResource(g_resourceInstance, MAKEINTRESOURCE(IDB_BITMAP_BK));

	int nId[] = {IDB_BITMAP_RECT, IDB_BITMAP_ELLIPSE, IDB_BITMAP_ARROW, 
		IDB_BITMAP_BRUSH, IDB_BITMAP_FONT, IDB_BITMAP_UNDO, IDB_BITMAP_SAVE,
		IDB_BITMAP_CANCEL, IDB_BITMAP_OK};

	const int nWidth = 24;
	const int nHeight = 25;
	int nStartPos = 2;
	int len = sizeof(nId) / sizeof(int);
	for (int i = 0; i < len; ++i)
	{
		CImage* pImage = new CImage();
		pImage->LoadFromResource(g_resourceInstance, MAKEINTRESOURCE(nId[i]));
		m_vecFuncPng.push_back(pImage);

		if (5 == i)
		{
			nStartPos += 9;
		}
		else if (7 == i)
		{
			nStartPos += 10;
		}
		else
		{
			nStartPos += 2;
		}

		if (i < len -1)
		{
			m_vecFuncRect.push_back(CRect(nStartPos, 3, nStartPos + nWidth, nHeight));
		}
		else
		{
			m_vecFuncRect.push_back(CRect(nStartPos, 3, nStartPos + 51, nHeight));
		}

		nStartPos += nWidth;
	}

	// 工具栏的默认值
	RECORDDATA drawData;
	drawData.ePaintSize = eDrawMid;
	m_mapRecordData[eDrawRect] = drawData;
	m_mapRecordData[eDrawEllipse] = drawData;
	m_mapRecordData[eDrawArrow] = drawData;

	drawData.ePaintSize = eDrawBig;
	m_mapRecordData[eDrawBrush] = drawData;

	drawData.nFontSize = 12;
	m_mapRecordData[eDrawFont] = drawData;
}

//-------------------------------------------------------------------------
void CWGScreenshotTool::Release(void)
{
	int len = (int)m_vecFuncPng.size();
	for (int i = 0; i < len; ++i)
	{
		delete m_vecFuncPng[i];
		m_vecFuncPng[i] = NULL;
	}

	m_Font.DeleteObject();
}

//-------------------------------------------------------------------------
void CWGScreenshotTool::ScreenToClient(CPoint& pt) const
{
	pt -= m_startPt;
}

//-------------------------------------------------------------------------
bool CWGScreenshotTool::Draw(CDC* pDC, const bool bIsShowPanel)
{
	if (NULL == pDC
		|| !m_bIsCanDraw)
	{
		return false;
	}

	ASSERT(m_vecFuncPng.size() == m_vecFuncRect.size());

	// 绘制记录
	DrawHistory(pDC);

	if (bIsShowPanel)
	{
		// 绘制背景
		m_pngBk.Draw(pDC->GetSafeHdc(), m_startPt);

		// 绘制功能
		int nWidth = 24;
		int len = (int)m_vecFuncPng.size();
		ASSERT(len == m_vecFuncRect.size());
		for (int i = 0; i < len; ++i)
		{
			CRect rc(m_vecFuncRect[i]);
			ClientToScreen(rc);

			if (i == len - 1)
			{
				nWidth = 51;
			}

			CRect rcPng;
			if (i == (int)(m_curDrawData.eToolType - 1))
			{
				rcPng = GetSelectRect(nWidth);
			}
			else
			{
				if (rc.PtInRect(m_movePt))
				{
					rcPng = GetMoveRect(nWidth);
				}
				else
				{
					rcPng = GetOrgRect(nWidth);
				}
			}

			m_vecFuncPng[i]->Draw(pDC->GetSafeHdc(), rc, rcPng);
		}

		if (eDrawFont != m_curDrawData.eToolType)
		{// 绘制当前数据
			Draw(pDC, m_curDrawData);
		}

		if (m_FontEdit.IsWindowVisible())
		{// 绘制当前Edit数据
			CRect rcEdit;
			m_FontEdit.GetWindowRect(rcEdit);
			m_pWnd->ScreenToClient(rcEdit);

			CDC* pEditDC = m_FontEdit.GetDC();
			pDC->BitBlt(rcEdit.left, rcEdit.top, rcEdit.Width(), rcEdit.Height(), pEditDC, 0, 0, SRCCOPY);
			m_FontEdit.ReleaseDC(pEditDC);
		}
	}

	return true;
}

//-------------------------------------------------------------------------
void CWGScreenshotTool::setIsCanDraw(bool bTrue, const CRect& orgRect, const CRect& showRect)
{
	m_bIsCanDraw = bTrue;

	m_orgRect = orgRect;

	m_showRect = showRect;

	CalcStartPos();

	ShowColorPanel();

	m_FontEdit.SetShowRect(showRect);
}

//-------------------------------------------------------------------------
void CWGScreenshotTool::CalcStartPos(void)
{
	// 计算起始位置的Y坐标
	m_bIsPosOnTop = false;
	const int nGap = 5;
	int nHeight = GetActualHeight() + nGap;
	if (nHeight < m_orgRect.bottom - m_showRect.bottom)
	{// 底部
		m_nPos = 1;

		m_startPt.y = m_showRect.bottom + nGap;
	}
	else if (nHeight < m_showRect.top - m_orgRect.top)
	{// 顶部
		m_nPos = 2;

		m_startPt.y = m_showRect.top - nGap - m_nHeight;
		m_bIsPosOnTop = true;
	}
	else
	{// 可视区域内
		m_nPos = 3;

		m_startPt.y  = m_showRect.top;
	}

	// 计算起始位置的X坐标
	int nWidth = GetActualWidth();
	if (nWidth < m_showRect.right - m_orgRect.left)
	{// 右边对齐
		m_startPt.x = m_showRect.right - nWidth;
	}
	else
	{// 左边对齐
		m_startPt.x = m_orgRect.left;
	}
}

//-------------------------------------------------------------------------
int CWGScreenshotTool::GetPos(void) const
{
	return m_nPos;
}

//-------------------------------------------------------------------------
void CWGScreenshotTool::OnLbButtonDown(UINT nFlags, CPoint point)
{
	if (m_FontEdit.IsWindowVisible())
	{
		CRect rcEdit;
		m_FontEdit.GetWindowRect(rcEdit);

		m_pWnd->ScreenToClient(rcEdit);
		if (!rcEdit.PtInRect(point))
		{
			ShowFontEdit(false);
		}
	}

	int len = (int)m_vecFuncRect.size();
	for (int i = 0; i < len; ++i)
	{
		CRect rc(m_vecFuncRect[i]);
		ClientToScreen(rc);

		if (rc.PtInRect(point))
		{
			ETOOLTYPE eToolType = ETOOLTYPE(i+1);
			TriggerFunc(eToolType);

			break;
		}
	}

	m_curDrawData.vecPt.clear();
	if (eUnknow != m_curDrawData.eToolType
		&& m_showRect.PtInRect(point))
	{
		m_curDrawData.vecPt.clear();
		m_curDrawData.vecPt.push_back(GetClosePt(point));
	}
}

//-------------------------------------------------------------------------
void CWGScreenshotTool::OnLbButtonUp(UINT nFlags, CPoint point)
{
	if (eUnknow != m_curDrawData.eToolType
		&& 0 < m_curDrawData.vecPt.size())
	{
		switch (m_curDrawData.eToolType)
		{
		case eDrawFont:
			{
				m_nFontProcess = 1;
				CPoint ptS(m_curDrawData.vecPt[0]), ptE(point);
				if (1 < m_curDrawData.vecPt.size())
				{
					ptE = m_curDrawData.vecPt[1];
				}

				CSize size(0, 0);
				CFont font;
				if(font.CreatePointFont(m_curDrawData.nFontSize * 10, _T("宋体")))
				{
					CDC* pDC = m_pWnd->GetDC();
					CFont* pOldFont = pDC->SelectObject(&font);

					size = pDC->GetTextExtent(_T("体"));

					pDC->SelectObject(pOldFont);
					font.DeleteObject();
				}


				if (ptE.x - ptS.x < size.cx)
				{
					ptE.x = ptS.x + size.cx;
				}

				if (ptE.y - ptS.y < size.cy)
				{
					ptE.y = ptS.y + size.cy * 2;
				}

				if (1 < m_curDrawData.vecPt.size())
				{
					m_curDrawData.vecPt[1] = ptE;
				}
				else
				{
					m_curDrawData.vecPt.push_back(ptE);
				}

				ShowFontEdit(true);
			}
			break;
		default:
			if (1 < m_curDrawData.vecPt.size())
			{
				m_lsDrawData.push_back(m_curDrawData);
				m_curDrawData.vecPt.clear();
			}
			break;
		}
	}
}

//-------------------------------------------------------------------------
void CWGScreenshotTool::OnMouseMove(UINT nFlags, CPoint point)
{
	m_movePt = point;
	if (nFlags == MK_LBUTTON)
	{// 左键按下
		if (m_curDrawData.vecPt.size() < 1)
		{
			return;
		}

		switch (m_curDrawData.eToolType)
		{
		case eDrawRect:
		case eDrawEllipse:
		case eDrawArrow:
		case eDrawFont:
			{
				if (1 < m_curDrawData.vecPt.size())
				{
					m_curDrawData.vecPt[1] = GetClosePt(point);
				}
				else
				{
					m_curDrawData.vecPt.push_back(GetClosePt(point));
				}

				ShowFontEdit(true);
			}
			break;
		case eDrawBrush:
			m_curDrawData.vecPt.push_back(GetClosePt(point));
			break;
		default:
			break;
		}
	}
}

//-------------------------------------------------------------------------
int CWGScreenshotTool::GetActualWidth(void) const
{
	int nWidth = m_nWidth;
	if (NULL != m_screenShotColorPanel.m_hWnd
		&& m_screenShotColorPanel.IsWindowVisible())
	{
		CRect rc;
		m_screenShotColorPanel.GetWindowRect(rc);
		if (nWidth < rc.Width())
		{
			nWidth = rc.Width();
		}
	}

	return nWidth;
}

//-------------------------------------------------------------------------
int CWGScreenshotTool::GetActualHeight(void) const
{	
	int nHeight = m_nHeight;
	if (NULL != m_screenShotColorPanel.m_hWnd
		&& m_screenShotColorPanel.IsWindowVisible())
	{
		CRect rc;
		m_screenShotColorPanel.GetWindowRect(rc);
		nHeight += rc.Height();
	}

	return nHeight;
}

//-------------------------------------------------------------------------
void CWGScreenshotTool::ClientToScreen(CRect& rc) const
{
	rc.left += m_startPt.x;
	rc.right += m_startPt.x;
	rc.top += m_startPt.y;
	rc.bottom += m_startPt.y;
}

//-------------------------------------------------------------------------
CRect CWGScreenshotTool::GetSelectRect(const int nWidth) const
{
	return CRect(nWidth * 3, 0, nWidth * 4, 22);
}

//-------------------------------------------------------------------------
CRect CWGScreenshotTool::GetOrgRect(const int nWidth) const
{
	return CRect(nWidth, 0, nWidth * 2, 22);
}

//-------------------------------------------------------------------------
CRect CWGScreenshotTool::GetMoveRect(const int nWidth) const
{
	return CRect(0, 0, nWidth, 22);
}

//-------------------------------------------------------------------------
void CWGScreenshotTool::Draw(CDC* pDC, const DRAWDATA& drawData) const
{
	if (NULL == pDC)
	{
		return;
	}

	switch (drawData.eToolType)
	{
	case eDrawRect:
		DrawRect(pDC, drawData);
		break;
	case eDrawEllipse:
		DrawEllipse(pDC, drawData);
		break;
	case eDrawArrow:
		DrawArrow(pDC, drawData);
		break;
	case eDrawBrush:
		DrawReBrush(pDC, drawData);
		break;
	case eDrawFont:
		pDC->SetBkMode(TRANSPARENT);
		DrawFont(pDC, drawData);
		break;
	default:
		break;
	}
}

//-------------------------------------------------------------------------
void CWGScreenshotTool::DrawHistory(CDC* pDC)
{
	std::list<DRAWDATA>::iterator itEnd = m_lsDrawData.end();
	for (std::list<DRAWDATA>::iterator itBegin = m_lsDrawData.begin(); itBegin != itEnd; ++itBegin)
	{
		Draw(pDC, *itBegin);
	}
}

//-------------------------------------------------------------------------
void CWGScreenshotTool::DrawRect(CDC* pDC, const DRAWDATA& drawData) const
{
	if (NULL == pDC)
	{
		return;
	}

	if (drawData.vecPt.size() < 2)
	{
		return;
	}

	CPoint ptS(drawData.vecPt[0]), ptE(drawData.vecPt[1]);

	int nPenSize = 1;
	switch (drawData.ePaintSize)
	{
	case eDrawMid:
		nPenSize = 3;
		break;
	case eDrawBig:
		nPenSize = 6;
		break;
	default:
		break;
	}

	CPen pen;
	pen.CreatePen(PS_SOLID, nPenSize, drawData.color);

	CPen* pOldPen = pDC->SelectObject(&pen);

	CBrush *pBrush = CBrush::FromHandle((HBRUSH)GetStockObject(NULL_BRUSH));
	CBrush *pOldBrush = pDC->SelectObject(pBrush);

	pDC->Rectangle(CRect(ptS, ptE));

	pDC->SelectObject(pOldBrush);
	pBrush->DeleteObject();

	pDC->SelectObject(pOldPen);
	pen.DeleteObject();
}

//-------------------------------------------------------------------------
void CWGScreenshotTool::DrawEllipse(CDC* pDC, const DRAWDATA& drawData) const
{
	if (NULL == pDC)
	{
		return;
	}

	if (drawData.vecPt.size() < 2)
	{
		return;
	}

	CPoint ptS(drawData.vecPt[0]), ptE(drawData.vecPt[1]);

	int nPenSize = 1;
	switch (drawData.ePaintSize)
	{
	case eDrawMid:
		nPenSize = 3;
		break;
	case eDrawBig:
		nPenSize = 6;
		break;
	default:
		break;
	}

	CPen pen;
	pen.CreatePen(PS_SOLID, nPenSize, drawData.color);

	CPen* pOldPen = pDC->SelectObject(&pen);

	CBrush *pBrush = CBrush::FromHandle((HBRUSH)GetStockObject(NULL_BRUSH));
	CBrush *pOldBrush = pDC->SelectObject(pBrush);

	pDC->Ellipse(CRect(ptS, ptE));

	pDC->SelectObject(pOldBrush);
	pBrush->DeleteObject();

	pDC->SelectObject(pOldPen);
	pen.DeleteObject();
}

//-------------------------------------------------------------------------
void CWGScreenshotTool::DrawArrow(CDC* pDC, const DRAWDATA& drawData) const
{
	if (NULL == pDC)
	{
		return;
	}

	if (drawData.vecPt.size() < 2)
	{
		return;
	}

	CPoint ptS(drawData.vecPt[0]), ptE(drawData.vecPt[1]);

	int nPenSize = 1;
	switch (drawData.ePaintSize)
	{
	case eDrawMid:
		nPenSize = 3;
		break;
	case eDrawBig:
		nPenSize = 6;
		break;
	default:
		break;
	}

	// 绘制箭头
	const double dPI = 3.1415926;
	const double dAngle = dPI / 12;		//	30°
	double dArrowLength = 20;

	double vecX = ptE.x - ptS.x, vecY = ptE.y - ptS.y;
	double dLength = sqrt((double)(vecX * vecX + vecY * vecY));
	vecX /= dLength;
	vecY /= dLength;

	if (dLength < 10)
	{
		return;
	}

	if (dLength < dArrowLength)
	{
		dArrowLength = dLength;
	}

	double dDif = dLength - dArrowLength;
	CPoint ptE1(ptS.x + (int)(vecX * dDif), ptS.y + (int)(vecY * dDif));

	double dWdith = tan(dAngle) * dArrowLength;
	int vecPrepX = (int)(-vecY * dWdith), vecPrepY = (int)(vecX * dWdith);
	CPoint pt1(ptE1.x + vecPrepX, ptE1.y + vecPrepY), pt2(ptE1.x - vecPrepX, ptE1.y - vecPrepY);

	CPoint pt[3];
	pt[0] = pt1;
	pt[1] = pt2;
	pt[2] = ptE;

	CPen pen;
	pen.CreatePen(PS_SOLID, nPenSize, drawData.color);

	CPen* pOldPen = pDC->SelectObject(&pen);

	CBrush brush(drawData.color);
	CBrush *pOldBrush = pDC->SelectObject(&brush);

	pDC->Polygon(pt, 3);

	pDC->MoveTo(ptS);
	pDC->LineTo(ptE1);

	pDC->SelectObject(pOldBrush);
	brush.DeleteObject();

	pDC->SelectObject(pOldPen);
	pen.DeleteObject();
}

//-------------------------------------------------------------------------
void CWGScreenshotTool::DrawReBrush(CDC* pDC, const DRAWDATA& drawData) const
{
	if (NULL == pDC)
	{
		return;
	}

	int nPenSize = 1;
	switch (drawData.ePaintSize)
	{
	case eDrawMid:
		nPenSize = 3;
		break;
	case eDrawBig:
		nPenSize = 6;
		break;
	default:
		break;
	}

	CPen pen;
	pen.CreatePen(PS_SOLID, nPenSize, drawData.color);

	CPen* pOldPen = pDC->SelectObject(&pen);

	int len = (int)drawData.vecPt.size();
	if (0 < len)
	{
		pDC->MoveTo(drawData.vecPt[0]);

		for (int i = 1; i < len; ++i)
		{
			pDC->LineTo(drawData.vecPt[i]);
		}
	}

	pDC->SelectObject(pOldPen);
	pen.DeleteObject();
}

//-------------------------------------------------------------------------
void CWGScreenshotTool::DrawFont(CDC* pDC, const DRAWDATA& drawData) const
{
	if (NULL == pDC)
	{
		return;
	}

	if (drawData.vecPt.size() < 2)
	{
		return;
	}

	CPoint ptS(drawData.vecPt[0]), ptE(drawData.vecPt[1]);

	CFont font;
	if(!font.CreatePointFont(drawData.nFontSize * 10, _T("宋体")))
	{
		return;
	}

	COLORREF oldTextColor = pDC->SetTextColor(drawData.color);

	CFont* pOldFont = pDC->SelectObject(&font);

	pDC->DrawText(drawData.strText, CRect(ptS, ptE), DT_EDITCONTROL | DT_WORDBREAK);

	pDC->SetTextColor(oldTextColor);

	pDC->SelectObject(pOldFont);
	font.DeleteObject();
}

//-------------------------------------------------------------------------
void CWGScreenshotTool::Undo(void)
{
	if (0 < m_lsDrawData.size())
	{
		m_lsDrawData.pop_back();
	}
}

//-------------------------------------------------------------------------
void CWGScreenshotTool::SetObserver(CWGScreenShotToolObserver* pOberver)
{
	m_pObserver = pOberver;
}

//-------------------------------------------------------------------------
CPoint CWGScreenshotTool::GetClosePt(const CPoint& pt)
{
	if (m_showRect.PtInRect(pt))
	{
		return pt;
	}

	CPoint ptDest(pt);
	if (ptDest.x < m_showRect.left)
	{
		ptDest.x = m_showRect.left;
	}
	else if (m_showRect.right < ptDest.x)
	{
		ptDest.x = m_showRect.right;
	}

	if (ptDest.y < m_showRect.top)
	{
		ptDest.y = m_showRect.top;
	}
	else if (m_showRect.bottom < ptDest.y)
	{
		ptDest.y = m_showRect.bottom;
	}

	return ptDest;
}

//-------------------------------------------------------------------------
void CWGScreenshotTool::Create(CWnd* pParentWnd)
{
	m_pWnd = pParentWnd;

	Init();

	m_screenShotColorPanel.SetDrawData(&m_curDrawData);
	m_screenShotColorPanel.SetObserver(this);

	m_FontEdit.Create(WS_CHILD | ES_MULTILINE | ES_WANTRETURN, CRect(0, 0, 0, 0), pParentWnd, WM_USER + 1);
	if(m_Font.CreatePointFont(120, _T("宋体")))
	{
		m_FontEdit.SetFont(&m_Font);
	}
}

//-------------------------------------------------------------------------
void CWGScreenshotTool::ShowColorPanel()
{
	bool bShowPanel = false;
	switch (m_curDrawData.eToolType)
	{
	case eDrawRect:
	case eDrawEllipse:
	case eDrawBrush:
	case eDrawArrow:
		m_nFontProcess = -1;
		bShowPanel = true;
		break;
	case eDrawFont:
		if (-1 == m_nFontProcess)
		{
			m_nFontProcess = 0;
		}
		bShowPanel = true;
		break;
	default:
		break;
	}

	if (bShowPanel)
	{
		if (NULL == m_screenShotColorPanel.m_hWnd)
		{
			m_screenShotColorPanel.Create(CScreenShotColorPanel::IDD, m_pWnd);
		}

		CRect rc;
		m_screenShotColorPanel.GetWindowRect(rc);

		int nTop = m_startPt.y + m_nHeight;
		CRect rcNew(m_startPt.x, nTop, m_startPt.x + rc.Width(), nTop + rc.Height());
		if (m_bIsPosOnTop)
		{
			rcNew = CRect(m_startPt.x, nTop - m_nHeight - rc.Height(), m_startPt.x + rc.Width(), nTop);
		}

		m_screenShotColorPanel.SetWindowPos(NULL, rcNew.left, rcNew.top, 0, 0, SWP_SHOWWINDOW | SWP_NOSIZE);
		m_screenShotColorPanel.UpdataUI();

		if (NULL != m_pWnd)
		{
			m_pWnd->SetFocus();
		}
	}
	else
	{
		if (NULL != m_screenShotColorPanel.m_hWnd)
		{
			m_screenShotColorPanel.ShowWindow(SW_HIDE);
		}
	}
}

//-------------------------------------------------------------------------
void CWGScreenshotTool::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	if (nCtlColor == CTLCOLOR_EDIT)
	{
		//pDC->SetBkMode(TRANSPARENT);
		if (eDrawFont == m_curDrawData.eToolType)
		{
			pDC->SetTextColor(m_curDrawData.color);
		}
	}
}

//-------------------------------------------------------------------------
void CWGScreenshotTool::ShowFontEdit(const bool bShow)
{
	if (!bShow)
	{
		m_FontEdit.ShowWindow(SW_HIDE);

		m_FontEdit.GetWindowText(m_curDrawData.strText);
		m_FontEdit.SetWindowText(_T(""));

		if (1 < m_curDrawData.vecPt.size())
		{
			CRect rc;
			m_FontEdit.GetWindowRect(rc);

			m_pWnd->ScreenToClient(rc);

			m_curDrawData.vecPt[0] = CPoint(rc.left, rc.top);
			m_curDrawData.vecPt[1] = CPoint(rc.right, rc.bottom);
		}

		if (!m_curDrawData.strText.IsEmpty())
		{
			m_lsDrawData.push_back(m_curDrawData);
		}

		m_curDrawData.vecPt.clear();
	}
	else
	{
		if (eDrawFont == m_curDrawData.eToolType
			&& 1 < m_curDrawData.vecPt.size())
		{
			CRect rc(m_curDrawData.vecPt[0], m_curDrawData.vecPt[1]);
			CWGGlobalTool::RegularRect(rc);

			if (!m_FontEdit.IsWindowVisible())
			{
				m_FontEdit.UpDateDC();
			}

			m_FontEdit.SetWindowPos(NULL, rc.left, rc.top, rc.Width(), rc.Height(), SWP_SHOWWINDOW);
		}
	}
}


//-------------------------------------------------------------------------
void CWGScreenshotTool::ModifyData(void)
{
	if (eDrawFont == m_curDrawData.eToolType
		&& m_FontEdit.IsWindowVisible())
	{
		m_Font.DeleteObject();
		if(m_Font.CreatePointFont(m_curDrawData.nFontSize * 10, _T("宋体")))
		{
			m_FontEdit.SetFont(&m_Font);
		}
	}
}

//-------------------------------------------------------------------------
void CWGScreenshotTool::TriggerFunc(const ETOOLTYPE eToolType)
{
	switch (eToolType)
	{
	case eUndo:
		Undo();
		break;
	case eCancel:
	case eFinish:
	case eSave:
		break;
	default:
		m_curDrawData.eToolType = eToolType;
		const RECORDDATA& drawData  = m_mapRecordData[eToolType];
		m_curDrawData.ePaintSize = drawData.ePaintSize;
		m_curDrawData.nFontSize = drawData.nFontSize;
		m_curDrawData.color = drawData.color;
		ShowColorPanel();
		break;
	}

	if (NULL != m_pObserver)
	{
		m_pObserver->DisposeTool(eToolType);
	}
}

//-------------------------------------------------------------------------
void CWGScreenshotTool::Reset(void)
{
	m_lsDrawData.clear();

	m_curDrawData = DRAWDATA();

	ShowFontEdit(false);

	ShowColorPanel();
}

//-------------------------------------------------------------------------
void CWGScreenshotTool::RedrawChildWnd(void)
{
	if (m_FontEdit.IsWindowVisible())
	{
		m_FontEdit.RedrawWindow();
	}
}

//-------------------------------------------------------------------------
void CWGScreenshotTool::GetToolTipInfo(std::map<CRect, CString>& mapToolTipRel) const
{
	CString strArray[] = {
		_T("添加矩形"),
		_T("添加椭圆"),
		_T("添加箭头"),
		_T("画刷工具"),
		_T("添加文字"),
		_T("撤销编辑"),
		_T("保存"),
		_T("退出截屏"),
		_T("完成截屏")
	};
	int len = (int)m_vecFuncRect.size();
	for (int i = 0; i < len; ++i)
	{
		CRect rc(m_vecFuncRect[i]);
		ClientToScreen(rc);
		mapToolTipRel.insert(std::make_pair(rc, strArray[i]));
	}
}
