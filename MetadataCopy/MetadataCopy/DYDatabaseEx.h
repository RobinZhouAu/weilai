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
	virtual void SetWriteLog(BOOL bWriteLog);		//�����Ƿ�д��־
	virtual void SetReportErr(BOOL bReportErr);	//�����Ƿ񱨸����
	
	virtual BOOL IsOpen();	//���ݿ��Ƿ��
	virtual HRESULT GetErrCode();	//�õ��������
	virtual CString &GetErrDescription();	//�õ���������
	virtual CD_DB_TYPE GetDBType();
	//CString GetDatasource(){ return m_strDataSource; }
	//CString &GetDatabaseName(){ return m_strDatabase; }
	
//protected:
//	virtual BOOL AppendParameter(_variant_t vtParameter, int nSize, BOOL bForInput = TRUE);
//	virtual void ClearAllParameters();
//	virtual _ConnectionPtr &GetConnection();
//	virtual _CommandPtr &GetCommand();
//	virtual CString GetValueForUpdate(CString &strValue, VARTYPE vt = VT_BSTR);	//�õ�����Update��Insert���ֶ�ֵ
//	virtual CString GetFieldForUpdate(LPCTSTR lpszField);						//�õ�����Update��Insert���ֶ���
	//Operations
public:
	virtual BOOL Open(CDDBCONNECT &connect);
	//BOOL Open(LPCTSTR lpszDataSource, LPCTSTR lpszUID,
	//			 LPCTSTR lpszPWD, LPCTSTR lpszDatabase);	
	virtual BOOL Open();	//�����ݿ�
	virtual BOOL Close();	//�����ݿ�
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
	virtual BOOL ExecuteSQL(CString &strSQL);	//ִ��SQL���
	virtual BOOL ExecuteSQLUncatch(CString &strSQL);

	void AddNWord(CString &strSQL);
	void ADDNWordAfter(CString strAfter, CString &strSQL);
//protected:
//	virtual void HandleError(_com_error &e, LPCTSTR lpszCmdLine = NULL);	//�������(Exception)
//	virtual BOOL WriteLog(CString &strDatabase, CString &strOperation);	//д��־};
private:
	CDYDatabaseEx* m_pDatabase;
	BOOL			m_bWriteLog;		//�Ƿ�д������־
	BOOL			m_bReportErr;		//�Ƿ���MessageBox�������
	
};

#endif // !defined(AFX_DYDATABASEEX_H__F46EF7B6_29D5_4F50_98AE_80BE414EA8EF__INCLUDED_)
