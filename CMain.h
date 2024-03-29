﻿#pragma once
#include "CMyTableCtrl.h"
#include "pch.h"

#include "CFriendList.h"
//#include "CFriendInfo.h"


// CMain 对话框

class CMain : public CDialogEx
{
	DECLARE_DYNAMIC(CMain)

public:
	CMain(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CMain();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MAIN };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CMyTableCtrl m_MyTable;
	CProgressCtrl myProCtrl;

	virtual BOOL OnInitDialog();
	afx_msg void OnTcnSelchangeFriendlist(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg BOOL OnCopyData(CWnd* pWnd, COPYDATASTRUCT* pCopyDataStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
protected:
	afx_msg LRESULT OnMymessage(WPARAM wParam, LPARAM lParam);
public:
	afx_msg void OnTcnSelchangeTab1(NMHDR* pNMHDR, LRESULT* pResult);
	void OnWxLogout();
};
