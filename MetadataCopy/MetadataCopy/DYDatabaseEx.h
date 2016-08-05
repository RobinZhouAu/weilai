// DYDatabaseEx.h: interface for the CDYDatabaseEx class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DYDATABASEEX_H__F46EF7B6_29D5_4F50_98AE_80BE414EA8EF__INCLUDED_)
#define AFX_DYDATABASEEX_H__F46EF7B6_29D5_4F50_98AE_80BE414EA8EF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "DBCommonStruct.h"

typedef void (CALLBACK *PFNHANDLEDBERROR)(HRESULT hrErrCode, LPCTSTR lpszDescription);
#define DB_LOG_FILENAME		_T("..\\Log\\dboperate.log")

#define FORMAT_ODT2STRING	_T("%Y-%m-%d %H:%M:%S")


// #define REGISTER_ITEM_DBC_MAMDB			_T("DYMAMDB")
// #define REGISTER_ITEM_DBC_WFDB			_T("DYWFDB")
// #define REGISTER_ITEM_DBC_ARCHIVEDB		_T("DYACDB")
// #define REGISTER_ITEM_DBC_USERVERIFYDB	_T("DYUVDB")
// #define REGISTER_ITEM_DBC_LOGDB			_T("DYLogDB")
// #define REGISTER_ITEM_DBC_TCDB			_T("DYTCDB")
// #define REGISTER_ITEM_DBC_MEDB			_T("DYMEDB")
// #define REGISTER_ITEM_DBC_EVIASDB		_T("EVIASDB")

#define REGISTER_ITEM_DBC_ARCHIVEDB		_T("EVIASDB")
#define REGISTER_ITEM_DBC_EVIASDB		_T("EVIASDB")

//get connection
BOOL DY_EXT_API GetDBConnection(CDDBCONNECT *pConnection,
								LPCTSTR lpszRegItem = REGISTER_ITEM_DBC_EVIASDB//reg item
								);

CString DY_EXT_API EncodeString(LPCTSTR lpszWord);
CString DY_EXT_API DecodeString(LPCTSTR lpszWord);

class DY_EXT_CLASS CDYDatabaseEx  
{
	friend class CDYRecordSetEx;
public:
	CDYDatabaseEx();
	virtual ~CDYDatabaseEx();
//Attribute
public:
	virtual void SetConnectMode(ConnectModeEnum enumConnectMode);
	virtual void SetHandleErrorCallBack(PFNHANDLEDBERROR pfnHandleError);
	virtual void SetWriteLog(BOOL bWriteLog);		//设置是否写日志
	virtual void SetReportErr(BOOL bReportErr);	//设置是否报告错误
	
	virtual BOOL IsOpen();	//数据库是否打开
	virtual HRESULT GetErrCode();	//得到错误代码
	virtual CString &GetErrDescription();	//得到错误描述
	virtual CD_DB_TYPE GetDBType();
	//CString GetDatasource(){ return m_strDataSource; }
	//CString &GetDatabaseName(){ return m_strDatabase; }
	
//protected:
//	virtual BOOL AppendParameter(_variant_t vtParameter, int nSize, BOOL bForInput = TRUE);
//	virtual void ClearAllParameters();
//	virtual _ConnectionPtr &GetConnection();
//	virtual _CommandPtr &GetCommand();
//	virtual CString GetValueForUpdate(CString &strValue, VARTYPE vt = VT_BSTR);	//得到用于Update和Insert的字段值
//	virtual CString GetFieldForUpdate(LPCTSTR lpszField);						//得到用于Update和Insert的字段名
	//Operations
public:
	virtual BOOL Open(CDDBCONNECT &connect);
	//BOOL Open(LPCTSTR lpszDataSource, LPCTSTR lpszUID,
	//			 LPCTSTR lpszPWD, LPCTSTR lpszDatabase);	
	virtual BOOL Open();	//打开数据库
	virtual BOOL Close();	//打开数据库
	virtual BOOL IsHttp();
	virtual void SetTimeOut(LONG lTime);
	virtual BOOL ReadConnectFromFile( LPCTSTR lpszFileName, CDDBCONNECT *pConnect );
	virtual BOOL ReadConnectFromFile( LPCTSTR lpszFileName );
	//protected:
public:
	virtual BOOL BeginTrans();
	virtual BOOL CommitTrans();
	virtual BOOL RollbackTrans();
	// Overrides
public:
	virtual BOOL HasField(LPCTSTR lpszTableName, LPCTSTR lpszFieldName);
	virtual BOOL HasTable(LPCTSTR lpszTableName);
	virtual BOOL HasView(LPCTSTR lpszViewName);
	virtual BOOL ExecStorProc(CString &strProc, CString &strParam);
	virtual BOOL ExecuteSQL(CString &strSQL);	//执行SQL语句
	virtual BOOL ExecuteSQLUncatch(CString &strSQL);

	void AddNWord(CString &strSQL);
	void ADDNWordAfter(CString strAfter, CString &strSQL);
//protected:
//	virtual void HandleError(_com_error &e, LPCTSTR lpszCmdLine = NULL);	//处理错误(Exception)
//	virtual BOOL WriteLog(CString &strDatabase, CString &strOperation);	//写日志};
private:
	CDYDatabaseEx* m_pDatabase;
	BOOL			m_bWriteLog;		//是否写操作日志
	BOOL			m_bReportErr;		//是否用MessageBox报告错误
	
};

#endif // !defined(AFX_DYDATABASEEX_H__F46EF7B6_29D5_4F50_98AE_80BE414EA8EF__INCLUDED_)
