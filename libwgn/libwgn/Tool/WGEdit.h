#pragma once


/// @class CAjEdit
/// @brief �Զ���༭��

class CAjEdit : public CEdit
{
	DECLARE_DYNAMIC(CAjEdit)

public:
	CAjEdit();
	virtual ~CAjEdit();
public:
	/// @brief ���ÿ�������
	/// @param [in ] rc ��������
	void SetShowRect(const CRect& rc);

	/// @brief ���¸�DC
	void UpDateDC(void);
protected:
	DECLARE_MESSAGE_MAP()

	/// @brief ������ˢ
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);

	/// @brief ������
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);

	/// @brief ����ƶ�
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);

	/// @brief ��ˢ
	afx_msg HBRUSH CtlColor(CDC* /*pDC*/, UINT /*nCtlColor*/);

	afx_msg UINT OnGetDlgCode();

	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);

	afx_msg void OnEnChange();

	afx_msg void OnPaint();
private:
	/// @brief ����λ��
	void CalcPos(void);

	/// @brief �ͷ�
	void Release(void);
private:
	CDC				m_dcParent;			//	����Դ��DC
	CBitmap			m_dcBmp;			//	��ԴͼƬ
	CBitmap*		m_pOldBmp;			//	����Դ�ľɵ�ͼƬ
	CRect			m_ShowRect;			//	��������
	CPoint			m_LBtnPt;			//	�����ť
};