#pragma once
#include "afxcmn.h"
#include "afxwin.h"


// CLoginDialog 对话框

class CLoginDialog : public CDialogEx
{
	DECLARE_DYNAMIC(CLoginDialog)

public:
	CLoginDialog(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CLoginDialog();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_LOGIN_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

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


