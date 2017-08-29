#pragma once
namespace CWGGlobalTool
{
	/// @brief 设置Internet代理服务器
	/// @param [in ] strProxyIP 服务器IP(包含端口号)
	bool SetProxy(const CString& strProxyIP);

	/// @brief 取消Internet代理
	bool CancelProxy(void);

	/// @brief 加载dll
	bool LoadDll(const CString& strFile,bool bErrorReport = false);

	/// @brief 获取当前路径
	CString GetSysPath(void);

	/// @brief 屏幕截图
	/// @param [in ] lpRect 截图区域
	/// @param [in ] bSave 是否保存
	HBITMAP ScreenShot(const LPRECT lpRect, const bool bSave = false);

	/// @brief 规整矩形
	/// @param [in ] rect 矩形
	bool RegularRect(CRect& rect);
}