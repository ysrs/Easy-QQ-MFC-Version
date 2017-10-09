#pragma once
#include "afxcmn.h"
#include "afxwin.h"


// CLoginDialog �Ի���

class CLoginDialog : public CDialogEx
{
	DECLARE_DYNAMIC(CLoginDialog)

public:
	CLoginDialog(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CLoginDialog();

// �Ի�������
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_LOGIN_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()

public:
	TCHAR m_strIPAddress[16];
	char m_cType;

public:
	afx_msg void OnBnClickedButtonLogin();
	CIPAddressCtrl m_ipAddress;
	virtual BOOL OnInitDialog();
	CString m_username;
	CString m_password;
	afx_msg void OnBnClickedButtonRegister();
};


