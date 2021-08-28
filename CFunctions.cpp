// CFunctions.cpp: 实现文件
//

#include "pch.h"
#include "My_InjectTools.h"
#include "CFunctions.h"
#include "afxdialogex.h"
#include "CMultiOpen.h"


// CFunctions 对话框

IMPLEMENT_DYNAMIC(CFunctions, CDialogEx)

CFunctions::CFunctions(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_FUNCTIONS, pParent)
{

}

CFunctions::~CFunctions()
{
}

void CFunctions::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CFunctions, CDialogEx)
	ON_BN_CLICKED(IDC_BT_MULTI, &CFunctions::OnBnClickedBtMulti)
END_MESSAGE_MAP()


// CFunctions 消息处理程序


void CFunctions::OnBnClickedBtMulti()
{
	// TODO: 在此添加控件通知处理程序代码
	if (PatchWeChat()==1) {
		OpenWeChat();
	}
}

BOOL CFunctions::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 在此添加专用代码和/或调用基类
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_ESCAPE)     return   TRUE;
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN)   return   TRUE;
	else
		return CDialogEx::PreTranslateMessage(pMsg);
}
