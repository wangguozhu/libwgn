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

		//ʹ������Ч
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


			//ʹ������Ч
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
				strTmp.Format(_T("����%sģ��ʧ��:������:%d!"),strFile,nErrorCode);
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
		// ȷ��ѡ������Ϊ�վ���
		if (IsRectEmpty(lpRect))
			return NULL;

		// ��Ļ���ڴ��豸������
		HDC hScrDC, hMemDC;

		//Ϊ��Ļ�����豸������
		hScrDC = CreateDC(_T("DISPLAY"), NULL, NULL, NULL);

		//Ϊ��Ļ�豸�����������ݵ��ڴ��豸������
		hMemDC = CreateCompatibleDC(hScrDC);

		// ���ѡ����������
		int nX = lpRect->left;
		int nY = lpRect->top;
		int nX2 = lpRect->right;
		int nY2 = lpRect->bottom;

		int xScreen = GetSystemMetrics(SM_CXSCREEN);
		int yScreen = GetSystemMetrics(SM_CYSCREEN);

		//ȷ��ѡ�������ǿɼ���
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

		// ����һ������Ļ�豸��������ݵ�λͼ
		HBITMAP hBitmap = CreateCompatibleBitmap(hScrDC, nWidth, nHeight);

		// ����λͼѡ���ڴ��豸��������
		HBITMAP hOldBitmap = (HBITMAP)SelectObject(hMemDC, hBitmap);

		BitBlt(hMemDC, 0, 0, nWidth, nHeight, hScrDC, nX, nY, SRCCOPY);

		hBitmap = (HBITMAP)SelectObject(hMemDC, hOldBitmap);

		//��� 
		DeleteDC(hScrDC);
		DeleteDC(hMemDC);
		// ����λͼ���
		if(bSave)
		{				
			if (::OpenClipboard(NULL)) 
			{
				//��ռ�����
				EmptyClipboard();
				//����Ļ����ճ������������,
				//hBitmap Ϊ�ղŵ���Ļλͼ���
				SetClipboardData(CF_BITMAP, hBitmap);
				//�رռ�����
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