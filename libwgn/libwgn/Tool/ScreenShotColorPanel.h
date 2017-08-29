#pragma once

#include "../resource.h"
#include "afxwin.h"
#include "WGScreenShotHead.h"

/// @class CScreenShotColorPanel
/// @brief ��ɫ���

class CScreenShotColorPanel : public CDialog
{
	DECLARE_DYNAMIC(CScreenShotColorPanel)

public:
	CScreenShotColorPanel(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CScreenShotColorPanel();

// �Ի�������
	enum { IDD = IDD_DIALOG_COLOR };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	/// @brief ���û�������
	/// @param [out] pDrawData ��������
	void SetDrawData(DRAWDATA* pDrawData);

	/// @brief ���ù۲���
	/// @param [in ] pObserver �۲���
	void SetObserver(CWGColorPanelObserver* pObserver);

	/// @brief ���½���
	void UpdataUI(void);
protected:
	/// @brief ��ʼ��
	BOOL OnInitDialog();

	/// @brief ����
	afx_msg void OnPaint();

	/// @brief ���Ʊ���
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);

	/// @brief ����ƶ�
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);

	/// @brief �������
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);

	/// @brief ��������޸�
	afx_msg void OnCbnSelchangeComboFont();

	/// @brief ���ù��
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);

	/// @brief ��Ϣ������
	virtual BOOL PreTranslateMessage(MSG* pMsg);
private:
	/// @brief ������ɫ���
	/// @param [out] pDC ����������
	/// @param [in ] nStartPos ��ʼλ��
	void DrawColorBar(CDC* pDC, const int nStartPos);

	/// @brief ������ɫ����
	/// @param [out] pDC ����������
	/// @param [in ] nX �����ʼλ��
	/// @param [in ] nY ������ʼλ��
	/// @param [in ] nWidth ���ο��
	/// @param [in ] nColor ������ɫ
	void DrawColor(CDC* pDC, const int nX, const int nY, const int nWidth, const COLORREF& nColor);

	/// @brief �������ֿؼ�
	/// @param [out] pDC ����������
	/// @param [out] nStartPos ��ʼλ��
	void DrawFontBar(CDC* pDC, int& nStartPos);

	/// @brief ���ƴ�С�ؼ�
	/// @param [out] pDC ����������
	/// @param [out] nStartPos ��ʼλ��
	void DrawSizeBar(CDC* pDC, int& nStartPos);

	/// @brief ��ȡ��ǰ������ĸ���ɫ��
	/// @param [in ] point ���λ��
	int GetColorIndex(const CPoint& point) const;

	/// @brief ��ȡ������ĸ���С�ؼ���
	/// @param [in ] point ���λ��
	int GetSizeIndex(const CPoint& point) const;

	/// @brief ��ȡĬ�ϵ�����
	CRect GetNormalRect(void) const;

	/// @brief ��ȡѡ�е�����
	CRect GetSelectRect(void) const;
	
	/// @brief ��ȡ�ƶ�������
	CRect GetMoveRect(void) const;

	/// �ͷ��ڴ�
	void Release(void);
private:
	CComboBox								m_comboFont;		//	�����С					
	CImage									m_ImageFont;		//	����ͼƬ
	std::vector<COLORREF>					m_vecColor;			//	��ɫ����
	std::vector<CRect>						m_vecColorRect;		//	��ɫ���εľ���
	std::vector<CImage*>					m_vecFuncPng;		//	����ͼƬ����
	std::vector<CRect>						m_vecFuncRect;		//	���ܰ�ť����
	COLORREF								m_nBKColor;			//	������ɫ
	COLORREF								m_nPenColor;		//	�ʵ���ɫ
	COLORREF								m_nSpliteColor;		//	�ָ������ɫ
	HCURSOR									m_hColorCursor;		//	����ɫ�Ĺ��
	int										m_nMoveIndex;		//	����ƶ����±�
	DRAWDATA*								m_pCurDrawData;		//	��ǰ��������
	CWGColorPanelObserver*					m_pObserver;		//	�۲���
	int										m_nCurSelect;		//	��ǰѡ�е�����һ��
};
