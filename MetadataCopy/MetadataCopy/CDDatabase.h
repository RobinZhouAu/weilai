// CDDatabase.h: interface for the CCDDatabase class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CDDATABASE_H__137B9321_22F6_4B6D_B46D_6ADBE8368A96__INCLUDED_)
#define AFX_CDDATABASE_H__137B9321_22F6_4B6D_B46D_6ADBE8368A96__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include <afxdb.h>
#include "comdef.h"		//Use for ADO
#include "DYDatabaseEx.h"
class COutPut;

class CCDDatabase : public CDYDatabaseEx  
{
	friend class CCDRecordset;
//Construction
public:
	CCDDatabase();
	CCDDatabase(LPCTSTR lpszDataSource, LPCTSTR lpszUID,
				 LPCTSTR lpszPWD, LPCTSTR lpszDatabase);
	virtual ~CCDDatabase();
//Attribute
public:
	void SetConnectMode(ConnectModeEnum enumConnectMode){ m_enumConnectMode = enumConnectMode; }
	void SetHandleErrorCallBack(PFNHANDLEDBERROR pfnHandleError){ m_pfnHandleError = pfnHandleError; }
	void SetWriteLog(BOOL bWriteLog){ m_bWriteLog = bWriteLog; }		//设置是否写日志
	void SetReportErr(BOOL bReportErr){ m_bReportErr = bReportErr; }	//设置是否报告错误

	BOOL IsOpen(){ return m_bOpen; }	//数据库是否打开
	HRESULT GetErrCode(){ return m_hrError; }	//得到错误代码
	CString &GetErrDescription(){ return m_strErrDescription; }	//得到错误描述
	CD_DB_TYPE GetDBType();
	//CD_DB_TYPE GetDBType(){ return m_connect.enumType; }
	//CString GetDatasource(){ return m_strDataSource; }
	//CString &GetDatabaseName(){ return m_strDatabase; }

protected:
	BOOL AppendParameter(_variant_t vtParameter, int nSize, BOOL bForInput = TRUE);
	void ClearAllParameters();
	_ConnectionPtr &GetConnection();//{ return m_pConnection; }
	_CommandPtr &GetCommand(){ return m_pCommand; }
	static CString GetValueForUpdate(CString &strValue, VARTYPE vt = VT_BSTR);	//得到用于Update和Insert的字段值
	static CString GetFieldForUpdate(LPCTSTR lpszField);						//得到用于Update和Insert的字段名
//Operations
public:
	BOOL Open(CDDBCONNECT &connect);
	//BOOL Open(LPCTSTR lpszDataSource, LPCTSTR lpszUID,
	//			 LPCTSTR lpszPWD, LPCTSTR lpszDatabase);	
	BOOL Open();	//打开数据库
	BOOL Close();	//打开数据库
	BOOL IsHttp();
	void SetTimeOut(LONG lTime) { m_pConnection->put_CommandTimeout(lTime); }
	BOOL ReadConnectFromFile( LPCTSTR lpszFileName, CDDBCONNECT *pConnect );
	BOOL ReadConnectFromFile( LPCTSTR lpszFileName );
//protected:
public:
	BOOL BeginTrans();
	BOOL CommitTrans();
	BOOL RollbackTrans();
// Overrides
public:
//	virtual BOOL HasField(LPCTSTR lpszTableName, LPCTSTR lpszFieldName);
//	virtual BOOL HasTable(LPCTSTR lpszTableName);
//	virtual BOOL HasView(LPCTSTR lpszViewName);
	virtual BOOL ExecStorProc(CString &strProc, CString &strParam);
	//virtual BOOL ExecuteSQL(CString &strSQL, int &nRecordsAffected);
	virtual BOOL ExecuteSQL(CString &strSQL);	//执行SQL语句
	virtual BOOL ExecuteSQLUncatch(CString &strSQL);
protected:
	virtual void HandleError(_com_error &e, LPCTSTR lpszCmdLine = NULL,
		BOOL bReOpen = TRUE
		);	//处理错误(Exception)
	virtual BOOL WriteLog(CString &strDatabase, CString &strOperation);	//写日志
	
	//to check if db is still connected
	BOOL IsDBConnected(_ConnectionPtr &connection);
	void SetLogFile(LPCTSTR lpszAppName);
	BOOL ExecEmptySQL();
	
protected:
	//用于数据库连接
	CDDBCONNECT m_connect;

	_ConnectionPtr m_pConnection;	//ADO连接
	_CommandPtr   m_pCommand;
	BOOL m_bOpen;			//是否打开
	BOOL m_bUseODBC;		//是否使用ODBC

	BOOL m_bWriteLog;		//是否写操作日志
	CString m_strAppExeName;//应用程序名

	BOOL m_bReportErr;				//是否用MessageBox报告错误
	HRESULT m_hrError;				//错误代码
	CString m_strErrDescription;	//错误信息
	PFNHANDLEDBERROR m_pfnHandleError;	//写数据库日志函数指针

	ConnectModeEnum m_enumConnectMode;

	CCriticalSection m_ReconnectLock;
	COutPut *m_pOutput;
	BOOL m_bExecEmptySQL;//whether exec empty sql before each sql exec
};

#endif // !defined(AFX_CDDATABASE_H__137B9321_22F6_4B6D_B46D_6ADBE8368A96__INCLUDED_)
