// DYDatabaseEx.cpp: implementation of the CDYDatabaseEx class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "DYDatabaseEx.h"
#include "CDHttpDatabase.h"
#include "CDDatabase.h"

#include "DYRecordSetEx.h"
#include "register.h"

#include "AutoLockUnlock.h"


BOOL g_bSQLTrace = FALSE;//是否把执行的SQL语句输出到DebugView

CCriticalSection g_DYRecordSetExLock;



#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//#ifndef _USE_CDDATABASE_
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDYDatabaseEx::CDYDatabaseEx()
{
	m_pDatabase = NULL;
	m_bReportErr = FALSE;
	m_bWriteLog = FALSE;
	CRegister reg;
	DWORD dwData = 0;
	if(reg.OpenKey(HKEY_LOCAL_MACHINE, DY_MAM_REGISTER_PARENT))
	{
		if(reg.GetDWord(_T("SQLTrace"), dwData))
		{
			g_bSQLTrace = dwData;
		}
	}
	reg.CloseKey();
	
}

CDYDatabaseEx::~CDYDatabaseEx()
{
	if (m_pDatabase != NULL) //表明是释放类对象而不是m_pDatabase指针
	{
		m_pDatabase->Close();
		delete m_pDatabase;
	}
}
void CDYDatabaseEx::SetConnectMode(ConnectModeEnum enumConnectMode)
{
	if (m_pDatabase == NULL)
		return ;	
	m_pDatabase->SetConnectMode(enumConnectMode);
}
void CDYDatabaseEx::SetHandleErrorCallBack(PFNHANDLEDBERROR pfnHandleError)
{
	if (m_pDatabase == NULL)
		return ;
	m_pDatabase->SetHandleErrorCallBack(pfnHandleError);
}
void CDYDatabaseEx::SetWriteLog(BOOL bWriteLog)
{
	if (m_pDatabase != NULL)
		m_pDatabase->SetWriteLog(bWriteLog);
	m_bWriteLog = bWriteLog;
}
void CDYDatabaseEx::SetReportErr(BOOL bReportErr)
{
	if (m_pDatabase != NULL)
		m_pDatabase->SetReportErr(bReportErr);
	m_bReportErr = bReportErr;
}

BOOL CDYDatabaseEx::IsOpen()
{
	if (m_pDatabase == NULL)
		return FALSE;
	return m_pDatabase->IsOpen();
}
HRESULT CDYDatabaseEx::GetErrCode()
{
	if(m_pDatabase == NULL)
		return 0;	
	return m_pDatabase->GetErrCode();
}
CString g_strError;
CString& CDYDatabaseEx::GetErrDescription()
{
	if(m_pDatabase == NULL)
		return g_strError;	
	return m_pDatabase->GetErrDescription();
}
CD_DB_TYPE CDYDatabaseEx::GetDBType()
{
	if (m_pDatabase == NULL)
		return CDDT_UNKNOW;
	return m_pDatabase->GetDBType();
}

BOOL CDYDatabaseEx::Open(CDDBCONNECT &connect)
{
	if (m_pDatabase != NULL)
	{
		delete m_pDatabase;	//清除以前的数据库连接
	}
	if (connect.enumType == CDDT_ORACLEHTTP)
		m_pDatabase = new CCDHttpDatabase;
	else
		m_pDatabase = new CCDDatabase;
	m_pDatabase->SetWriteLog(m_bWriteLog);
	m_pDatabase->SetReportErr(m_bReportErr);
	return m_pDatabase->Open(connect);
}
BOOL CDYDatabaseEx::Open()
{
	if (m_pDatabase != NULL)//这里如果http
		return m_pDatabase->Open();
	else 
		return FALSE;
}
BOOL CDYDatabaseEx::Close()
{
	if (m_pDatabase == NULL)
		return FALSE;
	if(!m_pDatabase->Close())
		return FALSE;
	deleteC(m_pDatabase);
	return TRUE;
}
void CDYDatabaseEx::SetTimeOut(LONG lTime)
{
	if(m_pDatabase == NULL)
		return;
	
	m_pDatabase->SetTimeOut(lTime);
}
BOOL CDYDatabaseEx::ReadConnectFromFile( LPCTSTR lpszFileName, CDDBCONNECT *pConnect )
{
	if (m_pDatabase == NULL)
		m_pDatabase = new CCDDatabase;//只有CCDDatabase里有这个函数
	return m_pDatabase->ReadConnectFromFile(lpszFileName, pConnect);
}
BOOL CDYDatabaseEx::ReadConnectFromFile( LPCTSTR lpszFileName )
{
	if (m_pDatabase == NULL)
		m_pDatabase = new CCDDatabase;	
	return m_pDatabase->ReadConnectFromFile(lpszFileName);
}
BOOL CDYDatabaseEx::BeginTrans()
{
	_AUTO_LOCK lock(&g_DYRecordSetExLock);

	if (m_pDatabase == NULL)
		return FALSE;
	return m_pDatabase->BeginTrans();
}
BOOL CDYDatabaseEx::CommitTrans()
{
	_AUTO_LOCK lock(&g_DYRecordSetExLock);

	if (m_pDatabase == NULL)
		return FALSE;
	return m_pDatabase->CommitTrans();
}
BOOL CDYDatabaseEx::RollbackTrans()
{
	_AUTO_LOCK lock(&g_DYRecordSetExLock);

	if(m_pDatabase == NULL)
		return FALSE;
	
	return m_pDatabase->RollbackTrans();
}
BOOL CDYDatabaseEx::HasField(LPCTSTR lpszTableName, LPCTSTR lpszFieldName)
{
	_AUTO_LOCK lock(&g_DYRecordSetExLock);

	if(m_pDatabase == NULL)
		return FALSE;

	//用查询sysobjects的方法判断,只实现了SQLServer的方法
	CString strSQL;
	switch(GetDBType())
	{
	case CDDT_SQLSERVER:
		strSQL.Format(_T("SELECT * FROM syscolumns a INNER JOIN sysobjects b ON a.id = b.id WHERE (b.name = '%s' and a.name = '%s')"), lpszTableName, lpszFieldName);
		break;
	case CDDT_DB2:
		strSQL.Format(_T("SELECT * FROM SYSIBM.SYSCOLUMNS WHERE TBNAME='%s' and NAME = '%s'"), lpszTableName, lpszFieldName);
		break;
	case CDDT_ORACLE:
	case CDDT_ORACLEHTTP:
		strSQL.Format(_T("SELECT * FROM USER_COL_COMMENTS WHERE TABLE_NAME = '%s' and COLUMN_NAME = '%s'"), lpszTableName, lpszFieldName);
		break;
	default:
		return FALSE;
	}
	CDYRecordSetEx rs(this);
	if(!rs.Open(strSQL))
		return FALSE;
	return !rs.IsEOF();
}
BOOL CDYDatabaseEx::HasTable(LPCTSTR lpszTableName)
{
	_AUTO_LOCK lock(&g_DYRecordSetExLock);

	if(m_pDatabase == NULL)
		return FALSE;
	CString strSQL;
	switch(GetDBType())
	{
	case CDDT_SQLSERVER:
		strSQL.Format(_T("SELECT * FROM sysobjects where xtype = 'U' AND name = '%s'"), lpszTableName);
		break;
	case CDDT_DB2:
		strSQL.Format(_T("SELECT * FROM SYSIBM.SYSTABLES where TYPE = 'T' AND NAME = '%s'"), lpszTableName);
		break;
	case CDDT_ORACLE:
	case CDDT_ORACLEHTTP:				
		strSQL.Format(_T("SELECT * FROM USER_CATALOG WHERE Table_Type = 'TABLE' AND Table_Name='%s'"), lpszTableName);
		break;
	default:
		return FALSE;
	}

	CDYRecordSetEx rs(this);
	if(!rs.Open(strSQL))
		return FALSE;
	return !rs.IsEOF();
}
BOOL CDYDatabaseEx::HasView(LPCTSTR lpszViewName)
{
	_AUTO_LOCK lock(&g_DYRecordSetExLock);

	if(m_pDatabase == NULL)
		return FALSE;
	CString strSQL;
	switch(GetDBType())
	{
	case CDDT_SQLSERVER:
		strSQL.Format(_T("SELECT * FROM sysobjects where xtype = 'V' AND name = '%s'"), lpszViewName);
		break;
	case CDDT_DB2:
		strSQL.Format(_T("SELECT * FROM SYSIBM.SYSTABLES where TYPE = 'V' AND NAME = '%s'"), lpszViewName);
		break;
	case CDDT_ORACLE:
	case CDDT_ORACLEHTTP:		
		strSQL.Format(_T("SELECT * FROM USER_CATALOG WHERE Table_Type = 'VIEW' AND Table_Name='%s'"), lpszViewName);
		break;
	default:
		return FALSE;
	}
	
	CDYRecordSetEx rs(this);
	if(!rs.Open(strSQL))
		return FALSE;
	return !rs.IsEOF();
}
BOOL CDYDatabaseEx::ExecStorProc(CString &strProc, CString &strParam)
{
	_AUTO_LOCK lock(&g_DYRecordSetExLock);

	if(m_pDatabase == NULL)
		return FALSE;
	CString strOutput;
	CString strTime;
	COleDateTime odt;
	if(g_bSQLTrace)
	{
		strTime = COleDateTime::GetCurrentTime().Format(FORMAT_ODT2STRING);
		strOutput.Format(_T("Exec Proc...[%s],[%s]\n"), strProc, strTime);
		OutputDebugString(strOutput);	
	}
	
	BOOL bOK = m_pDatabase->ExecStorProc(strProc, strParam);
	if(g_bSQLTrace)
	{
		strTime = COleDateTime::GetCurrentTime().Format(FORMAT_ODT2STRING);
		strOutput.Format(_T("Exec Proc End,[%s]\n"), strTime);
		OutputDebugString(strOutput);	
	}
	return bOK;
}
BOOL CDYDatabaseEx::ExecuteSQL(CString &strSQL)
{
	_AUTO_LOCK lock(&g_DYRecordSetExLock);

	if(m_pDatabase == NULL)
		return FALSE;

	CString strOutput;
	CString strTime;
	COleDateTime odt;
	if(g_bSQLTrace)
	{
		strTime = COleDateTime::GetCurrentTime().Format(FORMAT_ODT2STRING);
		strOutput.Format(_T("Exec SQL...[%s],[%s]\n"), strSQL, strTime);
		OutputDebugString(strOutput);	
	}

	//临时增加，处理泰文检索
	//AddNWord(strSQL);
	
	DWORD dwStart = ::GetTickCount();

	BOOL bOK = m_pDatabase->ExecuteSQL(strSQL);
	
	DWORD dwUsed = ::GetTickCount() - dwStart;

	if(g_bSQLTrace)
	{
		strTime = COleDateTime::GetCurrentTime().Format(FORMAT_ODT2STRING);
		strOutput.Format(_T("Exec SQL End,[%s]. Executetime used: [%d] milliseconds.\n"), strTime, dwUsed);
		OutputDebugString(strOutput);	
	}

	return bOK;
}
BOOL CDYDatabaseEx::ExecuteSQLUncatch(CString &strSQL)
{
	_AUTO_LOCK lock(&g_DYRecordSetExLock);

	if(m_pDatabase == NULL)
		return FALSE;
	CString strOutput;
	CString strTime;
	COleDateTime odt;
	if(g_bSQLTrace)
	{
		strTime = COleDateTime::GetCurrentTime().Format(FORMAT_ODT2STRING);
		strOutput.Format(_T("Exec SQL...[%s],[%s]\n"), strSQL, strTime);
		OutputDebugString(strOutput);	
	}
	
	DWORD dwStart = ::GetTickCount();

	BOOL bOK = m_pDatabase->ExecuteSQLUncatch(strSQL);

	DWORD dwUsed = ::GetTickCount() - dwStart;
	
	if(g_bSQLTrace)
	{
		strTime = COleDateTime::GetCurrentTime().Format(FORMAT_ODT2STRING);
		strOutput.Format(_T("Exec SQL End,[%s]. Executetime used: [%d] milliseconds.\n"), strTime, dwUsed);
		OutputDebugString(strOutput);	
	}
	return bOK;
}
BOOL CDYDatabaseEx::IsHttp()
{
	if(m_pDatabase == NULL)
		return FALSE;
	return m_pDatabase->IsHttp();
}
//#endif _USE_CDDATABASE_

//在等于或LIKE的字符串前增加N标识，把该字符串转为UNICODE的形式，用于泰国MCOL项目
void CDYDatabaseEx::AddNWord(CString &strSQL)
{

/*
简单处理方式，在第奇数个单引号前增加N标识
	int nIndex = 0;
	int nCount = 0;
	CString strTemp;
	
	while(nIndex >= 0)
	{
		nIndex = strSQL.Find(_T("'"), nIndex);
		if(nIndex > 0)
		{
			if(++nCount % 2 == 1)
			{
				strTemp = strSQL.Left(nIndex) + _T("N") + strSQL.Right(strSQL.GetLength() - nIndex);
				strSQL = strTemp;
				nIndex++;
				OutputDebugString(strSQL);
			}
			
			nIndex++;
		}
	}
*/

	//查找等号或LIKE后的第一个非空字符，如果为单引号，则在前面增加N标识
	ADDNWordAfter(_T("="), strSQL);
	ADDNWordAfter(_T("like"), strSQL);
	ADDNWordAfter(_T("LIKE"), strSQL);
}

void CDYDatabaseEx::ADDNWordAfter(CString strAfter, CString &strSQL)
{
	int nIndex = 0;
	CString strChar;
	CString strTemp;
	
	while(nIndex >= 0)
	{
		nIndex = strSQL.Find(strAfter, nIndex);
		strChar = _T(" ");

		if(nIndex >= 0)
		{
			nIndex += strAfter.GetLength();
			while(strChar == _T(" "))
			{
				strChar = strSQL.Mid(nIndex, 1);
				if(strChar == _T("'"))
				{
					strTemp = strSQL.Left(nIndex) + _T("N") + strSQL.Right(strSQL.GetLength() - nIndex);
					strSQL = strTemp;
					nIndex++;
				}
				nIndex++;
			}
			nIndex++;
		}
	}
}