#include "pch.h"
#include "CMultiOpen.h"
#include <TlHelp32.h>
#include <Shlwapi.h>
#pragma comment(lib, "shlwapi")
#include <stdio.h>


typedef NTSTATUS(NTAPI* NTQUERYOBJECT)(
	_In_opt_   HANDLE Handle,
	_In_       OBJECT_INFORMATION_CLASS ObjectInformationClass,
	_Out_opt_  PVOID ObjectInformation,
	_In_       ULONG ObjectInformationLength,
	_Out_opt_  PULONG ReturnLength
	);


typedef NTSTATUS
(NTAPI* ZWQUERYSYSTEMINFORMATION)(
	IN SYSTEM_INFORMATION_CLASS SystemInformationClass,
	OUT PVOID SystemInformation,
	IN ULONG SystemInformationLength,
	OUT PULONG ReturnLength OPTIONAL
	);
ZWQUERYSYSTEMINFORMATION ZwQuerySystemInformation = (ZWQUERYSYSTEMINFORMATION)GetProcAddress(GetModuleHandleA("ntdll.dll"), "ZwQuerySystemInformation");
NTQUERYOBJECT    NtQueryObject = (NTQUERYOBJECT)GetProcAddress(GetModuleHandleA("ntdll.dll"), "NtQueryObject");


//************************************************************
// ��������: ElevatePrivileges
// ����˵��: ������Ȩ
// ��    ��: GuiShou
// ʱ    ��: 2019/7/13
// ��    ��: void
// �� �� ֵ: BOOL
//***********************************************************
BOOL ElevatePrivileges()
{
	HANDLE hToken;
	TOKEN_PRIVILEGES tkp;
	tkp.PrivilegeCount = 1;
	if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken))
		return FALSE;
	LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &tkp.Privileges[0].Luid);
	tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
	if (!AdjustTokenPrivileges(hToken, FALSE, &tkp, sizeof(TOKEN_PRIVILEGES), NULL, NULL))
	{
		return FALSE;
	}

	return TRUE;
}


//************************************************************
// ��������: DuplicateHandleEx
// ����˵��: ���ƾ��
// ��    ��: GuiShou
// ʱ    ��: 2019/7/13
// ��    ��: void
// �� �� ֵ: BOOL
//***********************************************************
HANDLE DuplicateHandleEx(DWORD pid, HANDLE h, DWORD flags)
{
	HANDLE hHandle = NULL;

	HANDLE hProc = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
	if (hProc)
	{
		if (!DuplicateHandle(hProc,
			(HANDLE)h, GetCurrentProcess(),
			&hHandle, 0, FALSE, /*DUPLICATE_SAME_ACCESS*/flags))
		{
			hHandle = NULL;
		}
		CloseHandle(hProc);
	}
	
	return hHandle;
}


//************************************************************
// ��������: GetProcIds
// ����˵��: ��ȡPID
// ��    ��: GuiShou
// ʱ    ��: 2019/7/13
// ��    ��: Name Pids
// �� �� ֵ: int
//***********************************************************
int GetProcIds(LPWSTR Name, DWORD* Pids)
{
	PROCESSENTRY32 pe32 = { sizeof(pe32) };
	int num = 0;

	HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hSnap)
	{
		if (Process32First(hSnap, &pe32))
		{
			do {
				if (!StrCmpW(Name, pe32.szExeFile))
				{
					if (Pids)
					{
						Pids[num++] = pe32.th32ProcessID;
					}
				}
			} while (Process32Next(hSnap, &pe32));
		}
		CloseHandle(hSnap);
	}

	return num;
}


//************************************************************
// ��������: IsTargetPid
// ����˵��: �Ƿ���Ŀ��PID
// ��    ��: GuiShou
// ʱ    ��: 2019/7/13
// ��    ��: Pid Pids num
// �� �� ֵ: BOOL
//***********************************************************
BOOL IsTargetPid(DWORD Pid, DWORD* Pids, int num)
{
	for (int i = 0; i < num; i++)
	{
		if (Pid == Pids[i])
		{
			return TRUE;
		}
	}
	return FALSE;
}


//************************************************************
// ��������: PatchWeChat
// ����˵��: �޸�΢��ռ��Ȩ
// ��    ��: GuiShou
// ʱ    ��: 2019/7/13
// ��    ��: void
// �� �� ֵ: int
//***********************************************************
int PatchWeChat()
{
	DWORD dwSize = 0;
	POBJECT_NAME_INFORMATION pNameInfo;
	POBJECT_NAME_INFORMATION pNameType;
	PVOID pbuffer = NULL;
	NTSTATUS Status;
	DWORD nIndex = 0;
	DWORD dwFlags = 0;
	char szType[128] = { 0 };
	char szName[512] = { 0 };

	ElevatePrivileges();

	DWORD Pids[100] = { 0 };

	DWORD Num = GetProcIds(L"WeChat.exe", Pids);	//���΢���Ƿ���
	if (Num == 0)
	{
		return 0;
	}

	if (!ZwQuerySystemInformation)	//���ϵͳ��ϢDLL����û��������"ntdll.dll"��
	{
		goto Exit0;
	}

	pbuffer = VirtualAlloc(NULL, 0x1000, MEM_COMMIT, PAGE_READWRITE);	//���������ڴ�

	if (!pbuffer)
	{
		goto Exit0;
	}

	Status = ZwQuerySystemInformation(SystemHandleInformation, pbuffer, 0x1000, &dwSize);	//��ѯϵͳ�����Ϣ�����浽 pbuffer

	if (!NT_SUCCESS(Status))	//�����ѯ���ɹ������������ڴ���䲻�㣩�����·�������ڴ����
	{
		if (STATUS_INFO_LENGTH_MISMATCH != Status)	//�ж��Ƿ���ִ����Ϣ��ѯ
		{
			goto Exit0;
		}
		else
		{
			// �����ҿ��Ա�֤�������ȷ��ʹ��ѭ�������Ժ�
			if (NULL != pbuffer)
			{
				VirtualFree(pbuffer, 0, MEM_RELEASE);	//��� pbuffer
			}

			if (dwSize * 2 > 0x4000000)  // MAXSIZE	��Ϣ��������ܳ���4G�ڴ�
			{
				goto Exit0;
			}

			pbuffer = VirtualAlloc(NULL, dwSize * 2, MEM_COMMIT, PAGE_READWRITE);	//�������������ڴ�

			if (!pbuffer)
			{
				goto Exit0;
			}

			Status = ZwQuerySystemInformation(SystemHandleInformation, pbuffer, dwSize * 2, NULL);		//���²���ϵͳ��Ϣ

			if (!NT_SUCCESS(Status))	//��һ�β��Ҳ��ɹ����˳�
			{
				goto Exit0;
			}
		}
	}

	PSYSTEM_HANDLE_INFORMATION1 pHandleInfo = (PSYSTEM_HANDLE_INFORMATION1)pbuffer;		//��ϵͳ��Ϣ���Ƶ� ����pHandleInfo

	int nCount = 0;			//�رվ���Ĵ���
	for (nIndex = 0; nIndex < pHandleInfo->NumberOfHandles; nIndex++)	//����ϵͳ��Ϣ
	{
		if (IsTargetPid(pHandleInfo->Handles[nIndex].UniqueProcessId, Pids, Num))	//����Ƿ���΢�ŵĽ�����Ϣ
		{
			//
			HANDLE hHandle = DuplicateHandleEx(pHandleInfo->Handles[nIndex].UniqueProcessId,	//���΢��ռ��Ȩ��Ϣ���
				(HANDLE)pHandleInfo->Handles[nIndex].HandleValue,
				DUPLICATE_SAME_ACCESS
			);
			if (hHandle == NULL) continue;

			Status = NtQueryObject(hHandle, ObjectNameInformation, szName, 512, &dwFlags);	//��ѯ��Ϣ��������Ƿ���Ч

			if (!NT_SUCCESS(Status))
			{
				CloseHandle(hHandle);
				continue;
			}

			Status = NtQueryObject(hHandle, ObjectTypeInformation, szType, 128, &dwFlags);	//��ѯ��Ϣ��������Ƿ���Ч

			if (!NT_SUCCESS(Status))
			{
				CloseHandle(hHandle);
				continue;
			}

			pNameInfo = (POBJECT_NAME_INFORMATION)szName;	//��΢����Ϣ ���Ƹ��Ƶ�pNameInfo
			pNameType = (POBJECT_NAME_INFORMATION)szType;	//��΢����Ϣ ���͸��Ƶ�pNameType

			WCHAR TypName[1024] = { 0 };
			WCHAR Name[1024] = { 0 };

			wcsncpy_s(TypName, (WCHAR*)pNameType->Name.Buffer, pNameType->Name.Length / 2);
			wcsncpy_s(Name, (WCHAR*)pNameInfo->Name.Buffer, pNameInfo->Name.Length / 2);

			// ƥ���Ƿ�Ϊ��Ҫ�رյľ������
			if (0 == wcscmp(TypName, L"Mutant"))	//����Ƿ���Ͼ������
			{
				if (wcsstr(Name, L"_WeChat_App_Instance_Identity_Mutex_Name"))	//����Ƿ���Ͼ�����ƣ�΢�Ž��̻�������
				{
					CloseHandle(hHandle);

					hHandle = DuplicateHandleEx(pHandleInfo->Handles[nIndex].UniqueProcessId,	//�ر�΢�Ż���������
						(HANDLE)pHandleInfo->Handles[nIndex].HandleValue,
						DUPLICATE_CLOSE_SOURCE);

					if (hHandle)
					{
						OutputDebugStringA("+ Patch wechat success!\n");
						CloseHandle(hHandle);
						if (NULL != pbuffer)
						{
							VirtualFree(pbuffer, 0, MEM_RELEASE);
						}
						return 1;
					}
					else
					{
						OutputDebugStringA("- Patch error");
					}
					nCount++;
					if (nCount >= 2)
					{
						goto Exit0;
					}
				}
				else {
					CloseHandle(hHandle);
				}
			}
		}
	}

	Exit0:
	if (NULL != pbuffer)
	{
		VirtualFree(pbuffer, 0, MEM_RELEASE);
	}

	return 0;
}


//************************************************************
// ��������: OpenWeChat
// ����˵��: ��΢��
// ��    ��: GuiShou
// ʱ    ��: 2019/7/13
// ��    ��: void
// �� �� ֵ: void
//***********************************************************
void OpenWeChat()
{
	HKEY hKey = NULL;
	if (ERROR_SUCCESS != RegOpenKey(HKEY_CURRENT_USER, L"Software\\Tencent\\WeChat", &hKey))
	{
		return;
	}

	DWORD Type = REG_SZ;
	WCHAR Path[MAX_PATH] = { 0 };
	DWORD cbData = MAX_PATH * sizeof(WCHAR);
	if (ERROR_SUCCESS != RegQueryValueEx(hKey, L"InstallPath", 0, &Type, (LPBYTE)Path, &cbData))
	{
		goto __exit;
	}

	PathAppend(Path, L"WeChat.exe");

	ShellExecute(NULL, L"Open", Path, NULL, NULL, SW_SHOW);

__exit:
	if (hKey)
	{
		RegCloseKey(hKey);
	}
}

