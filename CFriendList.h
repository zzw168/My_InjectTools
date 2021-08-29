#pragma once
#include "pch.h"
#include "Thread.h"
#include "My_InjectTools.h"
#include "CFriendList.h"
#include "afxdialogex.h"
#include "CChatRecords.h"

#include "message.h"
#include "data.h"
#include <vector>
#include "CMain.h"
#include <iostream>
#include <fstream>

class MyThread : public Thread
{
protected:
	string m_strWXName;

	virtual void process() override
	{
		wstring m_WxName = L"WxName.txt";
		// 以读模式打开文件
		ifstream inName(m_WxName);
		if (!inName.good()) {
			exit;
		}
		getline(inName, m_strWXName);

		wstring m_Message = L"Message.txt";
		// 以读模式打开文件
		ifstream infile(m_Message);
		if (infile.good()) {
			//infile.open(m_Message);
			string m_data;
			while (getline(infile, m_data))
			{
				m_data = UTF8ToGB(m_data.c_str()).c_str();
				if (m_data != "") {
					//查找窗口
					CWnd* pWnd = CWnd::FindWindow(NULL, L"WeChatHelper");
					if (pWnd != NULL) {
						//填充数据到结构体
						MessageStruct* message = new MessageStruct(StringToWchar_t(m_strWXName), StringToWchar_t(m_data));
						COPYDATASTRUCT MessageData;
						MessageData.dwData = WM_SendTextMessage;
						MessageData.cbData = sizeof(MessageStruct);
						MessageData.lpData = message;

						////发送消息
						pWnd->SendMessage(WM_COPYDATA, NULL, (LPARAM)&MessageData);

						//清空文本
						delete message;
					}
				}
			}
		}
		// 关闭打开的文件
		infile.close();
		Sleep(5000);
	}
};

// CFriendList 对话框

class CFriendList : public CDialogEx
{
	DECLARE_DYNAMIC(CFriendList)

public:
	CFriendList(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CFriendList();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_FRIEND_LIST };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	CListCtrl m_FriendList;
	MyThread T_SendMessage;
	
protected:
	afx_msg LRESULT OnShowfriendlist(WPARAM wParam, LPARAM lParam);
public:
	afx_msg void OnLvnItemchangedFriendlist(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDblclkFriendlist(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnBnClickedGroupsend();
protected:
	afx_msg LRESULT OnMamessage(WPARAM wParam, LPARAM lParam);
public:
	afx_msg void OnBnClickedCancelall();
	afx_msg void OnBnClickedBtSeach();
	CEdit Ed_Seach;
	afx_msg void OnBnClickedCheckall();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnNMRClickFriendlist(NMHDR* pNMHDR, LRESULT* pResult);
public:
	afx_msg void OnSave();
	afx_msg void OnBnClickedReversechoose();
	afx_msg void OnBnClickedBtSeach2();
	afx_msg void OnLoad();
	afx_msg void OnSendimg();
	afx_msg void OnSendfile();
	afx_msg void OnSendcard();
};

