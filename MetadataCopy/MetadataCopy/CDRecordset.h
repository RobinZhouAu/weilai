// CDRecordset.h: interface for the CCDRecordset class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CDRECORDSET_H__686373DD_107C_469B_A5A6_7338A680FCB0__INCLUDED_)
#define AFX_CDRECORDSET_H__686373DD_107C_469B_A5A6_7338A680FCB0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "CDDatabase.h"
#include "DYRecordSetEx.h"

class CCDRecordset : public CDYRecordSetEx 
{
public:
	CCDRecordset(CDYDatabaseEx* pdb); 
	virtual ~CCDRecordset();
//Attribute
public:
	BOOL IsOpen(){ return m_bOpen; }	//数据库是否打开

	// 表的操作
	BOOL IsEOF();	//是否结束位置
	BOOL IsBOF();
	int  GetFieldCount();	//得到列的个数
	int  GetRecordCount();
	void SetDatabase(CDYDatabaseEx *pdb){ m_pDB = (CCDDatabase *)pdb; };	
	//CCDDatabase *GetDatabase(){ return m_pDB; }
	CString GetSQLStatemenet(){ return CString((BSTR)(_bstr_t)m_pRecordSet->GetSource()); }
	void SetPageSize(int nPageSize);
	int	 GetPageCount();
//Operations
public:
	BOOL AddNew();
	BOOL Update();

	BOOL PutFieldValue(short nIndex, const _variant_t &val);		//得到字段的名字
	BOOL PutFieldValue(LPCTSTR lpszFieldName, const _variant_t &val);

	BOOL GetFieldName(short nIndex, CString &strFieldName);			//得到字段的名字

	BOOL GetFieldValue(short nIndex, CString &strValue);			//得到字段的值
	
	BOOL GetFieldValue(short nIndex, int *pValue);	//得到int字段的值
	BOOL GetFieldValue(short nIndex, long *pValue);	//得到long字段的值
	BOOL GetFieldValue(short nIndex, CString *pValue);	//得到CString字段的值
	BOOL GetFieldValue(short nIndex, double *pValue);	//得到Double字段的值
	BOOL GetFieldValue(short nIndex, DWORD *pValue);	//得到DWORD字段的值
	BOOL GetFieldValue(short nIndex, COleDateTime *pValue);	//得到Date字段的值
	BOOL GetFieldValue(short nIndex, _variant_t *pValue);	//得到variant字段的值
	BOOL GetFieldValue(short nIndex, LPVOID lpValue);	//为了兼容以前的程序，枚举类型等
	
	BOOL GetFieldValue(LPCTSTR lpszFieldName, CString &strValue);	//得到字段的值
	
	BOOL GetFieldValue(LPCTSTR lpszFieldName, int *pValue);	//得到int字段的值
	BOOL GetFieldValue(LPCTSTR lpszFieldName, long *pValue);	//得到long字段的值
	BOOL GetFieldValue(LPCTSTR lpszFieldName, CString *pValue);	//得到CString字段的值
	BOOL GetFieldValue(LPCTSTR lpszFieldName, double *pValue);	//得到Double字段的值
	BOOL GetFieldValue(LPCTSTR lpszFieldName, DWORD *pValue);	//得到DWORD字段的值
	BOOL GetFieldValue(LPCTSTR lpszFieldName, COleDateTime *pValue);	//得到Date字段的值
	BOOL GetFieldValue(LPCTSTR lpszFieldName, _variant_t *pValue);	//得到variant字段的值
	BOOL GetFieldValue(LPCTSTR lpszFieldName, LPVOID lpValue);	//为了兼容以前的程序，枚举类型等
	
	BOOL MoveFirst();	//移动到第一条记录
	BOOL MoveNext();	//移到下一记录
	BOOL Move(long nMoveTo);//移到指定记录
	
	BOOL Open(CString &strSQL, BOOL bUseClient = TRUE, LockTypeEnum LockType = adLockOptimistic, CString &strResponse = CString(_T("")) );	//打开记录集
	BOOL Open(CString &strSQL, CursorLocationEnum CursorLocation,
		CursorTypeEnum CursorType = adOpenKeyset, LockTypeEnum LockType = adLockOptimistic,
		CommandTypeEnum CommaonType = adCmdUnspecified);	//打开记录集
	BOOL OpenByStoredProc(CString &strProcName);
	BOOL Close();	//关闭记录集
	BOOL OpenSchema(SchemaEnum QueryType);
	void SetAbsolutePage(int nPage);
	BOOL IsEmptyString(const _variant_t &varSrc);
protected:
	CCDDatabase* m_pDB;				//数据库
	_RecordsetPtr m_pRecordSet;		//记录集
private:
	BOOL m_bOpen;			//是否打开
	int m_nUpdateField;
};

#endif // !defined(AFX_CDRECORDSET_H__686373DD_107C_469B_A5A6_7338A680FCB0__INCLUDED_)
