#pragma once

DWORD ProcessNameFindPID(const char* ProcessName);	//ͨ����������ȡ����ID
BOOL InjectDll(HANDLE& wxPid); //ע��dll
BOOL CheckIsInject(DWORD dwProcessid);	//���DLL�Ƿ��Ѿ�ע��
