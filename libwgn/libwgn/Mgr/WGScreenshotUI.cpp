// AjScreenshotUI.cpp : 实现文件
//

#include "stdafx.h"
#include "WGScreenshotUI.h"
#include "afxdialogex.h"
#include <algorithm>

#include "../Tool/WGGlobalTool.h"

extern HINSTANCE g_resourceInstance;

//---------------------------------------------------------------------------------------
IMPLEMENT_DYNAMIC(CWGScreenshotUI, CDialogEx)

//---------------------------------------------------------------------------------------
CWGScreenshotUI::CWGScreenshotUI(CWnd* pParent /*=NULL*/)
: CDialog(CWGScreenshotUI::IDD, pParent)
, m_bIsLeftBtnDown(false)
, m_bIsDrawRect(false)
, m_nRectTrackHit(-1)
, m_bIsBanModifyRect(false)
, m_eCloseUICode(eUIUnKnow)
, m_strSaveFilePrefix(_T("截图_"))
{
	m_xScreen = GetSystemMetrics(SM_CXSCREEN);
	m_yScreen = GetSystemMetrics(SM_CYSCREEN);

	CRect rect(0, 0,m_xScreen,m_yScreen);
	m_screenBmp = CWGGlobalTool::ScreenShot(rect);

	m_pngMask.LoadImage(g_resourceInstance, MAKEINTRESOURCE(IDB_PNG_COVER));

	m_hColorCursor = LoadCursor(g_resourceInstance, MAKEINTRESOURCE(IDC_COLOR_ARROW));
	m_hDrawCursor = LoadCursor(g_resourceInstance, MAKEINTRESOURCE(IDC_CURSOR_DRAW));

	m_rectTracker.m_nStyle = CRectTracker::resizeInside | CRectTracker::solidLine;
	m_rectTracker.m_rect.SetRect(-1, -1, -1, -1);

	m_rectTracker.SetLineColor(RGB(0, 150, 250));
	m_rectTracker.SetResizeColor(RGB(0, 150, 250));

	m_rectOrg.SetRect(-1, -1, -1, -1);

	GetAllWindow();

	m_WGScreenShotTool.SetObserver(this);
}

//---------------------------------------------------------------------------------------
CWGScreenshotUI::~CWGScreenshotUI()
{
	if (NULL != m_hColorCursor)
	{
		DeleteObject(m_hColorCursor);
	}

	if (NULL != m_hDrawCursor)
	{
		DeleteObject(m_hDrawCursor);
	}

	if (NULL != m_screenBmp)
	{
		DeleteObject(m_screenBmp);
	}

	m_retBitMap.DeleteObject();
}

//---------------------------------------------------------------------------------------
void CWGScreenshotUI::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

//---------------------------------------------------------------------------------------
BEGIN_MESSAGE_MAP(CWGScreenshotUI, CDialog)
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_SETCURSOR()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_CTLCOLOR()
	ON_COMMAND(ID_MENU_RECT, OnMenuRect)
	ON_COMMAND(ID_MENU_ELLIPSE, OnMenuEllipse)
	ON_COMMAND(ID_MENU_ARROW, OnMenuArrow)
	ON_COMMAND(ID_MENU_BRUSH, OnMenuBrush)
	ON_COMMAND(ID_MENU_FONT, OnMenuFont)
	ON_COMMAND(ID_MENU_UNDO, OnMenuUndo)
	ON_COMMAND(ID_MENU_RESELECT, OnMenuReselect)
	ON_COMMAND(ID_MENU_FINNISH, OnMenuFinnish)
	ON_COMMAND(ID_MENU_SAVE, OnMenuSave)
	ON_COMMAND(ID_MENU_CANCEL, OnMenuCancel)
	ON_WM_RBUTTONDOWN()
END_MESSAGE_MAP()


// CAjScreenshotUI 消息处理程序

//---------------------------------------------------------------------------------------
BOOL CWGScreenshotUI::OnEraseBkgnd(CDC* pDC)
{
	return TRUE;
}

//---------------------------------------------------------------------------------------
void CWGScreenshotUI::OnPaint()
{
	CPaintDC dc(this); // device context for painting

	if (!IsIconic())
	{// 不是最小化
		DrawFrame(&dc);
	}
}

//---------------------------------------------------------------------------------------
BOOL CWGScreenshotUI::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_WGScreenShotTool.Create(this);

	//SetWindowPos(&wndTopMost, 0, 0, m_xScreen, m_yScreen, SWP_SHOWWINDOW);

	// 初始化ToopTip
	EnableToolTips(TRUE);
	m_ToolTip.Create(this);
	m_ToolTip.Activate(TRUE);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

//---------------------------------------------------------------------------------------
void CWGScreenshotUI::DrawFrame(CDC* pDC, const bool bIsShowRect)
{
	static bool bIsEnd = true;
	if(!bIsEnd)
	{
		return;
	}

	if (NULL == pDC)
	{
		return;
	}

	bIsEnd = false;

	CDC dcCompatible;
	dcCompatible.CreateCompatibleDC(pDC);

	CBitmap mCompatibleBmp;
	mCompatibleBmp.CreateCompatibleBitmap(pDC, m_xScreen, m_yScreen);

	HBITMAP hOldBitmap = (HBITMAP)dcCompatible.SelectObject(&mCompatibleBmp);

	// 绘制背景
	DrawBK(&dcCompatible);

	// 绘制区域
	DrawScreenShot(&dcCompatible, bIsShowRect);

	// 绘制工具栏
	m_WGScreenShotTool.Draw(&dcCompatible, bIsShowRect);

	if (bIsShowRect)
	{
		// 绘制放大镜
		//DrawZoom(&dcCompatible);
	}

	pDC->BitBlt(0, 0, m_xScreen, m_yScreen, &dcCompatible, 0, 0, SRCCOPY);

	dcCompatible.SelectObject(hOldBitmap);
	DeleteObject(hOldBitmap);

	mCompatibleBmp.DeleteObject();
	dcCompatible.DeleteDC();

	bIsEnd = true;
}

//---------------------------------------------------------------------------------------
bool CWGScreenshotUI::DrawScreenShot(CDC* pDC, const bool bIsShowRect)
{
	if (NULL == pDC)
	{
		return false;
	}

	CRect rectClient;
	GetClientRect(rectClient);

	const CRect& trackerRect = m_rectTracker.m_rect;
	if (1 < trackerRect.Width()
		&& 1 < trackerRect.Height())
	{
		// 绘制遮罩
		if (rectClient.top < trackerRect.top)
		{// 上
			m_pngMask.DrawImage(pDC, rectClient.left, rectClient.top, rectClient.Width(), trackerRect.top - rectClient.top, 0, 0, 8, 8);
		}

		if (trackerRect.bottom < rectClient.bottom)
		{// 下
			m_pngMask.DrawImage(pDC, rectClient.left, trackerRect.bottom, rectClient.Width(), rectClient.bottom - trackerRect.bottom, 0, 0, 8, 8);
		}

		if (rectClient.left < trackerRect.left)
		{// 左
			m_pngMask.DrawImage(pDC, rectClient.left, trackerRect.top, trackerRect.left - rectClient.left, trackerRect.Height(), 0, 0, 8, 8);
		}

		if (trackerRect.right < rectClient.right)
		{// 右
			m_pngMask.DrawImage(pDC, trackerRect.right, trackerRect.top, rectClient.right - trackerRect.right, trackerRect.Height(), 0, 0, 8, 8);
		}

		if (bIsShowRect)
		{
			if (m_bIsDrawRect || m_bIsLeftBtnDown)
			{
				m_rectTracker.Draw(pDC);
			}
			else
			{
				CPen pen;
				pen.CreatePen(PS_SOLID, 5, m_rectTracker.GetLineCorlor());
				CPen* pOldPen = pDC->SelectObject(&pen);

				CBrush *pBrush = CBrush::FromHandle((HBRUSH)GetStockObject(NULL_BRUSH));
				CBrush *pOldBrush = pDC->SelectObject(pBrush);

				pDC->Rectangle(m_rectTracker.m_rect);

				pDC->SelectObject(pOldBrush);
				pBrush->DeleteObject();

				pen.DeleteObject();
			}
		}
	}
	else
	{
		// 绘制遮罩
		m_pngMask.DrawImage(pDC, 0, 0, rectClient.Width(), rectClient.Height(), 0, 0, 8, 8);
	}

	return true;
}

//---------------------------------------------------------------------------------------
bool CWGScreenshotUI::DrawBK(CDC* pDC)
{
	if (NULL == pDC)
	{
		return false;
	}

	CImage Image;
	Image.Attach(m_screenBmp);
	Image.Draw(pDC->GetSafeHdc(), CRect(0, 0, m_xScreen, m_yScreen));
	Image.Detach();

	return true;
}

//---------------------------------------------------------------------------------------
void CWGScreenshotUI::OnLButtonDown(UINT nFlags, CPoint point)
{
	m_WGScreenShotTool.OnLbButtonDown(nFlags, point);

	ModifyCusor(point);

	m_ptLeftBtn = point;
	m_bIsLeftBtnDown = true;

	m_nRectTrackHit = m_rectTracker.HitTest(point);

	m_rectOrg = m_rectTracker.m_rect;

	if (m_bIsBanModifyRect)
	{
		if (m_rectTracker.m_rect.PtInRect(point))
		{
			SetCursor(m_hDrawCursor); 
		}
	}

	UpDataUI();

	CDialog::OnLButtonDown(nFlags, point);
}

//---------------------------------------------------------------------------------------
void CWGScreenshotUI::OnLButtonUp(UINT nFlags, CPoint point)
{
	m_WGScreenShotTool.OnLbButtonUp(nFlags, point);

	ModifyCusor(point);

	m_bIsLeftBtnDown = false;
	if (1 <= m_rectTracker.m_rect.Width() 
		&& 1 <= m_rectTracker.m_rect.Height())
	{
		SetIsDrawRect(true);
	}

	if (m_bIsBanModifyRect)
	{
		if (m_rectTracker.m_rect.PtInRect(point))
		{
			SetCursor(m_hDrawCursor); 
		}
	}

	UpDataUI();

	CDialog::OnLButtonUp(nFlags, point);
}

//---------------------------------------------------------------------------------------
void CWGScreenshotUI::OnMouseMove(UINT nFlags, CPoint point)
{
	m_WGScreenShotTool.OnMouseMove(nFlags, point);

	ModifyCusor(point);

	if (m_bIsLeftBtnDown)
	{
		if (!m_bIsDrawRect)
		{
			m_rectTracker.m_rect.left = m_ptLeftBtn.x;
			m_rectTracker.m_rect.right = point.x;
			m_rectTracker.m_rect.top = m_ptLeftBtn.y;
			m_rectTracker.m_rect.bottom = point.y;
		}
		else if(!m_bIsBanModifyRect)
		{
			bool bChangeRect = false;
			CRect rectTemp = m_rectTracker.m_rect;
			switch (m_nRectTrackHit)
			{
			case 0:					//	左上角
				rectTemp.left = point.x;
				rectTemp.top = point.y;
				bChangeRect = true;
				break;
			case 1:					//	右上角
				rectTemp.right = point.x;
				rectTemp.top = point.y;
				bChangeRect = true;
				break;
			case 2:					//	右下角
				rectTemp.right = point.x;
				rectTemp.bottom = point.y;
				bChangeRect = true;
				break;
			case 3:					//	左下角
				rectTemp.left = point.x;
				rectTemp.bottom = point.y;
				bChangeRect = true;
				break;
			case 4:					//	顶部
				rectTemp.top = point.y;
				bChangeRect = true;
				break;
			case 5:					//	右部
				rectTemp.right = point.x;
				bChangeRect = true;
				break;
			case 6:					//	底部
				rectTemp.bottom = point.y;
				bChangeRect = true;
				break;
			case 7:					//	左部
				rectTemp.left = point.x;
				bChangeRect = true;
				break;
			case 8:					//	内部
				{
					int x = point.x - m_ptLeftBtn.x;
					int y = point.y - m_ptLeftBtn.y;

					rectTemp.left = m_rectOrg.left + x;
					rectTemp.right = m_rectOrg.right + x;
					rectTemp.top = m_rectOrg.top + y;
					rectTemp.bottom = m_rectOrg.bottom + y;
					bChangeRect = true;
				}
				break;
			default:
				break;
			}

			if (bChangeRect)
			{
				CRect rcClient;
				GetClientRect(rcClient);

				if(CWGGlobalTool::RegularRect(rectTemp))
				{
					switch (m_nRectTrackHit)
					{
					case 0:					//	左上角
						m_nRectTrackHit = 2;
						break;
					case 1:					//	右上角
						m_nRectTrackHit = 3;
						break;
					case 2:					//	右下角
						m_nRectTrackHit = 0;
						break;
					case 3:					//	左下角
						m_nRectTrackHit = 1;
						break;
					case 4:					//	顶部
						m_nRectTrackHit = 6;
						break;
					case 5:					//	右部
						m_nRectTrackHit = 7;
						break;
					case 6:					//	底部
						m_nRectTrackHit = 4;
						break;
					case 7:					//	左部
						m_nRectTrackHit = 5;
						break;
					default:
						break;
					}
				}

				// 规整区域
				int nWidth = rectTemp.Width(), nHeight = rectTemp.Height();
				if (rectTemp.left < rcClient.left)
				{
					rectTemp.left = rcClient.left;
					rectTemp.right = rectTemp.left + nWidth;
				}
				else if (rcClient.right < rectTemp.right)
				{
					rectTemp.right = rcClient.right;
					rectTemp.left = rcClient.right - nWidth;
				}

				if (rectTemp.top < rcClient.top)
				{
					rectTemp.top = rcClient.top;
					rectTemp.bottom = rectTemp.top + nHeight;
				}
				else if (rcClient.bottom < rectTemp.bottom)
				{
					rectTemp.bottom = rcClient.bottom;
					rectTemp.top = rectTemp.bottom - nHeight;
				}

				m_rectTracker.m_rect = rectTemp;

				SetIsDrawRect(true);
			}
		}
	}
	else if (!m_bIsDrawRect)
	{
		CPoint pt(point);
		ClientToScreen(&pt);

		CRect rc;
		if (FindWndByPt(pt, rc))
		{
			if (rc.left < 0)
			{
				rc.left = 0;
			}

			if (rc.top < 0)
			{
				rc.top = 0;
			}

			m_rectTracker.m_rect = rc;
		}
	}

	if (m_bIsBanModifyRect)
	{
		if (m_rectTracker.m_rect.PtInRect(point))
		{
			SetCursor(m_hDrawCursor); 
		}
	}

	RedrawWindow();

	CDialog::OnMouseMove(nFlags, point);
}

//---------------------------------------------------------------------------------------
BOOL CWGScreenshotUI::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
	CPoint point;
	GetCursorPos(&point);
	pWnd->ScreenToClient(&point);

	if (m_bIsDrawRect && m_rectTracker.m_rect.PtInRect(point)) 
	{
		return TRUE; 
	}
	else if(!m_bIsLeftBtnDown)
	{
		//设置彩色光标
		SetCursor(m_hColorCursor);
		return TRUE;
	}

	return TRUE;
}

//---------------------------------------------------------------------------------------
void CWGScreenshotUI::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	if (0 < m_rectTracker.m_rect.PtInRect(point))
	{
		m_eCloseUICode = eUIFinish;

		SavePic();

		SendMessage(WM_CLOSE);
	}

	CDialog::OnLButtonDblClk(nFlags, point);
}

//---------------------------------------------------------------------------------------
bool CWGScreenshotUI::SavePic(const bool bSaveToFile /*= false*/, const CString& strFilePath/* = _T("")*/)
{
	m_retBitMap.DeleteObject();

	const CRect& trackerRect = m_rectTracker.m_rect;
	if (1 < trackerRect.Width()
		&& 1 < trackerRect.Height())
	{
		if (bSaveToFile && strFilePath.IsEmpty())
		{
			return false;
		}

		CClientDC dc(this);

		CDC dcCompatible;
		dcCompatible.CreateCompatibleDC(&dc);

		m_retBitMap.CreateCompatibleBitmap(&dc, trackerRect.Width(), trackerRect.Height());

		HBITMAP hOldBitmap = (HBITMAP)dcCompatible.SelectObject(&m_retBitMap);

		DrawFrame(&dc, false);

		dcCompatible.BitBlt(0, 0, trackerRect.Width(), trackerRect.Height(), &dc, trackerRect.left, trackerRect.top, SRCCOPY);

		dcCompatible.SelectObject(hOldBitmap);

		//if (!bSaveToFile)
		//{
		//	if (::OpenClipboard(NULL)) 
		//	{
		//		//清空剪贴板
		//		EmptyClipboard();

		//		//把屏幕内容粘贴到剪贴板上,
		//		//hBitmap 为刚才的屏幕位图句柄
		//		SetClipboardData(CF_BITMAP, mCompatibleBmp);

		//		//关闭剪贴板
		//		CloseClipboard();
		//	}
		//}
		if(bSaveToFile)
		{
			CImage Image;
			Image.Attach(m_retBitMap);
			Image.Save(strFilePath);
			Image.Detach();
		}

		dcCompatible.DeleteDC();
		ReleaseDC(&dc);

		return true;
	}

	return false;
}

//---------------------------------------------------------------------------------------
bool CWGScreenshotUI::FindWndByPt(const CPoint& pt, CRect& rc) const
{
	int len = (int)m_vecWndData.size();
	for (int i = 0; i < len; ++i)
	{
		if (m_vecWndData[i].rc.PtInRect(pt))
		{
			rc = m_vecWndData[i].rc;

			EnumChildWndData(m_vecWndData[i], pt, rc);

			return true;
		}
	}

	return false;
}

//---------------------------------------------------------------------------------------
bool CWGScreenshotUI::EnumChildWndData(const CAjWndData& data, const CPoint& pt, CRect& rc) const
{
	int len = (int)data.children.size();
	for (int i = 0; i < len; ++i)
	{
		if (data.children[i].rc.PtInRect(pt))
		{
			rc = data.children[i].rc;

			EnumChildWndData(data.children[i], pt, rc);

			return true;
		}
	}

	return false;
}

//---------------------------------------------------------------------------------------
bool CWGScreenshotUI::AddWndPos(const HWND hwnd)
{
	if (NULL == hwnd)
	{
		return false;
	}

	if (!IsWindow(hwnd))
	{
		return false;
	}

	TCHAR strTitle[255];
	::GetWindowText(hwnd, strTitle, 255);

	TCHAR strClassName[255];
	::GetClassName(hwnd, strClassName, 255);

	LONG lWndStyle = ::GetWindowLongW(hwnd, GWL_STYLE);
	LONG lWndExStyle = ::GetWindowLong(hwnd, GWL_EXSTYLE);

	CString strFormat1,strFormat2;
	strFormat1.Format(_T("%x"), lWndStyle);
	strFormat2.Format(_T("%x"), lWndExStyle);

	if (!(lWndStyle & WS_VISIBLE)
		|| (lWndStyle & WS_DISABLED)
		|| (lWndExStyle & WS_EX_TRANSPARENT))
	{
		return false;
	}

	CRect rc;
	::GetWindowRect(hwnd, rc);

	HWND hParentWnd = ::GetParent(hwnd);
	if (NULL == hParentWnd)
	{// 没有父窗口
		CAjWndData data;
		data.hwnd = hwnd;
		data.rc = rc;
		m_vecWndData.push_back(data);

		m_mapWnd[hwnd] = &*m_vecWndData.rbegin();
	}
	else
	{
		std::map<HWND, CAjWndData*>::iterator itFind = m_mapWnd.find(hParentWnd);
		if (itFind != m_mapWnd.end())
		{
			CAjWndData data;
			data.hwnd = hwnd;
			data.rc = rc;
			itFind->second->children.push_back(data);

			m_mapWnd[hwnd] = &*itFind->second->children.rbegin();
		}
		else if(lWndStyle & WS_POPUP)
		{// 父窗口不显示
			CAjWndData data;
			data.hwnd = hwnd;
			data.rc = rc;
			m_vecWndData.push_back(data);

			m_mapWnd[hwnd] = &*m_vecWndData.rbegin();
		}
	}

	return true;
}

//---------------------------------------------------------------------------------------
BOOL CWGScreenshotUI::EnumChildProc(HWND hwnd,LPARAM lParam)  
{//枚举子窗口的回调函数
	CWGScreenshotUI* pUI = (CWGScreenshotUI*)(lParam);
	if (NULL != pUI)
	{
		pUI->AddWndPos(hwnd);
	}

	return TRUE ; 
}

void CWGScreenshotUI::GetAllWindow(void)
{
	::EnumChildWindows(GetDesktopWindow()->GetSafeHwnd(), EnumChildProc, (LPARAM)this);

	m_mapWnd.clear();
}

//---------------------------------------------------------------------------------------
BOOL CWGScreenshotUI::PreTranslateMessage(MSG* pMsg)
{
	switch (pMsg->message)
	{
	case WM_KEYDOWN:
		{
			if (VK_ESCAPE == pMsg->wParam)
			{
				SendMessage(WM_CLOSE);
			}
		}
		break;
	default:
		break;
	}

	//m_ToolTip.RelayEvent(pMsg);

	return CDialog::PreTranslateMessage(pMsg);
}

//---------------------------------------------------------------------------------------
void CWGScreenshotUI::SetIsDrawRect(const bool bTrue)
{
	m_bIsDrawRect = bTrue;

	CRect rc;
	GetClientRect(rc);

	m_WGScreenShotTool.setIsCanDraw(bTrue, rc, m_rectTracker.m_rect);

	// 显示ToolTip
	std::map<CRect, CString> mapToolTipRel;
	m_WGScreenShotTool.GetToolTipInfo(mapToolTipRel);

	int UIDBegin = 10000;
	std::map<CRect, CString>::iterator itBegin = mapToolTipRel.begin();
	std::map<CRect, CString>::iterator itEnd = mapToolTipRel.end();
	for (; itBegin != itEnd; ++itBegin)
	{
		m_ToolTip.AddTool(this, itBegin->second, itBegin->first, UIDBegin++);
	}
}

//---------------------------------------------------------------------------------------
void CWGScreenshotUI::DisposeTool(const ETOOLTYPE type)
{
	bool bIsClose = false;
	switch(type)
	{
	case eSave:			// 保存
		{
			CString strFileName(m_strSaveFilePrefix);
			CTime tm = CTime::GetCurrentTime();
			strFileName += tm.Format(_T("%Y%m%d%H%M%S")) + _T(".png");

			CFileDialog dlg(FALSE, _T("PNG(*.png)"), strFileName, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, _T("PNG(*.png)|*.png|BMP(*.bmp)|*.bmp|JPG(*.jpg,*.jpeg)|*.jpg||"));
			if(IDOK == dlg.DoModal())
			{
				m_eCloseUICode = eUISaveToFile;
				SavePic(true, dlg.GetPathName());
				bIsClose = true;
			}
		}
		break;
	case eCancel:		//	取消
		m_eCloseUICode = eUICancel;
		bIsClose = true;
		break;
	case eFinish:		//	完成
		m_eCloseUICode = eUIFinish;
		SavePic();
		bIsClose = true;
		break;
	default:
		m_bIsBanModifyRect = true;
		break;
	}

	if (bIsClose)
	{
		SendMessage(WM_CLOSE);
	}
}

//---------------------------------------------------------------------------------------
void CWGScreenshotUI::ModifyCusor(const CPoint& point)
{
	if (!m_bIsBanModifyRect)
	{
		if (!m_bIsLeftBtnDown)
		{
			m_nRectTrackHit = m_rectTracker.HitTest(point);
		}

		switch (m_nRectTrackHit)
		{
		case 0:					//	左上角
			SetCursor(LoadCursor(NULL, MAKEINTRESOURCE(IDC_SIZENWSE))); 
			break;
		case 1:					//	右上角
			SetCursor(LoadCursor(NULL, MAKEINTRESOURCE(IDC_SIZENESW))); 
			break;
		case 2:					//	右下角
			SetCursor(LoadCursor(NULL, MAKEINTRESOURCE(IDC_SIZENWSE))); 
			break;
		case 3:					//	左下角
			SetCursor(LoadCursor(NULL, MAKEINTRESOURCE(IDC_SIZENESW))); 
			break;
		case 4:					//	顶部
			SetCursor(LoadCursor(NULL, MAKEINTRESOURCE(IDC_SIZENS))); 
			break;
		case 5:					//	右部
			SetCursor(LoadCursor(NULL, MAKEINTRESOURCE(IDC_SIZEWE))); 
			break;
		case 6:					//	底部
			SetCursor(LoadCursor(NULL, MAKEINTRESOURCE(IDC_SIZENS))); 
			break;
		case 7:					//	左部
			SetCursor(LoadCursor(NULL, MAKEINTRESOURCE(IDC_SIZEWE))); 
			break;
		case 8:					//	内部
			SetCursor(LoadCursor(NULL, MAKEINTRESOURCE(IDC_SIZEALL))); 
			break;
		default:
			SetCursor(m_hColorCursor);
			break;
		}
	}
}

//-------------------------------------------------------------------------
HBRUSH CWGScreenshotUI::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = __super::OnCtlColor(pDC, pWnd, nCtlColor);

	m_WGScreenShotTool.OnCtlColor(pDC, pWnd, nCtlColor);

	return hbr;
}

//-------------------------------------------------------------------------
void CWGScreenshotUI::OnMenuRect()
{
	m_WGScreenShotTool.TriggerFunc(eDrawRect);
	UpDataUI();
}

//-------------------------------------------------------------------------
void CWGScreenshotUI::OnMenuEllipse()
{
	m_WGScreenShotTool.TriggerFunc(eDrawEllipse);
	UpDataUI();
}

//-------------------------------------------------------------------------
void CWGScreenshotUI::OnMenuArrow()
{
	m_WGScreenShotTool.TriggerFunc(eDrawArrow);
	UpDataUI();
}

//-------------------------------------------------------------------------
void CWGScreenshotUI::OnMenuBrush()
{
	m_WGScreenShotTool.TriggerFunc(eDrawBrush);
	UpDataUI();
}

//-------------------------------------------------------------------------
void CWGScreenshotUI::OnMenuFont()
{
	m_WGScreenShotTool.TriggerFunc(eDrawFont);
	UpDataUI();
}

//-------------------------------------------------------------------------
void CWGScreenshotUI::OnMenuUndo()
{
	m_WGScreenShotTool.TriggerFunc(eUndo);
	UpDataUI();
}

//-------------------------------------------------------------------------
void CWGScreenshotUI::OnMenuReselect()
{
	m_bIsBanModifyRect = false;
	SetIsDrawRect(false);
	m_WGScreenShotTool.Reset();
	UpDataUI();
}

//-------------------------------------------------------------------------
void CWGScreenshotUI::OnMenuFinnish()
{
	m_WGScreenShotTool.TriggerFunc(eFinish);
	UpDataUI();
}

//-------------------------------------------------------------------------
void CWGScreenshotUI::OnMenuSave()
{
	m_WGScreenShotTool.TriggerFunc(eSave);
	UpDataUI();
}

//-------------------------------------------------------------------------
void CWGScreenshotUI::OnMenuCancel()
{
	m_WGScreenShotTool.TriggerFunc(eCancel);
	UpDataUI();
}

//-------------------------------------------------------------------------
void CWGScreenshotUI::OnRButtonDown(UINT nFlags, CPoint point)
{
	if (m_bIsDrawRect
		&& m_rectTracker.m_rect.PtInRect(point))
	{// 弹出菜单
		CMenu menu;
		menu.LoadMenu(IDR_MENU1);
		CMenu* pContextMenu=menu.GetSubMenu(0);
		pContextMenu->TrackPopupMenu(TPM_LEFTALIGN|TPM_RIGHTBUTTON, point.x, point.y, this);
	}
	else
	{
		OnMenuReselect();
	}

	__super::OnRButtonDown(nFlags, point);
}

//-------------------------------------------------------------------------
void CWGScreenshotUI::DrawZoom(CDC* pDC)
{
	CDC dcCompatible;
	dcCompatible.CreateCompatibleDC(pDC);

	CBitmap mCompatibleBmp;
	mCompatibleBmp.CreateCompatibleBitmap(pDC, m_xScreen, m_yScreen);

	CBitmap* pOldBitmap = dcCompatible.SelectObject(&mCompatibleBmp);

	dcCompatible.BitBlt(0, 0, m_xScreen, m_yScreen, pDC, 0, 0, SRCCOPY);

	DrawZoom(&dcCompatible, &dcCompatible); // 绘制临时的
	DrawZoom(pDC, &dcCompatible);

	dcCompatible.SelectObject(pOldBitmap);
	mCompatibleBmp.DeleteObject();
	dcCompatible.DeleteDC();
}

//-------------------------------------------------------------------------
void CWGScreenshotUI::DrawZoom(CDC* pDC, CDC* pBKDC)
{
	if (NULL == pDC
		|| NULL == pBKDC
		|| !m_bIsDrawRect)
	{
		return;
	}

	const int nWidth = 222, nHeight = 69, nGap = 5;
	CDC dcCompatible;
	dcCompatible.CreateCompatibleDC(pDC);

	CBitmap mCompatibleBmp;
	mCompatibleBmp.CreateCompatibleBitmap(pDC, nWidth, nHeight);

	HBITMAP hOldBitmap = (HBITMAP)dcCompatible.SelectObject(&mCompatibleBmp);

	// 计算位置
	CRect rc;
	GetClientRect(rc);
	const CRect& rcDraw = m_rectTracker.m_rect;

	int nStatus = 0; // 0 左上角 1 左边 2 右边 3 左下角 4 左下角工具下边 5 区域里面
	bool bInTop = nHeight + nGap < rcDraw.top - rc.top;
	bool bInBottom = nHeight + nGap < rc.bottom - rcDraw.bottom;
	int nScreenShotToolPos = m_WGScreenShotTool.GetPos();
	if (bInTop 
		&& ((2 == nScreenShotToolPos && nWidth + m_WGScreenShotTool.GetActualWidth() < rcDraw.Width())
		|| (2 != nScreenShotToolPos && nWidth < rc.right - rcDraw.left)))
	{// 左上角
		nStatus = 0;
	}
	else if(nWidth + nGap < rcDraw.left - rc.left)
	{// 左边
		nStatus = 1;
	}
	else if(nWidth + nGap < rc.right - rcDraw.right)
	{// 右边
		nStatus = 2;
	}
	else if (bInBottom && nWidth + m_WGScreenShotTool.GetActualWidth() < rcDraw.Width())
	{// 左下角
		nStatus = 3;
	}
	else if (bInBottom && nHeight + 2 * nGap + m_WGScreenShotTool.GetActualHeight() < rc.bottom - rcDraw.bottom)
	{// 左下角工具下边
		nStatus = 4;
	}
	else
	{
		nStatus = 5;
	}

	CRect rcPos(0, 0, 0, 0);
	switch (nStatus)
	{
	case 1:				//	左边
		{
			rcPos.left = rcDraw.left - nGap - nWidth;
			rcPos.top = rcDraw.top;
		}
		break;
	case 2:				//	右边
		{
			rcPos.left = rcDraw.right + nGap;
			rcPos.top = rcDraw.top;
		}
		break;
	case 3:				//	左下角
		{
			rcPos.left = rcDraw.left;
			rcPos.top = rcDraw.bottom + nGap;
		}
		break;
	case 4:				//	左下角工具下边	
		{
			rcPos.left = rcDraw.left;
			rcPos.top = rcDraw.bottom + 2 * nGap + m_WGScreenShotTool.GetActualHeight();
		}
		break;
	case 5:				//	区域里面	
		{
			rcPos.left = rcDraw.left;
			rcPos.top = rcDraw.top;
		}
		break;
	default:			//	左上角
		{
			rcPos.left = rcDraw.left;
			rcPos.top = rcDraw.top - nHeight - nGap;
		}
		break;
	}

	rcPos.right = rcPos.left + nWidth;
	rcPos.bottom = rcPos.top + nHeight;

	dcCompatible.BitBlt(0, 0, nWidth, nHeight, pDC, rcPos.left, rcPos.top, SRCCOPY);
	
	m_pngMask.DrawImage(&dcCompatible, 0, 0, nWidth, nHeight, 0, 0, 8, 8);

	// 绘制文字
	dcCompatible.SetBkMode(TRANSPARENT);
	COLORREF oldTextColor = dcCompatible.SetTextColor(RGB(255, 255, 255));

	CPoint pt;
	GetCursorPos(&pt);
	ScreenToClient(&pt);

	CFont font;
	if(!font.CreatePointFont(100, _T("宋体")))
	{
		return;
	}

	CFont* pOldFont = dcCompatible.SelectObject(&font);

	COLORREF rgb = pBKDC->GetPixel(pt);
	CString strText;
	strText.Format(_T("当前RGB:(%d,%d,%d)"), GetRValue(rgb), GetGValue(rgb), GetBValue(rgb));
	dcCompatible.DrawText(strText, CRect(73, 6, 222, 69), DT_EDITCONTROL | DT_LEFT);

	strText.Format(_T("区域大小:(%d,%d)"), rcDraw.Width(), rcDraw.Height());
	dcCompatible.DrawText(strText, CRect(73, 26, 222, 69), DT_EDITCONTROL | DT_LEFT);

	dcCompatible.DrawText(_T("双击可以快速完成截图"), CRect(73, 46, 222, 69), DT_EDITCONTROL | DT_LEFT);

	dcCompatible.SelectObject(pOldFont);
	font.DeleteObject();

	// 放大镜
	dcCompatible.StretchBlt(1, 1, 67, 67, pBKDC, pt.x - 8, pt.y - 8, 16, 16, SRCCOPY);

	CPen pen(PS_SOLID, 2, RGB(10, 124, 202));
	CPen* pOldPen = dcCompatible.SelectObject(&pen);
	dcCompatible.MoveTo(CPoint(34, 1));
	dcCompatible.LineTo(CPoint(34, 68));
	dcCompatible.MoveTo(CPoint(1, 34));
	dcCompatible.LineTo(CPoint(68, 34));
	dcCompatible.SelectObject(pOldPen);
	pen.DeleteObject();

	pDC->BitBlt(rcPos.left, rcPos.top, rcPos.Width(), rcPos.Height(), &dcCompatible, 0, 0, SRCCOPY);

	dcCompatible.SetTextColor(oldTextColor);
	dcCompatible.SelectObject(hOldBitmap);
	mCompatibleBmp.DeleteObject();
	dcCompatible.DeleteDC();
}

//-------------------------------------------------------------------------
void CWGScreenshotUI::UpDataUI(void)
{
	CDC* pDC = GetDC();
	if (NULL != pDC)
	{
		DrawFrame(pDC);
		ReleaseDC(pDC);

		// 刷新子控件
		m_WGScreenShotTool.RedrawChildWnd();
	}
}

//-------------------------------------------------------------------------
HBITMAP CWGScreenshotUI::GetScreenshotBitmap(void) const
{
	return m_retBitMap;
}

//-------------------------------------------------------------------------
ECLOSEUICODE CWGScreenshotUI::GetCloseCode() const
{
	return m_eCloseUICode;
}

//-------------------------------------------------------------------------
void CWGScreenshotUI::SetSaveFilePrefix(const char* strSaveFilePrefix)
{
	m_strSaveFilePrefix = strSaveFilePrefix;
}
