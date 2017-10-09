
// ServerDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "Server.h"
#include "ServerDlg.h"
#include "afxdialogex.h"

#include <tchar.h>
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


// CServerDlg 对话框



CServerDlg::CServerDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_SERVER_DIALOG, pParent)
	, m_strShowMsg(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_serverSocket = SOCKET_ERROR;
}

CServerDlg::~CServerDlg()
{
	// 保存用户信息
	SaveUserInfos(TEXT("data/user_infos.txt"));
	::WSACleanup();
}

void CServerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_SHOW_MSG, m_strShowMsg);
	DDX_Control(pDX, IDC_LIST_ONLINE_USER, m_listOnLineUser);
}

BEGIN_MESSAGE_MAP(CServerDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_START_SERVER, &CServerDlg::OnBnClickedButtonStartServer)
	ON_MESSAGE(WM_SOCKET, &CServerDlg::OnSocket)
END_MESSAGE_MAP()


// CServerDlg 消息处理程序

BOOL CServerDlg::OnInitDialog()
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
	m_listOnLineUser.InsertColumn(0, TEXT("用户名"), LVCFMT_LEFT, 30);
	m_listOnLineUser.SetColumnWidth(0, 70);

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CServerDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CServerDlg::OnPaint()
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
HCURSOR CServerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CServerDlg::OnBnClickedButtonStartServer()
{
	DWORD dwErrorCode = 0;
	CString str;
	static_cast<CButton *>(GetDlgItem(IDC_BUTTON_START_SERVER))->GetWindowText(str);
	if (str == TEXT("开启服务"))
	{
		if (OpenServer(0x8123, &dwErrorCode))
		{
			MessageBox(TEXT("开启服务器成功！"));
			ImportUserInfos(TEXT("data/user_infos.txt"));
			static_cast<CButton *>(GetDlgItem(IDC_BUTTON_START_SERVER))->SetWindowText(TEXT("停止服务"));
		}
		else
		{
			MessageBoxW(TEXT("开启服务器失败！"));
		}
	}
	else
	{
		static_cast<CButton *>(GetDlgItem(IDC_BUTTON_START_SERVER))->SetWindowText(TEXT("开启服务"));
		SaveUserInfos(TEXT("data/user_infos.txt"));
		::WSACleanup();
	}
}

bool CServerDlg::OpenServer(unsigned short usPort, DWORD* pDwErrorCode)
{
	bool bRet = false;

	WSADATA wsaData = { 0 };
	sockaddr_in serverAddr = { 0 };
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(usPort);
	serverAddr.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");

	do
	{
		if (!::WSAStartup(MAKEWORD(2, 2), &wsaData))
		{
			if (LOBYTE(wsaData.wVersion) == 2 && HIBYTE(wsaData.wVersion) == 2)
			{
				m_serverSocket = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
				if (INVALID_SOCKET != m_serverSocket)
				{
					if(!::WSAAsyncSelect(m_serverSocket, m_hWnd, WM_SOCKET, FD_READ | FD_WRITE | FD_ACCEPT | FD_CLOSE))
					{
						if (SOCKET_ERROR != ::bind(m_serverSocket, reinterpret_cast<sockaddr *>(&serverAddr), sizeof(sockaddr_in)))
						{
							if (SOCKET_ERROR != ::listen(m_serverSocket, 10))
							{
								bRet = true;
								break;
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
	while (false);

	return bRet;
}

bool CServerDlg::SaveUserInfos(CString strFilePath)
{
	bool bRet = true;
	CFile file;
	CFileException fileException;
	if (file.Open(strFilePath, CFile::modeCreate | CFile::modeWrite, &fileException))
	{
		for (auto userInfo : m_vecUserInfos)
		{
			file.Write(&userInfo, sizeof(UserInfo));
		}
	}
	else
	{
		TCHAR strErrorMessage[MAXBYTE] = { 0 };
		fileException.GetErrorMessage(strErrorMessage, MAXBYTE);
		MessageBox(strErrorMessage);
		bRet = false;
	}

	return bRet;
}

bool CServerDlg::ImportUserInfos(CString strFilePath)
{
	bool bRet = true;
	CFile file;
	CFileException fileException;
	m_vecUserInfos.clear();
	m_vecOnLineUser.clear();
	if (file.Open(strFilePath, CFile::modeRead, &fileException))
	{
		while (true)
		{
			UserInfo userInfo;
			UINT uiReadNumber = file.Read(&userInfo, sizeof(UserInfo));
			if (uiReadNumber != sizeof(UserInfo))
			{
				break;
			}
			m_vecUserInfos.push_back(userInfo);
		}
	}
	else
	{
		TCHAR strErrorMessage[MAXBYTE] = { 0 };
		fileException.GetErrorMessage(strErrorMessage, MAXBYTE);
		MessageBox(strErrorMessage);
		bRet = false;
	}

	return bRet;
}

afx_msg LRESULT CServerDlg::OnSocket(WPARAM wParam, LPARAM lParam)
{
	const UINT RECEIVENUMBER = 4096;
	char strReceiveBuf[RECEIVENUMBER] = { 0 };

	switch (WSAGETSELECTEVENT(lParam))
	{
	case FD_READ:
		{
			// 收到读消息
			::recv(wParam, strReceiveBuf, RECEIVENUMBER, 0);

			if (strReceiveBuf[0] == LoginMsg)
			{
				strReceiveBuf[0] = UserInfoError;
				// 登录请求
				// 还要判断是否已经登录
				for (auto onLineUser : m_vecOnLineUser)
				{
					if (strcmp(onLineUser.userInfo.strUsername, &strReceiveBuf[10]) == 0)
					{
						strReceiveBuf[0] = AlreadyLogin;
						break;
					}
				}

				if (strReceiveBuf[0] != AlreadyLogin)
				{
					// 提取用户名和密码进行查找，查找到则登录成功，否则登陆失败
					for (auto userInfo : m_vecUserInfos)
					{
						if (strcmp(userInfo.strUsername, &strReceiveBuf[10]) == 0)
						{
							if (strcmp(userInfo.strPassword, &strReceiveBuf[40]) == 0)
							{
								strReceiveBuf[0] = LoginSucces;

								int iTemp = 100;
								for (auto onLineUser : m_vecOnLineUser)
								{
									strcpy_s(&strReceiveBuf[iTemp], 30, onLineUser.userInfo.strUsername);
									iTemp += 30;
								}

								OnLineUser user;
								user.clientSocket = wParam;
								strcpy_s(user.userInfo.strUsername, userInfo.strUsername);
								strcpy_s(user.userInfo.strPassword, userInfo.strPassword);
								m_vecOnLineUser.push_back(user);
								break;
							}
						}
					}
				}

				if (strReceiveBuf[0] == LoginSucces)
				{
					CString strTemp(&strReceiveBuf[10]);
					m_listOnLineUser.InsertItem(0, strTemp);

					for (auto onLineUser : m_vecOnLineUser)
					{
						if (onLineUser.clientSocket != wParam)
						{
							::send(onLineUser.clientSocket, strReceiveBuf, RECEIVENUMBER, 0);
						}
					}

					strTemp += TEXT("进入聊天室\r\n");
					m_strShowMsg += strTemp;
				}

				::send(wParam, strReceiveBuf, RECEIVENUMBER, 0);
			}
			else if (strReceiveBuf[0] == RegisterMsg)
			{
				// 注册信息
				strReceiveBuf[0] = RegisterSuccess;
				for (auto userInfo : m_vecUserInfos)
				{
					if (strcmp(&strReceiveBuf[10], userInfo.strUsername) == 0)
					{
						strReceiveBuf[0] = AlreadyRegisted;
						break;
					}
				}
				if (strReceiveBuf[0] == RegisterSuccess)
				{
					UserInfo user;
					strcpy_s(user.strUsername, 30, &strReceiveBuf[10]);
					strcpy_s(user.strPassword, 30, &strReceiveBuf[40]);
					m_vecUserInfos.push_back(user);

					m_strShowMsg += TEXT("注册新用户:");
					m_strShowMsg += user.strUsername;
				}
				::send(wParam, strReceiveBuf, RECEIVENUMBER, 0);
			}
			else if (strReceiveBuf[0] == PrivateMsg)
			{
				// 私聊
				for (auto onLineUser : m_vecOnLineUser)
				{
					if (strcmp(onLineUser.userInfo.strUsername, &strReceiveBuf[70]) == 0)
					{
						::send(onLineUser.clientSocket, strReceiveBuf, RECEIVENUMBER, 0);
						m_strShowMsg += &strReceiveBuf[10];
						m_strShowMsg += TEXT("私聊");
						m_strShowMsg += onLineUser.userInfo.strUsername;
						m_strShowMsg += TEXT("说:");
						m_strShowMsg += &strReceiveBuf[100];
						m_strShowMsg += TEXT("\r\n");
					}
				}
			}
			else if (strReceiveBuf[0] == GroupMsg)
			{
				// 群聊
				for (auto onLineUser : m_vecOnLineUser)
				{
					if (onLineUser.clientSocket != wParam)
					{
						::send(onLineUser.clientSocket, strReceiveBuf, RECEIVENUMBER, 0);
					}
				}
				m_strShowMsg += &strReceiveBuf[10];
				m_strShowMsg += TEXT("群聊说:");
				m_strShowMsg += &strReceiveBuf[100];
				m_strShowMsg += TEXT("\r\n");
			}

			UpdateData(FALSE);
		}
		break;
	case FD_ACCEPT:
		{
			// 收到新的连接
			sockaddr_in clientAddr = { 0 };
			int addrLen = sizeof(sockaddr_in);
			SOCKET clientSocket = ::accept(wParam, reinterpret_cast<sockaddr *>(&clientAddr), &addrLen);
			if (INVALID_SOCKET == clientSocket)
			{
				::MessageBox(nullptr, TEXT("创建新的连接失败"), TEXT("提示"), MB_OK);
			}
		}
		break;
	case FD_CLOSE:
		// 收到关闭消息
		{
			strReceiveBuf[0] = LogoutMsg;
			std::vector<OnLineUser>::iterator it = m_vecOnLineUser.begin();
			for (; it!=m_vecOnLineUser.end(); ++it)
			{
				if (it->clientSocket == wParam)
				{
					strcpy_s(&strReceiveBuf[70], 30, it->userInfo.strUsername);

					for (int i=0; i<m_listOnLineUser.GetItemCount(); ++i)
					{
						CString str = m_listOnLineUser.GetItemText(i, 0);
						CStringA strTemp(str);
						if (strTemp == it->userInfo.strUsername)
						{
							m_listOnLineUser.DeleteItem(i);
							break;
						}
					}

					it = m_vecOnLineUser.erase(it);
					break;
				}
			}

			for (it=m_vecOnLineUser.begin(); it!=m_vecOnLineUser.end(); ++it)
			{
				::send(it->clientSocket, strReceiveBuf, RECEIVENUMBER, 0);
			}
		}
		break;
	default:
		break;
	}

	return 0;
}


