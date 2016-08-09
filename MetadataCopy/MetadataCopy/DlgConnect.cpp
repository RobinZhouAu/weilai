// DlgConnect.cpp : 实现文件
//

#include "stdafx.h"
#include "MetadataCopy.h"
#include "DlgConnect.h"
#include "afxdialogex.h"


CString  g_GetFullPath(CString strFileName)
{
	TCHAR	lpsFileName[256];
	::GetModuleFileName(AfxGetInstanceHandle(),lpsFileName, 255);
	CString szBinPath =lpsFileName;
	szBinPath = szBinPath.Left(szBinPath.ReverseFind('\\'));
	return szBinPath + _T("\\") + strFileName;

}

// CDlgConnect 对话框

IMPLEMENT_DYNAMIC(CDlgConnect, CDialogEx)

CDlgConnect::CDlgConnect(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDlgConnect::IDD, pParent)
{
	//m_strDataSource = _T("localhost");
	//m_strDatabase = _T("imamdb34_weilai");
	//m_strUser = _T("sa");
	//m_strPassword = _T("evias");

	//m_strDataSource2 = _T("localhost");
	//m_strDatabase2 = _T("imamdb34_new");
	//m_strUser2 = _T("sa");
	//m_strPassword2 = _T("evias");
}

CDlgConnect::~CDlgConnect()
{
}

void CDlgConnect::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_DBTYPE, m_cbDBType);
	DDX_Control(pDX, IDC_DBTYPE2, m_cbDBType2);
	DDX_Text(pDX, IDC_DATASOURCE, m_strDataSource);
	DDX_Text(pDX, IDC_DATABASE, m_strDatabase);
	DDX_Text(pDX, IDC_USER, m_strUser);
	DDX_Text(pDX, IDC_PASSWORD, m_strPassword);
	DDX_Text(pDX, IDC_DATASOURCE2, m_strDataSource2);
	DDX_Text(pDX, IDC_DATABASE2, m_strDatabase2);
	DDX_Text(pDX, IDC_USER2, m_strUser2);
	DDX_Text(pDX, IDC_PASSWORD2, m_strPassword2);
}


BEGIN_MESSAGE_MAP(CDlgConnect, CDialogEx)
	ON_BN_CLICKED(IDOK, &CDlgConnect::OnBnClickedOk)
END_MESSAGE_MAP()


// CDlgConnect 消息处理程序


void CDlgConnect::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(TRUE);

	m_connect.strDataSource = m_strDataSource;
	m_connect.strDatabase = m_strDatabase;
	m_connect.enumType = CDDT_SQLSERVER;
	m_connect.strUID = m_strUser;
	m_connect.strPWD = m_strPassword;

	CDYDatabaseEx db;
	db.SetReportErr(TRUE);
	db.SetWriteLog(TRUE);

	if (!db.Open(m_connect)) {
		return;
	}

	//db.Close();

	m_connect2.strDataSource = m_strDataSource2;
	m_connect2.strDatabase = m_strDatabase2;
	m_connect2.enumType = CDDT_SQLSERVER;
	m_connect2.strUID = m_strUser2;
	m_connect2.strPWD = m_strPassword2;

	CDYDatabaseEx db1;
	db1.SetReportErr(TRUE);
	db1.SetWriteLog(TRUE);
	if (!db1.Open(m_connect2)) {
		return;
	}

	
	//db1.Close();

	SetDBConnectionToFile(&m_connect, m_strDBCFileName);
	SetDBConnectionToFile(&m_connect2, m_strDBCFileName2);

	CDialogEx::OnOK();
}


BOOL CDlgConnect::OnInitDialog()
{
	CDialogEx::OnInitDialog();


	int nItem = m_cbDBType.InsertString(0, _T("SQLServer"));
	m_cbDBType.SetItemData(nItem, (DWORD)CDDT_SQLSERVER);
	nItem = m_cbDBType.InsertString(1, _T("Oracle"));
	m_cbDBType.SetItemData(nItem, (DWORD)CDDT_ORACLE);
	m_cbDBType.SetCurSel(0);

	nItem = m_cbDBType2.InsertString(0, _T("SQLServer"));
	m_cbDBType2.SetItemData(nItem, (DWORD)CDDT_SQLSERVER);
	nItem = m_cbDBType2.InsertString(1, _T("Oracle"));
	m_cbDBType2.SetItemData(nItem, (DWORD)CDDT_ORACLE);
	m_cbDBType2.SetCurSel(0);

	m_strDBCFileName = g_GetFullPath(_T("MetadataCopy.dbc"));
	m_strDBCFileName2 = g_GetFullPath(_T("MetadataCopy2.dbc"));
	GetDBConnectionFromFile(&m_connect, m_strDBCFileName);
	GetDBConnectionFromFile(&m_connect2, m_strDBCFileName2);

	m_strDataSource = m_connect.strDataSource;
	m_strDatabase = m_connect.strDatabase;
	m_cbDBType.SetCurSel(m_connect.enumType);
	m_strUser = m_connect.strUID;
	m_strPassword = m_connect.strPWD;

	m_strDataSource2 = m_connect2.strDataSource;
	m_strDatabase2 = m_connect2.strDatabase;
	m_cbDBType2.SetCurSel(m_connect2.enumType);
	m_strUser2 = m_connect2.strUID;
	m_strPassword2 = m_connect2.strPWD;

	UpdateData(FALSE);
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}
