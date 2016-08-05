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
	void SetWriteLog(BOOL bWriteLog){ m_bWriteLog = bWriteLog; }		//�����Ƿ�д��־
	void SetReportErr(BOOL bReportErr){ m_bReportErr = bReportErr; }	//�����Ƿ񱨸����

	BOOL IsOpen(){ return m_bOpen; }	//���ݿ��Ƿ��
	HRESULT GetErrCode(){ return m_hrError; }	//�õ��������
	CString &GetErrDescription(){ return m_strErrDescription; }	//�õ���������
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
	static CString GetValueForUpdate(CString &strValue, VARTYPE vt = VT_BSTR);	//�õ�����Update��Insert���ֶ�ֵ
	static CString GetFieldForUpdate(LPCTSTR lpszField);						//�õ�����Update��Insert���ֶ���
	static BOOL	GetServerPortFromDataSource(CString &strDataSource, CString &strServer, INTERNET_PORT &nPort);
		
//Operations
public:
	BOOL Open(CDDBCONNECT &connect);
	BOOL Open();	//�����ݿ�
	BOOL Close();	//�����ݿ�
	BOOL IsHttp();
public:
	BOOL BeginTrans();
	BOOL CommitTrans();
	BOOL RollbackTrans();
// Overrides
public:
	virtual BOOL ExecStorProc(CString &strProc, CString &strParam);
	virtual BOOL ExecuteSQL(CString &strSQL);	//ִ��SQL���
	virtual BOOL ExecuteSQLUncatch(CString &strSQL);
//	//����û��ʵ�֣�ֱ�ӷ���TRUE(Ŀǰֻ�����Ϲ�ʹ��)
//	virtual BOOL HasField(LPCTSTR lpszTableName, LPCTSTR lpszFieldName);

	CArrayParam m_aryParam;
protected:
	virtual void HandleError(_com_error &e, LPCTSTR lpszCmdLine = NULL);	//�������(Exception)
	virtual void HandleError(LPCTSTR lpszResponse, LPCTSTR lpszCmdLine = NULL);
	virtual BOOL WriteLog(CString &strDatabase, CString &strOperation);	//д��־
	
protected:
	//�������ݿ�����
	CDDBCONNECT m_connect;
	CInternetSession *m_pSession;
	CHttpConnection* m_pConnect;
	CString m_strServerIP;
	INTERNET_PORT m_nPort;
	

	BOOL m_bOpen;			//�Ƿ��
	BOOL m_bUseODBC;		//�Ƿ�ʹ��ODBC

	BOOL m_bWriteLog;		//�Ƿ�д������־
	CString m_strAppExeName;//Ӧ�ó�����

	BOOL m_bReportErr;				//�Ƿ���MessageBox�������
	HRESULT m_hrError;				//�������
	CString m_strErrDescription;	//������Ϣ
	PFNHANDLEDBERROR m_pfnHandleError;	//д���ݿ���־����ָ��
};

//#endif//#ifdef _USE_HTTP
#endif // !defined(AFX_CDDATABASE_H__137B9321_22F6_4B6D_B46D_6ADBE8368A96__INCLUDED_)
