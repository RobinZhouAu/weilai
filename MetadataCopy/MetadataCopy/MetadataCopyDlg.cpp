
// MetadataCopyDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "MetadataCopy.h"
#include "MetadataCopyDlg.h"
#include "afxdialogex.h"

#include "DlgConnect.h"
#include "DlgCompare.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CMetadataCopyDlg 对话框


CMetadataCopyDlg::CMetadataCopyDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CMetadataCopyDlg::IDD, pParent)
	, m_strUserbit(_T(""))
	, m_strItemCount(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_strUserbit = _T("41004E01\r\n41004E02\r\n41004E03");
	m_bTestMode = FALSE;
}

void CMetadataCopyDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LOGLIST, m_lstLog);
	DDX_Text(pDX, IDC_USERBIT, m_strUserbit);
	DDX_Control(pDX, IDC_ITEMS, m_lstItems);
	DDX_Text(pDX, IDC_ITEM_COUNT, m_strItemCount);
	DDX_Control(pDX, IDC_TARGET_PATH, m_cbTargetPath);
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
	ON_BN_CLICKED(IDC_COMPARE, &CMetadataCopyDlg::OnBnClickedCompare)
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
	g_bSQLTrace = TRUE;
	GetDlgItem(IDC_SEARCH)->EnableWindow(FALSE);
	GetDlgItem(IDC_COMPARE)->EnableWindow(FALSE);
	GetDlgItem(IDC_COPY)->EnableWindow(FALSE);
	GetDlgItem(IDC_TARGET_PATH)->EnableWindow(FALSE);
	GetDlgItem(IDC_STATUS)->SetWindowText(_T(""));

	m_lstLog.InsertColumn(0, _T("Time"), LVCFMT_LEFT, 80, TRUE);
	m_lstLog.InsertColumn(1, _T("Log"), LVCFMT_LEFT, 600, TRUE);

	m_lstItems.InsertColumn(0, _T("USERBIT"), LVCFMT_LEFT, 80, TRUE);
	m_lstItems.InsertColumn(1, _T("Name"), LVCFMT_LEFT, 200, TRUE);
	m_lstItems.InsertColumn(2, _T("InSource"), LVCFMT_LEFT, 80, TRUE);
	m_lstItems.InsertColumn(3, _T("Files"), LVCFMT_LEFT, 60, TRUE);
	m_lstItems.InsertColumn(4, _T("InTarget"), LVCFMT_LEFT, 60, TRUE);
	m_lstItems.InsertColumn(5, _T("Archived"), LVCFMT_LEFT, 60, TRUE);
	m_lstItems.InsertColumn(6, _T("Status"), LVCFMT_LEFT, 60, TRUE);

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
	GetDlgItem(IDC_COMPARE)->EnableWindow(TRUE);
	GetDlgItem(IDC_TARGET_PATH)->EnableWindow(TRUE);
	WriteLog(_T("Connect DB successfully"));
}

void CMetadataCopyDlg::OnBnClickedSearch()
{
	CString strLog;
	UpdateData(TRUE);

	int nCurSel = m_cbTargetPath.GetCurSel();
	if (nCurSel == CB_ERR) {
		AfxMessageBox(_T("Please select the target path where the high res file are!"));
		return;
	}
	CString strUNCPath = m_aryStoragePaths[nCurSel].strUNCPath;
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
	for (int i = 0; i < m_aryObjectItems.GetSize(); i ++) {
		OBJECTITEM &objectItem = m_aryObjectItems[i];
		if (!ReadObjectFromDB(objectItem.strUserbit, objectItem))
			return;
		if (!FindFileInTarget(objectItem, strUNCPath))
			return;
		if (!CheckObjectExistInTarget(objectItem))
			return;
	}
	FillList();
}

void CMetadataCopyDlg::OnBnClickedCopy()
{
	int nCurSel = m_cbTargetPath.GetCurSel();
	if (nCurSel == -1) {
		AfxMessageBox(_T("Please select the target path where the high res file are!"));
		return;
	}
	STORAGEPATH &path = m_aryStoragePaths[nCurSel];

	CRegister reg;
	CString strKey = _T("Software\\Wow6432Node\\Dayang\\IMAM");
	if (!reg.OpenKey(HKEY_LOCAL_MACHINE, strKey)) {
		if (!reg.CreateKey(HKEY_LOCAL_MACHINE, strKey)) {
			CString strErrMsg;
			strErrMsg.Format(_T("OpenKey[%s] error!"), strKey);
			g_output.OutPut(TRUE, strErrMsg);
			AfxMessageBox(strErrMsg);
			return;
		}
		reg.OpenKey(HKEY_LOCAL_MACHINE, strKey);
	}
	reg.SetString(_T("DefaultPathID"), path.strPathID);

	int nCopyCount = 0;
	int nStatusCol = 6;
	for (int i = 0; i < m_aryObjectItems.GetSize(); i ++) {
		OBJECTITEM &objectItem = m_aryObjectItems[i];
		if (objectItem.bAlreadyExist){
			m_lstItems.SetItemText(i, nStatusCol, _T("Skipped"));
			continue;
		}
			
		if (!objectItem.bFindInSource || !objectItem.bFileInTarget) {
			m_lstItems.SetItemText(i, nStatusCol, _T("Skipped"));
			continue;
		}
		if (!CopyBetweenDB(objectItem, path)) {
			AfxMessageBox(_T("Copy failed!"));
			break;
		}
		m_lstItems.SetItemText(i, nStatusCol, _T("Copied"));
		nCopyCount ++;
		CString strLog;
		strLog.Format(_T("Copy object successfully![%s][%s]"), objectItem.strUserbit, objectItem.strSourceName);
		WriteLog(strLog);
	}

	CString strMsg;
	strMsg.Format(_T("Copy successfully! %d items copied"), nCopyCount);
	AfxMessageBox(strMsg);
}

BOOL CMetadataCopyDlg::CopyBetweenDB(OBJECTITEM &objectItem, STORAGEPATH &path)
{
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

	CString strSQL;
	strSQL.Format(_T("UPDATE COM_BASICINFO SET FILESTATUS=%d,NEEDREPAIR=0 WHERE OBJECTID='%s'"), XC_FS_ONLINE/*在线*/, objectItem.strTargetObjectID);
	if (!ExecuteSQLInTarget(strSQL))
		return FALSE;

	strSQL.Format(_T("UPDATE SPM_FILE SET STATUS=%d WHERE OBJECTID='%s'"), FSS_ONLINE/*在线*/, objectItem.strTargetObjectID);
	if (!ExecuteSQLInTarget(strSQL))
		return FALSE;
	
	strSQL.Format(_T("UPDATE SPM_FILE SET PATHID='%s',SAID='%s' WHERE OBJECTID='%s' AND FILETYPE IN(0,3,5)"),
		path.strPathID, path.strSAID, objectItem.strTargetObjectID);
	if (!ExecuteSQLInTarget(strSQL))
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
	CString strSQL;
	strSQL.Format(_T("select OBJECTID from cob_program where userbit='%s'"), objectItem.strUserbit);

	CDYRecordSetEx rsSource(&g_dbSource);
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
	rsSource.Close();

	CDYRecordSetEx rsTarget(&g_dbTarget);
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

BOOL CMetadataCopyDlg::ExecuteSQLInTarget(CString &strSQL)
{
	if (!g_dbTarget.ExecuteSQL(strSQL))
		return FALSE;
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

BOOL CMetadataCopyDlg::FindFileInTarget(OBJECTITEM &objectItem, CString &strUNCPath)
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
	//if (!rs.GetFieldValue(_T("PATHID"), objectItem.strPathID))
	//	return FALSE;
	rs.Close();

	//CString strUNCPath;
	//if (!m_mapUNCPath.Lookup(objectItem.strPathID, strUNCPath))
	//	return FALSE;
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
	m_aryStoragePaths.RemoveAll();
	m_cbTargetPath.ResetContent();
	CString strSQL = _T("select p.*,sa.name from spm_storagepath p, spm_storagearea sa where p.said=sa.said");
	CDYRecordSetEx rs(&g_dbTarget);
	if (!rs.Open(strSQL))
		return FALSE;
	CRegister reg;
	CString strKey = _T("Software\\Wow6432Node\\Dayang\\IMAM");
	if (!reg.OpenKey(HKEY_LOCAL_MACHINE, strKey)) {
		if (!reg.CreateKey(HKEY_LOCAL_MACHINE, strKey)) {
			CString strErrMsg;
			strErrMsg.Format(_T("OpenKey[%s] error!"), strKey);
			g_output.OutPut(TRUE, strErrMsg);
			AfxMessageBox(strErrMsg);
			return FALSE;
		}
		reg.OpenKey(HKEY_LOCAL_MACHINE, strKey);
	}
	CString strDefaultPathID = reg.GetString(_T("DefaultPathID"));
	int nDefaultPathIndex = 0;
	m_mapUNCPath.RemoveAll();
	while(!rs.IsEOF()) {
		STORAGEPATH path;
		if (!rs.GetFieldValue(_T("PATHID"), path.strPathID))
			return FALSE;
		if (!rs.GetFieldValue(_T("SAID"), path.strSAID))
			return FALSE;
		if (!rs.GetFieldValue(_T("UNCPATH"), path.strUNCPath))
			return FALSE;
		if (!rs.GetFieldValue(_T("NAME"), path.strStorageAreaName))
			return FALSE;
		int nItem = m_cbTargetPath.AddString(path.strStorageAreaName + path.strUNCPath);
		if (path.strPathID == strDefaultPathID)
			nDefaultPathIndex = nItem;
		m_aryStoragePaths.Add(path);
		g_output.OutPut(_T("%d, %s, %s\n"), nItem, path.strStorageAreaName, path.strUNCPath);
		rs.MoveNext();
	}
	m_cbTargetPath.SetCurSel(nDefaultPathIndex);
	rs.Close();
	return TRUE;
}

BOOL CMetadataCopyDlg::CheckObjectExistInTarget(OBJECTITEM &objectItem)
{
	CString strSQL;
	strSQL.Format(_T("select count(status) c from spm_file where objectid='%s' and filetype in(0,3,5) and status in(3,4,5)"), objectItem.strTargetObjectID);
	CDYRecordSetEx rs(&g_dbTarget);
	if (!rs.Open(strSQL))
		return FALSE;
	objectItem.bAlreadyExist = FALSE;
	int nCount = 0;
	if (rs.IsEOF())
		return FALSE;
	if (!rs.GetFieldValue(_T("c"), &nCount))
		return FALSE;
	objectItem.bAlreadyExist = nCount > 0;
	return TRUE;

}

void CMetadataCopyDlg::FillList()
{
	BOOL bEnableCopy = FALSE;
	m_lstItems.DeleteAllItems();
	CString strLog;
	for (int i = 0; i < m_aryObjectItems.GetSize(); i ++) {
		OBJECTITEM &objectItem = m_aryObjectItems[i];
		m_lstItems.InsertItem(i, objectItem.strUserbit);
		CString strName = objectItem.strSourceName.IsEmpty() ? objectItem.strTargetName : objectItem.strSourceName;
		m_lstItems.SetItemText(i, 1, objectItem.strSourceName);
		CString strText = objectItem.bFindInSource ? _T("Y") : _T("NULL");
		m_lstItems.SetItemText(i, 2, strText);
		strText = objectItem.bFileInTarget ? _T("Y") : _T("NULL");
		m_lstItems.SetItemText(i, 3, strText);
		strText = objectItem.bFindInTarget ? _T("Y") : _T("NULL");
		m_lstItems.SetItemText(i, 4, strText);
		strText = objectItem.bAlreadyExist ? _T("Y") : _T("NULL");
		m_lstItems.SetItemText(i, 5, strText);

		if (!objectItem.bFindInSource) {
			strLog.Format(_T("Can not find [%s] from source db"), objectItem.strUserbit);
			WriteLog(strLog);
		}
		if (objectItem.bAlreadyExist) {
			strLog.Format(_T("[%s] already copied. Can not copy it again"), objectItem.strUserbit);
			WriteLog(strLog);
		}
		if (!objectItem.bFindInTarget) {
			strLog.Format(_T("Can not find [%s] from target db"), objectItem.strUserbit);
			WriteLog(strLog);
		}

		if (objectItem.bFindInSource && objectItem.bFileInTarget)
			bEnableCopy = TRUE;
		if (!objectItem.bFindInTarget) 
			objectItem.strTargetObjectID = objectItem.strSourceObjectID;


		if (m_bTestMode) {
			if (objectItem.bFindInSource && !objectItem.bFindInTarget) {
				objectItem.strTargetObjectID = objectItem.strSourceObjectID;
				objectItem.strTargetObjectID.Replace(_T("-"), _T(""));
			}
		}
	}

	m_strItemCount.Format(_T("%d"), m_lstItems.GetItemCount());

	GetDlgItem(IDC_ITEM_COUNT)->SetWindowText(m_strItemCount);
	GetDlgItem(IDC_COPY)->EnableWindow(bEnableCopy);
}

void CMetadataCopyDlg::OnBnClickedCompare()
{
	int nCurSel = m_cbTargetPath.GetCurSel();
	if (nCurSel == CB_ERR) {
		AfxMessageBox(_T("Please select the target path where the high res file are!"));
		return;
	}
	CString strUNCPath = m_aryStoragePaths[nCurSel].strUNCPath;

	CDlgCompare dlg;
	if (dlg.DoModal() != IDOK)
		return;
	m_aryObjectItems.RemoveAll();
	for (int i = 0; i < dlg.m_arySelectedItems.GetSize(); i ++) {
		OBJECTITEM &objectItem = dlg.m_aryObjectItems[dlg.m_arySelectedItems[i]];
		if (!FindFileInTarget(objectItem, strUNCPath))
			return;
		if (!CheckObjectExistInTarget(objectItem))
			return;

		m_aryObjectItems.Add(objectItem);
	}
	FillList();
}
