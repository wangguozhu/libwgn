#pragma once

#include "../resource.h"
#include "WGScreenShotHead.h"
#include "ScreenShotColorPanel.h"
#include "WGEdit.h"

/// @class AjScreenshotTool
/// @brief ����������

class CWGScreenshotTool : public CWGColorPanelObserver
{
	/// @struct RECORDDATA
	/// @brief ��¼����
	struct RECORDDATA
	{
		RECORDDATA() : color(RGB(255, 0, 0)), ePaintSize(eDrawSmall), nFontSize(12) {}
		COLORREF		color;				//	��ɫ
		EPAINTSIZE		ePaintSize;			//	���ƴ�С
		int				nFontSize;			//	���ִ�С
	};
public:
	explicit CWGScreenshotTool();
	~CWGScreenshotTool();
public:
	/// @brief ��������
	/// @param [in ] pParentWnd ������
	void Create(CWnd* pParentWnd);

	/// @brief �����Ƿ���Ի���
	/// @param [in ] bTrue �Ƿ���Ի���
	/// @param [in ] orgRect ԭʼ����
	/// @param [in ] showRect ��������
	void setIsCanDraw(bool bTrue, const CRect& orgRect, const CRect& showRect);

	/// @brief ���ù۲���
	/// @param [in ] pOberver �۲���
	void SetObserver(CWGScreenShotToolObserver* pOberver);

	/// @brief ��������
	/// @param [in ] eToolType ���ܵ�ö��
	void TriggerFunc(const ETOOLTYPE eToolType);

	/// @brief ����
	void Reset(void);

	/// @brief ����
	/// @param [in ] bIsShowPanel �Ƿ���ʾ���
	bool Draw(CDC* pDC, const bool bIsShowPanel = true);

	/// @brief ����ƶ�
	void OnMouseMove(UINT nFlags, CPoint point);

	/// @brief ��갴�����
	void OnLbButtonDown(UINT nFlags, CPoint point);

	/// @brief ����������
	void OnLbButtonUp(UINT nFlags, CPoint point);

	/// @brief �޸Ŀؼ���ɫ
	void OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);

	/// @brief ���ݸ���
	virtual void ModifyData(void);

	/// @brief ��ȡλ��
	/// @return 1 �ײ� 2 ���� 3 ����������
	int GetPos(void) const;

	/// @brief ��ȡʵ�ʿ��
	int GetActualWidth(void) const;

	/// @brief ��ȡʵ�ʸ߶�
	int GetActualHeight(void) const;

	/// @brief ˢ���ӿؼ�
	void RedrawChildWnd(void);

	/// @brief ��ȡToolTip��Ϣ
	/// @param [out] mapToolTipRel ToolTip��Ϣ
	void GetToolTipInfo(std::map<CRect, CString>& mapToolTipRel) const;
private:
	/// @brief ��ʼ��
	void Init(void);

	/// @brief ��ʾ��ɫ��
	void ShowColorPanel();

	/// @brief ����ת��
	/// @param [out] pt ת��ǰ�������
	void ScreenToClient(CPoint& pt) const;

	/// @brief ����ת��
	/// @param [out] rc ת��ǰ��ľ���
	void ClientToScreen(CRect& rc) const;

	/// @brief ������ʼ��
	void CalcStartPos(void);

	/// @brief ��ȡ����ͼƬ��ѡ������
	/// @param [in ] nWidth ͼƬ�Ŀ��
	CRect GetSelectRect(const int nWidth = 24) const;

	/// @brief ��ȡ����ͼƬ��ԭʼ����
	/// @param [in ] nWidth ͼƬ�Ŀ��
	CRect GetOrgRect(const int nWidth = 24) const;

	/// @brief ��ȡ����ͼƬ���ƶ�����
	/// @param [in ] nWidth ͼƬ�Ŀ��
	CRect GetMoveRect(const int nWidth = 24) const;

	/// �ͷ��ڴ�
	void Release(void);

	/// @brief ��ȡ��������ʱ�������
	/// @param [in ] pt ��
	CPoint GetClosePt(const CPoint& pt);
private:
	/// @brief ���Ƽ�¼
	/// @param [out] ����DC
	/// @param [in ] drawData ��������
	void Draw(CDC* pDC, const DRAWDATA& drawData) const;

	/// @brief ������ʷ��¼
	/// @param [out] ����DC
	void DrawHistory(CDC* pDC);

	/// @brief ���ƾ���
	/// @param [out] ����DC
	/// @param [in ] drawData ��������
	void DrawRect(CDC* pDC, const DRAWDATA& drawData) const;

	/// @brief ������Բ
	/// @param [out] ����DC
	/// @param [in ] drawData ��������
	void DrawEllipse(CDC* pDC, const DRAWDATA& drawData) const;

	/// @brief ���Ƽ�ͷ
	/// @param [out] ����DC
	/// @param [in ] drawData ��������
	void DrawArrow(CDC* pDC, const DRAWDATA& drawData) const;

	/// @brief ���ƻ�ˢ
	/// @param [out] ����DC
	/// @param [in ] drawData ��������
	void DrawReBrush(CDC* pDC, const DRAWDATA& drawData) const;

	/// @brief ��������
	/// @param [out] ����DC
	/// @param [in ] drawData ��������
	void DrawFont(CDC* pDC, const DRAWDATA& drawData) const;

	/// @brief ����
	void Undo(void);

	/// @brief ��ʾ��������༭��
	/// @param [in ] bShow �Ƿ���ʾ
	void ShowFontEdit(const bool bShow);
private:
	CScreenShotColorPanel					m_screenShotColorPanel;		//	��ɫ���
	CAjEdit									m_FontEdit;					//	�ı�
	CFont									m_Font;						//	����
	CRectTracker							m_FontTracker;				//	������Ƥ��
	CPoint									m_startPt;					//	��ʼλ��
	CPoint									m_movePt;					//	����ƶ��ĵ�
	CImage									m_pngBk;					//	����ͼƬ
	DRAWDATA								m_curDrawData;				//	��ǰ��������
	std::map<ETOOLTYPE, RECORDDATA>			m_mapRecordData;			//	��¼����
	std::vector<CImage*>					m_vecFuncPng;				//	����ͼƬ����
	std::vector<CRect>						m_vecFuncRect;				//	���ܰ�ť����
	std::list<DRAWDATA>						m_lsDrawData;				//	��������
	CRect									m_orgRect;					//	ԭʼ����
	CRect									m_showRect;					//	��������
	CWGScreenShotToolObserver*				m_pObserver;				//	�۲���
	CWnd*									m_pWnd;						//  ������
	int										m_nWidth;					//	���
	int										m_nHeight;					//	�߶�
	int										m_nFontProcess;				//	-1 û�� 0 ѡλ�� 1��������
	int										m_nPos;						//	λ�� 1 �ײ� 2 ���� 3 ����������
	bool									m_bIsCanDraw;				//	�Ƿ���Ի���
	bool									m_bIsPosOnTop;				//	�Ƿ��ڶ���
};
