
// ServerDlg.h : ͷ�ļ�
//

#pragma once
#include "afxcmn.h"

#include <vector>


#define WM_SOCKET	(WM_USER + 100)    //�Զ���socket��Ϣ


enum EMsgType
{
	ErrorMsg,			// ������Ϣ
	PrivateMsg,			// ˽����Ϣ
	GroupMsg,			// Ⱥ��Ϣ
	RegisterMsg,		// ע����Ϣ
	LoginMsg,			// ��½��Ϣ
	LogoutMsg,			// �˳���Ϣ
	LoginSucces,		// ��¼�ɹ�
	UserInfoError,		// �û���Ϣ����
	AlreadyLogin,		// �û��Ѿ���¼
	RegisterSuccess,	// ע��ɹ�
	AlreadyRegisted		// �Ѿ���ע���
};

struct UserInfo
{
	char strUsername[30];
	char strPassword[30];
};

struct OnLineUser
{
	UserInfo userInfo;
	SOCKET clientSocket;
};


// CServerDlg �Ի���
class CServerDlg : public CDialogEx
{
// ����
public:
	CServerDlg(CWnd* pParent = NULL);	// ��׼���캯��
	~CServerDlg();

// �Ի�������
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_SERVER_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()


private:
	bool OpenServer(unsigned short usPort, DWORD *pDwErrorCode);
	bool SaveUserInfos(CString strFilePath);
	bool ImportUserInfos(CString strFilePath);


private:
	SOCKET m_serverSocket;
	std::vector<OnLineUser> m_vecOnLineUser;
	std::vector<UserInfo> m_vecUserInfos;	// ���е��û���Ϣ

public:
	afx_msg void OnBnClickedButtonStartServer();
protected:
	afx_msg LRESULT OnSocket(WPARAM wParam, LPARAM lParam);
public:
	CString m_strShowMsg;
	CListCtrl m_listOnLineUser;
};


