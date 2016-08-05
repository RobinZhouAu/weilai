// DYRecordSetEx.h: interface for the CDYRecordSetEx class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DYRECORDSETEX_H__227810CB_713C_4423_B6E9_C5CDD7391C8F__INCLUDED_)
#define AFX_DYRECORDSETEX_H__227810CB_713C_4423_B6E9_C5CDD7391C8F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "DYDatabaseEx.h"

class DY_EXT_CLASS CDYRecordSetEx  
{
public:
	CDYRecordSetEx(CDYDatabaseEx* pDatabase = NULL);

	virtual ~CDYRecordSetEx();
	//Attribute
public:
	virtual BOOL IsOpen();	//���ݿ��Ƿ��
	
	// ��Ĳ���
	virtual BOOL IsEOF();	//�Ƿ����λ��
	virtual BOOL IsBOF();
	virtual int  GetFieldCount();	//�õ��еĸ���
	virtual int  GetRecordCount();
	virtual void SetDatabase(CDYDatabaseEx *pdb);	
	virtual CDYDatabaseEx *GetDatabase();
	virtual CString GetSQLStatemenet();
	virtual void SetPageSize(int nPageSize);
	virtual int	 GetPageCount();
	//Operations
public:	
	virtual BOOL AddNew();
	virtual BOOL Update();
	
	virtual BOOL PutFieldValue(short nIndex, const _variant_t &val);		//�õ��ֶε�����
	virtual BOOL PutFieldValue(LPCTSTR lpszFieldName, const _variant_t &val);
	
	virtual BOOL GetFieldName(short nIndex, CString &strFieldName);			//�õ��ֶε�����
	/**********����ȡ�ֶεĺ�������___CCDDatabase��CHTTPDatabaseӦ����Ӧ�ĸĶ�***********/
	virtual BOOL GetFieldValue(short nIndex, CString &strValue);			//�õ��ֶε�ֵ
	virtual BOOL GetFieldValue(short nIndex, int *pValue);	//�õ�int�ֶε�ֵ
	virtual BOOL GetFieldValue(short nIndex, long *pValue);	//�õ�long�ֶε�ֵ
	virtual BOOL GetFieldValue(short nIndex, CString *pValue);	//�õ�CString�ֶε�ֵ
	virtual BOOL GetFieldValue(short nIndex, double *pValue);	//�õ�Double�ֶε�ֵ
	virtual BOOL GetFieldValue(short nIndex, DWORD *pValue);	//�õ�DWORD�ֶε�ֵ
	virtual BOOL GetFieldValue(short nIndex, COleDateTime *pValue);	//�õ�Date�ֶε�ֵ
	virtual BOOL GetFieldValue(short nIndex, _variant_t *pValue);	//�õ�variant�ֶε�ֵ
	virtual BOOL GetFieldValue(short nIndex, LPVOID pValue);	//Ϊ�˼�����ǰ�ĳ���ö�����͵�
	
	virtual BOOL GetFieldValue(LPCTSTR lpszFieldName, CString &strValue);	//�õ��ֶε�ֵ
	virtual BOOL GetFieldValue(LPCTSTR lpszFieldName, int *pValue);	//�õ�int�ֶε�ֵ
	virtual BOOL GetFieldValue(LPCTSTR lpszFieldName, long *pValue);	//�õ�long�ֶε�ֵ
	virtual BOOL GetFieldValue(LPCTSTR lpszFieldName, CString *pValue);	//�õ�CString�ֶε�ֵ
	virtual BOOL GetFieldValue(LPCTSTR lpszFieldName, double *pValue);	//�õ�Double�ֶε�ֵ
	virtual BOOL GetFieldValue(LPCTSTR lpszFieldName, DWORD *pValue);	//�õ�DWORD�ֶε�ֵ
	virtual BOOL GetFieldValue(LPCTSTR lpszFieldName, COleDateTime *pValue);	//�õ�Date�ֶε�ֵ
	virtual BOOL GetFieldValue(LPCTSTR lpszFieldName, _variant_t *pValue);	//�õ�variant�ֶε�ֵ
	virtual BOOL GetFieldValue(LPCTSTR lpszFieldName, LPVOID pValue);	//�õ�variant�ֶε�ֵ
	/**************************************************************/

	virtual BOOL MoveNext();	//�Ƶ���һ��¼
	virtual BOOL MoveFirst();	//�Ƶ���¼�� CCDRecordSet��û��ʵ��	
	virtual BOOL Move(long nIndex);	//�Ƶ�ָ����¼	
	
	virtual BOOL Open(CString &strSQL, BOOL bUseClient = TRUE,
		LockTypeEnum LockType = adLockOptimistic, CString &strResponse = CString(_T(""))/*����HTTP*/);	//�򿪼�¼��
	virtual BOOL Open(CString &strSQL, CursorLocationEnum CursorLocation,
		CursorTypeEnum CursorType = adOpenKeyset, LockTypeEnum LockType = adLockOptimistic,
		CommandTypeEnum CommaonType = adCmdUnspecified);	//�򿪼�¼��
	virtual BOOL OpenByStoredProc(CString &strProcName);
	virtual BOOL Close();	//�رռ�¼��
	virtual BOOL OpenSchema(SchemaEnum QueryType);
	virtual void SetAbsolutePage(int nPage);

	//���ú�����
	static  BOOL Variant2Memory(BYTE *pBuf, DWORD dwBufSize, _variant_t &var);
	static  BOOL Memory2Variant(BYTE *pBuf, DWORD dwBufSize, _variant_t &var);
	static  HBITMAP DIBVariant2Bmp(int nBmpWidth, int nBmpHeight, _variant_t &var);
	static  BOOL Bmp2DIBVariant(HBITMAP hBitmap, _variant_t &var);
	static  BOOL Bitmap2Variant(HBITMAP hBitmap, _variant_t &var);
	static  HBITMAP Variant2Bitmap(_variant_t &var);
	static  BOOL Variant2MemFile(CMemFile &file, _variant_t &var);
	static  BOOL MemFile2Variant(CMemFile &file, _variant_t &var);
	
	static  BOOL Var2String(_variant_t &val,CString &strValue);
	
	virtual BOOL OpenByPage(CString &strSQL, int nCurPage,//0 based
		int nItemsPerPage,
		BOOL bUseClient = TRUE, LockTypeEnum LockType = adLockOptimistic, CString &strResponse = CString(_T("")) );	//�򿪼�¼��
	
	virtual BOOL OpenByPage(CString &strSQL, int nCurPage,//0 based
		int nItemsPerPage,
		CursorLocationEnum CursorLocation,
		CursorTypeEnum CursorType = adOpenKeyset, LockTypeEnum LockType = adLockOptimistic,
		CommandTypeEnum CommaonType = adCmdUnspecified);	//�򿪼�¼��
	
protected:
	CDYDatabaseEx* m_pDBEx;			//���ݿ�
	CDYRecordSetEx* m_pRecordSet;	//�����
};
//#endif	//_USE_CDDATABASE_

#endif // !defined(AFX_DYRECORDSETEX_H__227810CB_713C_4423_B6E9_C5CDD7391C8F__INCLUDED_)
