#pragma once
// ExtendedComboBoxAPP.h: PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
#error "include 'pch.h' before including this file for PCH"
#endif

#include "resource.h"		// ������


// CExtendedComboBoxApp:
// �йش����ʵ�֣������ ExtendedComboBox.cpp
//

class CExtendedComboBoxApp : public CWinApp
{
public:
	CExtendedComboBoxApp();

	// ��д
public:
	virtual BOOL InitInstance();

	// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CExtendedComboBoxApp theApp;