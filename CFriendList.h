#pragma once


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
