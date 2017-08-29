#include "StdAfx.h"
#include "WGGlobalTool.h"
#include <WinInet.h>
#include <string>

namespace CWGGlobalTool
{
	//-----------------------------------------------------------------------------------
	bool SetProxy(const CString& strProxyIP)
	{
		HKEY hKeyIn = HKEY_CURRENT_USER, hKeyOut;
		if(RegOpenKeyEx(
			hKeyIn, 
			_T("Software\\Microsoft\\Windows\\CurrentVersion\\Internet Settings"), 
			0, 
			KEY_CREATE_LINK | KEY_WRITE | KEY_READ | KEY_NOTIFY, 
			&hKeyOut) != ERROR_SUCCESS )

		{
			return false;
		}

		ULONG regsize = 0;
		if(RegQueryValueEx(
			hKeyOut, 
			_T("ProxyEnable"), 
			NULL, 
			NULL, 
			NULL, 
			&regsize)!=ERROR_SUCCESS)
		{
			return false;
		}

		LPBYTE pValue = new BYTE[regsize];
		memset(pValue, 0x00, regsize);

		if(RegQueryValueEx(
			hKeyOut, 
			_T("ProxyEnable"), 
			NULL, 
			NULL, 
			pValue, 
			&regsize)!=ERROR_SUCCESS)
		{
			return false;
		}

		BOOL bEnable=(BOOL)*pValue;
		delete[] pValue;
		pValue = NULL;


		/************************************************************/
		if(!bEnable)
		{
			DWORD dwData = 1;
			if(RegSetValueEx(
				hKeyOut,
				_T("ProxyEnable"),
				0,
				REG_DWORD,
				(LPBYTE)&dwData,
				sizeof(DWORD))!=ERROR_SUCCESS)
			{
				return false;
			}
		}

		CString strConvert(strProxyIP);
		if(RegSetValueEx(
			hKeyOut, 
			_T("ProxyServer"), 
			0, 
			REG_SZ, 
			(BYTE*)strConvert.GetBuffer(), 
			strConvert.GetLength() * 2)!=ERROR_SUCCESS)
		{
			return false;
		}

		RegCloseKey(hKeyOut);

		//使设置生效
		if(!InternetSetOption(NULL, INTERNET_OPTION_SETTINGS_CHANGED, NULL, 0))
		{
			return false;
		}

		return true;
	}

	//-----------------------------------------------------------------------------------
	bool CancelProxy( void )
	{
		HKEY hKeyIn = HKEY_CURRENT_USER, hKeyOut;
		if(RegOpenKeyEx(
			hKeyIn, 
			_T("Software\\Microsoft\\Windows\\CurrentVersion\\Internet Settings"), 
			0, 
			KEY_CREATE_LINK | KEY_WRITE | KEY_READ | KEY_NOTIFY, 
			&hKeyOut) != ERROR_SUCCESS )

		{
			return false;
		}

		ULONG regsize = 0;
		if(RegQueryValueEx(
			hKeyOut, 
			_T("ProxyEnable"), 
			NULL, 
			NULL, 
			NULL, 
			&regsize)!=ERROR_SUCCESS)
		{
			return false;
		}

		LPBYTE pValue = new BYTE[regsize];
		memset(pValue, 0x00, regsize);

		if(RegQueryValueEx(
			hKeyOut, 
			_T("ProxyEnable"), 
			NULL, 
			NULL, 
			pValue, 
			&regsize)!=ERROR_SUCCESS)
		{
			return false;
		}

		BOOL bEnable=(BOOL)*pValue;
		delete[] pValue;
		pValue = NULL;


		/************************************************************/
		if(bEnable)
		{
			DWORD dwData=0;
			if(RegSetValueEx(
				hKeyOut,
				_T("ProxyEnable"),
				0,
				REG_DWORD,
				(LPBYTE)&dwData,
				sizeof(DWORD))!=ERROR_SUCCESS)
			{
				return false;
			}

			RegCloseKey(hKeyOut);


			//使设置生效
			if(!InternetSetOption(NULL, INTERNET_OPTION_SETTINGS_CHANGED, NULL, 0))
			{
				return false;
			}
		}
		return true;
	}

	//-----------------------------------------------------------------------------------
	bool LoadDll( const CString& strFile,bool bErrorReport /*= false*/ )
	{
		HMODULE hIns2 = LoadLibrary(strFile);
		if (hIns2 == NULL)
		{
			if (bErrorReport)
			{
				int nErrorCode = GetLastError();
				CString strTmp;
				strTmp.Format(_T("加载%s模块失败:错误码:%d!"),strFile,nErrorCode);
				AfxMessageBox(strTmp);
			}
			return false;
		}
		return true;
	}

	//-----------------------------------------------------------------------------------
	CString GetSysPath( void )
	{
		TCHAR strFilePath[MAX_PATH];
		GetModuleFileName(NULL, strFilePath, MAX_PATH);

		CString strPath(strFilePath);
		int nPos = strPath.ReverseFind(_T('\\'));
		strPath.Delete(nPos, strPath.GetLength() - nPos);

		return strPath;
	}

	//-----------------------------------------------------------------------------------
	HBITMAP ScreenShot(const LPRECT lpRect, const bool bSave)
	{
		// 确保选定区域不为空矩形
		if (IsRectEmpty(lpRect))
			return NULL;

		// 屏幕和内存设备描述表
		HDC hScrDC, hMemDC;

		//为屏幕创建设备描述表
		hScrDC = CreateDC(_T("DISPLAY"), NULL, NULL, NULL);

		//为屏幕设备描述表创建兼容的内存设备描述表
		hMemDC = CreateCompatibleDC(hScrDC);

		// 获得选定区域坐标
		int nX = lpRect->left;
		int nY = lpRect->top;
		int nX2 = lpRect->right;
		int nY2 = lpRect->bottom;

		int xScreen = GetSystemMetrics(SM_CXSCREEN);
		int yScreen = GetSystemMetrics(SM_CYSCREEN);

		//确保选定区域是可见的
		if (nX < 0)
			nX = 0;
		if (nY < 0)
			nY = 0;
		if (nX2 > xScreen)
			nX2 = xScreen;
		if (nY2 > yScreen)
			nY2 = yScreen;

		int nWidth = nX2 - nX;

		int nHeight = nY2 - nY;

		// 创建一个与屏幕设备描述表兼容的位图
		HBITMAP hBitmap = CreateCompatibleBitmap(hScrDC, nWidth, nHeight);

		// 把新位图选到内存设备描述表中
		HBITMAP hOldBitmap = (HBITMAP)SelectObject(hMemDC, hBitmap);

		BitBlt(hMemDC, 0, 0, nWidth, nHeight, hScrDC, nX, nY, SRCCOPY);

		hBitmap = (HBITMAP)SelectObject(hMemDC, hOldBitmap);

		//清除 
		DeleteDC(hScrDC);
		DeleteDC(hMemDC);
		// 返回位图句柄
		if(bSave)
		{				
			if (::OpenClipboard(NULL)) 
			{
				//清空剪贴板
				EmptyClipboard();
				//把屏幕内容粘贴到剪贴板上,
				//hBitmap 为刚才的屏幕位图句柄
				SetClipboardData(CF_BITMAP, hBitmap);
				//关闭剪贴板
				CloseClipboard();
			}
		}

		return hBitmap;
	}

	//-------------------------------------------------------------------------
	bool RegularRect(CRect& rect)
	{
		bool bRet = false;
		if (rect.right < rect.left)
		{
			int nTemp = rect.right;
			rect.right = rect.left;
			rect.left = nTemp;

			bRet = true;
		}

		if (rect.bottom < rect.top)
		{
			int nTemp = rect.bottom;
			rect.bottom = rect.top;
			rect.top = nTemp;

			bRet = true;
		}

		return bRet;
	}

}