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
	virtual BOOL IsOpen();	//数据库是否打开
	
	// 表的操作
	virtual BOOL IsEOF();	//是否结束位置
	virtual BOOL IsBOF();
	virtual int  GetFieldCount();	//得到列的个数
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
	
	virtual BOOL PutFieldValue(short nIndex, const _variant_t &val);		//得到字段的名字
	virtual BOOL PutFieldValue(LPCTSTR lpszFieldName, const _variant_t &val);
	
	virtual BOOL GetFieldName(short nIndex, CString &strFieldName);			//得到字段的名字
	/**********更改取字段的函数如下___CCDDatabase和CHTTPDatabase应该相应的改动***********/
	virtual BOOL GetFieldValue(short nIndex, CString &strValue);			//得到字段的值
	virtual BOOL GetFieldValue(short nIndex, int *pValue);	//得到int字段的值
	virtual BOOL GetFieldValue(short nIndex, long *pValue);	//得到long字段的值
	virtual BOOL GetFieldValue(short nIndex, CString *pValue);	//得到CString字段的值
	virtual BOOL GetFieldValue(short nIndex, double *pValue);	//得到Double字段的值
	virtual BOOL GetFieldValue(short nIndex, DWORD *pValue);	//得到DWORD字段的值
	virtual BOOL GetFieldValue(short nIndex, COleDateTime *pValue);	//得到Date字段的值
	virtual BOOL GetFieldValue(short nIndex, _variant_t *pValue);	//得到variant字段的值
	virtual BOOL GetFieldValue(short nIndex, LPVOID pValue);	//为了兼容以前的程序，枚举类型等
	
	virtual BOOL GetFieldValue(LPCTSTR lpszFieldName, CString &strValue);	//得到字段的值
	virtual BOOL GetFieldValue(LPCTSTR lpszFieldName, int *pValue);	//得到int字段的值
	virtual BOOL GetFieldValue(LPCTSTR lpszFieldName, long *pValue);	//得到long字段的值
	virtual BOOL GetFieldValue(LPCTSTR lpszFieldName, CString *pValue);	//得到CString字段的值
	virtual BOOL GetFieldValue(LPCTSTR lpszFieldName, double *pValue);	//得到Double字段的值
	virtual BOOL GetFieldValue(LPCTSTR lpszFieldName, DWORD *pValue);	//得到DWORD字段的值
	virtual BOOL GetFieldValue(LPCTSTR lpszFieldName, COleDateTime *pValue);	//得到Date字段的值
	virtual BOOL GetFieldValue(LPCTSTR lpszFieldName, _variant_t *pValue);	//得到variant字段的值
	virtual BOOL GetFieldValue(LPCTSTR lpszFieldName, LPVOID pValue);	//得到variant字段的值
	/**************************************************************/

	virtual BOOL MoveNext();	//移到下一记录
	virtual BOOL MoveFirst();	//移到记录首 CCDRecordSet还没有实现	
	virtual BOOL Move(long nIndex);	//移到指定记录	
	
	virtual BOOL Open(CString &strSQL, BOOL bUseClient = TRUE,
		LockTypeEnum LockType = adLockOptimistic, CString &strResponse = CString(_T(""))/*兼容HTTP*/);	//打开记录集
	virtual BOOL Open(CString &strSQL, CursorLocationEnum CursorLocation,
		CursorTypeEnum CursorType = adOpenKeyset, LockTypeEnum LockType = adLockOptimistic,
		CommandTypeEnum CommaonType = adCmdUnspecified);	//打开记录集
	virtual BOOL OpenByStoredProc(CString &strProcName);
	virtual BOOL Close();	//关闭记录集
	virtual BOOL OpenSchema(SchemaEnum QueryType);
	virtual void SetAbsolutePage(int nPage);

	//公用函数：
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
		BOOL bUseClient = TRUE, LockTypeEnum LockType = adLockOptimistic, CString &strResponse = CString(_T("")) );	//打开记录集
	
	virtual BOOL OpenByPage(CString &strSQL, int nCurPage,//0 based
		int nItemsPerPage,
		CursorLocationEnum CursorLocation,
		CursorTypeEnum CursorType = adOpenKeyset, LockTypeEnum LockType = adLockOptimistic,
		CommandTypeEnum CommaonType = adCmdUnspecified);	//打开记录集
	
protected:
	CDYDatabaseEx* m_pDBEx;			//数据库
	CDYRecordSetEx* m_pRecordSet;	//结果集
};
//#endif	//_USE_CDDATABASE_

#endif // !defined(AFX_DYRECORDSETEX_H__227810CB_713C_4423_B6E9_C5CDD7391C8F__INCLUDED_)
