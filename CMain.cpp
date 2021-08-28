// CMain.cpp: 实现文件
//

#include "pch.h"
#include "My_InjectTools.h"
#include "CMain.h"
#include "afxdialogex.h"

#include "CFriendList.h"
#include "CChatRecords.h"
#include "CFunctions.h"

#include "data.h"
#include "message.h"

// CMain 对话框

IMPLEMENT_DYNAMIC(CMain, CDialogEx)

CMain::CMain(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_MAIN, pParent)
{

}

CMain::~CMain()
{
}

void CMain::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TAB1, m_MyTable);
}


BEGIN_MESSAGE_MAP(CMain, CDialogEx)
	ON_WM_COPYDATA()
	ON_WM_SIZE()
	ON_MESSAGE(WM_MYMESSAGE, &CMain::OnMymessage)
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB1, &CMain::OnTcnSelchangeTab1)
END_MESSAGE_MAP()


// CMain 消息处理程序




BOOL CMain::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	TCHAR * Name[3] = { TEXT("好友列表"), TEXT("聊天记录") , TEXT("功能大全") };
	//TCHAR Name[3][5] = { L"好友列表",L"聊天记录",L"功能大全"};
	for (int i = 0; i < 3; i++)
	{
		m_MyTable.InsertItem(i, Name[i]);
	}

	//给子窗口指针赋值

	m_MyTable.m_Dia[0] = new CFriendList();
	m_MyTable.m_Dia[1] = new CChatRecords();
	m_MyTable.m_Dia[2] = new CFunctions();

	//创建子窗口
	UINT DiaIds[3] = { IDD_FRIEND_LIST,IDD_CHAT_RECORDS ,IDD_FUNCTIONS };
	for (int i = 0; i < 3; i++)
	{
		m_MyTable.m_Dia[i]->Create(DiaIds[i], &m_MyTable);
	}

	//控制两个窗口的大小
	CRect rc;
	m_MyTable.GetClientRect(rc);
	rc.DeflateRect(2, 23, 2, 2);
	for (int i = 0; i < 2; i++)
	{
		m_MyTable.m_Dia[i]->MoveWindow(rc);
	}


	//显示第一个子窗口
	m_MyTable.m_Dia[0]->ShowWindow(SW_SHOW);
	for (int i = 1; i < 3; i++)
	{
		m_MyTable.m_Dia[i]->ShowWindow(SW_HIDE);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}


void CMain::OnTcnSelchangeFriendlist(NMHDR* pNMHDR, LRESULT* pResult)
{
	// TODO: 在此添加控件通知处理程序代码
	*pResult = 0;
}


//************************************************************
// 函数名称: OnCopyData
// 函数说明: 响应CopyData消息
// 作    者: GuiShou
// 时    间: 2019/7/5
// 参    数: CWnd* pWnd, COPYDATASTRUCT* pCopyDataStruct
// 返 回 值: BOOL
//***********************************************************
BOOL CMain::OnCopyData(CWnd* pWnd, COPYDATASTRUCT* pCopyDataStruct)
{
	//显示好友列表
	if (pCopyDataStruct->dwData == WM_GetFriendList)
	{
		UserInfo* user = (UserInfo*)pCopyDataStruct->lpData;
		//将消息发送给子窗口
		m_MyTable.m_Dia[0]->SendMessage(WM_ShowFriendList, (WPARAM)user, NULL);
	}
	//显示聊天记录
	else if (pCopyDataStruct->dwData == WM_ShowChatRecord)
	{
		ChatMessageData* msg = (ChatMessageData*)pCopyDataStruct->lpData;
		//将消息发送给子窗口
		m_MyTable.m_Dia[1]->SendMessage(WM_ShowMessage, (WPARAM)msg, NULL);
	}


	return CDialogEx::OnCopyData(pWnd, pCopyDataStruct);
}

void CMain::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	// TODO: 在此处添加消息处理程序代码
	m_MyTable.m_Dia[0]->MoveWindow(0, 20, cx, cy-15);
	m_MyTable.m_Dia[1]->MoveWindow(0, 20, cx, cy-15);
	m_MyTable.m_Dia[2]->MoveWindow(0, 20, cx, cy - 15);
}


afx_msg LRESULT CMain::OnMymessage(WPARAM wParam, LPARAM lParam)
{
	MessageBox(L"ADFA~",L"DFS",MB_OK);
	return 0;
}



void CMain::OnTcnSelchangeTab1(NMHDR* pNMHDR, LRESULT* pResult)
{
	// TODO: 在此添加控件通知处理程序代码
	*pResult = 0;
}
