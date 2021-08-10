#pragma once

//ͨ����Ϣ�ṹ��
struct MessageStruct
{
	wchar_t msgdata1[MAX_PATH];
	wchar_t msgdata2[MAX_PATH];

	MessageStruct(wchar_t* pString1, wchar_t* pString2)
	{
		wcscpy_s(msgdata1, wcslen(pString1) + 1, pString1);
		wcscpy_s(msgdata2, wcslen(pString2) + 1, pString2);
	}

	MessageStruct(CString pString1, CString pString2)
	{
		wcscpy_s(msgdata1, wcslen(pString1) + 1, pString1);
		wcscpy_s(msgdata2, wcslen(pString2) + 1, pString2);
	}
};


//�����¼����Ϣ�ṹ��
struct ChatMessageData
{
	DWORD dwtype;				//��Ϣ����
	wchar_t sztype[0x20];			//��Ϣ����
	wchar_t source[0x400];			//��Ϣ��Դ
	wchar_t wxid[0x40];			//΢��ID/ȺID
	wchar_t wxname[0x200];			//΢������/Ⱥ����
	wchar_t sender[0x100];			//��Ϣ������
	wchar_t sendername[0x100];		//��Ϣ�������ǳ�
	wchar_t content[0x5000];	//��Ϣ����
};

//������Ƭ��Ϣ�ṹ��
struct XmlCardMessage
{
	wchar_t RecverWxid[50];		//�����ߵ�΢��ID
	wchar_t SendWxid[50];		//��Ҫ���͵�΢��ID
	wchar_t NickName[50];		//�ǳ�
};
//���Ͱ�����Ϣ
struct AtMsg
{
	wchar_t chatroomid[50] = { 0 };
	wchar_t memberwxid[50] = { 0 };
	wchar_t membernickname[50] = { 0 };
	wchar_t msgcontent[100] = { 0 };
};
//XML������Ϣ
struct SendXmlArticleStruct
{
	wchar_t title[50];
	wchar_t subtitle[50];
	wchar_t urllink[200];
	wchar_t picpath[260];
	wchar_t selfwxid[50];
	wchar_t recverwxid[50];
};


//������Ϣ�Ľṹ��
struct PersonalInformation
{
	wchar_t wxid[40];			//΢��ID
	wchar_t wxcount[40];		//΢���˺�
	wchar_t v1[150];			//V1
	wchar_t nickname[50];		//΢���ǳ�
	wchar_t remark[50];			//��ע
	wchar_t wxsex[10];			//�Ա�
	wchar_t phonenumber[30];	//�ֻ���
	wchar_t device[20];			//��½�豸
	wchar_t nation[20];			//����
	wchar_t province[20];		//ʡ��
	wchar_t city[20];			//����
	wchar_t area[20];			//����
	wchar_t language[10];		//����
	wchar_t bigheader[0x100];	//��ͷ��
	wchar_t smallheader[0x100];	//Сͷ��
	wchar_t signature[50];		//����ǩ��
	wchar_t background[0x100];	//����Ȧ����
	wchar_t cachedir[MAX_PATH];	//����Ŀ¼
	wchar_t startdir[MAX_PATH];	//����Ŀ¼
};


//�����б���û���Ϣ
struct UserInfo
{
	wchar_t UserId[80];
	wchar_t UserNumber[80];
	wchar_t UserRemark[80];
	wchar_t UserNickName[80];
};

//Ⱥ��Ա����Ϣ�ṹ��
struct ChatRoomMemberInfo
{
	wchar_t UserId[0x100];
	wchar_t UserNumber[0x100];
	wchar_t UserNickName[0x100];
};

//������ϸ��Ϣ�ṹ��
struct UserInfoDetail
{
	wchar_t UserId[50];			//΢��ID
	wchar_t UserNumber[50];		//΢���˺�
	wchar_t V1[200];				//V1
	wchar_t Remark[50];			//��ע
	wchar_t UserNickName[50];	//΢���ǳ�
	wchar_t smallHeader[1024];	//Сͷ��
	wchar_t bigHeader[1024];	//��ͷ��
};