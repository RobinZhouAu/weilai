#pragma once
#include "afxwin.h"

#include "DYDatabaseEx.h"
#include "ConnectFuc.h"
// CDlgConnect �Ի���

class CDlgConnect : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgConnect)

public:
	CDlgConnect(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CDlgConnect();

// �Ի�������
	enum { IDD = IDD_CONNECT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	CComboBox m_cbDBType;
	CComboBox m_cbDBType2;
	afx_msg void OnBnClickedOk();
	CString m_strDataSource;
	CString m_strDatabase;
	CString m_strUser;
	CString m_strPassword;
	CString m_strDataSource2;
	CString m_strDatabase2;
	CString m_strUser2;
	CString m_strPassword2;

	CDDBCONNECT m_connect;
	CDDBCONNECT m_connect2;
	CString m_strDBCFileName;
	CString m_strDBCFileName2;
	virtual BOOL OnInitDialog();
};
