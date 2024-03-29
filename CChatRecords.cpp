﻿// CChatRecords.cpp: 实现文件
//

#include "pch.h"
#include "My_InjectTools.h"
#include "CChatRecords.h"
#include "afxdialogex.h"

#include "data.h"
#include "message.h"
#include <string>
#include <locale.h>
#include "CMain.h"
#include <fstream>

DWORD g_index = 0;

// CChatRecords 对话框

IMPLEMENT_DYNAMIC(CChatRecords, CDialogEx)

CChatRecords::CChatRecords(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_CHAT_RECORDS, pParent)
{

}

CChatRecords::~CChatRecords()
{
}

void CChatRecords::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_ChatRecord);
	DDX_Control(pDX, IDC_ED_Accept, Accept_id);
	DDX_Control(pDX, IDC_ED_Send, Send_id);
	DDX_Control(pDX, IDC_ED_MB, m_Member);
	DDX_Control(pDX, IDC_CH_All, m_CheckAll);
	DDX_Control(pDX, IDC_CH_Post, m_CH_Post);
}


BEGIN_MESSAGE_MAP(CChatRecords, CDialogEx)
	ON_MESSAGE(WM_ShowMessage, &CChatRecords::OnShowmessage)
	ON_MESSAGE(WM_Set_AcceptED, &CChatRecords::OnSetAccepted)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST1, &CChatRecords::OnNMDblclkList1)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST1, &CChatRecords::OnLvnItemchangedList1)
	ON_BN_CLICKED(IDC_CH_Post, &CChatRecords::OnBnClickedChPost)
END_MESSAGE_MAP()


// CChatRecords 消息处理程序

#include "HTTPRequest.hpp"
#include <iostream>

string my_HttpPost(string& host, string& data)	//
{
	string result = "ok";
	try
	{
		// you can pass http::InternetProtocol::V6 to Request to make an IPv6 request
		http::Request request(host);

		// send a post request
		const http::Response postResponse = request.send("POST", data, {
			"Content-Type: application/x-www-form-urlencoded"
			}, std::chrono::seconds(2));
		//std::cout << std::string(postResponse.body.begin(), postResponse.body.end()) << '\n'; // print the result
		//result = string(postResponse.body.begin(), postResponse.body.end());
	}
	catch (const std::exception& e)
	{
		//std::cerr << "Request failed, error: " << e.what() << '\n';
		result = string(e.what());
	}
	return result;
}


//************************************************************
// 函数名称: OnShowmessage
// 函数说明: 响应Showmessage消息 处理父窗口发过来的消息
// 作    者: GuiShou
// 时    间: 2019/7/6
// 参    数: WPARAM wParam, LPARAM lParam
// 返 回 值: BOOL
//***********************************************************
afx_msg LRESULT CChatRecords::OnShowmessage(WPARAM wParam, LPARAM lParam)
{
	//取数据
	ChatMessageData* msg = (ChatMessageData*)wParam;
	//MessageBox(TEXT("nih"),L"tis", MB_OK);
	string type = Wchar_tToString(msg->sztype);
	string wxid = Wchar_tToString(msg->wxname);
	string source = Wchar_tToString(msg->source);
	string sendername = Wchar_tToString(msg->sendername);
	string content = Wchar_tToString(msg->content);
	if (IsDlgButtonChecked(IDC_CH_Save)) {
		//保存数据到本地
		Log(type.c_str(), wxid.c_str(), source.c_str(), sendername.c_str(), content.c_str());
	}

	//显示到控件
	m_ChatRecord.InsertItem(g_index, msg->sztype);
	m_ChatRecord.SetItemText(g_index, 1, msg->source);
	m_ChatRecord.SetItemText(g_index, 2, msg->wxname);
	m_ChatRecord.SetItemText(g_index, 3, msg->sendername);
	m_ChatRecord.SetItemText(g_index, 4, msg->content);

	//转发信息到 指定微信id***********************************************
	CString  Ed_Accept;
	GetDlgItem(IDC_ED_Accept)->GetWindowText(Ed_Accept);	//转发接收者ID
	string s_Ed_Accept =CT2A(Ed_Accept.GetString());
	CString  Ed_wxid;
	GetDlgItem(IDC_ED_Send)->GetWindowText(Ed_wxid);		//消息发送者ID
	string s_Ed_wxid = CT2A(Ed_wxid.GetString());
	CString  Ed_wxmember;
	GetDlgItem(IDC_ED_MB)->GetWindowText(Ed_wxmember);
	string s_Ed_wxmember = CT2A(Ed_wxmember.GetString());	//消息发送者
	if ((0 == strcmp(type.c_str(), "文字")) 
		&& (0 == strcmp(wxid.c_str(), s_Ed_wxid.c_str()))	//对比监听微信ID
		&& ((0 == strcmp(sendername.c_str(), s_Ed_wxmember.c_str()))||(m_CheckAll.GetCheck()))	//对比监听群成员ID，或者监听全群
		&&((Ed_Accept !="")||(m_CH_Post.GetCheck()))) {		//判断接收者ID ,或者发送到IP地址
		//string content = Wchar_tToString(msg->content);
		string& m_content = content + "\n发送者:" + wxid + " 成员:" + sendername +"\n---------------------------";

		//查找窗口
		CWnd* pWnd = CWnd::FindWindow(NULL, L"WeChatHelper");
		if (pWnd != NULL) {
			try {
				CMain* m_Main = (CMain*)this->GetParent()->GetParent();
				if (m_Main->m_MyTable.m_Dia[0]->IsDlgButtonChecked(IDC_CH_Forward)) {
					MessageStruct* message = new MessageStruct(TEXT("GMessage"), CA2T(m_content.c_str()));
					m_Main->m_MyTable.m_Dia[0]->SendMessage(WM_MAMESSAGE, (WPARAM)message, 0);
				}
				//转发信息到 指定微信id**************************************************
				else if (Ed_Accept != "") {
					//填充数据到结构体
					MessageStruct* message = new MessageStruct(StringToWchar_t(s_Ed_Accept), StringToWchar_t(m_content));
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
			catch (exception e) {
				MessageBox(LPCTSTR(e.what()));
			}
		}
		else {
			MessageBox(L"找不到 WeChatHelper 窗口");
		}
		//post信息到 指定IP地址**************************************************
		if (m_CH_Post.GetCheck() == TRUE) {
			// TODO: 在此添加命令处理程序代码
			wstring m_PostIp = L"PostLists.txt";
			// 以读模式打开文件
			ifstream infile;
			infile.open(m_PostIp);
			string m_data = "";
			while (getline(infile, m_data))
			{
				if (m_data.find("http")!= string::npos) {
					string& host = m_data;
					string result = my_HttpPost(host, m_content);
					if (result != "ok") {
						cout << result << endl;
						m_CH_Post.SetCheck(FALSE);
					}
				}
				
			}
			// 关闭打开的文件
			infile.close();
		}
	}
	return 0;
}

//************************************************************
// 函数名称: string_To_UTF8
// 函数说明: string转UTF8
// 作    者: GuiShou
// 时    间: 2019/11/7
// 参    数: str 需要转换的字符串
// 返 回 值: string 转换后的字符串
//***********************************************************
string string_To_UTF8(const std::string& str)
{
	int nwLen = ::MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, NULL, 0);

	wchar_t* pwBuf = new wchar_t[nwLen + 1];//一定要加1，不然会出现尾巴 
	ZeroMemory(pwBuf, nwLen * 2 + 2);

	::MultiByteToWideChar(CP_ACP, 0, str.c_str(), str.length(), pwBuf, nwLen);

	int nLen = ::WideCharToMultiByte(CP_UTF8, 0, pwBuf, -1, NULL, NULL, NULL, NULL);

	char* pBuf = new char[nLen + 1];
	ZeroMemory(pBuf, nLen + 1);

	::WideCharToMultiByte(CP_UTF8, 0, pwBuf, nwLen, pBuf, nLen, NULL, NULL);

	std::string retStr(pBuf);

	delete[]pwBuf;
	delete[]pBuf;

	pwBuf = NULL;
	pBuf = NULL;

	return retStr;
}

string UTF8ToGB(const char* str)
{
	string result;
	WCHAR* strSrc;
	LPSTR szRes;

	//获得临时变量的大小
	int i = MultiByteToWideChar(CP_UTF8, 0, str, -1, NULL, 0);
	strSrc = new WCHAR[i + 1];
	MultiByteToWideChar(CP_UTF8, 0, str, -1, strSrc, i);

	//获得临时变量的大小
	i = WideCharToMultiByte(CP_ACP, 0, strSrc, -1, NULL, 0, NULL, NULL);
	szRes = new CHAR[i + 1];
	WideCharToMultiByte(CP_ACP, 0, strSrc, -1, szRes, i, NULL, NULL);

	result = szRes;
	delete[]strSrc;
	delete[]szRes;

	return result;
}

//************************************************************
// 函数名称: Wchar_tToString
// 函数说明: wchar转string
// 作    者: GuiShou
// 时    间: 2019/11/7
// 参    数: wchar 需要转换的字符串
// 返 回 值: string 转换后的字符串
//***********************************************************
string Wchar_tToString(wchar_t* wchar)
{
	std::string szDst;
	wchar_t* wText = wchar;
	DWORD dwNum = WideCharToMultiByte(CP_OEMCP, NULL, wText, -1, NULL, 0, NULL, FALSE);// WideCharToMultiByte的运用
	char* psText; // psText为char*的临时数组，作为赋值给std::string的中间变量
	psText = new char[dwNum];
	WideCharToMultiByte(CP_OEMCP, NULL, wText, -1, psText, dwNum, NULL, FALSE);// WideCharToMultiByte的再次运用
	szDst = psText;// std::string赋值
	delete[]psText;// psText的清除
	return szDst;
}


//************************************************************
// 函数名称: StringToWchar_t
// 函数说明: string转wchar
// 作    者: GuiShou
// 时    间: 2019/11/7
// 参    数: str 需要转换的字符串
// 返 回 值: wchar_t* 转换后的字符串
//***********************************************************
wchar_t* StringToWchar_t(const std::string& str)
{
	wchar_t* m_chatroommmsg = new wchar_t[str.size()+1];  //
	memset(m_chatroommmsg, 0, str.size()+1);
	//setlocale(LC_ALL, "zh_CN.UTF-8");
	setlocale(LC_ALL, "");
	swprintf(m_chatroommmsg, str.size()+1, L"%S", str.c_str());

	return m_chatroommmsg;
}


#pragma warning(disable:4996)
//************************************************************
// 函数名称: Log
// 函数说明: 记录聊天记录到本地
// 作    者: GuiShou
// 时    间: 2019/11/7
// 参    数: type 消息类型 wxid微信ID source消息来源 msgSender消息发送者 content消息内容
// 返 回 值: void
//***********************************************************
void Log(const std::string& type, const std::string& wxid, const std::string& source, const std::string& msgSender, const std::string& content)
{
	setlocale(LC_ALL, "zh_CN.UTF-8");
	time_t t = time(0);
	char ch[64];
	strftime(ch, sizeof(ch), "%Y-%m-%d %H-%M-%S", localtime(&t)); //年-月-日 时-分-秒
	std::string times = ch;
	std::string log;
	if (strstr(msgSender.c_str(), "NULL") != NULL)
	{
		log = string_To_UTF8(
			"************************ " + times + " ************************" +
			"\n" + "消息类型：" + type +
			"\n" + "消息来源：" + source +
			"\n" + "微信昵称：" + wxid +
			"\n" + "消息内容：" + content +
			"\n" + "-------------------------------- 分割线 --------------------------------\n\n\n"
		);
	}
	else
	{
		log = string_To_UTF8(
			"************************ " + times + " ************************" +
			"\n" + "消息类型：" + type +
			"\n" + "消息来源：" + source +
			"\n" + "群名称：" + wxid +
			"\n" + "群发送者：" + msgSender +
			"\n" + "消息内容：" + content +
			"\n" + "-------------------------------- 分割线 --------------------------------\n\n\n"
		);
	}


	FILE* fp = fopen("log.txt", "ab+");
	fwrite(log.c_str(), log.length(), 1, fp);
	fclose(fp);
}


BOOL CChatRecords::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	m_ChatRecord.InsertColumn(0, L"消息类型", 0, 100);
	m_ChatRecord.InsertColumn(1, L"消息来源", 0, 100);
	m_ChatRecord.InsertColumn(2, L"微信昵称/群名称", 0, 150);
	m_ChatRecord.InsertColumn(3, L"群发送者", 0, 150);
	m_ChatRecord.InsertColumn(4, L"消息内容", 0, 465);
	m_ChatRecord.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}


afx_msg LRESULT CChatRecords::OnSetAccepted(WPARAM wParam, LPARAM lParam)
{
	//取数据
	MessageStruct* msg = (MessageStruct*)wParam;
	CString num = (msg->msgdata1);
	CString wxid = (msg->msgdata2);
	if (num == TEXT("Accept")) {
		SetDlgItemText(IDC_ED_Accept, wxid);
	}
	else if (num == TEXT("NameInfor")) {
		SetDlgItemText(IDC_ED_Send, wxid);
	}
	else if (num == TEXT("Member")) {
		SetDlgItemText(IDC_ED_MB, wxid);
	}
	else {
		CheckDlgButton(IDC_CH_All,TRUE);
	}
		
	return 0;
}


void CChatRecords::OnNMDblclkList1(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	CListCtrl* pListCtrl = (CListCtrl*)GetDlgItem(IDC_LIST1);
	ASSERT(pListCtrl != NULL);
	POSITION pos = pListCtrl->GetFirstSelectedItemPosition();
	if (pos == NULL) {
		TRACE0("No items were selected!\n");
		/*SetDlgItemText(IDC_ED_Send, TEXT("ok"));
		SetDlgItemText(IDC_ED_MB, TEXT("no!"));*/
	}
	else
	{
		while (pos)
		{
			int nItem = pListCtrl->GetNextSelectedItem(pos);
			// you could do your own processing on nItem here
			CString NameInfor = pListCtrl->GetItemText(nItem, 2);
			MessageStruct* message_a = new MessageStruct(TEXT("NameInfor"), NameInfor);
			this->SendMessage(WM_Set_AcceptED, (WPARAM)message_a, NULL);
			
			CString Member = pListCtrl->GetItemText(nItem, 3);
			MessageStruct* message_b = new MessageStruct(TEXT("Member"), Member);
			this->SendMessage(WM_Set_AcceptED, (WPARAM)message_b, NULL);

			delete message_a;
			delete message_b;

		}
	}

	*pResult = 0;
}


BOOL CChatRecords::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 在此添加专用代码和/或调用基类
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_ESCAPE)     return   TRUE;
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN)   return   TRUE;
	else
		return   CDialog::PreTranslateMessage(pMsg);
}


void CChatRecords::OnLvnItemchangedList1(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	*pResult = 0;
}


void CChatRecords::OnBnClickedChPost()
{
	// TODO: 在此添加控件通知处理程序代码
}
