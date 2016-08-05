typedef struct _OBJECT_ITEM
{
	CString strSourceObjectID;
	CString strTargetObjectID;
	CString strSourceName;
	CString strTargetName;
	CString strUserbit;
	BOOL bFindInSource;
	BOOL bFindInTarget;
	BOOL bFileInTarget;
	CString strPathID;
	CString strHighFileName;
	_OBJECT_ITEM()
	{
		bFindInSource = FALSE;
		bFindInTarget = FALSE;
		bFileInTarget = FALSE;
	}
}OBJECTITEM;
typedef CArray<OBJECTITEM, OBJECTITEM&> CObjectItemArray;

// MetadataCopyDlg.h : ͷ�ļ�
//

#pragma once
#include "afxcmn.h"

#include "DYRecordSetEx.h"


// CMetadataCopyDlg �Ի���
class CMetadataCopyDlg : public CDialogEx
{
// ����
public:
	CMetadataCopyDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_METADATACOPY_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��

	BOOL CopyBetweenDB(OBJECTITEM objectItem);
	BOOL CopyChildObjects(CString &strSourceObjectID, CString &strTargetObjectID);
	BOOL CopyBetweenTable(LPCTSTR lpszTable, LPCTSTR lpszFieldName, CString &strSourceObjectID, CString &strTargetObjectID);
	BOOL CopyBetweenTableBySQL(CString& strSelectSourceSQL, CString &strSelectTargetSQL, CString &strDeleteTargetSQL, CString &strSourceObjectID, CString &strTargetObjectID);
	BOOL CopyBetweenRecordSet(CDYRecordSetEx &rsSource, CDYRecordSetEx &rsTarget, int nFieldCount, CString &strSourceObjectID, CString &strTargetObjectID);
	
	BOOL ReadObjectFromDB(CString &strUserbit, OBJECTITEM &objectItem);
	BOOL FindFileInTarget(OBJECTITEM &objectItem);
	BOOL ReadAllStoragePath();


	void WriteLog(LPCTSTR lpszLog);
// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
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
	//BOOL m_bTestMode;//�Ƿ����ģʽ
};
