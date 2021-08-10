// CResult.cpp: 实现文件
//

#include "pch.h"
#include "My_InjectTools.h"
#include "CResult.h"
#include "afxdialogex.h"


// CResult 对话框

IMPLEMENT_DYNAMIC(CResult, CDialogEx)

CResult::CResult(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_RESULT, pParent)
{

}

CResult::~CResult()
{
}

void CResult::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CResult, CDialogEx)
END_MESSAGE_MAP()


// CResult 消息处理程序
