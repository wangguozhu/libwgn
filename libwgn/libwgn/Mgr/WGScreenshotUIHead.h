#pragma once

/// @class
/// @brief 界面结束返回码
enum ECLOSEUICODE
{
	eUIUnKnow  = -1,		//	未知
	eUIFinish,				//	完成
	eUISaveToFile,			//	保存文件
	eUICancel				//	取消
};

#ifdef LIBWGN_MODEL
#define LIBWGN_EXPORT __declspec(dllexport)
#else
#define LIBWGN_EXPORT __declspec(dllimport)
#endif // LIBWGN_MODEL