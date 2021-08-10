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
	//������ر���
	HKEY hKey;
	CString strAppRegeditPath("");
	TCHAR szProductType[MAX_PATH];
	memset(szProductType, 0, sizeof(szProductType));

	DWORD dwBuflen = MAX_PATH;
	LONG lRet = 0;

	//�����Ǵ�ע���,ֻ�д򿪺��������������
	lRet = RegOpenKeyEx(HKEY_CURRENT_USER, //Ҫ�򿪵ĸ���
		LPCTSTR(strAppName), //Ҫ�򿪵����Ӽ�
		0, //���һ��Ϊ0
		KEY_QUERY_VALUE, //ָ���򿪷�ʽ,��Ϊ��
		&hKey); //�������ؾ��

	if (lRet != ERROR_SUCCESS) //�ж��Ƿ�򿪳ɹ�
	{
		return strAppRegeditPath;
	}
	else
	{
		//���濪ʼ��ѯ
		lRet = RegQueryValueEx(hKey, //��ע���ʱ���صľ��
			TEXT("Wechat"), //Ҫ��ѯ������,��ѯ�������װĿ¼������
			NULL, //һ��ΪNULL����0
			NULL,
			(LPBYTE)szProductType, //����Ҫ�Ķ�����������
			&dwBuflen);

		if (lRet != ERROR_SUCCESS) //�ж��Ƿ��ѯ�ɹ�
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
				CString sSubStr = strAppRegeditPath.Left(nPos - 1);//����$,�������ʱnPos+1
				strAppRegeditPath = sSubStr;
			}
		}
	}
	return strAppRegeditPath;
}
CString GetAppRegeditPath2(CString strAppName)
{
	//������ر���
	HKEY hKey;
	CString strAppRegeditPath("");
	TCHAR szProductType[MAX_PATH];
	memset(szProductType, 0, sizeof(szProductType));

	DWORD dwBuflen = MAX_PATH;
	LONG lRet = 0;

	//�����Ǵ�ע���,ֻ�д򿪺��������������
	lRet = RegOpenKeyEx(HKEY_CURRENT_USER, //Ҫ�򿪵ĸ���
		LPCTSTR(strAppName), //Ҫ�򿪵����Ӽ�
		0, //���һ��Ϊ0
		KEY_QUERY_VALUE, //ָ���򿪷�ʽ,��Ϊ��
		&hKey); //�������ؾ��

	if (lRet != ERROR_SUCCESS) //�ж��Ƿ�򿪳ɹ�
	{
		return strAppRegeditPath;
	}
	else
	{
		//���濪ʼ��ѯ
		lRet = RegQueryValueEx(hKey, //��ע���ʱ���صľ��
			TEXT("InstallPath"), //Ҫ��ѯ������,��ѯ�������װĿ¼������
			NULL, //һ��ΪNULL����0
			NULL,
			(LPBYTE)szProductType, //����Ҫ�Ķ�����������
			&dwBuflen);

		if (lRet != ERROR_SUCCESS) //�ж��Ƿ��ѯ�ɹ�
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
// ��������: ProcessNameFindPID
// ����˵��: ͨ���������ҵ�����ID
// ��    ��: GuiShou
// ʱ    ��: 2019/6/30
// ��    ��: ProcessName ������
// �� �� ֵ: DWORD ����PID
//************************************************************
DWORD ProcessNameFindPID(const char* ProcessName)
{
	PROCESSENTRY32 pe32 = { 0 };
	pe32.dwSize = sizeof(PROCESSENTRY32);
	HANDLE hProcess = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);	//��ȡ���̿��վ��
	if (Process32First(hProcess, &pe32) == TRUE)	//�ѽ��̾����ÿ���������Ƹ�������pe32
	{
		do
		{
			USES_CONVERSION;	//�ַ�������ת����ʶ��
			if (strcmp(ProcessName, W2A(pe32.szExeFile)) == 0)	//"wchar* to char*" ���ֽ� ת���� �ֽڣ��ж��Ƿ����ͬ������
			{
				return pe32.th32ProcessID;	//�����򷵻ؽ���ID
			}
		} while (Process32Next(hProcess, &pe32)); // ��������
	}
	return 0;
}

//************************************************************
// ��������: InjectDll
// ����˵��: ע��DLL
// ��    ��: GuiShou
// ʱ    ��: 2019/6/30
// ��    ��: void
// �� �� ֵ: void
//************************************************************
BOOL InjectDll(HANDLE& wxPid)
{
	//��ȡ��ǰ����Ŀ¼�µ�dll
	char szPath[MAX_PATH] = { 0 };
	char* buffer = NULL;
	if ((buffer = _getcwd(NULL, 0)) == NULL)	//��ȡ��ǰ����Ŀ¼
	{
		MessageBoxA(NULL, "��ȡ��ǰ����Ŀ¼ʧ��", "����", 0);
		return FALSE;
	}
	sprintf_s(szPath, "%s\\%s", buffer, DLLNAME);	//��װע��DLL·��
	//��ȡ΢��Pid
	DWORD dwPid = ProcessNameFindPID(WECHAT_PROCESS_NAME);
	if (dwPid == 0)		//����Ҳ������̣��򡣡���
	{
		CString wxStrAppName = TEXT("Software\\Microsoft\\Windows\\CurrentVersion\\Run");
		CString szProductType = GetAppRegeditPath(wxStrAppName);	//��ע������΢��·�������������

		if (szProductType.GetLength()<5)
		{
			wxStrAppName = TEXT("Software\\Tencent\\WeChat");
		    szProductType = GetAppRegeditPath2(wxStrAppName);	//��ע������΢��·����InstallPath�ļ�ֵ��΢�Ű�װ·����
			szProductType.Append(L"\\WeChat.exe");
		}
		STARTUPINFO si;			//���봴��������Ϣ
		PROCESS_INFORMATION pi;	//���������Ϣ
		ZeroMemory(&si, sizeof(si));
		si.cb = sizeof(si);
		ZeroMemory(&pi, sizeof(pi));

		si.dwFlags = STARTF_USESHOWWINDOW;// ָ��wShowWindow��Ա��Ч
		si.wShowWindow = TRUE;          // �˳�Ա��ΪTRUE�Ļ�����ʾ�½����̵������ڣ�
									   // ΪFALSE�Ļ�����ʾ

		CreateProcess(szProductType, NULL, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi);	//��΢��EXE����

		HWND  hWechatMainForm = NULL;
		//WeChatLoginWndForPC
		while (NULL == hWechatMainForm)	//����ѭ���������̴������ƣ����΢�ŵ�¼�����Ƿ񴴽�
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
	//���dll�Ƿ��Ѿ�ע��
	//if (CheckIsInject(dwPid))
	if (TRUE)
	{
		//�򿪽���
		HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwPid);
		if (hProcess == NULL)
		{
			MessageBoxA(NULL, "���̴�ʧ��", "����", 0);
			return FALSE;
		}
		//��΢�Ž����������ڴ�
		LPVOID pAddress = VirtualAllocEx(hProcess, NULL, MAX_PATH, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
		if (pAddress == NULL)
		{
			MessageBoxA(NULL, "�ڴ����ʧ��", "����", 0);
			return FALSE;
		}
		//д��dll·����΢�Ž���
		if (WriteProcessMemory(hProcess, pAddress, szPath, MAX_PATH, NULL) == 0)
		{
			MessageBoxA(NULL, "·��д��ʧ��", "����", 0);
			return FALSE;
		}

		//��ȡLoadLibraryA������ַ
		FARPROC pLoadLibraryAddress = GetProcAddress(GetModuleHandleA("kernel32.dll"), "LoadLibraryA");
		if (pLoadLibraryAddress == NULL)
		{
			MessageBoxA(NULL, "��ȡLoadLibraryA������ַʧ��", "����", 0);
			return FALSE;
		}
		//Զ���߳�ע��dll
		HANDLE hRemoteThread = CreateRemoteThread(hProcess, NULL, 0, (LPTHREAD_START_ROUTINE)pLoadLibraryAddress, pAddress, 0, NULL);
		if (hRemoteThread == NULL)
		{
			MessageBoxA(NULL, "Զ���߳�ע��ʧ��", "����", 0);
			return FALSE;
		}

		CloseHandle(hRemoteThread);
		CloseHandle(hProcess);
	}
	else
	{
		MessageBoxA(NULL, "dll�Ѿ�ע�룬�����ظ�ע��", "��ʾ", 0);
		return FALSE;
	}

	return TRUE;
}

//************************************************************
// ��������: CheckIsInject
// ����˵��: ����Ƿ��Ѿ�ע��dll
// ��    ��: GuiShou
// ʱ    ��: 2019/6/30
// ��    ��: void
// �� �� ֵ: BOOL 
//************************************************************
BOOL CheckIsInject(DWORD dwProcessid)
{
	HANDLE hModuleSnap = INVALID_HANDLE_VALUE;
	//��ʼ��ģ����Ϣ�ṹ��
	MODULEENTRY32 me32 = { sizeof(MODULEENTRY32) };
	//����ģ����� 1 �������� 2 ����ID
	hModuleSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, dwProcessid);
	//��������Ч�ͷ���false
	if (hModuleSnap == INVALID_HANDLE_VALUE)
	{
		MessageBoxA(NULL, "����ģ�����ʧ��", "����", MB_OK);
		return FALSE;
	}
	//ͨ��ģ����վ����ȡ��һ��ģ�����Ϣ
	if (!Module32First(hModuleSnap, &me32))
	{
		MessageBoxA(NULL, "��ȡ��һ��ģ�����Ϣʧ��", "����", MB_OK);
		//��ȡʧ����رվ��
		CloseHandle(hModuleSnap);
		return FALSE;
	}
	do
	{
		if (StrCmpW(me32.szModule, L"My_WeChatDll.dll") == 0)	//StrCmpW�Ƚ������ַ�������򷵻�0
		{
			return FALSE;
		}

	} while (Module32Next(hModuleSnap, &me32));	//ѭ������΢�Ž������Ƿ����"WeChatHelper.dll" ģ�飬�����򷵻�FALSE
	return TRUE;
}

