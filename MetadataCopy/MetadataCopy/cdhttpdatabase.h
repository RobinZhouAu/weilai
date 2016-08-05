// CDDatabase.h: interface for the CCDHttpDatabase class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CD_HTTP_DATABASE_H__137B9321_22F6_4B6D_B46D_6ADBE8368A96__INCLUDED_)
#define AFX_CD_HTTP_DATABASE_H__137B9321_22F6_4B6D_B46D_6ADBE8368A96__INCLUDED_

//#ifdef _USE_HTTP
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include <afxdb.h>
#include <comdef.h>
#include <afxTempl.h>
#include <afxinet.h>//mfc internet class

#include "DYDatabaseEx.h"

struct _PROC_PARAM
{
	_variant_t vtParameter;
	int nSize;
	BOOL bForInput;
	_PROC_PARAM()
	{
		bForInput = TRUE;
	};
};
typedef CArray<_PROC_PARAM , _PROC_PARAM&> CArrayParam;

class CCDHttpDatabase : public CDYDatabaseEx
{
	friend class CCDHttpRecordset;
//Construction
public:
	CCDHttpDatabase();
	CCDHttpDatabase(LPCTSTR lpszDataSource, LPCTSTR lpszUID,
				 LPCTSTR lpszPWD, LPCTSTR lpszDatabase);
	virtual ~CCDHttpDatabase();
//Attribute
public:
	void SetHandleErrorCallBack(PFNHANDLEDBERROR pfnHandleError){ m_pfnHandleError = pfnHandleError; }
	void SetWriteLog(BOOL bWriteLog){ m_bWriteLog = bWriteLog; }		//设置是否写日志
	void SetReportErr(BOOL bReportErr){ m_bReportErr = bReportErr; }	//设置是否报告错误

	BOOL IsOpen(){ return m_bOpen; }	//数据库是否打开
	HRESULT GetErrCode(){ return m_hrError; }	//得到错误代码
	CString &GetErrDescription(){ return m_strErrDescription; }	//得到错误描述
	CD_DB_TYPE GetDBType(){ return m_connect.enumType; }
	//CString GetDatasource(){ return m_strDataSource; }
	//CString &GetDatabaseName(){ return m_strDatabase; }
protected:
	BOOL AppendParameter(_variant_t vtParameter, int nSize, BOOL bForInput = TRUE);
	void ClearAllParameters();
	
	CString m_strURLOpenSql;
	CString m_strURLOpenProcedure;
	CString m_strURLUpdate;
	CString m_strURLExecSql;
	

protected:
	static CString GetValueForUpdate(CString &strValue, VARTYPE vt = VT_BSTR);	//得到用于Update和Insert的字段值
	static CString GetFieldForUpdate(LPCTSTR lpszField);						//得到用于Update和Insert的字段名
	static BOOL	GetServerPortFromDataSource(CString &strDataSource, CString &strServer, INTERNET_PORT &nPort);
		
//Operations
public:
	BOOL Open(CDDBCONNECT &connect);
	BOOL Open();	//打开数据库
	BOOL Close();	//打开数据库
	BOOL IsHttp();
public:
	BOOL BeginTrans();
	BOOL CommitTrans();
	BOOL RollbackTrans();
// Overrides
public:
	virtual BOOL ExecStorProc(CString &strProc, CString &strParam);
	virtual BOOL ExecuteSQL(CString &strSQL);	//执行SQL语句
	virtual BOOL ExecuteSQLUncatch(CString &strSQL);
//	//具体没有实现，直接返回TRUE(目前只有资料馆使用)
//	virtual BOOL HasField(LPCTSTR lpszTableName, LPCTSTR lpszFieldName);

	CArrayParam m_aryParam;
protected:
	virtual void HandleError(_com_error &e, LPCTSTR lpszCmdLine = NULL);	//处理错误(Exception)
	virtual void HandleError(LPCTSTR lpszResponse, LPCTSTR lpszCmdLine = NULL);
	virtual BOOL WriteLog(CString &strDatabase, CString &strOperation);	//写日志
	
protected:
	//用于数据库连接
	CDDBCONNECT m_connect;
	CInternetSession *m_pSession;
	CHttpConnection* m_pConnect;
	CString m_strServerIP;
	INTERNET_PORT m_nPort;
	

	BOOL m_bOpen;			//是否打开
	BOOL m_bUseODBC;		//是否使用ODBC

	BOOL m_bWriteLog;		//是否写操作日志
	CString m_strAppExeName;//应用程序名

	BOOL m_bReportErr;				//是否用MessageBox报告错误
	HRESULT m_hrError;				//错误代码
	CString m_strErrDescription;	//错误信息
	PFNHANDLEDBERROR m_pfnHandleError;	//写数据库日志函数指针
};

//#endif//#ifdef _USE_HTTP
#endif // !defined(AFX_CDDATABASE_H__137B9321_22F6_4B6D_B46D_6ADBE8368A96__INCLUDED_)
