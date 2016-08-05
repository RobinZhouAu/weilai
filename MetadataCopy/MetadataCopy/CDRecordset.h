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
	BOOL IsOpen(){ return m_bOpen; }	//���ݿ��Ƿ��

	// ��Ĳ���
	BOOL IsEOF();	//�Ƿ����λ��
	BOOL IsBOF();
	int  GetFieldCount();	//�õ��еĸ���
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

	BOOL PutFieldValue(short nIndex, const _variant_t &val);		//�õ��ֶε�����
	BOOL PutFieldValue(LPCTSTR lpszFieldName, const _variant_t &val);

	BOOL GetFieldName(short nIndex, CString &strFieldName);			//�õ��ֶε�����

	BOOL GetFieldValue(short nIndex, CString &strValue);			//�õ��ֶε�ֵ
	
	BOOL GetFieldValue(short nIndex, int *pValue);	//�õ�int�ֶε�ֵ
	BOOL GetFieldValue(short nIndex, long *pValue);	//�õ�long�ֶε�ֵ
	BOOL GetFieldValue(short nIndex, CString *pValue);	//�õ�CString�ֶε�ֵ
	BOOL GetFieldValue(short nIndex, double *pValue);	//�õ�Double�ֶε�ֵ
	BOOL GetFieldValue(short nIndex, DWORD *pValue);	//�õ�DWORD�ֶε�ֵ
	BOOL GetFieldValue(short nIndex, COleDateTime *pValue);	//�õ�Date�ֶε�ֵ
	BOOL GetFieldValue(short nIndex, _variant_t *pValue);	//�õ�variant�ֶε�ֵ
	BOOL GetFieldValue(short nIndex, LPVOID lpValue);	//Ϊ�˼�����ǰ�ĳ���ö�����͵�
	
	BOOL GetFieldValue(LPCTSTR lpszFieldName, CString &strValue);	//�õ��ֶε�ֵ
	
	BOOL GetFieldValue(LPCTSTR lpszFieldName, int *pValue);	//�õ�int�ֶε�ֵ
	BOOL GetFieldValue(LPCTSTR lpszFieldName, long *pValue);	//�õ�long�ֶε�ֵ
	BOOL GetFieldValue(LPCTSTR lpszFieldName, CString *pValue);	//�õ�CString�ֶε�ֵ
	BOOL GetFieldValue(LPCTSTR lpszFieldName, double *pValue);	//�õ�Double�ֶε�ֵ
	BOOL GetFieldValue(LPCTSTR lpszFieldName, DWORD *pValue);	//�õ�DWORD�ֶε�ֵ
	BOOL GetFieldValue(LPCTSTR lpszFieldName, COleDateTime *pValue);	//�õ�Date�ֶε�ֵ
	BOOL GetFieldValue(LPCTSTR lpszFieldName, _variant_t *pValue);	//�õ�variant�ֶε�ֵ
	BOOL GetFieldValue(LPCTSTR lpszFieldName, LPVOID lpValue);	//Ϊ�˼�����ǰ�ĳ���ö�����͵�
	
	BOOL MoveFirst();	//�ƶ�����һ����¼
	BOOL MoveNext();	//�Ƶ���һ��¼
	BOOL Move(long nMoveTo);//�Ƶ�ָ����¼
	
	BOOL Open(CString &strSQL, BOOL bUseClient = TRUE, LockTypeEnum LockType = adLockOptimistic, CString &strResponse = CString(_T("")) );	//�򿪼�¼��
	BOOL Open(CString &strSQL, CursorLocationEnum CursorLocation,
		CursorTypeEnum CursorType = adOpenKeyset, LockTypeEnum LockType = adLockOptimistic,
		CommandTypeEnum CommaonType = adCmdUnspecified);	//�򿪼�¼��
	BOOL OpenByStoredProc(CString &strProcName);
	BOOL Close();	//�رռ�¼��
	BOOL OpenSchema(SchemaEnum QueryType);
	void SetAbsolutePage(int nPage);
	BOOL IsEmptyString(const _variant_t &varSrc);
protected:
	CCDDatabase* m_pDB;				//���ݿ�
	_RecordsetPtr m_pRecordSet;		//��¼��
private:
	BOOL m_bOpen;			//�Ƿ��
	int m_nUpdateField;
};

#endif // !defined(AFX_CDRECORDSET_H__686373DD_107C_469B_A5A6_7338A680FCB0__INCLUDED_)
