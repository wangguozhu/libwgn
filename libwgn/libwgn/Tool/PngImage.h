#pragma once

#define  SafeDelete(pData) { try { delete pData; } catch (...) { ASSERT(FALSE); } pData=NULL; } 

class CWGImage
{
	//��������
protected:
	Gdiplus::Image *							m_pImage;							//ͼƬ����

	//��������
public:
	//���캯��
	CWGImage();
	//��������
	virtual ~CWGImage();

	//״̬����
public:
	//�Ƿ����
	bool IsNull();
	//��ȡ���
	INT GetWidth();
	//��ȡ�߶�
	INT GetHeight();

	//������
public:
	//����ͼƬ
	bool DestroyImage();
	//����ͼƬ
	bool LoadImage(LPCTSTR pszFileName);
	//����ͼƬ
	bool LoadImage(HINSTANCE hInstance, LPCTSTR pszResourceName);

	//�滭����
public:
	//�滭ͼ��
	bool DrawImage(CDC * pDC, INT nXPos, INT nYPos);
	//�滭ͼ��
	bool DrawImage(CDC * pDC, INT nXDest, INT nYDest, INT nDestWidth, INT nDestHeight, INT nXScr, INT YSrc);
	//�滭ͼ��
	bool DrawImage(CDC * pDC, INT nXDest, INT nYDest, INT nDestWidth, INT nDestHeight, INT nXScr, INT YSrc, INT nSrcWidth, INT nSrcHeight);
	//�滭ͼ��
	bool DrawImage(CDC * pDC, const CRect& rcDest, const CRect& rcOrg);
	//��������
public:
	//��ȡ����
	Gdiplus::Image * GetImage() { return m_pImage; }
	//ָ������
	Gdiplus::Image * operator->() { return m_pImage; }
};
