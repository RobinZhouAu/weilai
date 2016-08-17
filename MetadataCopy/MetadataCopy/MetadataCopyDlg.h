
typedef struct _STORAGE_PATH
{
	CString strPathID;
	CString strSAID;
	CString strStorageAreaName;
	CString strUNCPath;
}STORAGEPATH;
typedef CArray<STORAGEPATH, STORAGEPATH&> CStoragePathArray;

// MetadataCopyDlg.h : 头文件
//

#pragma once
#include "afxcmn.h"

#include "afxwin.h"

extern BOOL g_bSQLTrace;


// CMetadataCopyDlg 对话框
class CMetadataCopyDlg : public CDialogEx
{
// 构造
public:
	CMetadataCopyDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_METADATACOPY_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持

	BOOL CopyBetweenDB(OBJECTITEM &objectItem, STORAGEPATH &path);
	BOOL CopyChildObjects(CString &strSourceObjectID, CString &strTargetObjectID);
	BOOL CopyBetweenTable(LPCTSTR lpszTable, LPCTSTR lpszFieldName, CString &strSourceObjectID, CString &strTargetObjectID);
	BOOL CopyBetweenTableBySQL(CString& strSelectSourceSQL, CString &strSelectTargetSQL, CString &strDeleteTargetSQL, CString &strSourceObjectID, CString &strTargetObjectID);
	BOOL CopyBetweenRecordSet(CDYRecordSetEx &rsSource, CDYRecordSetEx &rsTarget, int nFieldCount, CString &strSourceObjectID, CString &strTargetObjectID);
	BOOL ExecuteSQLInTarget(CString &strSQL);
	
	BOOL ReadObjectFromDB(CString &strUserbit, OBJECTITEM &objectItem);
	BOOL FindFileInTarget(OBJECTITEM &objectItem, CString &strUNCPath);
	BOOL CheckObjectExistInTarget(OBJECTITEM &objectItem);
	BOOL ReadAllStoragePath();

	void FillList();


	void WriteLog(LPCTSTR lpszLog);
// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedConnect();
	afx_msg void OnBnClickedCopy();
	afx_msg void OnBnClickedSearch();
	afx_msg void OnClose();

	CString m_strItemCount;
	CString m_strUserbit;
	CListCtrl m_lstItems;
	CListCtrl m_lstLog;

	CObjectItemArray m_aryObjectItems;
	CMapStringToString m_mapUNCPath;
	CComboBox m_cbTargetPath;
	CStoragePathArray m_aryStoragePaths;
	BOOL m_bTestMode;
	afx_msg void OnBnClickedCompare();
};
