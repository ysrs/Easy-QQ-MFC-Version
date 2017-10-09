// LoginDialog.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "Client.h"
#include "LoginDialog.h"
#include "afxdialogex.h"


// CLoginDialog �Ի���

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


// CLoginDialog ��Ϣ�������


BOOL CLoginDialog::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  �ڴ���Ӷ���ĳ�ʼ��
	m_ipAddress.SetWindowText(TEXT("127.0.0.1"));

	return TRUE;  // return TRUE unless you set the focus to a control
				  // �쳣: OCX ����ҳӦ���� FALSE
}


void CLoginDialog::OnBnClickedButtonLogin()
{
	UpdateData(TRUE);
	m_ipAddress.GetWindowText(m_strIPAddress, 16);
	m_cType = 4;	// 4�ǵ�¼��־
	OnOK();
}

void CLoginDialog::OnBnClickedButtonRegister()
{
	UpdateData(TRUE);
	m_ipAddress.GetWindowText(m_strIPAddress, 16);
	m_cType = 3;	// 3��ע���־
	OnOK();
}


