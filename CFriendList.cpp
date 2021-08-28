// CFriendList.cpp: 实现文件
//

#include "pch.h"
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


DWORD dwIndex = 0;
wchar_t wxid[50];	//鼠标左键点击时拿到的微信ID
std::vector<CString> g_wxidvector;					//保存微信ID的容器
// CFriendList 对话框

IMPLEMENT_DYNAMIC(CFriendList, CDialogEx)

CFriendList::CFriendList(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_FRIEND_LIST, pParent)
{

}

CFriendList::~CFriendList()
{
}

void CFriendList::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_FRIENDLIST, m_FriendList);
	DDX_Control(pDX, IDC_ED_Seach, Ed_Seach);
}


BEGIN_MESSAGE_MAP(CFriendList, CDialogEx)
	ON_MESSAGE(WM_ShowFriendList, &CFriendList::OnShowfriendlist)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_FRIENDLIST, &CFriendList::OnLvnItemchangedFriendlist)
	ON_NOTIFY(NM_DBLCLK, IDC_FRIENDLIST, &CFriendList::OnDblclkFriendlist)
	ON_BN_CLICKED(IDC_GroupSend, &CFriendList::OnBnClickedGroupsend)
	ON_MESSAGE(WM_MAMESSAGE, &CFriendList::OnMamessage)
	ON_BN_CLICKED(IDC_CancelAll, &CFriendList::OnBnClickedCancelall)
	ON_BN_CLICKED(IDC_BT_Seach, &CFriendList::OnBnClickedBtSeach)
	ON_BN_CLICKED(IDC_CheckAll, &CFriendList::OnBnClickedCheckall)
	ON_NOTIFY(NM_RCLICK, IDC_FRIENDLIST, &CFriendList::OnNMRClickFriendlist)
	ON_COMMAND(FList_Save, &CFriendList::OnSave)
	ON_BN_CLICKED(IDC_Reversechoose, &CFriendList::OnBnClickedReversechoose)
	ON_BN_CLICKED(IDC_BT_Seach2, &CFriendList::OnBnClickedBtSeach2)
	ON_COMMAND(FList_Load, &CFriendList::OnLoad)
	ON_COMMAND(ID_SendImg, &CFriendList::OnSendimg)
	ON_COMMAND(ID_SendFile, &CFriendList::OnSendfile)
	ON_COMMAND(ID_SendCard, &CFriendList::OnSendcard)
END_MESSAGE_MAP()


// CFriendList 消息处理程序


BOOL CFriendList::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	m_FriendList.InsertColumn(0, L"序号", 0, 50);
	m_FriendList.InsertColumn(1, L"微信ID", 0, 200);
	m_FriendList.InsertColumn(2, L"微信号", 0, 200);
	m_FriendList.InsertColumn(3, L"昵称", 0, 250);
	m_FriendList.InsertColumn(4, L"备注", 0, 253);

	m_FriendList.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_CHECKBOXES);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}


//************************************************************
// 函数名称: OnShowfriendlist
// 函数说明: 响应Showfriendlist消息 处理父窗口发过来的联系人
// 作    者: GuiShou
// 时    间: 2019/7/5
// 参    数: WPARAM wParam, LPARAM lParam
// 返 回 值: BOOL
//***********************************************************
afx_msg LRESULT CFriendList::OnShowfriendlist(WPARAM wParam, LPARAM lParam)
{
	//取数据
	UserInfo* userinfo = (UserInfo*)wParam;
	CString index;
	index.Format(L"%d", dwIndex + 1);
	//显示到List控件

	m_FriendList.InsertItem(dwIndex, index);
	m_FriendList.SetItemText(dwIndex, 1, userinfo->UserId);
	m_FriendList.SetItemText(dwIndex, 2, userinfo->UserNumber);
	m_FriendList.SetItemText(dwIndex, 3, userinfo->UserNickName);
	m_FriendList.SetItemText(dwIndex, 4, userinfo->UserRemark);
	dwIndex++;
	return 0;
}



void CFriendList::OnLvnItemchangedFriendlist(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	*pResult = 0;
}


void CFriendList::OnDblclkFriendlist(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	CListCtrl* pListCtrl = (CListCtrl*)GetDlgItem(IDC_FRIENDLIST);
	ASSERT(pListCtrl != NULL);
	POSITION pos = pListCtrl->GetFirstSelectedItemPosition();
	if (pos == NULL) {
		TRACE0("No items were selected!\n");
	}
	else
	{
		while (pos)
		{
			int nItem = pListCtrl->GetNextSelectedItem(pos);
			//TRACE1("Item %d was selected!\n", nItem);
			// you could do your own processing on nItem here
			CString strInfor = pListCtrl->GetItemText(nItem, 1);

			TRACE1("Item %s was selected!\n", strInfor);
			//SetDlgItemText(IDC_EDIT1,TEXT("11"));
			//MessageStruct* My_Msg =  new MessageStruct(strInfor, to_string(nItem));
			
			MessageStruct* message = new MessageStruct(TEXT("Accept"), strInfor);
			
			CMain* parent = (CMain*)GetParent()->GetParent();
			parent->m_MyTable.m_Dia[1]->SendMessage(WM_Set_AcceptED, (WPARAM)message, NULL);

			delete message;
		}
	}

	*pResult = 0;

}


void CFriendList::OnBnClickedGroupsend()
{
	// TODO: 在此添加控件通知处理程序代码
	CString strInfor;
	this->GetDlgItemTextW(IDC_ED_GMESSAGE, strInfor);
	MessageStruct* message = new MessageStruct(TEXT("GMessage"), strInfor);
	this->SendMessage(WM_MAMESSAGE, (WPARAM)message, 0);
}


afx_msg LRESULT CFriendList::OnMamessage(WPARAM wParam, LPARAM lParam)
{
	//取数据
	MessageStruct* msg = (MessageStruct*)wParam;
	CString num = (msg->msgdata1);
	CString wxid = (msg->msgdata2);
	if (wxid == "")
	{
		MessageBoxW(L"消息不能为空", L"Tip", 0);
		return 0;
	}
	if (num == TEXT("Seach")) {
		int m_num = m_FriendList.GetItemCount();
		for (int i = m_num-1; i >= 0; i--) {
			string pa = CT2A(m_FriendList.GetItemText(i, 3).GetString());
			cout << pa;
			string pb = CT2A(wxid.GetString());
			int idx = pa.find(pb);//在pb中查找pa
			if (idx != string::npos) {
				m_FriendList.EnsureVisible(i, FALSE);
				m_FriendList.SetItemState(i, LVIS_SELECTED |LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);
				m_FriendList.SetFocus();
			}
		}
	}
	else if (num == TEXT("Seach2")) {
		int m_num = m_FriendList.GetItemCount();				//行数
		int m = m_FriendList.GetHeaderCtrl()->GetItemCount();	//列数
		if (m_num > 1) {
			vector< vector<CString> > m_arry;
			vector<CString> m_item;
			for (int i = m_num - 1; i >= 0; i--) {
				string pa = CT2A(m_FriendList.GetItemText(i, 4).GetString());
				cout << pa << endl;
				string pb = CT2A(wxid.GetString());
				int idx = pa.find(pb);	//在pb中查找pa
				if (idx != string::npos) {
					m_item.resize(0);
					for (int j = 0; j < m; j++) {
						m_item.push_back(m_FriendList.GetItemText(i, j));
					}
					m_arry.push_back(m_item);
					m_FriendList.DeleteItem(i);
					//m_FriendList.EnsureVisible(i, FALSE);
				}
			}
			int n = size(m_arry);
			if (n > 0){
				for (int k = 0; k < n; k++) {
					m_FriendList.InsertItem(0, m_arry[k][0]);
					for (int l = 1; l < m; l++) {
						m_FriendList.SetItemText(0, l, m_arry[k][l]);
					}
				}
				for (int o = n - 1; o >= 0; o--) {
					//m_FriendList.EnsureVisible(o, FALSE);
					m_FriendList.SetItemState(o, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);
					m_FriendList.SetFocus();
				}
			}
			m_arry.clear();
		}
	}
	//群发******************************
	else if (num == TEXT("GMessage")) {
		int m_num = m_FriendList.GetItemCount();
		for (int i = 0; i < m_num; i++) {
			if (m_FriendList.GetCheck(i)) {
				//查找窗口
				CWnd* pWnd = CWnd::FindWindow(NULL, L"WeChatHelper");
				if (pWnd != NULL) {
					//同步控件数据到变量
					UpdateData(TRUE);
					//填充数据到结构体
					MessageStruct* message = new MessageStruct(m_FriendList.GetItemText(i, 1), wxid);
					
					COPYDATASTRUCT MessageData;
					MessageData.dwData = WM_SendTextMessage;	//发送文本信息标志
					MessageData.cbData = sizeof(MessageStruct);
					MessageData.lpData = message;
					//发送消息
					pWnd->SendMessage(WM_COPYDATA, NULL, (LPARAM)&MessageData);
					Sleep(200);
					//清空文本
					delete message;
					UpdateData(FALSE);
				}
				else {
					MessageBox(L"找不到 WeChatHelper 窗口");
				}
			}
		}
	}
	return 0;
}


void CFriendList::OnBnClickedCancelall()
{
	// TODO: 在此添加控件通知处理程序代码
	for (int i = 0; i < m_FriendList.GetItemCount(); i++)
	{
		m_FriendList.SetCheck(i, FALSE);
	}
}


void CFriendList::OnBnClickedBtSeach()
{
	// TODO: 在此添加控件通知处理程序代码
	CString mSeach;
	this->GetDlgItemText(IDC_ED_Seach, mSeach);
	MessageStruct* message = new MessageStruct(TEXT("Seach"), mSeach);
	this->SendMessage(WM_MAMESSAGE, (WPARAM)message, 0);
	delete message;
	//parent->GetWindowTextW(s);
	//MessageBox(s);
	//parent->m_MyTable.m_Dia[1]->SendMessage(WM_Set_AcceptED, 0, 0);
}

void CFriendList::OnBnClickedBtSeach2()
{
	// TODO: 在此添加控件通知处理程序代码
	CString mSeach;
	this->GetDlgItemText(IDC_ED_Seach, mSeach);
	MessageStruct* message = new MessageStruct(TEXT("Seach2"), mSeach);
	this->SendMessage(WM_MAMESSAGE, (WPARAM)message, 0);
	delete message;
}

void CFriendList::OnBnClickedCheckall()
{
	// TODO: 在此添加控件通知处理程序代码


}


BOOL CFriendList::PreTranslateMessage(MSG* pMsg)	//取消对话框对 ESC，回车键的响应
{
	// TODO: 在此添加专用代码和/或调用基类
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_ESCAPE)     return   TRUE;
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN)   return   TRUE;
	else
		return   CDialog::PreTranslateMessage(pMsg);
}


//************************************************************
// 函数名称: OnRclickFriendlist
// 函数说明: 响应List控件的右键点击消息 
// 作    者: GuiShou
// 时    间: 2019/7/6
// 参    数: WPARAM wParam, LPARAM lParam
// 返 回 值: void
//***********************************************************
void CFriendList::OnNMRClickFriendlist(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);

	//判断选中的是群ID还是个人微信ID
	if (StrStrW(wxid, L"chatroom") == NULL)
	{
		//弹出好友相关的菜单
		CMenu m_Menu, * tMenu;
		m_Menu.LoadMenu(IDR_MENU2);

		//拿到第0个下拉菜单(菜单可能有很多列 这个函数是拿到第几列)
		tMenu = m_Menu.GetSubMenu(0);

		//获取鼠标位置
		CPoint pt;
		GetCursorPos(&pt);

		//弹出菜单
		TrackPopupMenu(tMenu->m_hMenu, TPM_LEFTALIGN, pt.x, pt.y, 0, m_hWnd, 0);

	}
	else
	{
		//弹出群相关的菜单
		CMenu m_Menu, * tMenu;
		m_Menu.LoadMenu(IDR_MENU2);

		//拿到第0个下拉菜单(菜单可能有很多列 这个函数是拿到第几列)
		tMenu = m_Menu.GetSubMenu(1);

		//获取鼠标位置
		CPoint pt;
		GetCursorPos(&pt);

		//弹出菜单
		TrackPopupMenu(tMenu->m_hMenu, TPM_LEFTALIGN, pt.x, pt.y, 0, m_hWnd, 0);
	}
	*pResult = 0;
}

void CFriendList::OnLoad()
{
	// TODO: 在此添加命令处理程序代码
	wstring wxUserFileName = L"UserLists.txt";

	// 以读模式打开文件
	ifstream infile;
	infile.open(wxUserFileName);
	string m_data = "";
	CString m_str;
	int n = m_FriendList.GetItemCount();
	int m = m_FriendList.GetHeaderCtrl()->GetItemCount();	//列数
	if (n > 1) {
		vector< vector<CString> > m_arry;
		vector<CString> m_item;
		while (getline(infile, m_data))
		{
			m_str = m_data.c_str();
			for (int i = n-1; i >=0; i--) {
				if (m_FriendList.GetItemText(i, 1) == m_str) {
					//m_FriendList.SetCheck(i);
					m_item.resize(0);
					for (int j = 0; j < m; j++) {
						m_item.push_back(m_FriendList.GetItemText(i, j));
					}
					m_arry.push_back(m_item);
					m_FriendList.DeleteItem(i);

				}
			}
		}
		int m_num = size(m_arry);
		if (m_num > 0) {
			for (int k = 0; k < m_num; k++) {
				m_FriendList.InsertItem(0, m_arry[k][0]);
				for (int l = 1; l < m; l++) {
					m_FriendList.SetItemText(0, l, m_arry[k][l]);
				}
			}
			for (int o = m_num - 1; o >= 0; o--) {
				//m_FriendList.EnsureVisible(o, FALSE);
				m_FriendList.SetCheck(o);
				m_FriendList.SetItemState(o, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);
				m_FriendList.SetFocus();
			}
		}
		m_arry.clear();
	}
	//如果列表中没有好友信息，则只添加微信ID
	else {
		while (getline(infile, m_data))
		{
			m_str = m_data.c_str();
			m_FriendList.InsertItem(0, L"id");
			m_FriendList.SetItemText(0, 1, m_str);
		}
		for (int i = 0; i < m_FriendList.GetItemCount();i++) {
			m_FriendList.SetCheck(i);
		}
	}
	// 关闭打开的文件
	infile.close();
}

//************************************************************
// 函数名称: OnSavefriendlist
// 函数说明: 响应保存好友列表消息
// 作    者: GuiShou
// 时    间: 2020/02/19
// 参    数: void
// 返 回 值: void
//***********************************************************
void CFriendList::OnSave()
{
	// TODO: 在此添加命令处理程序代码
	wstring wxUserFileName = L"UserLists.txt";

	//作为输出文件打开
	ofstream ofile;
	ofile.open(wxUserFileName, ios_base::trunc | ios_base::binary | ios_base::in);

	//获取所有行的数量
	int nRow = m_FriendList.GetItemCount();

	//获取所有列的数量
	//int nCol = m_FriendList.GetHeaderCtrl()->GetItemCount();

	//开始循环行和列
	for (int i = 0; i < nRow; i++)
	{
		if (m_FriendList.GetCheck(i))
		{
			USES_CONVERSION;
			CString csTemp = m_FriendList.GetItemText(i, 1);
			string sTemp(CW2A(csTemp.GetString()));
			char const* pos = (char const*)sTemp.c_str();
			//写入文件
			ofile.write(pos, sTemp.length());

			char const* cLine = "\r\n";
			ofile.write(cLine, strlen(cLine));
		}
			
	}

	ofile.flush();
	ofile.close();
}


void CFriendList::OnBnClickedReversechoose()
{
	// TODO: 在此添加控件通知处理程序代码
	CString s;
	GetDlgItemText(IDC_CH_Forward, s);
	if (IsDlgButtonChecked(IDC_CH_Forward)) {
		MessageBox(s);
	}
}




void CFriendList::OnSendimg()
{
	// TODO: 在此添加命令处理程序代码
	CString strFile = _T("");
	CFileDialog dlgFile(TRUE, NULL, NULL, OFN_HIDEREADONLY, _T("Describe Files (*.jpg)|*.jpg|All Files (*.*)|*.*||"), NULL);
	if (dlgFile.DoModal())
	{
		strFile = dlgFile.GetPathName();
		if (GetFileAttributes(strFile) == INVALID_FILE_ATTRIBUTES)
		{
			MessageBox(L"文件不存在 请重试");
			return;
		}
		if (MessageBox(TEXT("是否确定发送图片："+ strFile), L"提示", MB_YESNO)==IDYES){
			int m_num = m_FriendList.GetItemCount();
			for (int i = 0; i < m_num; i++) {
				if (m_FriendList.GetCheck(i)) {
					//查找窗口
					CWnd* pWnd = CWnd::FindWindow(NULL, L"WeChatHelper");
					if (pWnd != NULL) {
						//同步控件数据到变量
						UpdateData(TRUE);
						//填充数据到结构体
						MessageStruct* message = new MessageStruct(m_FriendList.GetItemText(i, 1), strFile);
					
						COPYDATASTRUCT MessageData;
						MessageData.dwData = WM_SendImageMessage;	//发送图片信息标志
						MessageData.cbData = sizeof(MessageStruct);
						MessageData.lpData = message;
						//发送消息
						pWnd->SendMessage(WM_COPYDATA, NULL, (LPARAM)&MessageData);
						Sleep(200);
						//清空文本
						delete message;
						UpdateData(FALSE);
					}
					else {
						MessageBox(L"找不到 WeChatHelper 窗口");
					}
				}
			}
		}
	}
}


void CFriendList::OnSendfile()
{
	// TODO: 在此添加命令处理程序代码
	CString strFile = _T("");
	CFileDialog dlgFile(TRUE, NULL, NULL, OFN_HIDEREADONLY, _T("Describe Files (*.zip)|*.zip|All Files (*.*)|*.*||"), NULL);
	if (dlgFile.DoModal())
	{
		strFile = dlgFile.GetPathName();
		if (GetFileAttributes(strFile) == INVALID_FILE_ATTRIBUTES)
		{
			MessageBox(L"文件不存在 请重试");
			return;
		}
		if (MessageBox(TEXT("是否确定发送图片：" + strFile), L"提示", MB_YESNO) == IDYES) {
			int m_num = m_FriendList.GetItemCount();
			for (int i = 0; i < m_num; i++) {
				if (m_FriendList.GetCheck(i)) {
					//查找窗口
					CWnd* pWnd = CWnd::FindWindow(NULL, L"WeChatHelper");
					if (pWnd != NULL) {
						//同步控件数据到变量
						UpdateData(TRUE);
						//填充数据到结构体
						MessageStruct* message = new MessageStruct(m_FriendList.GetItemText(i, 1), strFile);
						COPYDATASTRUCT MessageData;
						MessageData.dwData = WM_SendFileMessage;	//发送文件信息标志
						MessageData.cbData = sizeof(MessageStruct);
						MessageData.lpData = message;
						//发送消息
						pWnd->SendMessage(WM_COPYDATA, NULL, (LPARAM)&MessageData);
						Sleep(200);
						//清空文本
						delete message;
						UpdateData(FALSE);
					}
					else {
						MessageBox(L"找不到 WeChatHelper 窗口");
					}
				}
			}
		}
	}
}


void CFriendList::OnSendcard()
{
	// TODO: 在此添加命令处理程序代码
	CString wx_id ,wx_name;
	int sel_num = m_FriendList.GetSelectionMark();
	wx_id = m_FriendList.GetItemText(sel_num, 1);
	wx_name = m_FriendList.GetItemText(sel_num, 3);
	if (wx_id != "") {
		if (MessageBox(TEXT("是否确定发送用户 " + wx_name + " 名片？"), L"提示", MB_YESNO) == IDYES){
			int m_num = m_FriendList.GetItemCount();
			for (int i = 0; i < m_num; i++) {
				if (m_FriendList.GetCheck(i)) {
					//查找窗口
					CWnd* pWnd = CWnd::FindWindow(NULL, L"WeChatHelper");
					if (pWnd != NULL) {
						//同步控件数据到变量
						UpdateData(TRUE);
						CString m_Wxid = m_FriendList.GetItemText(i, 1);
						unique_ptr<XmlCardMessage> pCardMessage(new XmlCardMessage);
						wcscpy_s(pCardMessage->RecverWxid, wcslen(m_Wxid) + 1, m_Wxid);
						wcscpy_s(pCardMessage->SendWxid, wcslen(wx_id) + 1, wx_id);
						wcscpy_s(pCardMessage->NickName, wcslen(wx_name) + 1, wx_name);

						COPYDATASTRUCT sendcard;
						sendcard.dwData = WM_SendXmlCard;
						sendcard.cbData = sizeof(XmlCardMessage);
						sendcard.lpData = pCardMessage.get();
						//发送消息
						pWnd->SendMessage(WM_COPYDATA, NULL, (LPARAM)&sendcard);

						UpdateData(FALSE);
					}
					else {
						MessageBox(L"找不到 WeChatHelper 窗口");
					}
				}
			}
		}
	}
	
}
