#include "pch.h"
//#include "stdafx.h"
#include "CInjectTools.h"
#include <direct.h>
#include <stdlib.h>
#include <TlHelp32.h>
#include <stdio.h>
#pragma comment(lib,"advapi32")


#define WECHAT_PROCESS_NAME "WeChat.exe"
#define DLLNAME "My_WeChatDll.dll"


CString GetAppRegeditPath(CString strAppName)
{
	//定义相关变量
	HKEY hKey;
	CString strAppRegeditPath("");
	TCHAR szProductType[MAX_PATH];
	memset(szProductType, 0, sizeof(szProductType));

	DWORD dwBuflen = MAX_PATH;
	LONG lRet = 0;

	//下面是打开注册表,只有打开后才能做其他操作
	lRet = RegOpenKeyEx(HKEY_CURRENT_USER, //要打开的根键
		LPCTSTR(strAppName), //要打开的子子键
		0, //这个一定为0
		KEY_QUERY_VALUE, //指定打开方式,此为读
		&hKey); //用来返回句柄

	if (lRet != ERROR_SUCCESS) //判断是否打开成功
	{
		return strAppRegeditPath;
	}
	else
	{
		//下面开始查询
		lRet = RegQueryValueEx(hKey, //打开注册表时返回的句柄
			TEXT("Wechat"), //要查询的名称,查询的软件安装目录在这里
			NULL, //一定为NULL或者0
			NULL,
			(LPBYTE)szProductType, //我们要的东西放在这里
			&dwBuflen);

		if (lRet != ERROR_SUCCESS) //判断是否查询成功
		{
			return strAppRegeditPath;
		}
		else
		{
			RegCloseKey(hKey);
			
			strAppRegeditPath = szProductType;

			int nPos = strAppRegeditPath.Find('-');

			if (nPos >= 0)
			{
				CString sSubStr = strAppRegeditPath.Left(nPos - 1);//包含$,不想包含时nPos+1
				strAppRegeditPath = sSubStr;
			}
		}
	}
	return strAppRegeditPath;
}
CString GetAppRegeditPath2(CString strAppName)
{
	//定义相关变量
	HKEY hKey;
	CString strAppRegeditPath("");
	TCHAR szProductType[MAX_PATH];
	memset(szProductType, 0, sizeof(szProductType));

	DWORD dwBuflen = MAX_PATH;
	LONG lRet = 0;

	//下面是打开注册表,只有打开后才能做其他操作
	lRet = RegOpenKeyEx(HKEY_CURRENT_USER, //要打开的根键
		LPCTSTR(strAppName), //要打开的子子键
		0, //这个一定为0
		KEY_QUERY_VALUE, //指定打开方式,此为读
		&hKey); //用来返回句柄

	if (lRet != ERROR_SUCCESS) //判断是否打开成功
	{
		return strAppRegeditPath;
	}
	else
	{
		//下面开始查询
		lRet = RegQueryValueEx(hKey, //打开注册表时返回的句柄
			TEXT("InstallPath"), //要查询的名称,查询的软件安装目录在这里
			NULL, //一定为NULL或者0
			NULL,
			(LPBYTE)szProductType, //我们要的东西放在这里
			&dwBuflen);

		if (lRet != ERROR_SUCCESS) //判断是否查询成功
		{
			return strAppRegeditPath;
		}
		else
		{
			RegCloseKey(hKey);
			strAppRegeditPath = szProductType;

		}
	}
	return strAppRegeditPath;
}
//************************************************************
// 函数名称: ProcessNameFindPID
// 函数说明: 通过进程名找到进程ID
// 作    者: GuiShou
// 时    间: 2019/6/30
// 参    数: ProcessName 进程名
// 返 回 值: DWORD 进程PID
//************************************************************
DWORD ProcessNameFindPID(const char* ProcessName)
{
	PROCESSENTRY32 pe32 = { 0 };
	pe32.dwSize = sizeof(PROCESSENTRY32);
	HANDLE hProcess = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);	//获取进程快照句柄
	if (Process32First(hProcess, &pe32) == TRUE)	//把进程句柄中每个进程名称赋给变量pe32
	{
		do
		{
			USES_CONVERSION;	//字符串类型转换标识符
			if (strcmp(ProcessName, W2A(pe32.szExeFile)) == 0)	//"wchar* to char*" 长字节 转换成 字节，判断是否存在同名进程
			{
				return pe32.th32ProcessID;	//存在则返回进程ID
			}
		} while (Process32Next(hProcess, &pe32)); // 遍历快照
	}
	return 0;
}

//************************************************************
// 函数名称: InjectDll
// 函数说明: 注入DLL
// 作    者: GuiShou
// 时    间: 2019/6/30
// 参    数: void
// 返 回 值: void
//************************************************************
BOOL InjectDll(HANDLE& wxPid)
{
	//获取当前工作目录下的dll
	char szPath[MAX_PATH] = { 0 };
	char* buffer = NULL;
	if ((buffer = _getcwd(NULL, 0)) == NULL)	//获取当前工作目录
	{
		MessageBoxA(NULL, "获取当前工作目录失败", "错误", 0);
		return FALSE;
	}
	sprintf_s(szPath, "%s\\%s", buffer, DLLNAME);	//组装注入DLL路径
	//获取微信Pid
	DWORD dwPid = ProcessNameFindPID(WECHAT_PROCESS_NAME);
	if (dwPid == 0)		//如果找不到进程，则。。。
	{
		CString wxStrAppName = TEXT("Software\\Microsoft\\Windows\\CurrentVersion\\Run");
		CString szProductType = GetAppRegeditPath(wxStrAppName);	//从注册表查找微信路径（开机启动项）

		if (szProductType.GetLength()<5)
		{
			wxStrAppName = TEXT("Software\\Tencent\\WeChat");
		    szProductType = GetAppRegeditPath2(wxStrAppName);	//从注册表查找微信路径（InstallPath的键值是微信安装路径）
			szProductType.Append(L"\\WeChat.exe");
		}
		STARTUPINFO si;			//传入创建进程信息
		PROCESS_INFORMATION pi;	//保存进程信息
		ZeroMemory(&si, sizeof(si));
		si.cb = sizeof(si);
		ZeroMemory(&pi, sizeof(pi));

		si.dwFlags = STARTF_USESHOWWINDOW;// 指定wShowWindow成员有效
		si.wShowWindow = TRUE;          // 此成员设为TRUE的话则显示新建进程的主窗口，
									   // 为FALSE的话则不显示

		CreateProcess(szProductType, NULL, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi);	//打开微信EXE进程

		HWND  hWechatMainForm = NULL;
		//WeChatLoginWndForPC
		while (NULL == hWechatMainForm)	//无限循环遍历进程窗口名称，检查微信登录窗口是否创建
		{
			hWechatMainForm = FindWindow(TEXT("WeChatLoginWndForPC"), NULL);
			Sleep(500);
		}
		if (NULL == hWechatMainForm)
		{
			return FALSE;
		}
		dwPid = pi.dwProcessId;
		wxPid = pi.hProcess;
	}
	//检测dll是否已经注入
	//if (CheckIsInject(dwPid))
	if (TRUE)
	{
		//打开进程
		HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwPid);
		if (hProcess == NULL)
		{
			MessageBoxA(NULL, "进程打开失败", "错误", 0);
			return FALSE;
		}
		//在微信进程中申请内存
		LPVOID pAddress = VirtualAllocEx(hProcess, NULL, MAX_PATH, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
		if (pAddress == NULL)
		{
			MessageBoxA(NULL, "内存分配失败", "错误", 0);
			return FALSE;
		}
		//写入dll路径到微信进程
		if (WriteProcessMemory(hProcess, pAddress, szPath, MAX_PATH, NULL) == 0)
		{
			MessageBoxA(NULL, "路径写入失败", "错误", 0);
			return FALSE;
		}

		//获取LoadLibraryA函数地址
		FARPROC pLoadLibraryAddress = GetProcAddress(GetModuleHandleA("kernel32.dll"), "LoadLibraryA");
		if (pLoadLibraryAddress == NULL)
		{
			MessageBoxA(NULL, "获取LoadLibraryA函数地址失败", "错误", 0);
			return FALSE;
		}
		//远程线程注入dll
		HANDLE hRemoteThread = CreateRemoteThread(hProcess, NULL, 0, (LPTHREAD_START_ROUTINE)pLoadLibraryAddress, pAddress, 0, NULL);
		if (hRemoteThread == NULL)
		{
			MessageBoxA(NULL, "远程线程注入失败", "错误", 0);
			return FALSE;
		}

		CloseHandle(hRemoteThread);
		CloseHandle(hProcess);
	}
	else
	{
		MessageBoxA(NULL, "dll已经注入，请勿重复注入", "提示", 0);
		return FALSE;
	}

	return TRUE;
}

//************************************************************
// 函数名称: CheckIsInject
// 函数说明: 检测是否已经注入dll
// 作    者: GuiShou
// 时    间: 2019/6/30
// 参    数: void
// 返 回 值: BOOL 
//************************************************************
BOOL CheckIsInject(DWORD dwProcessid)
{
	HANDLE hModuleSnap = INVALID_HANDLE_VALUE;
	//初始化模块信息结构体
	MODULEENTRY32 me32 = { sizeof(MODULEENTRY32) };
	//创建模块快照 1 快照类型 2 进程ID
	hModuleSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, dwProcessid);
	//如果句柄无效就返回false
	if (hModuleSnap == INVALID_HANDLE_VALUE)
	{
		MessageBoxA(NULL, "创建模块快照失败", "错误", MB_OK);
		return FALSE;
	}
	//通过模块快照句柄获取第一个模块的信息
	if (!Module32First(hModuleSnap, &me32))
	{
		MessageBoxA(NULL, "获取第一个模块的信息失败", "错误", MB_OK);
		//获取失败则关闭句柄
		CloseHandle(hModuleSnap);
		return FALSE;
	}
	do
	{
		if (StrCmpW(me32.szModule, L"My_WeChatDll.dll") == 0)	//StrCmpW比较两个字符串相等则返回0
		{
			return FALSE;
		}

	} while (Module32Next(hModuleSnap, &me32));	//循环遍历微信进程中是否存在"WeChatHelper.dll" 模块，存在则返回FALSE
	return TRUE;
}

