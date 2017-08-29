// AjEdit.cpp : 实现文件
//

#include "stdafx.h"
#include "WGEdit.h"

//-------------------------------------------------------------------------
IMPLEMENT_DYNAMIC(CAjEdit, CEdit)

//-------------------------------------------------------------------------
CAjEdit::CAjEdit()
: m_ShowRect(0, 0, 0, 0)
, m_LBtnPt(0, 0)
, m_pOldBmp(NULL)
{

}

//-------------------------------------------------------------------------
CAjEdit::~CAjEdit()
{
	Release();
}

//-------------------------------------------------------------------------
BEGIN_MESSAGE_MAP(CAjEdit, CEdit)
	ON_WM_ERASEBKGND()
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_CTLCOLOR_REFLECT()
	ON_WM_GETDLGCODE()
	ON_WM_SETCURSOR()
	ON_WM_PAINT()
	ON_CONTROL_REFLECT(EN_CHANGE, OnEnChange)
END_MESSAGE_MAP()

//-------------------------------------------------------------------------
BOOL CAjEdit::OnEraseBkgnd(CDC* pDC)
{
	CRect rc;
	GetWindowRect(rc);

	CWnd* pParentWnd = GetParent();
	if (NULL != pParentWnd)
	{
		pParentWnd->ScreenToClient(rc);
	}

	pDC->BitBlt(0, 0, rc.Width(), rc.Height(), &m_dcParent, rc.left, rc.top, SRCCOPY);
	
	return TRUE;
}

//-------------------------------------------------------------------------
void CAjEdit::SetShowRect(const CRect& rc)
{
	m_ShowRect = rc;
}

//-------------------------------------------------------------------------
void CAjEdit::CalcPos(void)
{
	CString strText;
	GetWindowText(strText);

	CDC* pDC = GetDC();
	//获取设备指针
	CFont* font = GetFont();
	//获取当前字体
	CFont* oldFont = pDC->SelectObject(font);
	//设置当前设备
	CSize size = pDC->GetTextExtent(strText);
	//以CSize的形式返回字符串的高度和宽度
	pDC->SelectObject(oldFont);
	ReleaseDC(pDC);

	int nCout = GetLineCount();
	size.cy = (nCout + 1) * size.cy;

	CRect rect;
	GetWindowRect(rect);

	CWnd* pParentWnd = GetParent();
	if (NULL != pParentWnd)
	{
		pParentWnd->ScreenToClient(rect);
	}

	if (size.cx < rect.Width())
	{
		size.cx = rect.Width();
	}

	if (size.cy < rect.Height())
	{
		size.cy = rect.Height();
	}

	if (m_ShowRect.right < rect.left + size.cx)
	{
		size.cx = m_ShowRect.right - rect.left;
	}

	if (m_ShowRect.bottom < rect.top + size.cy)
	{
		size.cy = m_ShowRect.bottom - rect.top;
	}

	SetWindowPos(NULL, 0, 0, size.cx, size.cy, SWP_NOMOVE);
}

//-------------------------------------------------------------------------
void CAjEdit::OnLButtonDown(UINT nFlags, CPoint point)
{
	m_LBtnPt = point;

	CEdit::OnLButtonDown(nFlags, point);
}

//-------------------------------------------------------------------------
void CAjEdit::OnMouseMove(UINT nFlags, CPoint point)
{
	if (nFlags == VK_LBUTTON)
	{
		CRect rect;
		GetWindowRect(rect);

		CWnd* pParentWnd = GetParent();
		if (NULL != pParentWnd)
		{
			pParentWnd->ScreenToClient(rect);
		}

		// 计算位置
		point.x -= m_LBtnPt.x;
		point.y -= m_LBtnPt.y;

		rect.left += point.x;
		rect.right += point.x;
		rect.top += point.y;
		rect.bottom += point.y;

		// 规整区域
		int nWidth = rect.Width(), nHeight = rect.Height();
		if (rect.left < m_ShowRect.left)
		{
			rect.left = m_ShowRect.left;
			rect.right = rect.left + nWidth;
		}
		else if (m_ShowRect.right < rect.right)
		{
			rect.right = m_ShowRect.right;
			rect.left = m_ShowRect.right - nWidth;
		}

		if (rect.top < m_ShowRect.top)
		{
			rect.top = m_ShowRect.top;
			rect.bottom = rect.top + nHeight;
		}
		else if (m_ShowRect.bottom < rect.bottom)
		{
			rect.bottom = m_ShowRect.bottom;
			rect.top = rect.bottom - nHeight;
		}

		// 设置位置
		SetWindowPos(NULL, rect.left, rect.top, rect.Width(), rect.Height(), SWP_SHOWWINDOW);

		RedrawWindow();
	}

	CEdit::OnMouseMove(nFlags, point);

	CWnd* pParentWnd = GetParent();
	if (NULL != pParentWnd)
	{
		pParentWnd->SendMessage(WM_MOUSEMOVE, 0, LPARAM(&point));
	}
}

//-------------------------------------------------------------------------
HBRUSH CAjEdit::CtlColor(CDC* pDC, UINT /*nCtlColor*/)
{
	pDC->SetBkMode(TRANSPARENT); //设置背景透明
	return HBRUSH(GetStockObject(NULL_BRUSH));
}

//-------------------------------------------------------------------------
UINT CAjEdit::OnGetDlgCode()
{
	Invalidate();

	return CEdit::OnGetDlgCode();
}

//-------------------------------------------------------------------------
void CAjEdit::Release(void)
{
	if (NULL != m_pOldBmp)
	{
		m_dcParent.SelectObject(m_pOldBmp);
		m_pOldBmp->DeleteObject();
	}

	m_dcParent.DeleteDC();
	m_dcBmp.DeleteObject();
}

//-------------------------------------------------------------------------
void CAjEdit::UpDateDC(void)
{
	Release();

	CWnd* pParenWnd = GetParent();
	if (NULL != pParenWnd)
	{
		CDC* pDC = pParenWnd->GetDC();
		m_dcParent.CreateCompatibleDC(pDC);

		CRect rc;
		pParenWnd->GetWindowRect(rc);

		m_dcBmp.CreateCompatibleBitmap(pDC, rc.Width(), rc.Height());

		m_pOldBmp = m_dcParent.SelectObject(&m_dcBmp);

		m_dcParent.BitBlt(0, 0, rc.Width(), rc.Height(), pDC, 0, 0, SRCCOPY);

		ReleaseDC(pDC);
	}
}

//-------------------------------------------------------------------------
BOOL CAjEdit::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
	SetCursor(LoadCursor(NULL, MAKEINTRESOURCE(IDC_IBEAM)));
	return TRUE;
}

//-------------------------------------------------------------------------
void CAjEdit::OnEnChange()
{
	CalcPos();
}

//-------------------------------------------------------------------------
void CAjEdit::OnPaint()
{
	__super::OnPaint();

	CRect rc;
	GetClientRect(rc);

	CDC* pDC = GetDC();

	//CDC dcCompatible;
	//dcCompatible.CreateCompatibleDC(pDC);
	//
	//CBitmap mCompatibleBmp;
	//mCompatibleBmp.CreateCompatibleBitmap(pDC, rc.Width(), rc.Height());

	//CBitmap* pOldBitmap = dcCompatible.SelectObject(&mCompatibleBmp);

	//dcCompatible.BitBlt(0, 0, rc.Width(), rc.Height(), pDC, 0, 0, SRCCOPY);

	CPen pen(PS_DASHDOT, 0.8, RGB(0,0,0));
	CPen* pOldPen = pDC->SelectObject(&pen);

	CBrush *pBrush = CBrush::FromHandle((HBRUSH)GetStockObject(NULL_BRUSH));
	CBrush *pOldBrush = pDC->SelectObject(pBrush);

	pDC->Rectangle(rc);

	pDC->SelectObject(pOldBrush);
	pBrush->DeleteObject();

	pDC->SelectObject(pOldPen);
	pen.DeleteObject();

	ReleaseDC(pDC);
}
