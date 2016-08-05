// CDDatabase.cpp: implementation of the CCDDatabase class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "CDDatabase.h"
#include "UnicodeStdioFile.h"
#include "AutoLockUnLock.h"
#include "OutPut.h"
#include "register.h"

extern BOOL g_bSQLTrace;

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#define PROVIDER_ODBC		_T("MSDASQL")
#define PROVIDER_SQLSERVER	_T("SQLOLEDB.1")
#define PROVIDER_DB2		_T("IBMDADB2")
#define PROVIDER_ORACLE		_T("OraOLEDB.Oracle")
#define PROVIDER_ACCESS		_T("Microsoft.Jet.OLEDB.4.0")
#define PROVIDER_EXCEL		_T("Microsoft.Jet.OLEDB.4.0")

#include "AutoLockUnlock.h"
struct _DYDB_Connection
{
	_ConnectionPtr pConPtr;
	CDDBCONNECT conInfo;
	int nConnectionCount;
	_DYDB_Connection()
	{
		pConPtr = NULL;
		nConnectionCount = 0;
	}
};
typedef CArray<_DYDB_Connection , _DYDB_Connection &>	CDBConnectionPool;

CDBConnectionPool g_aryConnectionPool;
CCriticalSection g_dbPoolLock;
BOOL operator == (const CDDBCONNECT &con1,const CDDBCONNECT &con2)
{	
	return con1.bUseDSN == con2.bUseDSN
		&& con1.enumType == con2.enumType
		&& con1.strDatabase.CompareNoCase(con2.strDatabase) == 0
		&& con1.strDataSource.CompareNoCase(con2.strDataSource) == 0
		&& con1.strUID.CompareNoCase(con2.strUID) == 0;
}

CString g_GetEmptySQL(CD_DB_TYPE enumType)
{
	CString strEmptySQL;
	switch(enumType)
	{
	case CDDT_SQLSERVER:
		strEmptySQL = _T(" select '' ");
		break;
	case CDDT_DB2:
		strEmptySQL = _T(" select '' from SYSIBM.SYSDUMMY1 ");
		break;
	case CDDT_ORACLE:
		strEmptySQL = _T(" select '' from dual ");
		break;
	default:
		break;
	}
	return strEmptySQL;
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CCDDatabase::CCDDatabase( )
{
	m_bOpen = FALSE;
	m_bReportErr = FALSE;

	m_pfnHandleError = NULL;
	m_pConnection = NULL;
	m_pCommand = NULL;
	//m_bUseODBC = FALSE;

	m_bWriteLog = FALSE;
	TCHAR szFileName[1024];
	::GetModuleFileName(NULL, szFileName, 1024);
	m_strAppExeName = szFileName;
	m_enumConnectMode = adModeReadWrite;
	m_hrError = 0;
	
	m_pOutput = NULL;
	SetLogFile(m_strAppExeName);

	//check if exec empty sql
	CRegister reg;
	DWORD dwData = 1;
	if(reg.OpenKey(HKEY_LOCAL_MACHINE, DY_MAM_REGISTER_PARENT))
	{
		if(reg.GetDWord(_T("ExecEmptySQL"), dwData))
		{
			m_bExecEmptySQL = dwData;
		}
		else//set value
		{
			reg.SetDWord(_T("ExecEmptySQL"), dwData);
		}
	}
	reg.CloseKey();

}
BOOL CCDDatabase::ExecEmptySQL()
{
	CString strEmptySQL = g_GetEmptySQL(m_connect.enumType);
	if(strEmptySQL.IsEmpty())
		return TRUE;

	BOOL bUseEmptySQl = m_bExecEmptySQL;
	m_bExecEmptySQL = FALSE;//for not loop ExecEmptySQL
	BOOL bReportError = m_bReportErr;
	m_bReportErr = FALSE;//not report error for empty sql
	

	BOOL bOK = ExecuteSQL(strEmptySQL);	


	//restore setup
	m_bExecEmptySQL = bUseEmptySQl;
	m_bReportErr = bReportError;
	
	return TRUE;
}
CCDDatabase::CCDDatabase(LPCTSTR lpszDataSource, LPCTSTR lpszUID,
						 LPCTSTR lpszPWD, LPCTSTR lpszDatabase)
{
	m_bOpen = FALSE;
	m_bReportErr = FALSE;
	m_pConnection = NULL;
	m_pCommand = NULL;


	m_connect.strDataSource = lpszDataSource;
	m_connect.strUID = lpszUID;
	m_connect.strPWD = lpszPWD;
	m_connect.strDatabase = lpszDatabase;

	m_pfnHandleError = NULL;

	m_bWriteLog = FALSE;
	TCHAR szFileName[1024];
	::GetModuleFileName(NULL, szFileName, 1024);
	m_strAppExeName = szFileName;
	m_hrError = 0;
	m_pOutput = NULL;
	SetLogFile(m_strAppExeName);
}

void CCDDatabase::SetLogFile(LPCTSTR lpszAppName)
{
	if(m_pOutput == NULL)
		m_pOutput = new COutPut();
	CString strApp = lpszAppName;
	int nPos = strApp.ReverseFind(TCHAR('\\'));
	CString strLogFile;
	if(nPos > 0)
		strLogFile = strApp.Left(nPos);
	nPos = strLogFile.ReverseFind(TCHAR('\\'));
	if(nPos > 0)
		strLogFile = strLogFile.Left(nPos);
	
	strLogFile = strLogFile + _T("\\Log\\dboperate.log");
	m_pOutput->SetLogFile(strLogFile);
	m_pOutput->SetLogHeader(strApp);
}
CCDDatabase::~CCDDatabase()
{
	if (IsOpen() )
		Close();
	deleteC(m_pOutput);
}

CD_DB_TYPE CCDDatabase::GetDBType()
{
	if(m_connect.enumType != CDDT_ODBCDRIVER)
		return m_connect.enumType;
	if(m_connect.strDatabase.CompareNoCase(_T("Oracle")) == 0)
		return CDDT_ORACLE;
	else if(m_connect.strDatabase.CompareNoCase(_T("DB2")) == 0)
		return CDDT_DB2;
	else if(m_connect.strDatabase.CompareNoCase(_T("SQLServer")) == 0)
		return CDDT_SQLSERVER;
	return m_connect.enumType; 
}

BOOL CCDDatabase::WriteLog(CString &strDatabase, CString &strOperation)
{
	if(!m_bWriteLog)
		return TRUE;
	CString strLog;
	strLog.Format(_T("[%s],[%s]"),
		strDatabase, strOperation);
	m_pOutput->OutPut(TRUE,strLog);

	return TRUE;
}
BOOL CCDDatabase::IsDBConnected(_ConnectionPtr &connection)
{
	CString strEmptySQL = g_GetEmptySQL(m_connect.enumType);
	try
	{
		if(strEmptySQL.IsEmpty())
			connection->OpenSchema(adSchemaTables);
		else
		{
			_bstr_t bstrSQL = _bstr_t(LPCTSTR(strEmptySQL));
			connection->Execute(bstrSQL, NULL, adOptionUnspecified);
		}
	}
	catch (_com_error &e)
	{
		m_hrError = e.Error();
		return FALSE;//DB not connected
	}

	return TRUE;
}

BOOL CCDDatabase::Open()
{
	Close();
	//in db2, use database. in function writelog,also use database
	if(m_connect.strDatabase.IsEmpty())
		m_connect.strDatabase = m_connect.strDataSource;
	CString strConnect;	
	HRESULT hr;
	_AUTO_LOCK lock(&m_ReconnectLock);

	// {{ Commented out by yxm, 2012-10-15
	//注释掉的原因：
	//    虽然在底层采用共享连接的想法不错，但是由于数据的访问涉及到记录集的裸调用
	//且没有进行数据库连接对象的共享访问控制，结果会导致应用层多个不同对象中建立了
	//相同的数据库连接后，共用了同一个对象，而多个类之间又只能内部做到同步访问，
	//不能做到类间的同步控制，结果可能导致类似于Oralce环境下被并发访问出错(OraOleDBrst11.dll)
	//    原则上修改方案，应该尽量也从底层入手，包括CDDatabase、CDRecordset等入手
	//但是暂时没有找到合适的修改方案，考虑到发布版本的稳定性和目前对连接数的要求不高，
	//暂时不考虑使用共享连接的方式
	/*
	//try to find con in pool
	_AUTO_LOCK lockPool(&g_dbPoolLock);
	int nDBs = g_aryConnectionPool.GetSize();
	for(int i = 0; i < nDBs; i++)
	{
		_DYDB_Connection &DBCon = g_aryConnectionPool[i];
		if(DBCon.conInfo == m_connect)//find db
		{
			//check if is open, if not open, remove DBCon and reopen it
			if(!IsDBConnected(DBCon.pConPtr))
			{
				DBCon.pConPtr->Close();
				DBCon.pConPtr.Release();
				g_aryConnectionPool.RemoveAt(i);
				break;
			}
			m_pConnection = DBCon.pConPtr;
			DBCon.nConnectionCount ++;
			m_bOpen = TRUE;			
			return TRUE;
		}
	}
	*/
	// }} Commented out by yxm, 2012-10-15

	//not find in pool, create new 
	try
	{	
		hr = m_pConnection.CreateInstance(__uuidof(Connection));
		if (!SUCCEEDED(hr))
		{
			_com_error e(hr);
			HandleError(e, _T("Create Connection Instance"));
			return FALSE;
		}
		
		//m_pConnection->PutMode(m_enumConnectMode);
		switch(m_connect.enumType)
		{
		case CDDT_SQLSERVER:
			m_pConnection->Provider = PROVIDER_SQLSERVER;
			strConnect.Format(_T("Data Source=%s;Persist Security Info=True;Database=%s"), m_connect.strDataSource, m_connect.strDatabase);
			break;
		case CDDT_DB2:
			m_pConnection->Provider = PROVIDER_DB2;//PROVIDER_ODBC;//;
			//DSN: Name of the database alias, The DB2 database alias in the database	directory.
			strConnect.Format(_T("DSN=%s;"), m_connect.strDatabase);
			break;
		case CDDT_ODBCDRIVER:
			m_pConnection->Provider = PROVIDER_ODBC;
			strConnect.Format(_T("Data Source=%s;Persist Security Info=True;"), m_connect.strDataSource);
			break;
		case CDDT_ORACLE:
			m_pConnection->Provider = PROVIDER_ORACLE;
			strConnect.Format(_T("Data Source=%s;Persist Security Info=True;"), m_connect.strDataSource);
			break;
		case CDDT_ACCESS:
			m_pConnection->Provider = PROVIDER_ACCESS;
			strConnect.Format(_T("Data Source=%s;"), m_connect.strDataSource);
		case CDDT_EXCEL:
			m_pConnection->Provider = PROVIDER_EXCEL;
			strConnect.Format(_T("Data Source=%s;Extended Properties=\"Excel 8.0\";"), m_connect.strDataSource);
		}
		if(m_connect.bUseDSN)
		{
			m_pConnection->Provider = PROVIDER_ODBC;//对于SQLServer如果采用"仅window"认证，必须用ODBC方式，而且使用这个Provider(东森连LOUTH数据库)
			strConnect.Format(_T("DSN=%s"), m_connect.strDataSource);
		}

		hr = m_pConnection->Open(_bstr_t(LPCTSTR(strConnect)),
			_bstr_t(m_connect.strUID), _bstr_t(m_connect.strPWD), adModeUnknown);
		if (!SUCCEEDED(hr))
		{
			_com_error e(hr);
			HandleError(e, _T("Open Connection"));
			return FALSE;
		}
	}
	catch (_com_error &e)
	{
		if(m_connect.enumType == CDDT_ORACLE)
		{//check exe dir if has ')'
			TCHAR lpszModuleName[1025];
			GetModuleFileName(NULL,lpszModuleName,1024);
			CString strName = lpszModuleName;
			if(strName.Find( TCHAR(')') ) >= 0 )
			{
				CString strMsg;
				strMsg.Format(_T("\r\n\r\nDrirecory [%s] has ')', may cause connect oracle db error!"),
					strName);
				strConnect += strMsg;
			}
			
		}
		HandleError(e, strConnect,FALSE);
		if(m_pConnection != NULL)
		{
			m_pConnection.Release();
			m_pConnection = NULL;
		}
		return FALSE;
	}
	m_bOpen = TRUE;
	_DYDB_Connection DBCon;
	DBCon.nConnectionCount = 1;
	DBCon.conInfo = m_connect;
	DBCon.pConPtr = m_pConnection;

	//to remove this line if not need db pool again
	// {{ Commented by yxm, 2012-10-15
	//配合上面不使用连接池的修改，这里也不再追加到连接池
	//g_aryConnectionPool.Add(DBCon);
	// }} Commented by yxm, 2012-10-15
	
	return TRUE;
}

BOOL CCDDatabase::Open(CDDBCONNECT &connect)
{
	m_connect = connect;
	return Open();
}

BOOL CCDDatabase::Close()
{
	try{
	if(m_pCommand != NULL)
	{
		m_pCommand.Release();
		m_pCommand = NULL;
	}
	if(m_pConnection != NULL)
	{
		//find in con pool
		_AUTO_LOCK lock(&g_dbPoolLock);
		int nDBs = g_aryConnectionPool.GetSize();
		BOOL bFind = FALSE;
		for(int i = 0; i < nDBs; i++)
		{
			_DYDB_Connection &DBcon = g_aryConnectionPool[i];
			if(DBcon.conInfo == m_connect
				&& DBcon.pConPtr == m_pConnection)
			{
				bFind = TRUE;
				DBcon.nConnectionCount --;
				if(DBcon.nConnectionCount <= 0)
				{
					DBcon.pConPtr->Close();
					DBcon.pConPtr.Release();
					g_aryConnectionPool.RemoveAt(i);
				}
				break;
			}
			
		}
		if(!bFind)	//if not use db pool, will not find
			m_pConnection->Close();
		
		m_pConnection.Release();//always call release
	}
	}
	catch (...)
	{
	}
	m_pCommand = NULL;
	m_pConnection = NULL;

	m_bOpen = FALSE;
	return TRUE;
}

_ConnectionPtr &CCDDatabase::GetConnection()
{
	if(m_pConnection == NULL)
		return m_pConnection;
	//check connect pool,get newest con
	
	_AUTO_LOCK lock(&g_dbPoolLock);
	int nDBs = g_aryConnectionPool.GetSize();
	for(int i = 0; i < nDBs; i++)
	{
		_DYDB_Connection &DBcon = g_aryConnectionPool[i];
		if(DBcon.conInfo == m_connect
			&& DBcon.pConPtr != m_pConnection)
		{
			m_pConnection = DBcon.pConPtr;
			break;
		}
	}
	
	return m_pConnection;
	
}
BOOL CCDDatabase::IsHttp()
{
	return FALSE;
}

void CCDDatabase::HandleError(_com_error &e, LPCTSTR lpszCmdLine/* = NULL*/,
							  BOOL bReOpen //= TRUE
							  )
{
	m_strErrDescription = CString((BSTR)e.Description());
	m_hrError = e.Error();
	CString strCmdLine;
	if(lpszCmdLine != NULL)
		strCmdLine = lpszCmdLine;
	CString strErrMsg, strLog;
	strLog.Format(_T("ErrorCode:%x, Descrption:%s, CmdLine:%s"), 
		m_hrError, 
		m_strErrDescription,
		strCmdLine); 
	strErrMsg.Format(_T("DBError\n%s\n"), strLog); 
	//TRACE(strErrMsg);
	if(bReOpen && (
		m_hrError == 0x80004005 
		|| m_hrError == 0x80040e14//
		|| m_hrError == 0x80004003//Invalid Pointer 
		|| m_hrError == 0x800a0e7d//db2, The connection cannot be used to perform this operation. It is either closed or invalid in this context
		) )
	{
		Open();
		m_bOpen = TRUE;//must set true, 
		//to make RecordSet can continue exec operation if db not opened
	}
	WriteLog(m_connect.strDatabase, strLog);
	if(m_pfnHandleError != NULL)	//回调处理
		m_pfnHandleError(m_hrError, strLog);
	if(m_bReportErr)
		AfxMessageBox(strErrMsg);	
}

BOOL CCDDatabase::ExecuteSQL(CString &strSQL)
{
	if(!IsOpen())
		return FALSE;
	if(m_bExecEmptySQL)
		ExecEmptySQL();

	_bstr_t bstrSQL;
	bstrSQL = _bstr_t(LPCTSTR(strSQL));
	try
	{
		m_pConnection->Execute(bstrSQL, NULL, adOptionUnspecified);
	}
	catch(_com_error &e)
	{ 
		HandleError(e, strSQL);
        return FALSE;
	}
	return TRUE;
}

BOOL CCDDatabase::ExecuteSQLUncatch(CString &strSQL)
{
	if(!IsOpen())
		return FALSE;
	if(m_bExecEmptySQL)
		ExecEmptySQL();

	_bstr_t bstrSQL;
	bstrSQL = _bstr_t(LPCTSTR(strSQL));
	try
	{
		m_pConnection->Execute(bstrSQL, NULL, adOptionUnspecified);
	}
	catch(_com_error &e)
	{ 
		m_strErrDescription = CString((BSTR)e.Description());
		m_hrError = e.Error();
		CString strCmdLine;
		strCmdLine = strSQL;
		CString strErrMsg, strLog;
		strLog.Format(_T("ErrorCode:%x, Description:%s, CmdLine:%s"), 
			m_hrError, 
			m_strErrDescription,
			strCmdLine); 
		strErrMsg.Format(_T("DBError\n%s\n"), strLog); 
		//TRACE(strErrMsg);
		if(m_bReportErr)
			AfxMessageBox(strErrMsg);			
        return FALSE;
	}
	return TRUE;
}

CString CCDDatabase::GetValueForUpdate(CString &strValue, VARTYPE vt/* = VT_BSTR*/)
{
	CString strValueForUpdate;
#ifdef _CD_ORACLE
	switch(vt)
	{
	case VT_DATE:
		strValueForUpdate.Format(_T("TO_DATE('%s','YYYY-MM-DD HH24:MI:SS')"), strValue);
		break;
	case VT_ARRAY|VT_UI1:
		strValueForUpdate.Format(_T("HEXTORAW('%s')"), strValue);
		break;
	case VT_BSTR:
		strValueForUpdate.Format(_T("'%s'"), strValue);
		break;
	default:
		strValueForUpdate = strValue;
	}
#else
	switch(vt)
	{
	case VT_DATE:
		strValueForUpdate.Format(_T("'%s'"), strValue);
		break;
	case VT_ARRAY|VT_UI1:
		strValueForUpdate.Format(_T("0x%s"), strValue);
		break;
	case VT_BSTR:
		strValueForUpdate.Format(_T("'%s'"), strValue);
		break;
	default:
		strValueForUpdate = strValue;
	}
#endif	//_CD_ORACLE
	return strValueForUpdate;
}

CString CCDDatabase::GetFieldForUpdate(LPCTSTR lpszField)
{
	CString strFieldForUpdate;
#ifdef _CD_ORACLE	
	strFieldForUpdate.Format(_T("\"%s\""), lpszField);
#else
	strFieldForUpdate.Format(_T("[%s]"), lpszField);
#endif	//_CD_ORACLE
	return strFieldForUpdate;
}
/*
BOOL CCDDatabase::ExecuteSQL(CString &strSQL, int &nRecordsAffected)
{
	if(!IsOpen())
		return FALSE;
	_bstr_t bstrSQL;
	bstrSQL = _bstr_t(LPCTSTR(strSQL));
	VARIANT varRecordsAffected;
	varRecordsAffected.vt = VT_I4;
	varRecordsAffected.lVal = -1;
	try
	{
		//m_pCommand->CommandText = bstrSQL;
		//m_pCommand->Execute(&varRecordsAffected, NULL, adCmdText);
		m_pConnection->Execute(bstrSQL, &varRecordsAffected, adCmdText);
	}
	catch(_com_error &e)
	{ 
		HandleError(e, strSQL);
        return FALSE;
	}
	nRecordsAffected = varRecordsAffected.lVal;
	return TRUE;
}
*/
BOOL CCDDatabase::BeginTrans()
{
	if(!IsOpen())
		return FALSE;
	try
	{
		m_pConnection->BeginTrans();
	}
	catch(_com_error &e)
	{ 
		HandleError(e, _T("BeginTrans"));
        return FALSE;
	}
	return TRUE;
}

BOOL CCDDatabase::CommitTrans()
{
	if(!IsOpen())
		return FALSE;
	try
	{
		m_pConnection->CommitTrans();
	}
	catch(_com_error &e)
	{ 
		HandleError(e, _T("CommitTrans"));
        return FALSE;
	}
	return TRUE;
}

BOOL CCDDatabase::RollbackTrans()
{
	if(!IsOpen())
		return FALSE;
	try
	{
		m_pConnection->RollbackTrans();
	}
	catch(_com_error &e)
	{ 
		HandleError(e, _T("RollbackTrans"));
        return FALSE;
	}
	return TRUE;
}

BOOL CCDDatabase::AppendParameter(_variant_t vtParameter, int nSize, BOOL bForInput/* = TRUE*/)
{
	DataTypeEnum Type;
	ParameterDirectionEnum Direction;
	switch(vtParameter.vt)
	{
	case VT_BSTR:
		{
			Type = adVarChar;
		}
		break;
	case VT_I4:
		{
			Type = adInteger;
		}
		break;
	default:
		ASSERT(FALSE);
		return FALSE;
	}
	if(bForInput)
		Direction = adParamInput;
	else
		Direction = adParamOutput;
    _ParameterPtr pprm = m_pCommand->CreateParameter(
        _T(""), Type, Direction, nSize, vtParameter);
    m_pCommand->Parameters->Append(pprm);
	return TRUE;
}

void CCDDatabase::ClearAllParameters()
{
	int nCount = m_pCommand->Parameters->Count;
	for(int i = 0; i < nCount; i ++)
	{
		_variant_t vtIndex = (short)0;
		m_pCommand->Parameters->Delete(vtIndex);
	}
}

BOOL CCDDatabase::ExecStorProc(CString &strProc, CString &strParam)
{
	CString strSQL;
	if(m_bExecEmptySQL)
		ExecEmptySQL();

	switch(m_connect.enumType)
	{
	case CDDT_SQLSERVER:
		strSQL.Format(_T("EXEC %s %s"), strProc, strParam);
		break;
	case CDDT_DB2:
		strSQL.Format(_T("{CALL %s(%s)}"), strProc, strParam);
		break;
	default:
		return FALSE;
	}
	return ExecuteSQL(strSQL);
}

#define CONFIG_ITEM_DATASOURCE		_T("DATASOURCE")
#define CONFIG_ITEM_DATABASE		_T("DATABASE")
#define CONFIG_ITEM_DBTYPE			_T("DBTYPE")
#define CONFIG_ITEM_UID				_T("UID")
#define CONFIG_ITEM_PWD				_T("PWD")

BOOL CCDDatabase::ReadConnectFromFile( LPCTSTR lpszFileName )
{
	return ReadConnectFromFile( lpszFileName, &m_connect );
}

BOOL CCDDatabase::ReadConnectFromFile( LPCTSTR lpszFileName, CDDBCONNECT *pConnect )
{
	CUnicodeStdioFile file;
	if( !file.Open( lpszFileName, CFile::modeRead) )
		return FALSE;
	CString strRead;
	while( file.ReadString( strRead ) )
	{
		strRead.TrimLeft();
		strRead.TrimRight();
		if(strRead.IsEmpty())
			continue;
		if( strRead.GetAt(0) == _T('#') )
			continue;
		int nPos = strRead.Find(_T("="));
		if( nPos <= 0 )
			continue;
		CString strName = strRead.Left( nPos );
		CString strValue = strRead.Mid( nPos + 1 );
		strName.TrimRight();
		strValue.TrimLeft();
		strName.MakeUpper();
		if( strName == CONFIG_ITEM_DATASOURCE )
		{
			pConnect->strDataSource = strValue;
		}
		else if( strName == CONFIG_ITEM_DATABASE )
		{
			pConnect->strDatabase = strValue;
		}
		else if( strName == CONFIG_ITEM_DBTYPE )
		{
			pConnect->enumType = (CD_DB_TYPE)_ttoi( strValue );
		}
		else if( strName == CONFIG_ITEM_UID )
		{
			pConnect->strUID = strValue;
		}
		else if( strName == CONFIG_ITEM_PWD )
		{
			pConnect->strPWD = strValue;
		}
	}
	file.Close();
	return TRUE;	
}

//BOOL CCDDatabase::HasField(LPCTSTR lpszTableName, LPCTSTR lpszFieldName)
//{
//	//用查询sysobjects的方法判断,只实现了SQLServer的方法
//	CString strSQL;
//	switch(m_connect.enumType)
//	{
//	case CDDT_SQLSERVER:
//		strSQL.Format(_T("SELECT * FROM syscolumns a INNER JOIN sysobjects b ON a.id = b.id WHERE (b.name = '%s' and a.name = '%s')"), lpszTableName, lpszFieldName);
//		break;
//	case CDDT_DB2:
//		strSQL.Format(_T("SELECT * FROM SYSIBM.SYSCOLUMNS WHERE TBNAME='%s' and NAME = '%s'"), lpszTableName, lpszFieldName);
//		break;
//	case CDDT_ORACLE:
//		strSQL.Format(_T("SELECT * FROM USER_COL_COMMENTS WHERE TABLE_NAME = '%s' and COLUMN_NAME = '%s'"), lpszTableName, lpszFieldName);
//		break;
//	default:
//		return FALSE;
//	}
//	
//	_RecordsetPtr	ptrRs;
//	_bstr_t bstrQuery;
//	bstrQuery = _bstr_t(LPCTSTR(strSQL));
//	
//	HRESULT hr;
//	try
//	{
//		hr = ptrRs.CreateInstance(__uuidof(Recordset));
//		if (!SUCCEEDED(hr))
//		{
//			ptrRs.Release();			
//			_com_error e(hr);
//			HandleError(e, _T("Create Recordset Instance"));
//			return FALSE;
//		}
//		ptrRs->CursorLocation = adUseClient;
//		_variant_t ActiveConnection = _variant_t((IDispatch *)m_pConnection, true);
//		hr = ptrRs->Open(bstrQuery, ActiveConnection, adOpenKeyset, adLockOptimistic, adCmdUnspecified);
//		if (!SUCCEEDED(hr))
//		{
//			ptrRs->Close();
//			ptrRs.Release();			
//			_com_error e(hr);
//			HandleError(e, _T("Open Recordset"));
//			return FALSE;
//		}
//		if (VARIANT_TRUE != ptrRs->GetadoEOF() )
//		{
//			ptrRs->Close();
//			ptrRs.Release();			
//			return TRUE;
//		}
//		else
//		{
//			ptrRs->Close();
//			ptrRs.Release();			
//			return FALSE;
//		}
//	}
//	catch(_com_error &e)
//	{ 
//		HandleError(e, (LPCTSTR)bstrQuery);//strSQL);
//        return FALSE;
//	}
//		
//	return TRUE;
//}
//
//BOOL CCDDatabase::HasTable(LPCTSTR lpszTableName)
//{
//	//用查询sysobjects的方法判断,只实现了SQLServer的方法
//	CString strSQL;
//	switch(m_connect.enumType)
//	{
//	case CDDT_SQLSERVER:
//		strSQL.Format(_T("SELECT * FROM sysobjects where xtype = 'U' AND name = '%s'"), lpszTableName);
//		break;
//	case CDDT_DB2:
//		strSQL.Format(_T("SELECT * FROM SYSIBM.SYSTABLES where TYPE = 'T' AND NAME = '%s'"), lpszTableName);
//		break;
//	case CDDT_ORACLE:
//		strSQL.Format(_T("SELECT * FROM USER_CATALOG WHERE Table_Type = 'TABLE' AND Table_Name='%s'"), lpszTableName);
//		break;
//	default:
//		return FALSE;
//	}
//	
//	_RecordsetPtr	ptrRs;
//	_bstr_t bstrQuery;
//	bstrQuery = _bstr_t(LPCTSTR(strSQL));
//	
//	HRESULT hr;
//	try
//	{
//		hr = ptrRs.CreateInstance(__uuidof(Recordset));
//		if (!SUCCEEDED(hr))
//		{
//			ptrRs.Release();			
//			_com_error e(hr);
//			HandleError(e, _T("Create Recordset Instance"));
//			return FALSE;
//		}
//		ptrRs->CursorLocation = adUseClient;
//		_variant_t ActiveConnection = _variant_t((IDispatch *)m_pConnection, true);
//		hr = ptrRs->Open(bstrQuery, ActiveConnection, adOpenKeyset, adLockOptimistic, adCmdUnspecified);
//		if (!SUCCEEDED(hr))
//		{
//			ptrRs->Close();
//			ptrRs.Release();			
//			_com_error e(hr);
//			HandleError(e, _T("Open Recordset"));
//			return FALSE;
//		}
//		if (VARIANT_TRUE != ptrRs->GetadoEOF() )
//		{
//			ptrRs->Close();
//			ptrRs.Release();			
//			return TRUE;
//		}
//		else
//		{
//			ptrRs->Close();
//			ptrRs.Release();			
//			return FALSE;
//		}
//	}
//	catch(_com_error &e)
//	{ 
//		HandleError(e, (LPCTSTR)bstrQuery);//strSQL);
//        return FALSE;
//	}
//
//	return TRUE;
//}
//
//BOOL CCDDatabase::HasView(LPCTSTR lpszViewName)
//{
//	//用查询sysobjects的方法判断,只实现了SQLServer的方法
//	CString strSQL;
//	switch(m_connect.enumType)
//	{
//	case CDDT_SQLSERVER:
//		strSQL.Format(_T("SELECT * FROM sysobjects where xtype = 'V' AND name = '%s'"), lpszViewName);
//		break;
//	case CDDT_DB2:
//		strSQL.Format(_T("SELECT * FROM SYSIBM.SYSTABLES where TYPE = 'V' AND NAME = '%s'"), lpszViewName);
//		break;
//	case CDDT_ORACLE:
//		strSQL.Format(_T("SELECT * FROM USER_CATALOG WHERE Table_Type = 'VIEW' AND Table_Name='%s'"), lpszViewName);
//		break;
//	default:
//		return FALSE;
//	}
//	
//	_RecordsetPtr	ptrRs;
//	_bstr_t bstrQuery;
//	bstrQuery = _bstr_t(LPCTSTR(strSQL));
//	
//	HRESULT hr;
//	try
//	{
//		hr = ptrRs.CreateInstance(__uuidof(Recordset));
//		if (!SUCCEEDED(hr))
//		{
//			ptrRs.Release();			
//			_com_error e(hr);
//			HandleError(e, _T("Create Recordset Instance"));
//			return FALSE;
//		}
//		ptrRs->CursorLocation = adUseClient;
//		_variant_t ActiveConnection = _variant_t((IDispatch *)m_pConnection, true);
//		hr = ptrRs->Open(bstrQuery, ActiveConnection, adOpenKeyset, adLockOptimistic, adCmdUnspecified);
//		if (!SUCCEEDED(hr))
//		{
//			ptrRs->Close();
//			ptrRs.Release();			
//			_com_error e(hr);
//			HandleError(e, _T("Open Recordset"));
//			return FALSE;
//		}
//		if (VARIANT_TRUE != ptrRs->GetadoEOF() )
//		{
//			ptrRs->Close();
//			ptrRs.Release();			
//			return TRUE;
//		}
//		else
//		{
//			ptrRs->Close();
//			ptrRs.Release();			
//			return FALSE;
//		}
//	}
//	catch(_com_error &e)
//	{ 
//		HandleError(e, (LPCTSTR)bstrQuery);//strSQL);
//        return FALSE;
//	}
//
//	return TRUE;
//}

