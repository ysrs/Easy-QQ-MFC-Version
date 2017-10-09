
// ClientDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "Client.h"
#include "ClientDlg.h"
#include "afxdialogex.h"

#include "LoginDialog.h"


#include <WinSock2.h>

#pragma comment(lib, "ws2_32.lib")


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// �Ի�������
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CClientDlg �Ի���



CClientDlg::CClientDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_CLIENT_DIALOG, pParent)
	, m_strShowMsg(_T(""))
	, m_strSendMsg(_T(""))
	, m_bPrivateChat(FALSE)
	, m_strPrivateFrientName(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_clientSocket = SOCKET_ERROR;
}

void CClientDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_FRIENDS, m_listFriends);
	DDX_Text(pDX, IDC_EDIT_SHOW_MSG, m_strShowMsg);
	DDX_Text(pDX, IDC_EDIT_SEND_MSG, m_strSendMsg);
	DDX_Check(pDX, IDC_CHECK_PRIVATE_CHAT, m_bPrivateChat);
	DDX_Text(pDX, IDC_EDIT_PRIVATE_FRIENT, m_strPrivateFrientName);
}

BEGIN_MESSAGE_MAP(CClientDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_SEND, &CClientDlg::OnBnClickedButtonSend)
	ON_MESSAGE(WM_SOCKET, &CClientDlg::OnSocket)
ON_NOTIFY(NM_DBLCLK, IDC_LIST_FRIENDS, &CClientDlg::OnDblclkListFriends)
END_MESSAGE_MAP()


// CClientDlg ��Ϣ�������

BOOL CClientDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ��������...���˵�����ӵ�ϵͳ�˵��С�

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// ���ô˶Ի����ͼ�ꡣ  ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO: �ڴ���Ӷ���ĳ�ʼ������
	bool bInitSuccess = false;
	const UINT RECEIVENUMBER = 4096;
	do
	{
		CLoginDialog loginDialog;
		if (loginDialog.DoModal() != IDOK)
		{
			::PostQuitMessage(0);
			return TRUE;
		}

		if (_tcslen(loginDialog.m_strIPAddress) == 0)
		{
			MessageBox(_T("������IP��ַ��"));
			continue;
		}

		if (loginDialog.m_username.IsEmpty())
		{
			MessageBox(_T("�������û�����"));
			continue;
		}

		if (loginDialog.m_password.IsEmpty())
		{
			MessageBox(_T("���������룡"));
			continue;
		}

		m_strUsername = loginDialog.m_username;
		m_strPassword = loginDialog.m_password;

		DWORD dwErrorCode = 0;
		if (!ConnectServer(0x8123, &dwErrorCode))
		{
			MessageBoxW(TEXT("��������ʧ�ܣ�"));
			continue;
		}

		// ��¼����ע��
		char strMsg[RECEIVENUMBER] = { 0 };
		strMsg[0] = loginDialog.m_cType;
		strcpy_s(&strMsg[10], 30, m_strUsername.GetString());
		strcpy_s(&strMsg[40], 30, m_strPassword.GetString());

		if (RECEIVENUMBER != ::send(m_clientSocket, strMsg, RECEIVENUMBER, 0))
		{
			MessageBox(TEXT("���͵�¼����ʧ�ܣ�"));
			continue;
		}

		memset(strMsg, 0, RECEIVENUMBER);

		while (true)
		{
			int nErrorCode = ::recv(m_clientSocket, strMsg, RECEIVENUMBER, 0);
			if (SOCKET_ERROR == nErrorCode)
			{
				nErrorCode = WSAGetLastError();

				if (WSAEWOULDBLOCK == nErrorCode || WSAEINVAL == nErrorCode)
				{
					Sleep(300);
					continue;
				}
				else
				{
					MessageBox(TEXT("���յ�¼����ʧ�ܣ�"));
					break;
				}
			}
			else
			{
				if (nErrorCode == RECEIVENUMBER)
				{
					// ���ճɹ�
					bInitSuccess = true;
				}
				break;
			}
		}

		if (bInitSuccess)
		{
			bInitSuccess = false;
		}
		else
		{
			continue;
		}


		// ��Ҫ���ݷ��ؽ��������Ӧ�Ĵ���
		if (strMsg[0] == UserInfoError)
		{
			MessageBox(TEXT("�û��������������"));
			continue;
		}
		else if (strMsg[0] == AlreadyLogin)
		{
			MessageBox(TEXT("�û��ѵ�¼��"));
			continue;
		}
		else if (strMsg[0] == AlreadyRegisted)
		{
			MessageBox(TEXT("���˺��Ѿ���ע���"));
			continue;
		}
		else if (strMsg[0] == RegisterSuccess)
		{
			MessageBox(TEXT("��ϲ���ɹ�ע�����˺�"));
			continue;
		}

		if (::WSAAsyncSelect(m_clientSocket, m_hWnd, WM_SOCKET, FD_READ | FD_CLOSE))
		{
			// ˵��WSAAsyncSelect����ִ��ʧ��
			MessageBox(TEXT("WSAAsyncSelect(m_clientSocket, m_hWnd, WM_SOCKET, FD_READ | FD_CLOSE)"));
			continue;
		}

		CString str(m_strUsername);
		str += TEXT(" �����ӵ�������");
		SetWindowText(str);

		int iTemp = 100;
		while (true)
		{
			// ��ȡ���к�������
			CString strTemp(&strMsg[iTemp]);
			TCHAR *pName = new TCHAR[30];
			_tcscpy_s(pName, 30, strTemp.GetString());
			iTemp += 30;

			if (pName[0] == '\0')
			{
				break;
			}

			LVITEM lvitem;
			lvitem.mask = LVIF_TEXT;
			lvitem.iItem = 0;
			lvitem.iSubItem = 0;
			lvitem.pszText = pName;
			m_listFriends.InsertItem(&lvitem);
		}

		bInitSuccess = true;
	}
	while (!bInitSuccess);

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

void CClientDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ  ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CClientDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CClientDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

afx_msg LRESULT CClientDlg::OnSocket(WPARAM wParam, LPARAM lParam)
{
	switch (WSAGETSELECTEVENT(lParam))
	{
	case FD_READ:
		{
		const UINT RECEIVENUMBER = 4096;
			char strReceiveMsg[RECEIVENUMBER] = { 0 };
			if (RECEIVENUMBER == ::recv(m_clientSocket, strReceiveMsg, RECEIVENUMBER, 0))
			{
				// ���ճɹ�
				UpdateData(TRUE);
				CStringA str;
				if (strReceiveMsg[0] == PrivateMsg)
				{
					str.Format("%s˽����˵:%s\r\n", &strReceiveMsg[10], &strReceiveMsg[100]);
				}
				else if (strReceiveMsg[0] == GroupMsg)
				{
					str.Format("%s˵:%s\r\n", &strReceiveMsg[10], &strReceiveMsg[100]);
				}
				else if (strReceiveMsg[0] == LoginSucces)
				{
					str.Format("��ӭ%s����������\r\n", &strReceiveMsg[10]);
					CString temp(&strReceiveMsg[10]);

					TCHAR *pName = new TCHAR[temp.GetLength() + 1];
					_tcscpy_s(pName, temp.GetLength() + 1, temp.GetString());

					LVITEM lvitem;
					lvitem.mask = LVIF_TEXT;
					lvitem.iItem = 0;
					lvitem.iSubItem = 0;
					lvitem.pszText = pName;
					m_listFriends.InsertItem(&lvitem);
				}
				else if (strReceiveMsg[0] == LogoutMsg)
				{
					str.Format("%s�˳���������\r\n", &strReceiveMsg[70]);
					for (int i=0; i<m_listFriends.GetItemCount(); ++i)
					{
						CString str = m_listFriends.GetItemText(i, 0);
						CStringA strTemp(str);
						if (strTemp == &strReceiveMsg[70])
						{
							m_listFriends.DeleteItem(i);
							break;
						}
					}
				}
				m_strShowMsg += str;
				UpdateData(FALSE);
			}
			else
			{
				MessageBox(TEXT("������Ϣʧ�ܣ�"));
			}
		}
		break;
	case FD_CLOSE:
		{
			MessageBox(TEXT("�������Ѿ��ر�"));
			CString str(m_strUsername);
			str += TEXT(" ��������Ͽ�����");
			SetWindowText(str);
		}
		break;
	default:
		break;
	}

	return 0;
}

void CClientDlg::OnBnClickedButtonSend()
{
	const UINT RECEIVENUMBER = 4096;
	UpdateData(TRUE);
	if (m_strSendMsg.GetLength() <= 0)
	{
		MessageBox(TEXT("���ܷ�������Ϣ"));
		return;
	}

	CStringA strSendMsg(m_strSendMsg);
	char strMsg[RECEIVENUMBER] = { 0 };
	strcpy_s(&strMsg[10], 30, m_strUsername.GetString());
	strcpy_s(&strMsg[100], RECEIVENUMBER - 100, strSendMsg.GetString());

	if (m_bPrivateChat)
	{
		// ˽����Ϣ
		if (m_strPrivateFrientName.GetLength() <= 0)
		{
			MessageBox(TEXT("����ѡ��һ������"));
			return;
		}
		strMsg[0] = PrivateMsg;
		CStringA strTemp(m_strPrivateFrientName);
		strcpy_s(&strMsg[70], 30, strTemp.GetString());
	}
	else
	{
		// Ⱥ����Ϣ
		strMsg[0] = GroupMsg;
	}
	
	if (RECEIVENUMBER == ::send(m_clientSocket, strMsg, RECEIVENUMBER, 0))
	{
		// ���ͳɹ�
		if (m_bPrivateChat)
		{
			m_strShowMsg += TEXT("��˽��%s˵��");
		}
		else
		{
			m_strShowMsg += TEXT("��˵��");
		}
		m_strShowMsg += m_strSendMsg + TEXT("\r\n");
		m_strSendMsg.Empty();
	}
	else
	{
		MessageBox(TEXT("����ʧ��"));
	}

	UpdateData(FALSE);
}

bool CClientDlg::ConnectServer(unsigned short usPort, DWORD* pDwErrorCode)
{
	bool bRet = false;
	WSADATA wsaData = { 0 };
	sockaddr_in serverAddr = { 0 };
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(usPort);
	serverAddr.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");

	do
	{
		if (!::WSAStartup(0x0202, &wsaData))
		{
			if (LOBYTE(wsaData.wVersion) == 2 && HIBYTE(wsaData.wVersion) == 2)
			{
				m_clientSocket = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
				if (INVALID_SOCKET != m_clientSocket)
				{
					if (!::WSAAsyncSelect(m_clientSocket, m_hWnd, WM_SOCKET, FD_CLOSE))
					{
						// WSAAsyncSelect��������0��˵��ִ�гɹ�
						int nErrorCode = 0;
						while (true)
						{
							nErrorCode = ::connect(m_clientSocket, reinterpret_cast<sockaddr *>(&serverAddr), sizeof(sockaddr));
							if (nErrorCode == SOCKET_ERROR)
							{
								nErrorCode = WSAGetLastError();
								if (WSAEWOULDBLOCK == nErrorCode || WSAEINVAL == nErrorCode)
								{
									Sleep(300);
									continue;
								}
								else if (WSAEISCONN == nErrorCode)// ���ӳɹ�
								{
									bRet = true;
									break;
								}
								else
								{
									AfxMessageBox(_T("����ʧ��"));
									break;
								}
							}
						}
					}
					else
					{
						*pDwErrorCode = ::WSAGetLastError();
					}
				}
				else
				{
					*pDwErrorCode = ::WSAGetLastError();
				}
			}
			else
			{
				*pDwErrorCode = ::WSAGetLastError();
			}
		}
		else
		{
			*pDwErrorCode = ::WSAGetLastError();
		}
	} while (false);

	return bRet;
}

void CClientDlg::OnDblclkListFriends(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	int iIndex = pNMItemActivate->iItem;
	m_strPrivateFrientName = m_listFriends.GetItemText(iIndex, 0);
	UpdateData(FALSE);

	*pResult = 0;
}


