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
	void SetWriteLog(BOOL bWriteLog){ m_bWriteLog = bWriteLog; }		//�����Ƿ�д��־
	void SetReportErr(BOOL bReportErr){ m_bReportErr = bReportErr; }	//�����Ƿ񱨸����

	BOOL IsOpen(){ return m_bOpen; }	//���ݿ��Ƿ��
	HRESULT GetErrCode(){ return m_hrError; }	//�õ��������
	CString &GetErrDescription(){ return m_strErrDescription; }	//�õ���������
	CD_DB_TYPE GetDBType();
	//CD_DB_TYPE GetDBType(){ return m_connect.enumType; }
	//CString GetDatasource(){ return m_strDataSource; }
	//CString &GetDatabaseName(){ return m_strDatabase; }

protected:
	BOOL AppendParameter(_variant_t vtParameter, int nSize, BOOL bForInput = TRUE);
	void ClearAllParameters();
	_ConnectionPtr &GetConnection();//{ return m_pConnection; }
	_CommandPtr &GetCommand(){ return m_pCommand; }
	static CString GetValueForUpdate(CString &strValue, VARTYPE vt = VT_BSTR);	//�õ�����Update��Insert���ֶ�ֵ
	static CString GetFieldForUpdate(LPCTSTR lpszField);						//�õ�����Update��Insert���ֶ���
//Operations
public:
	BOOL Open(CDDBCONNECT &connect);
	//BOOL Open(LPCTSTR lpszDataSource, LPCTSTR lpszUID,
	//			 LPCTSTR lpszPWD, LPCTSTR lpszDatabase);	
	BOOL Open();	//�����ݿ�
	BOOL Close();	//�����ݿ�
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
	virtual BOOL ExecuteSQL(CString &strSQL);	//ִ��SQL���
	virtual BOOL ExecuteSQLUncatch(CString &strSQL);
protected:
	virtual void HandleError(_com_error &e, LPCTSTR lpszCmdLine = NULL,
		BOOL bReOpen = TRUE
		);	//�������(Exception)
	virtual BOOL WriteLog(CString &strDatabase, CString &strOperation);	//д��־
	
	//to check if db is still connected
	BOOL IsDBConnected(_ConnectionPtr &connection);
	void SetLogFile(LPCTSTR lpszAppName);
	BOOL ExecEmptySQL();
	
protected:
	//�������ݿ�����
	CDDBCONNECT m_connect;

	_ConnectionPtr m_pConnection;	//ADO����
	_CommandPtr   m_pCommand;
	BOOL m_bOpen;			//�Ƿ��
	BOOL m_bUseODBC;		//�Ƿ�ʹ��ODBC

	BOOL m_bWriteLog;		//�Ƿ�д������־
	CString m_strAppExeName;//Ӧ�ó�����

	BOOL m_bReportErr;				//�Ƿ���MessageBox�������
	HRESULT m_hrError;				//�������
	CString m_strErrDescription;	//������Ϣ
	PFNHANDLEDBERROR m_pfnHandleError;	//д���ݿ���־����ָ��

	ConnectModeEnum m_enumConnectMode;

	CCriticalSection m_ReconnectLock;
	COutPut *m_pOutput;
	BOOL m_bExecEmptySQL;//whether exec empty sql before each sql exec
};

#endif // !defined(AFX_CDDATABASE_H__137B9321_22F6_4B6D_B46D_6ADBE8368A96__INCLUDED_)
