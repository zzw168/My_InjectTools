#pragma once
#include "pch.h"
#include "string"

// CChatRecords 对话框

class CChatRecords : public CDialogEx
{
	DECLARE_DYNAMIC(CChatRecords)

public:
	CChatRecords(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CChatRecords();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_CHAT_RECORDS };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
//	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);

protected:
	afx_msg LRESULT OnShowmessage(WPARAM wParam, LPARAM lParam);
public:
	virtual BOOL OnInitDialog();
	CListCtrl m_ChatRecord;
	CEdit Accept_id;
	CEdit Send_id;

protected:
	afx_msg LRESULT OnSetAccepted(WPARAM wParam, LPARAM lParam);
public:
	afx_msg void OnNMDblclkList1(NMHDR* pNMHDR, LRESULT* pResult);
	CEdit m_Member;
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	CButton m_CheckAll;
	CButton m_CH_Post;
	afx_msg void OnLvnItemchangedList1(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnBnClickedChPost();
};

string my_HttpPost(string& host, string& data);

string string_To_UTF8(const std::string& str);

string Wchar_tToString(wchar_t* wchar);

wchar_t* StringToWchar_t(const std::string& str);

void Log(const std::string& type, const std::string& wxid, const std::string& source, const std::string& msgSender, const std::string& content);
