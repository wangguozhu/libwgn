#pragma once

#include "WGScreenshotUIHead.h"
#include "../Tool/WGScreenshotTool.h"
#include "../Tool/WGScreenShotHead.h"
#include "../Tool/RectColorTracker.h"
#include "../Tool/PngImage.h"

/// @class CAjScreenshotUI
/// @brief ��Ļ��ͼ����

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
	CWGScreenshotUI(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CWGScreenshotUI();

// �Ի�������
	enum { IDD = IDD_SCREENSHOT_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	/// @brief ���ñ����ļ���ǰ׺
	/// @param [in ] strSaveFilePrefix �ļ�ǰ׺
	void SetSaveFilePrefix(const char* strSaveFilePrefix);

	/// @brief ��ͼ������ķ���ֵ 
	ECLOSEUICODE GetCloseCode() const;

	/// @brief ��ȡͼƬ
	HBITMAP GetScreenshotBitmap(void) const;
private:
	/// @brief ��Ӵ��ں�λ��
	/// @param [in ] hwnd ����
	bool AddWndPos(const HWND hwnd);

	/// @brief ������Ϣ
	/// @param [in ] type ����ö��
	virtual void DisposeTool(const ETOOLTYPE type);
private:
	/// @brief ������ʾ
	void UpDataUI(void);

	/// @brief ���û��ƾ���
	/// @param [in ] �Ƿ�����˾���
	void SetIsDrawRect(const bool bTrue);

	/// @brief ���ƿ��
	/// @param [out] ��Դ�ļ�
	/// @param [in ] bIsShowRect �Ƿ���ƾ�����Ƥ��
	void DrawFrame(CDC* pDC = NULL, const bool bIsShowRect = true);

	/// @brief ���Ʊ���ͼƬ
	/// @param [out] pDC ��ͼ��Դ
	bool DrawBK(CDC* pDC);

	/// @brief ���ƽ�ͼ
	/// @param [out] pDC ��ͼ��Դ
	/// @param [in ] bIsShowRect �Ƿ���ƾ�����Ƥ��
	bool DrawScreenShot(CDC* pDC, const bool bIsShowRect = true);

	/// @brief ���ƹ���
	/// @param [out] pDC ��ͼ��Դ
	bool DrawTool(CDC* pDC) const;

	/// @brief ����λͼ
	/// @param [in ] bSaveToFile �Ƿ񱣴���ļ�
	/// @param [in ] strFilePath �ļ�ȫ·��
	bool SavePic(const bool bSaveToFile = false, const CString& strFilePath = _T(""));

	/// @brief ��ȡ���д���
	void GetAllWindow(void);

	/// @brief ����λ�û�ȡ����
	/// @param [in ] pt λ��
	/// @param [in ] rc ����
	bool FindWndByPt(const CPoint& pt, CRect& rc) const;

	/// @brief �޸Ĺ��
	/// @param [in ] point ����Ϣ
	void ModifyCusor(const CPoint& point);

	/// @brief ���ƷŴ�
	/// @param [in ] bDrawBk �Ƿ���ʾ����
	void DrawZoom(CDC* pDC);

	/// @brief ���ƷŴ�
	/// @param [in ] pBKDC ����DC
	void DrawZoom(CDC* pDC, CDC* pBKDC);

	/// @brief ö���Ӵ�������
	/// @param [in ] data ��������
	/// @param [in ] pt λ��
	/// @param [in ] rc ����
	bool EnumChildWndData(const CAjWndData& data, const CPoint& pt, CRect& rc) const;

	/// @brief �����Ӵ���
	static BOOL CALLBACK EnumChildProc(HWND hwnd,LPARAM lParam);

	/// @brief ��������
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

	// �˵�����
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
	std::vector<CAjWndData>				m_vecWndData;			//	��������
	std::map<HWND, CAjWndData*>			m_mapWnd;				//	���ڼ�Ĺ�ϵ
	CString								m_strSaveFilePrefix;	//	������ļ�ǰ׺
	CWGScreenshotTool					m_WGScreenShotTool;		//	��ͼ������					
	CWGImage							m_pngMask;				//	�ڸ�ͼƬ
	CBitmap								m_retBitMap;			//	���ص�ͼƬ
	CToolTipCtrl						m_ToolTip;				//	���ְ���
	CRectColorTracker					m_rectTracker;			//	������Ƥ��
	CRect								m_rectOrg;				//	ԭ����
	CPoint								m_ptLeftBtn;			//	���µ�����ĵ�
	int									m_xScreen;				//	��Ļ�Ŀ��
	int									m_yScreen;				//	��Ļ�ĸ߶�
	int									m_nRectTrackHit;		//	�������ο��λ��
	HBITMAP								m_screenBmp;			//	ȫ����ͼƬ
	HCURSOR								m_hColorCursor;			//	����ɫ�Ĺ��
	HCURSOR								m_hDrawCursor;			//	���ƵĹ��
	ECLOSEUICODE						m_eCloseUICode;			//	�Ի����������ֵ
	bool								m_bIsLeftBtnDown;		//	����Ƿ���
	bool								m_bIsDrawRect;			//	�Ƿ���ƾ���
	bool								m_bIsBanModifyRect;		//	�Ƿ��ֹ�޸ľ���
};
