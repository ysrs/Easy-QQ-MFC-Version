
// ClientDlg.h : ͷ�ļ�
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


// CClientDlg �Ի���
class CClientDlg : public CDialogEx
{
// ����
public:
	CClientDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_CLIENT_DIALOG };
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
	bool ConnectServer(unsigned short usPort, DWORD *pDwErrorCode);


private:
	SOCKET m_clientSocket;
	CStringA m_strUsername;
	CStringA m_strPassword;

	std::vector<CString> m_vecFriendNames;	// ��������



public:
	CListCtrl m_listFriends;
	CString m_strShowMsg;
	CString m_strSendMsg;
	BOOL m_bPrivateChat;
	afx_msg void OnBnClickedButtonSend();
	afx_msg void OnDblclkListFriends(NMHDR *pNMHDR, LRESULT *pResult);
protected:
	afx_msg LRESULT OnSocket(WPARAM wParam, LPARAM lParam);
public:
	CString m_strPrivateFrientName;
};
