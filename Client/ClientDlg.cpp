
// ClientDlg.cpp : 实现文件
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


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
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


// CClientDlg 对话框



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


// CClientDlg 消息处理程序

BOOL CClientDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
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

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
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
			MessageBox(_T("请输入IP地址！"));
			continue;
		}

		if (loginDialog.m_username.IsEmpty())
		{
			MessageBox(_T("请输入用户名！"));
			continue;
		}

		if (loginDialog.m_password.IsEmpty())
		{
			MessageBox(_T("请输入密码！"));
			continue;
		}

		m_strUsername = loginDialog.m_username;
		m_strPassword = loginDialog.m_password;

		DWORD dwErrorCode = 0;
		if (!ConnectServer(0x8123, &dwErrorCode))
		{
			MessageBoxW(TEXT("创建连接失败！"));
			continue;
		}

		// 登录或者注册
		char strMsg[RECEIVENUMBER] = { 0 };
		strMsg[0] = loginDialog.m_cType;
		strcpy_s(&strMsg[10], 30, m_strUsername.GetString());
		strcpy_s(&strMsg[40], 30, m_strPassword.GetString());

		if (RECEIVENUMBER != ::send(m_clientSocket, strMsg, RECEIVENUMBER, 0))
		{
			MessageBox(TEXT("发送登录请求失败！"));
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
					MessageBox(TEXT("接收登录请求失败！"));
					break;
				}
			}
			else
			{
				if (nErrorCode == RECEIVENUMBER)
				{
					// 接收成功
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


		// 还要根据返回结果进行相应的处理
		if (strMsg[0] == UserInfoError)
		{
			MessageBox(TEXT("用户名或者密码错误！"));
			continue;
		}
		else if (strMsg[0] == AlreadyLogin)
		{
			MessageBox(TEXT("用户已登录！"));
			continue;
		}
		else if (strMsg[0] == AlreadyRegisted)
		{
			MessageBox(TEXT("该账号已经被注册过"));
			continue;
		}
		else if (strMsg[0] == RegisterSuccess)
		{
			MessageBox(TEXT("恭喜您成功注册新账号"));
			continue;
		}

		if (::WSAAsyncSelect(m_clientSocket, m_hWnd, WM_SOCKET, FD_READ | FD_CLOSE))
		{
			// 说明WSAAsyncSelect函数执行失败
			MessageBox(TEXT("WSAAsyncSelect(m_clientSocket, m_hWnd, WM_SOCKET, FD_READ | FD_CLOSE)"));
			continue;
		}

		CString str(m_strUsername);
		str += TEXT(" 已连接到服务器");
		SetWindowText(str);

		int iTemp = 100;
		while (true)
		{
			// 读取所有好友名字
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

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
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

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CClientDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
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
				// 接收成功
				UpdateData(TRUE);
				CStringA str;
				if (strReceiveMsg[0] == PrivateMsg)
				{
					str.Format("%s私聊我说:%s\r\n", &strReceiveMsg[10], &strReceiveMsg[100]);
				}
				else if (strReceiveMsg[0] == GroupMsg)
				{
					str.Format("%s说:%s\r\n", &strReceiveMsg[10], &strReceiveMsg[100]);
				}
				else if (strReceiveMsg[0] == LoginSucces)
				{
					str.Format("欢迎%s进入聊天室\r\n", &strReceiveMsg[10]);
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
					str.Format("%s退出了聊天室\r\n", &strReceiveMsg[70]);
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
				MessageBox(TEXT("接收消息失败！"));
			}
		}
		break;
	case FD_CLOSE:
		{
			MessageBox(TEXT("服务器已经关闭"));
			CString str(m_strUsername);
			str += TEXT(" 与服务器断开连接");
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
		MessageBox(TEXT("不能发生空消息"));
		return;
	}

	CStringA strSendMsg(m_strSendMsg);
	char strMsg[RECEIVENUMBER] = { 0 };
	strcpy_s(&strMsg[10], 30, m_strUsername.GetString());
	strcpy_s(&strMsg[100], RECEIVENUMBER - 100, strSendMsg.GetString());

	if (m_bPrivateChat)
	{
		// 私聊消息
		if (m_strPrivateFrientName.GetLength() <= 0)
		{
			MessageBox(TEXT("请先选择一个好友"));
			return;
		}
		strMsg[0] = PrivateMsg;
		CStringA strTemp(m_strPrivateFrientName);
		strcpy_s(&strMsg[70], 30, strTemp.GetString());
	}
	else
	{
		// 群聊消息
		strMsg[0] = GroupMsg;
	}
	
	if (RECEIVENUMBER == ::send(m_clientSocket, strMsg, RECEIVENUMBER, 0))
	{
		// 发送成功
		if (m_bPrivateChat)
		{
			m_strShowMsg += TEXT("我私聊%s说：");
		}
		else
		{
			m_strShowMsg += TEXT("我说：");
		}
		m_strShowMsg += m_strSendMsg + TEXT("\r\n");
		m_strSendMsg.Empty();
	}
	else
	{
		MessageBox(TEXT("发送失败"));
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
						// WSAAsyncSelect函数返回0，说明执行成功
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
								else if (WSAEISCONN == nErrorCode)// 连接成功
								{
									bRet = true;
									break;
								}
								else
								{
									AfxMessageBox(_T("连接失败"));
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
	// TODO: 在此添加控件通知处理程序代码
	int iIndex = pNMItemActivate->iItem;
	m_strPrivateFrientName = m_listFriends.GetItemText(iIndex, 0);
	UpdateData(FALSE);

	*pResult = 0;
}


