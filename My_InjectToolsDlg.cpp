﻿
// My_InjectToolsDlg.cpp: 实现文件
//

#include "pch.h"
#include "framework.h"
#include "My_InjectTools.h"
#include "My_InjectToolsDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif
#include "message.h"
#include <string>
#include <sys/stat.h>
#include "CInjectTools.h"
#include "CMain.h"

HANDLE wxPid = NULL;		//微信的PID

// CMyInjectToolsDlg 对话框



CMyInjectToolsDlg::CMyInjectToolsDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_MY_INJECTTOOLS_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CMyInjectToolsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CMyInjectToolsDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_SHOW_QRPIC, &CMyInjectToolsDlg::OnBnClickedShowQrpic)
	ON_WM_COPYDATA()
	ON_BN_CLICKED(IDC_BUTTON1, &CMyInjectToolsDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &CMyInjectToolsDlg::OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON3, &CMyInjectToolsDlg::OnBnClickedButton3)
END_MESSAGE_MAP()


// CMyInjectToolsDlg 消息处理程序

BOOL CMyInjectToolsDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	//ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	//ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			//pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码

	//防多开
	RunSingle();

	//往微信注入DLL
	if (InjectDll(wxPid) == FALSE)
	{
		ExitProcess(-1);
	}

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CMyInjectToolsDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	CDialogEx::OnSysCommand(nID, lParam);
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CMyInjectToolsDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CMyInjectToolsDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CMyInjectToolsDlg::OnBnClickedShowQrpic()
{
	INT_PTR nRes;
	nRes = MessageBox(_T("是否继续？"), _T("提示"), MB_OKCANCEL | MB_ICONQUESTION);
	if (IDOK == nRes) {
		//查找窗口
		CWnd* pWnd = CWnd::FindWindow(NULL, L"WeChatHelper");
		COPYDATASTRUCT show_qrpic;
		show_qrpic.dwData = WM_ShowQrPicture;
		show_qrpic.cbData = 0;
		show_qrpic.lpData = NULL;
		//发送消息
		pWnd->SendMessage(WM_COPYDATA, NULL, (LPARAM)&show_qrpic);

		Sleep(200);
		//显示图片
		ShowPicture();
	}

}

//************************************************************
// 函数名称: ShowPicture
// 函数说明: 显示二维码
// 作    者: GuiShou
// 时    间: 2019/6/30
// 参    数: void
// 返 回 值: void
//***********************************************************
void CMyInjectToolsDlg::ShowPicture()
{
	char szTempPath[MAX_PATH] = { 0 };
	char szPicturePath[MAX_PATH] = { 0 };

	//获取Temp目录
	GetTempPathA(MAX_PATH, szTempPath);
	//拼接图片完整路径
	sprintf_s(szPicturePath, "%s%s", szTempPath, "qrcode.png");

	USES_CONVERSION;
	if (isFileExists_fopen(szPicturePath)) {
		CImage QrPic;
		QrPic.Load(A2W(szPicturePath));
		CRect rect;
		CWnd* pWnd = GetDlgItem(IDC_QRPIC);
		CDC* pDC = pWnd->GetDC();
		pWnd->GetClientRect(&rect);
		pDC->SetStretchBltMode(STRETCH_HALFTONE);
		QrPic.Draw(pDC->m_hDC, rect);
		ReleaseDC(pDC);
		QrPic.Destroy();
	}
	else {
		MessageBox(L"文件不存在", L"提示", MB_OK);
	}
	

	//删除临时文件夹下的图片
	DeleteFileA(szPicturePath);
}

inline bool isFileExists_fopen(const std::string& name) {
	struct stat buffer;
	return (stat(name.c_str(), &buffer) == 0);
}

//************************************************************
// 函数名称: RunSingle
// 函数说明: 防多开
// 作    者: GuiShou
// 时    间: 2019/7/9
// 参    数: void
// 返 回 值: void
//***********************************************************
void CMyInjectToolsDlg::RunSingle()
{
	HANDLE hMutex = NULL;
	hMutex = CreateMutexA(NULL, FALSE, "Get_WeChat");	//加锁 ，（解锁：ReleaseMutex(hMutex);）
	if (hMutex)
	{
		if (GetLastError() == ERROR_ALREADY_EXISTS)
		{
			ExitProcess(-1);
		}
	}
}

BOOL CMyInjectToolsDlg::OnCopyData(CWnd* pWnd, COPYDATASTRUCT* pCopyDataStruct)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if (pCopyDataStruct->dwData == WM_Login)
	{
		EndDialog(0);
		CMain mainWindow;
		mainWindow.DoModal();
	}
	else if (pCopyDataStruct->dwData == WM_AlreadyLogin)
	{
		MessageBoxA(NULL, "已经登陆微信 请重启微信 在未登录状态下运行程序", "Tip", 0);
		ExitProcess(-1);
	}

	return CDialogEx::OnCopyData(pWnd, pCopyDataStruct);
}


void CMyInjectToolsDlg::OnBnClickedButton1()
{
	// TODO: 在此添加控件通知处理程序代码
	CMain mainWindow;
	mainWindow.DoModal();
}

#include "md5.h"

string md5(string& strPlain)	//字符串MD5加密
{
	MD5_CTX mdContext;
	//int bytes;
	//unsigned char data[1024];

	MD5Init(&mdContext);
	MD5Update(&mdContext, (unsigned char*)const_cast<char*>(strPlain.c_str()), strPlain.size());
	MD5Final(&mdContext);

	string md5;
	char buf[3];
	for (int i = 0; i < 16; i++)
	{
		sprintf_s(buf, "%02x", mdContext.digest[i]);
		md5.append(buf);
	}
	return md5;
}

void CMyInjectToolsDlg::OnBnClickedButton2()
{
	// TODO: 在此添加控件通知处理程序代码
	string str = "zzw";
	string strMD5 = md5(str);
	MessageBox(CA2T(strMD5.c_str()));
	printf(strMD5.c_str());
}

#include "HTTPRequest.hpp"
#include "CChatRecords.h"
#include <iostream>

void CMyInjectToolsDlg::OnBnClickedButton3()
{
	// TODO: 在此添加控件通知处理程序代码
	try
	{

	}
	catch (const std::exception& e)
	{
		std::cout << "Request failed, error: " << e.what() << '\n';
	}
	string host = "http://127.0.0.1:1314/test";
	string data = "zzw168";
	string result = my_HttpPost(host, data);
	MessageBox(CA2T(result.c_str()));
}
