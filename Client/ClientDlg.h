
// ClientDlg.h : 头文件
//

#pragma once
#include "afxcmn.h"

#include <vector>


#define WM_SOCKET	(WM_USER + 100)    //自定义socket消息


enum EMsgType
{
	ErrorMsg,			// 错误消息
	PrivateMsg,			// 私聊消息
	GroupMsg,			// 群消息
	RegisterMsg,		// 注册消息
	LoginMsg,			// 登陆消息
	LogoutMsg,			// 退出消息
	LoginSucces,		// 登录成功
	UserInfoError,		// 用户信息错误
	AlreadyLogin,		// 用户已经登录
	RegisterSuccess,	// 注册成功
	AlreadyRegisted		// 已经被注册过
};


// CClientDlg 对话框
class CClientDlg : public CDialogEx
{
// 构造
public:
	CClientDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_CLIENT_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
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

	std::vector<CString> m_vecFriendNames;	// 好友名字



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
