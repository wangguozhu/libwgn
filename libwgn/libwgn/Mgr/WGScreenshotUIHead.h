#pragma once

/// @class
/// @brief �������������
enum ECLOSEUICODE
{
	eUIUnKnow  = -1,		//	δ֪
	eUIFinish,				//	���
	eUISaveToFile,			//	�����ļ�
	eUICancel				//	ȡ��
};

#ifdef LIBWGN_MODEL
#define LIBWGN_EXPORT __declspec(dllexport)
#else
#define LIBWGN_EXPORT __declspec(dllimport)
#endif // LIBWGN_MODEL