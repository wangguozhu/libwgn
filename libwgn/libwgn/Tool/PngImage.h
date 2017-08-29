#pragma once

#define  SafeDelete(pData) { try { delete pData; } catch (...) { ASSERT(FALSE); } pData=NULL; } 

class CWGImage
{
	//变量定义
protected:
	Gdiplus::Image *							m_pImage;							//图片对象

	//函数定义
public:
	//构造函数
	CWGImage();
	//析构函数
	virtual ~CWGImage();

	//状态函数
public:
	//是否加载
	bool IsNull();
	//获取宽度
	INT GetWidth();
	//获取高度
	INT GetHeight();

	//管理函数
public:
	//销毁图片
	bool DestroyImage();
	//加载图片
	bool LoadImage(LPCTSTR pszFileName);
	//加载图片
	bool LoadImage(HINSTANCE hInstance, LPCTSTR pszResourceName);

	//绘画函数
public:
	//绘画图像
	bool DrawImage(CDC * pDC, INT nXPos, INT nYPos);
	//绘画图像
	bool DrawImage(CDC * pDC, INT nXDest, INT nYDest, INT nDestWidth, INT nDestHeight, INT nXScr, INT YSrc);
	//绘画图像
	bool DrawImage(CDC * pDC, INT nXDest, INT nYDest, INT nDestWidth, INT nDestHeight, INT nXScr, INT YSrc, INT nSrcWidth, INT nSrcHeight);
	//绘画图像
	bool DrawImage(CDC * pDC, const CRect& rcDest, const CRect& rcOrg);
	//操作重载
public:
	//获取对象
	Gdiplus::Image * GetImage() { return m_pImage; }
	//指针重载
	Gdiplus::Image * operator->() { return m_pImage; }
};
