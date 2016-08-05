
// MetadataCopyDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "MetadataCopy.h"
#include "MetadataCopyDlg.h"
#include "afxdialogex.h"

#include "DlgConnect.h"

#include "OutPut.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


ULONGLONG g_GetFileSize(LPCTSTR lpszFileName)
{
	CFile file;
	HANDLE hFile = ::CreateFile( lpszFileName, 0, 0, NULL, 
		OPEN_EXISTING, 0, NULL );
	if(hFile == INVALID_HANDLE_VALUE)
		return 0;
	ULONGLONG ullFileSize;
	DWORD dwFileSizeLow, dwFileSizeHigh;
	dwFileSizeLow = ::GetFileSize(hFile, &dwFileSizeHigh);
	ullFileSize = dwFileSizeHigh;
	ullFileSize = ullFileSize << 32;
	ullFileSize += dwFileSizeLow;

	CloseHandle(hFile);
	return ullFileSize;
}

// CMetadataCopyDlg 对话框

CDYDatabaseEx g_dbSource;
CDYDatabaseEx g_dbTarget;

COutPut g_output;

CMetadataCopyDlg::CMetadataCopyDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CMetadataCopyDlg::IDD, pParent)
	, m_strUserbit(_T(""))
	, m_strItemCount(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_strUserbit = _T("3100CE3F");
	//m_bTestMode = FALSE;
}

void CMetadataCopyDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LOGLIST, m_lstLog);
	DDX_Text(pDX, IDC_USERBIT, m_strUserbit);
	DDX_Control(pDX, IDC_ITEMS, m_lstItems);
	DDX_Text(pDX, IDC_ITEM_COUNT, m_strItemCount);
}

BEGIN_MESSAGE_MAP(CMetadataCopyDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDOK, &CMetadataCopyDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CMetadataCopyDlg::OnBnClickedCancel)
	ON_BN_CLICKED(IDC_CONNECT, &CMetadataCopyDlg::OnBnClickedConnect)
	ON_BN_CLICKED(IDC_COPY, &CMetadataCopyDlg::OnBnClickedCopy)
	ON_BN_CLICKED(IDC_SEARCH, &CMetadataCopyDlg::OnBnClickedSearch)
	ON_WM_CLOSE()
END_MESSAGE_MAP()


// CMetadataCopyDlg 消息处理程序

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CMetadataCopyDlg::OnPaint()
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
HCURSOR CMetadataCopyDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CMetadataCopyDlg::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	//CDialogEx::OnOK();
}

void CMetadataCopyDlg::OnBnClickedCancel()
{
	// TODO: 在此添加控件通知处理程序代码
	//CDialogEx::OnOK();
}

BOOL CMetadataCopyDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	g_output.SetLogFile(_T("metadatacopy.log"));
	GetDlgItem(IDC_SEARCH)->EnableWindow(FALSE);
	GetDlgItem(IDC_COPY)->EnableWindow(FALSE);
	GetDlgItem(IDC_STATUS)->SetWindowText(_T(""));

	m_lstLog.InsertColumn(0, _T("Time"), LVCFMT_LEFT, 80, TRUE);
	m_lstLog.InsertColumn(1, _T("Log"), LVCFMT_LEFT, 600, TRUE);

	m_lstItems.InsertColumn(0, _T("USERBIT"), LVCFMT_LEFT, 80, TRUE);
	m_lstItems.InsertColumn(1, _T("Name"), LVCFMT_LEFT, 200, TRUE);
	m_lstItems.InsertColumn(2, _T("InSource"), LVCFMT_LEFT, 80, TRUE);
	m_lstItems.InsertColumn(3, _T("Files"), LVCFMT_LEFT, 60, TRUE);
	m_lstItems.InsertColumn(4, _T("InTarget"), LVCFMT_LEFT, 60, TRUE);
	m_lstItems.InsertColumn(5, _T("Status"), LVCFMT_LEFT, 60, TRUE);

	m_lstItems.SetExtendedStyle(m_lstItems.GetExtendedStyle() || LVS_EX_GRIDLINES);

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CMetadataCopyDlg::OnClose()
{
	CDialogEx::OnOK();
	CDialogEx::OnClose();
}

void CMetadataCopyDlg::OnBnClickedConnect()
{
	CDlgConnect dlgConnect;
	if (dlgConnect.DoModal() != IDOK) {
		GetDlgItem(IDC_STATUS)->SetWindowText(_T(""));
		return;
	}
	g_dbTarget.Close();
	g_dbSource.Close();
	if (!g_dbSource.Open(dlgConnect.m_connect))
		return;
	if (!g_dbTarget.Open(dlgConnect.m_connect2))
		return;

	GetDlgItem(IDC_STATUS)->SetWindowText(_T("Connect DB successfully"));
	g_dbSource.SetReportErr(TRUE);
	g_dbSource.SetWriteLog(TRUE);
	g_dbTarget.SetReportErr(TRUE);
	g_dbTarget.SetWriteLog(TRUE);

	ReadAllStoragePath();
	GetDlgItem(IDC_SEARCH)->EnableWindow(TRUE);
	WriteLog(_T("Connect DB successfully"));
}

void CMetadataCopyDlg::OnBnClickedSearch()
{
	CString strLog;
	UpdateData(TRUE);
	m_strUserbit.Trim();
	if (m_strUserbit.IsEmpty()) {
		AfxMessageBox(_T("Please input userbit!"));
		return;
	}

	strLog.Format(_T("Search by userbit=[%s]"), m_strUserbit);
	WriteLog(strLog);

	m_aryObjectItems.RemoveAll();

	TCHAR * pch = _tcstok ((LPTSTR)(LPCTSTR)m_strUserbit, _T("\r\n"));
	while (pch != NULL)
	{
		CString strUserbit = pch;
		strUserbit.Trim();
		if (!strUserbit.IsEmpty()) {
			OBJECTITEM objectItem;
			objectItem.strUserbit = strUserbit;
			m_aryObjectItems.Add(objectItem);
		}
		pch = _tcstok (NULL, _T("\r\n"));
	}

	BOOL bEnableCopy = FALSE;
	m_lstItems.DeleteAllItems();
	for (int i = 0; i < m_aryObjectItems.GetSize(); i ++) {
		OBJECTITEM &objectItem = m_aryObjectItems[i];
		if (!ReadObjectFromDB(objectItem.strUserbit, objectItem))
			return;
		if (!FindFileInTarget(objectItem))
			return;
		m_lstItems.InsertItem(i, objectItem.strUserbit);
		CString strName = objectItem.strSourceName.IsEmpty() ? objectItem.strTargetName : objectItem.strSourceName;
		m_lstItems.SetItemText(i, 1, objectItem.strSourceName);
		CString strText = objectItem.bFindInSource ? _T("Y") : _T("NULL");
		m_lstItems.SetItemText(i, 2, strText);
		strText = objectItem.bFileInTarget ? _T("Y") : _T("NULL");
		m_lstItems.SetItemText(i, 3, strText);
		strText = objectItem.bFindInTarget ? _T("Y") : _T("NULL");
		m_lstItems.SetItemText(i, 4, strText);

		if (!objectItem.bFindInSource) {
			strLog.Format(_T("Can not find [%s] from source db"), objectItem.strUserbit);
			WriteLog(strLog);
		}
		if (!objectItem.bFindInTarget) {
			strLog.Format(_T("Can not find [%s] from target db"), objectItem.strUserbit);
			WriteLog(strLog);
		}

		if (objectItem.bFindInSource && objectItem.bFileInTarget)
			bEnableCopy = TRUE;
		if (!objectItem.bFileInTarget)
			objectItem.strTargetObjectID = objectItem.strSourceObjectID;

		//if (m_bTestMode) {
		//	if (objectItem.bFindInSource && !objectItem.bFindInTarget)
		//		bEnableCopy = TRUE;
		//	objectItem.strTargetObjectID = objectItem.strSourceObjectID;
		//	objectItem.strTargetObjectID.Replace(_T("-"), _T(""));
		//} else {
		//	if (objectItem.bFindInSource && objectItem.bFindInTarget)
		//		bEnableCopy = TRUE;
		//}
	}

	m_strItemCount.Format(_T("%d"), m_lstItems.GetItemCount());
	
	GetDlgItem(IDC_ITEM_COUNT)->SetWindowText(m_strItemCount);
	GetDlgItem(IDC_COPY)->EnableWindow(bEnableCopy);
}

void CMetadataCopyDlg::OnBnClickedCopy()
{
	int nCopyCount = 0;
	for (int i = 0; i < m_aryObjectItems.GetSize(); i ++) {
		OBJECTITEM &objectItem = m_aryObjectItems[i];
		if (!objectItem.bFindInSource || !objectItem.bFileInTarget) {
			m_lstItems.SetItemText(i, 5, _T("Skipped"));
			continue;
		}
		if (!CopyBetweenDB(objectItem)) {
			AfxMessageBox(_T("Copy failed!"));
			break;
		}
		m_lstItems.SetItemText(i, 5, _T("Copied"));
		nCopyCount ++;
		CString strLog;
		strLog.Format(_T("Copy object successfully![%s][%s]"), objectItem.strUserbit, objectItem.strSourceName);
		WriteLog(strLog);
	}

	CString strMsg;
	strMsg.Format(_T("Copy successfully! %d items copied"), nCopyCount);
	AfxMessageBox(strMsg);
}

BOOL CMetadataCopyDlg::CopyBetweenDB(OBJECTITEM objectItem)
{
	//if (m_bTestMode) {//测试模式
	//	if (!CopyBetweenTable(_T("COM_BASICINFO"), _T("OBJECTID"), objectItem.strSourceObjectID, objectItem.strTargetObjectID))
	//		return FALSE;

	//	if (!CopyBetweenTable(_T("COB_PROGRAM"), _T("OBJECTID"), objectItem.strSourceObjectID, objectItem.strTargetObjectID))
	//		return FALSE;

	//	CString strSelectSourceSQL, strSelectTargetSQL, strDeleteTargetSQL;
	//	//Copy data from spm_file
	//	strSelectSourceSQL.Format(_T("select * from spm_file where filetype not in (2,4) and objectid='%s'"), strSourceObjectID);
	//	strDeleteTargetSQL.Format(_T("delete from spm_file where filetype not in(2,4) and objectid='%s'"), strTargetObjectID);
	//	strSelectTargetSQL.Format(_T("select * from spm_file where 1=2"));//空集

	//	if (!CopyBetweenTableBySQL(strSelectSourceSQL, strSelectTargetSQL, strDeleteTargetSQL, strSourceObjectID, strTargetObjectID))
	//		return FALSE;
	//} else {
	//	if (!CopyBetweenTable(_T("COB_PROGRAM"), _T("OBJECTID"), strSourceObjectID, strTargetObjectID))
	//		return FALSE;
	//	CString strSelectSourceSQL, strSelectTargetSQL, strDeleteTargetSQL;
	//	//Copy data from spm_file
	//	strSelectSourceSQL.Format(_T("select * from spm_file where filetype in (2,4) and objectid='%s'"), strSourceObjectID);
	//	strDeleteTargetSQL.Format(_T("delete from spm_file where filetype in(2,4) and objectid='%s'"), strTargetObjectID);
	//	strSelectTargetSQL.Format(_T("select * from spm_file where 1=2"));//空集

	//	if (!CopyBetweenTableBySQL(strSelectSourceSQL, strSelectTargetSQL, strDeleteTargetSQL, strSourceObjectID, strTargetObjectID))
	//		return FALSE;
	//}
	if (!CopyBetweenTable(_T("COM_BASICINFO"), _T("OBJECTID"), objectItem.strSourceObjectID, objectItem.strTargetObjectID))
		return FALSE;
	if (!CopyBetweenTable(_T("COB_PROGRAM"), _T("OBJECTID"), objectItem.strSourceObjectID, objectItem.strTargetObjectID))
		return FALSE;
	if (!CopyBetweenTable(_T("SPM_FILE"), _T("OBJECTID"), objectItem.strSourceObjectID, objectItem.strTargetObjectID))
		return FALSE;
	if (!CopyBetweenTable(_T("COM_KEYFRAME"), _T("CLIPID"), objectItem.strSourceObjectID, objectItem.strTargetObjectID))
		return FALSE;
	if (!CopyBetweenTable(_T("COM_SPECIALSEQUENCE"), _T("OBJECTID"), objectItem.strSourceObjectID, objectItem.strTargetObjectID))
		return FALSE;
	return TRUE;
}

BOOL CMetadataCopyDlg::CopyChildObjects(CString &strSourceObjectID, CString &strTargetObjectID)
{
	CString strSelectSourceSQL, strSelectTargetSQL, strDeleteTargetSQL;
	//Copy data from cob_sequence
	strSelectSourceSQL.Format(_T("select * from COB_SEQUENCE where OBJECTID in(select OBJECTID from COM_BASICINFO where ROOTID='%s')"), strSourceObjectID);
	strDeleteTargetSQL.Format(_T("delete from COB_SEQUENCE where OBJECTID in(select OBJECTID from COM_BASICINFO where ROOTID='%s')"), strTargetObjectID);
	strSelectTargetSQL.Format(_T("select * from COB_SEQUENCE where 1=2"));//空集
	if (!CopyBetweenTableBySQL(strSelectSourceSQL, strSelectTargetSQL, strDeleteTargetSQL, strSourceObjectID, strTargetObjectID))
		return FALSE;

	//Copy data from cob_scene
	strSelectSourceSQL.Format(_T("select * from COB_SCENE where OBJECTID in(select OBJECTID from COM_BASICINFO where ROOTID='%s')"), strSourceObjectID);
	strDeleteTargetSQL.Format(_T("delete from COB_SCENE where OBJECTID in(select OBJECTID from COM_BASICINFO where ROOTID='%s')"), strTargetObjectID);
	strSelectTargetSQL.Format(_T("select * from COB_SCENE where 1=2"));//空集
	if (!CopyBetweenTableBySQL(strSelectSourceSQL, strSelectTargetSQL, strDeleteTargetSQL, strSourceObjectID, strTargetObjectID))
		return FALSE;

	//Copy data from cob_shot
	strSelectSourceSQL.Format(_T("select * from COB_SHOT where OBJECTID in(select OBJECTID from COM_BASICINFO where ROOTID='%s')"), strSourceObjectID);
	strDeleteTargetSQL.Format(_T("delete from COB_SHOT where OBJECTID in(select OBJECTID from COM_BASICINFO where ROOTID='%s')"), strTargetObjectID);
	strSelectTargetSQL.Format(_T("select * from COB_SHOT where 1=2"));//空集
	if (!CopyBetweenTableBySQL(strSelectSourceSQL, strSelectTargetSQL, strDeleteTargetSQL, strSourceObjectID, strTargetObjectID))
		return FALSE;

	//Copy child data from com_basicinfo
	strSelectSourceSQL.Format(_T("select * from COM_BASICINFO where ROOTID='%s' AND OBJECTID!='%s'"), strSourceObjectID, strSourceObjectID);
	strDeleteTargetSQL.Format(_T("delete from COM_BASICINFO where ROOTID='%s' AND OBJECTID!='%s'"), strTargetObjectID, strTargetObjectID);
	strSelectTargetSQL.Format(_T("select * from COM_BASICINFO where 1=2"));//空集
	if (!CopyBetweenTableBySQL(strSelectSourceSQL, strSelectTargetSQL, strDeleteTargetSQL, strSourceObjectID, strTargetObjectID))
		return FALSE;

	return TRUE;
}

BOOL CMetadataCopyDlg::CopyBetweenTable(LPCTSTR lpszTable, LPCTSTR lpszFieldName, CString &strSourceObjectID, CString &strTargetObjectID)
{
	CString strSelectSourceSQL, strSelectTargetSQL, strDeleteTargetSQL;
	strSelectSourceSQL.Format(_T("select * from %s where %s='%s'"), lpszTable, lpszFieldName, strSourceObjectID);
	strDeleteTargetSQL.Format(_T("delete from %s where %s='%s'"), lpszTable, lpszFieldName, strTargetObjectID);
	strSelectTargetSQL.Format(_T("select * from %s where 1=2"), lpszTable);
	if (!CopyBetweenTableBySQL(strSelectSourceSQL, strSelectTargetSQL, strDeleteTargetSQL, strSourceObjectID, strTargetObjectID))
		return FALSE;

	return TRUE;
}

BOOL CMetadataCopyDlg::CopyBetweenTableBySQL(CString& strSelectSourceSQL, CString &strSelectTargetSQL, CString &strDeleteTargetSQL, CString &strSourceObjectID, CString &strTargetObjectID)
{
	if (!g_dbTarget.ExecuteSQL(strDeleteTargetSQL))
		return FALSE;

	CDYRecordSetEx rsSource(&g_dbSource);
	if (!rsSource.Open(strSelectSourceSQL))
		return FALSE;
	CDYRecordSetEx rsTarget(&g_dbTarget);
	if (!rsTarget.Open(strSelectTargetSQL))
		return FALSE;

	int nFieldCount = rsSource.GetFieldCount();
	while(!rsSource.IsEOF()) {
		if (!rsTarget.AddNew())
			return FALSE;

		CopyBetweenRecordSet(rsSource, rsTarget, nFieldCount, strSourceObjectID, strTargetObjectID);
		rsSource.MoveNext();
	}

	if (!rsTarget.Update())
		return FALSE;

	rsTarget.Close();
	rsSource.Close();
	return TRUE;
}

BOOL CMetadataCopyDlg::CopyBetweenRecordSet(CDYRecordSetEx &rsSource, CDYRecordSetEx &rsTarget, int nFieldCount, CString &strSourceObjectID, CString &strTargetObjectID)
{
	_variant_t val;
	for (short i = 0; i < nFieldCount; i ++) {
		if (!rsSource.GetFieldValue(i, &val))
			return FALSE;

		if (val.vt == VT_BSTR) {
			CString strValue = CString((BSTR)(_bstr_t)val);
			if (strValue == strSourceObjectID)
				val = strTargetObjectID;
		}
		if (!rsTarget.PutFieldValue(i, val))
			return FALSE;
	}
	return TRUE;
}

BOOL CMetadataCopyDlg::ReadObjectFromDB(CString &strUserbit, OBJECTITEM &objectItem)
{
	if (!g_dbSource.IsOpen() || !g_dbTarget.IsOpen())
		return FALSE;
	CDYRecordSetEx rsSource(&g_dbSource);
	CDYRecordSetEx rsTarget(&g_dbTarget);
	CString strSQL;
	strSQL.Format(_T("select objectid from cob_program where userbit='%s'"), objectItem.strUserbit);
	if (!rsSource.Open(strSQL)) {
		return FALSE;
	}
	if (rsSource.IsEOF()) {
		objectItem.bFindInSource = FALSE;
	} else {
		objectItem.bFindInSource = TRUE;
		if (!rsSource.GetFieldValue(_T("OBJECTID"), objectItem.strSourceObjectID))
			return FALSE;
	}
	if (!rsTarget.Open(strSQL)) {
		return FALSE;
	}
	if (rsTarget.IsEOF()) {
		objectItem.bFindInTarget = FALSE;
	} else {
		objectItem.bFindInTarget = TRUE;
		if (!rsTarget.GetFieldValue(_T("OBJECTID"), objectItem.strTargetObjectID))
			return FALSE;
	}
	rsSource.Close();
	rsTarget.Close();

	strSQL.Format(_T("select * from com_basicinfo where objectid='%s'"), objectItem.strSourceObjectID);

	if (objectItem.bFindInSource) {
		if (!rsSource.Open(strSQL))
			return FALSE;
		if (rsSource.IsEOF()) {
			objectItem.bFindInSource = FALSE;	
		} else {
			if (!rsSource.GetFieldValue(_T("NAME"), objectItem.strSourceName))
				return FALSE;
		}
		rsSource.Close();
	}

	strSQL.Format(_T("select * from com_basicinfo where objectid='%s'"), objectItem.strTargetObjectID);
	if (objectItem.bFindInTarget) {
		if (!rsTarget.Open(strSQL))
			return FALSE;
		if (rsTarget.IsEOF()) {
			objectItem.bFindInTarget = FALSE;	
		} else {
			if (!rsTarget.GetFieldValue(_T("NAME"), objectItem.strTargetName))
				return FALSE;
		}
		rsTarget.Close();
	}

	return TRUE;

}


void CMetadataCopyDlg::WriteLog(LPCTSTR lpszLog)
{
	int nCount = m_lstLog.GetItemCount();
	CTime time = CTime::GetCurrentTime();;
	CString strTime = time.Format( _T("[%H:%M:%S]\t") );
	m_lstLog.InsertItem(nCount, strTime);
	m_lstLog.SetItemText(nCount, 1, lpszLog);
	m_lstLog.EnsureVisible(nCount, FALSE);
	g_output.OutPut(TRUE, lpszLog);
}

BOOL CMetadataCopyDlg::FindFileInTarget(OBJECTITEM &objectItem)
{
	objectItem.bFileInTarget = FALSE;

	CString strSQL;
	strSQL.Format(_T("select filename,pathid from spm_file where objectid='%s' and filetype in(0,3,5)"), objectItem.strSourceObjectID);
	CDYRecordSetEx rs(&g_dbSource);
	if (!rs.Open(strSQL))
		return FALSE;
	if (rs.IsEOF()) {
		CString strLog;
		strLog.Format(_T("Not find file record with userbit[%s]"), objectItem.strUserbit);
		WriteLog(strLog);
		return TRUE;
	}

	if (!rs.GetFieldValue(_T("FILENAME"), objectItem.strHighFileName))
		return FALSE;
	if (!rs.GetFieldValue(_T("PATHID"), objectItem.strPathID))
		return FALSE;
	rs.Close();

	CString strUNCPath;
	if (!m_mapUNCPath.Lookup(objectItem.strPathID, strUNCPath))
		return FALSE;
	CString strUNCFileName;
	strUNCFileName.Format(_T("%s/%s"), strUNCPath, objectItem.strHighFileName);
	strUNCPath.Replace(_T("//"), _T("/"));
	g_output.OutPut(TRUE, strUNCFileName);
	ULONGLONG ullFileSize = g_GetFileSize(strUNCFileName);
	if (ullFileSize > 0)
		objectItem.bFileInTarget = TRUE;
		
	return TRUE;
}

BOOL CMetadataCopyDlg::ReadAllStoragePath()
{
	CString strSQL = _T("select pathid,uncpath from spm_storagepath");
	CDYRecordSetEx rs(&g_dbTarget);
	if (!rs.Open(strSQL))
		return FALSE;
	m_mapUNCPath.RemoveAll();
	while(!rs.IsEOF()) {
		CString strPathID, strUNCPath;
		if (!rs.GetFieldValue(_T("PATHID"), strPathID))
			return FALSE;
		if (!rs.GetFieldValue(_T("UNCPATH"), strUNCPath))
			return FALSE;
		strUNCPath = _T("D:\\0Project\\纬来\\high");//For test
		m_mapUNCPath.SetAt(strPathID, strUNCPath);
		rs.MoveNext();
	}
	rs.Close();
	return TRUE;
}