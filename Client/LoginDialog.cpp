// LoginDialog.cpp : 实现文件
//

#include "stdafx.h"
#include "Client.h"
#include "LoginDialog.h"
#include "afxdialogex.h"


// CLoginDialog 对话框

IMPLEMENT_DYNAMIC(CLoginDialog, CDialogEx)

CLoginDialog::CLoginDialog(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_LOGIN_DIALOG, pParent)
	, m_username(_T(""))
	, m_password(_T(""))
{

}

CLoginDialog::~CLoginDialog()
{
}

void CLoginDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_IPADDRESS, m_ipAddress);
	DDX_Text(pDX, IDC_EDIT_USERNAME, m_username);
	DDX_Text(pDX, IDC_EDIT_PASSWORD, m_password);
}


BEGIN_MESSAGE_MAP(CLoginDialog, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON_LOGIN, &CLoginDialog::OnBnClickedButtonLogin)
	ON_BN_CLICKED(IDC_BUTTON_REGISTER, &CLoginDialog::OnBnClickedButtonRegister)
END_MESSAGE_MAP()


// CLoginDialog 消息处理程序


BOOL CLoginDialog::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	m_ipAddress.SetWindowText(TEXT("127.0.0.1"));

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}


void CLoginDialog::OnBnClickedButtonLogin()
{
	UpdateData(TRUE);
	m_ipAddress.GetWindowText(m_strIPAddress, 16);
	m_cType = 4;	// 4是登录标志
	OnOK();
}

void CLoginDialog::OnBnClickedButtonRegister()
{
	UpdateData(TRUE);
	m_ipAddress.GetWindowText(m_strIPAddress, 16);
	m_cType = 3;	// 3是注册标志
	OnOK();
}


