#pragma once
namespace CWGGlobalTool
{
	/// @brief ����Internet���������
	/// @param [in ] strProxyIP ������IP(�����˿ں�)
	bool SetProxy(const CString& strProxyIP);

	/// @brief ȡ��Internet����
	bool CancelProxy(void);

	/// @brief ����dll
	bool LoadDll(const CString& strFile,bool bErrorReport = false);

	/// @brief ��ȡ��ǰ·��
	CString GetSysPath(void);

	/// @brief ��Ļ��ͼ
	/// @param [in ] lpRect ��ͼ����
	/// @param [in ] bSave �Ƿ񱣴�
	HBITMAP ScreenShot(const LPRECT lpRect, const bool bSave = false);

	/// @brief ��������
	/// @param [in ] rect ����
	bool RegularRect(CRect& rect);
}