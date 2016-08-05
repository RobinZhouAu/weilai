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
	BOOL IsOpen(){ return m_bOpen; }	//���ݿ��Ƿ��

	// ��Ĳ���
	BOOL IsEOF();	//�Ƿ����λ��
	BOOL IsBOF();
	int  GetFieldCount();	//�õ��еĸ���
	int  GetRecordCount();
	void SetDatabase(CDYDatabaseEx *pdb){ m_pDB = (CCDHttpDatabase*)pdb; };	
	CDYDatabaseEx *GetDatabase(){ return m_pDB; }
//Operations
	BOOL AddNew();
	BOOL Update();//��ADO��¼�����µ�ǰ��¼��ͬ��
	//�ò������¼�¼�������иı�������ļ�¼��ֻ���ڸ��ļ�¼��������һ��
	//ADO��¼���ڴӱ༭�����ļ�¼������ʱ���Զ�����Update���¸��ĵļ�¼��
	//��HttpRecordset��Ҫ�������ʽ���øú������и��¡�
	
	BOOL ExecuteSQL(CString &strSQL);
		
	BOOL PutFieldValue(short nIndex, const _variant_t &val);		//�õ��ֶε�����
	BOOL PutFieldValue(LPCTSTR lpszFieldName, const _variant_t &val);
	BOOL GetFieldName(short nIndex, CString &strFieldName);			//�õ��ֶε�����
	
	BOOL GetFieldValue(short nIndex, CString &strValue);			//�õ��ֶε�ֵ
	//BOOL GetFieldValue(short nIndex, _variant_t &val);				//�õ��ֶε�ֵ

	BOOL GetFieldValue(short nIndex, int *pValue);	//�õ�int�ֶε�ֵ
	BOOL GetFieldValue(short nIndex, long *pValue);	//�õ�long�ֶε�ֵ
	BOOL GetFieldValue(short nIndex, CString *pValue);	//�õ�CString�ֶε�ֵ
	BOOL GetFieldValue(short nIndex, double *pValue);	//�õ�Double�ֶε�ֵ
	BOOL GetFieldValue(short nIndex, DWORD *pValue);	//�õ�DWORD�ֶε�ֵ
	BOOL GetFieldValue(short nIndex, COleDateTime *pValue);	//�õ�Date�ֶε�ֵ
	BOOL GetFieldValue(short nIndex, _variant_t *pValue);	//�õ�variant�ֶε�ֵ
	BOOL GetFieldValue(short nIndex, LPVOID pValue);		//Ϊ�˼�����ǰ�ĳ���

//	BOOL GetFieldValue(LPCTSTR lpszFieldName, _variant_t &val);		//�õ��ֶε�ֵ
	BOOL GetFieldValue(LPCTSTR lpszFieldName, CString &strValue);	//�õ��ֶε�ֵ
	
	BOOL GetFieldValue(LPCTSTR lpszFieldName, int *pValue);	//�õ�int�ֶε�ֵ
	BOOL GetFieldValue(LPCTSTR lpszFieldName, long *pValue);	//�õ�long�ֶε�ֵ
	BOOL GetFieldValue(LPCTSTR lpszFieldName, CString *pValue);	//�õ�CString�ֶε�ֵ
	BOOL GetFieldValue(LPCTSTR lpszFieldName, double *pValue);	//�õ�Double�ֶε�ֵ
	BOOL GetFieldValue(LPCTSTR lpszFieldName, DWORD *pValue);	//�õ�DWORD�ֶε�ֵ
	BOOL GetFieldValue(LPCTSTR lpszFieldName, COleDateTime *pValue);	//�õ�Date�ֶε�ֵ
	BOOL GetFieldValue(LPCTSTR lpszFieldName, _variant_t *pValue);	//�õ�variant�ֶε�ֵ
	BOOL GetFieldValue(LPCTSTR lpszFieldName, LPVOID pValue);		//Ϊ�˼�����ǰ�ĳ���
	
	BOOL MoveNext();	//�Ƶ���һ��¼��
	BOOL MoveFirst();	//�Ƶ��׼�¼.
	BOOL Move(long nIndex);	//�Ƶ�ָ����¼
	
	BOOL Open(CString &strSQL, BOOL bUseClient = TRUE, LockTypeEnum LockType = adLockOptimistic,CString &strResponse = CString(_T("")));	//�򿪼�¼��adLockOptimistic
	BOOL OpenByStoredProc(CString &strProcName);
	BOOL Close();	//�رռ�¼��
		
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
	
	//�����ϰ汾
	BOOL GetFieldValue(short nIndex, CString &strValue,BOOL bGetNew, VARTYPE vt = VT_NULL);			//�õ��ֶε�ֵ
	BOOL GetFieldValue(short nIndex, _variant_t &val,BOOL bGetNew);				//�õ��ֶε�ֵ
	
	CString m_strErrorCode;
	
	int m_nKeyFieldNum;
	CUIntArray m_aryFieldType;//field data type
	CUIntArray m_aryKeyField;//key field 
	
	CCDHttpDatabase* m_pDB;		//���ݿ�
	CArrayRecord m_recordSet;//��¼��
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
	BOOL m_bOpen;			//�Ƿ��
};

//#endif //#ifdef _USE_HTTP
#endif // !defined(AFX_CDRECORDSET_H__686373DD_107C_469B_A5A6_7338A680FCB0__INCLUDED_)
