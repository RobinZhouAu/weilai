// CDRecordset.h: interface for the CCDHttpRecordset class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CD_HTTP_RECORDSET_H__686373DD_107C_469B_A5A6_7338A680FCB0__INCLUDED_)
#define AFX_CD_HTTP_RECORDSET_H__686373DD_107C_469B_A5A6_7338A680FCB0__INCLUDED_
//#ifdef _USE_HTTP

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "DYHttpClient.h"
#include "CDHttpDatabase.h"
#include "DYRecordsetEx.h"

struct _FIELD_VALUE {
	_variant_t varOri;
	_variant_t varNew;
	BOOL bChanged;
	_FIELD_VALUE()
	{
		bChanged = FALSE;
	}
	~_FIELD_VALUE()
	{
	}
};
typedef CArray<_FIELD_VALUE , _FIELD_VALUE&> CArrayField;
enum _RECORD_TYPE
{
	_RECORD_NORMAL,
	_RECORD_NEW,
	_RECORD_UPDATE,
};

struct _RECORD_VALUE {
	CArrayField aryField;
	_RECORD_TYPE enumType;
	_RECORD_VALUE()
	{
		enumType = _RECORD_NORMAL;
	}
};

typedef CArray<_RECORD_VALUE*, _RECORD_VALUE*&> CArrayRecord;


//#include <afxinet.h>//mfc internet class


enum _FIELD_TYPE
{
	FIELD_STRING = 1,
	FIELD_BINARY,
	FIELD_INTEGER,
	FIELD_FLOAT,
	FIELD_DATE,
	FIELD_LONGRAW

};
class CCDHttpRecordset : public CDYRecordSetEx 
{
public:
	CCDHttpRecordset(CDYDatabaseEx* pdb);
	virtual ~CCDHttpRecordset();
//Attribute
public:
	BOOL IsOpen(){ return m_bOpen; }	//数据库是否打开

	// 表的操作
	BOOL IsEOF();	//是否结束位置
	BOOL IsBOF();
	int  GetFieldCount();	//得到列的个数
	int  GetRecordCount();
	void SetDatabase(CDYDatabaseEx *pdb){ m_pDB = (CCDHttpDatabase*)pdb; };	
	CDYDatabaseEx *GetDatabase(){ return m_pDB; }
//Operations
	BOOL AddNew();
	BOOL Update();//与ADO记录集更新当前记录不同，
	//该操作更新记录集里所有改变或新增的记录，只需在更改记录集最后调用一次
	//ADO记录集在从编辑或插入的记录中移走时会自动调用Update更新更改的记录，
	//但HttpRecordset需要在最后显式调用该函数进行更新。
	
	BOOL ExecuteSQL(CString &strSQL);
		
	BOOL PutFieldValue(short nIndex, const _variant_t &val);		//得到字段的名字
	BOOL PutFieldValue(LPCTSTR lpszFieldName, const _variant_t &val);
	BOOL GetFieldName(short nIndex, CString &strFieldName);			//得到字段的名字
	
	BOOL GetFieldValue(short nIndex, CString &strValue);			//得到字段的值
	//BOOL GetFieldValue(short nIndex, _variant_t &val);				//得到字段的值

	BOOL GetFieldValue(short nIndex, int *pValue);	//得到int字段的值
	BOOL GetFieldValue(short nIndex, long *pValue);	//得到long字段的值
	BOOL GetFieldValue(short nIndex, CString *pValue);	//得到CString字段的值
	BOOL GetFieldValue(short nIndex, double *pValue);	//得到Double字段的值
	BOOL GetFieldValue(short nIndex, DWORD *pValue);	//得到DWORD字段的值
	BOOL GetFieldValue(short nIndex, COleDateTime *pValue);	//得到Date字段的值
	BOOL GetFieldValue(short nIndex, _variant_t *pValue);	//得到variant字段的值
	BOOL GetFieldValue(short nIndex, LPVOID pValue);		//为了兼容以前的程序

//	BOOL GetFieldValue(LPCTSTR lpszFieldName, _variant_t &val);		//得到字段的值
	BOOL GetFieldValue(LPCTSTR lpszFieldName, CString &strValue);	//得到字段的值
	
	BOOL GetFieldValue(LPCTSTR lpszFieldName, int *pValue);	//得到int字段的值
	BOOL GetFieldValue(LPCTSTR lpszFieldName, long *pValue);	//得到long字段的值
	BOOL GetFieldValue(LPCTSTR lpszFieldName, CString *pValue);	//得到CString字段的值
	BOOL GetFieldValue(LPCTSTR lpszFieldName, double *pValue);	//得到Double字段的值
	BOOL GetFieldValue(LPCTSTR lpszFieldName, DWORD *pValue);	//得到DWORD字段的值
	BOOL GetFieldValue(LPCTSTR lpszFieldName, COleDateTime *pValue);	//得到Date字段的值
	BOOL GetFieldValue(LPCTSTR lpszFieldName, _variant_t *pValue);	//得到variant字段的值
	BOOL GetFieldValue(LPCTSTR lpszFieldName, LPVOID pValue);		//为了兼容以前的程序
	
	BOOL MoveNext();	//移到下一记录。
	BOOL MoveFirst();	//移到首记录.
	BOOL Move(long nIndex);	//移到指定记录
	
	BOOL Open(CString &strSQL, BOOL bUseClient = TRUE, LockTypeEnum LockType = adLockOptimistic,CString &strResponse = CString(_T("")));	//打开记录集adLockOptimistic
	BOOL OpenByStoredProc(CString &strProcName);
	BOOL Close();	//关闭记录集
		
protected:
	BOOL Request(LPCTSTR lpszURL, CString &strData, CString &strResponse, BOOL bNotTransfer = TRUE);
	BOOL ReadString(CString& rString);
	BOOL Decode(_variant_t &val,BSTR &bstr);//bstr ->val
	BOOL Encode(_variant_t &var,CString &strValue);//var -> strvalue
	CString GetUpdateCondition(_RECORD_VALUE *pRecoed,BOOL bInsert = FALSE);
	BOOL GetFieldValueForXML(int i,CString &strValue, BOOL bGetNew = TRUE, VARTYPE vt = VT_NULL);
	int GetColIndex(LPCTSTR lpszColName);
	BOOL SetColType(int nCol, int nVarType);

	BOOL FillXML(CString &strXML);
	BOOL ParseRecordXML(LPCSTR lpszXML);//CString &strXml);
	
	//兼容老版本
	BOOL GetFieldValue(short nIndex, CString &strValue,BOOL bGetNew, VARTYPE vt = VT_NULL);			//得到字段的值
	BOOL GetFieldValue(short nIndex, _variant_t &val,BOOL bGetNew);				//得到字段的值
	
	CString m_strErrorCode;
	
	int m_nKeyFieldNum;
	CUIntArray m_aryFieldType;//field data type
	CUIntArray m_aryKeyField;//key field 
	
	CCDHttpDatabase* m_pDB;		//数据库
	CArrayRecord m_recordSet;//记录集
	CString m_strTable;
	CMap <LPCTSTR ,LPCTSTR, int , int> m_colMap;//column->column index
	CStringArray m_aryFields;//record field name
	int m_nFields;
	int m_nRecordIndex;
	CHttpFile *m_pHttpFile;
	CString m_strUpdateCondition;
	PBYTE m_pReadBuffer;
	UINT m_nLength;

	CDYHttpClient m_httpClient;

private:
	BOOL m_bOpen;			//是否打开
};

//#endif //#ifdef _USE_HTTP
#endif // !defined(AFX_CDRECORDSET_H__686373DD_107C_469B_A5A6_7338A680FCB0__INCLUDED_)
