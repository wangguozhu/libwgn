#pragma once
#include "afxext.h"
class CRectColorTracker : public CRectTracker
{
public:
	CRectColorTracker(void);
	~CRectColorTracker(void);
public:
	/// @brief ��ȡ�ߵ���ɫ
	const DWORD& GetLineCorlor(void) const;

	/// @brief ��������ɫ
	/// @param [in ] nColor ��ɫ
	void SetLineColor(const DWORD& nColor);

	/// @brief ��ȡ���ε���ɫ
	const DWORD& GetResizeColor(void) const;

	/// @brief ���þ�����ɫ
	/// @param [in ] nColor ��ɫ
	void SetResizeColor(const DWORD& nColor);

	/// @brief ����
	/// @param [out] pDC ��ͼ�������� 
	void Draw(CDC* pDC) const;
private:
	DWORD			m_nLineColor;					//	����ɫ
	DWORD			m_nResizeClolor;				//	С���ο����ɫ
};

