
// MFC_DEMO01.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CMFC_DEMO01App: 
// �йش����ʵ�֣������ MFC_DEMO01.cpp
//

class CMFC_DEMO01App : public CWinApp
{
public:
	CMFC_DEMO01App();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CMFC_DEMO01App theApp;