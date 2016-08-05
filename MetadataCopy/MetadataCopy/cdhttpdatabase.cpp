// CDDatabase.cpp: implementation of the CCDHttpDatabase class.
//
//////////////////////////////////////////////////////////////////////


#include "stdafx.h"
#include "CDHttpDatabase.h"
#include "CDHttpRecordSet.h"
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
//#ifdef _USE_HTTP

CCDHttpDatabase::CCDHttpDatabase( )
{
	m_bOpen = FALSE;
	m_bReportErr = FALSE;

	m_pfnHandleError = NULL;

	m_bWriteLog = FALSE;
	TCHAR szFileName[1024];
	::GetModuleFileName(NULL, szFileName, 1024);
	m_strAppExeName = szFileName;
	m_pConnect = NULL;
	m_pSession = NULL;	
	
	m_strURLOpenProcedure = _T("/dmam/query/test.do");
	m_strURLExecSql = "/tempWeb/ReceiveCDBExe.do";
	m_strURLUpdate = "/tempWeb/ReceiveCDBUpdate.do";
	m_strURLOpenSql = "/tempWeb/CaptureDB";///wftestWeb/ReceiveCDBRe.do
	
	
}

CCDHttpDatabase::CCDHttpDatabase(LPCTSTR lpszDataSource, LPCTSTR lpszUID,
						 LPCTSTR lpszPWD, LPCTSTR lpszDatabase)
{
	CCDHttpDatabase();	
}

CCDHttpDatabase::~CCDHttpDatabase()
{
	Close();
}

BOOL CCDHttpDatabase::WriteLog(CString &strDatabase, CString &strOperation)
{
	if(!m_bWriteLog)
		return TRUE;
	CString strPath = _T("..\\Log");
	CFileFind fFind;
	if(!fFind.FindFile(strPath))
	{
		::CreateDirectory(strPath, NULL);
	}	
	
	CStdioFile file;
	if(!file.Open(DB_LOG_FILENAME, CFile::modeWrite|CFile::typeText|CFile::shareDenyWrite)) 
	{
		if(!file.Open(DB_LOG_FILENAME, CFile::modeCreate|CFile::modeWrite|CFile::typeText|CFile::shareDenyWrite)) 
		{ 
			return FALSE;
		}
	}
	else
		file.SeekToEnd();

	if(file.m_pStream == NULL)
		return FALSE;
	CString strLog;
	SYSTEMTIME time;
	GetLocalTime(&time);
	strLog.Format(_T("[%d-%02d-%02d %02d:%02d:%02d:%03d],[%s],[%s],[%s]\n"),
		time.wYear, time.wMonth, time.wDay, time.wHour, time.wMinute, time.wSecond, time.wMilliseconds,
		m_strAppExeName, strDatabase, strOperation);
	file.WriteString(strLog);
	if(file.m_pStream != NULL)
		file.Close();

	return TRUE;
}

BOOL CCDHttpDatabase::GetServerPortFromDataSource(CString &strDataSource, CString &strServer, INTERNET_PORT &nPort)
{
	int nPos = strDataSource.ReverseFind(':');
	if (nPos == -1)
		return FALSE;
	strServer = strDataSource.Left(nPos);
	nPort = _ttoi(strDataSource.Mid(nPos+1, 4) );
	return TRUE;
}

BOOL CCDHttpDatabase::Open()
{
	if(!GetServerPortFromDataSource(m_connect.strDataSource, m_strServerIP, m_nPort))
	{
		HandleError(_T(""), _T("GetServerInformation"));
		return FALSE;
	}
	if(m_connect.strDatabase == _T(""))
	{
		HandleError(_T(""), _T("Invalid Parameter") );
		return FALSE;
	}
	m_strURLOpenSql = m_connect.strDatabase;//
	if(m_connect.strUID == _T(""))
	{
		HandleError(_T(""), _T("Invalid Parameter") );
		return FALSE;
	}
	m_strURLUpdate = m_connect.strUID;//
	if(m_connect.strPWD == _T(""))
	{
		HandleError(_T(""), _T("Invalid Parameter") );		
		return FALSE;
	}
	m_strURLExecSql = m_connect.strPWD;

	if(m_pConnect)
	{
		delete m_pConnect;
		m_pConnect = NULL;
	}
	if(!m_pSession)
	{
		m_pSession = new CInternetSession(m_strAppExeName);
	}
	m_pConnect = m_pSession->GetHttpConnection(m_strServerIP, m_nPort);
	if(m_pConnect)
		m_bOpen = TRUE;
	else
		m_bOpen = FALSE;
	m_connect.enumType = CDDT_ORACLE;
	return m_bOpen;
}

BOOL CCDHttpDatabase::Open(CDDBCONNECT &connect)
{	
	m_connect = connect;
	return Open();
}

BOOL CCDHttpDatabase::Close()
{
	if(m_pConnect)
	{
		delete m_pConnect;
		m_pConnect = NULL;
	}
	if(m_pSession)
	{
		delete m_pSession;
		m_pSession = NULL;
	}
	m_bOpen = FALSE;
	return TRUE;
}

BOOL CCDHttpDatabase::IsHttp()
{
	return TRUE;
}
void CCDHttpDatabase::HandleError(_com_error &e, LPCTSTR lpszCmdLine/* = NULL*/)
{
	m_strErrDescription = CString((BSTR)e.Description());
	m_hrError = e.Error();
	CString strCmdLine;
	if(lpszCmdLine != NULL)
		strCmdLine = lpszCmdLine;
	CString strErrMsg, strLog;
	strLog.Format(_T("错误代码:%x, 错误描述:%s, 源命令行:%s"), 
		m_hrError, 
		m_strErrDescription,
		strCmdLine); 
	strErrMsg.Format(_T("访问数据库出错\n%s\n"), strLog); 
	//TRACE(strErrMsg);
	WriteLog(m_connect.strDatabase, strLog);
	if(m_pfnHandleError != NULL)	//回调处理
		m_pfnHandleError(m_hrError, strLog);
	if(m_bReportErr)
		AfxMessageBox(strErrMsg);	
}

void CCDHttpDatabase::HandleError(LPCTSTR lpszResponse, LPCTSTR lpszCmdLine/* = NULL*/)
{
	CString strCmdLine;
	if(lpszCmdLine != NULL)
		strCmdLine = lpszCmdLine;
	CString strErrMsg, strLog;
	strLog.Format(_T("CmdLine:%s\nResponse:[%s]"), 
		strCmdLine,
		lpszResponse); 
	strErrMsg.Format(_T("DBError\n%s\n"), strLog); 
	WriteLog(m_connect.strDatabase, strLog);
	if(m_bReportErr)
		AfxMessageBox(strErrMsg);	
}

BOOL CCDHttpDatabase::ExecuteSQL(CString &strSQL)
{
	if(!IsOpen())
		return FALSE;
	CCDHttpRecordset rs(this);
	return rs.ExecuteSQL(strSQL);
}

BOOL CCDHttpDatabase::ExecuteSQLUncatch(CString &strSQL)
{
	if(!IsOpen())
		return FALSE;
	return ExecuteSQL(strSQL);	
}

CString CCDHttpDatabase::GetValueForUpdate(CString &strValue, VARTYPE vt/* = VT_BSTR*/)
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

CString CCDHttpDatabase::GetFieldForUpdate(LPCTSTR lpszField)
{
	CString strFieldForUpdate;
#ifdef _CD_ORACLE	
	strFieldForUpdate.Format(_T("\"%s\""), lpszField);
#else
	strFieldForUpdate.Format(_T("[%s]"), lpszField);
#endif	//_CD_ORACLE
	return strFieldForUpdate;
}


BOOL CCDHttpDatabase::BeginTrans()
{
//	if(!IsOpen())
//		return FALSE;
//	try
//	{
//		m_pConnection->BeginTrans();
//	}
//	catch(_com_error &e)
//	{ 
//		HandleError(e, _T("BeginTrans"));
//        return FALSE;
//	}
	return TRUE;
}

BOOL CCDHttpDatabase::CommitTrans()
{
//	if(!IsOpen())
//		return FALSE;
//	try
//	{
//		m_pConnection->CommitTrans();
//	}
//	catch(_com_error &e)
//	{ 
//		HandleError(e, _T("CommitTrans"));
//        return FALSE;
//	}
	return TRUE;
}

BOOL CCDHttpDatabase::RollbackTrans()
{
	return TRUE;
}

BOOL CCDHttpDatabase::AppendParameter(_variant_t vtParameter, int nSize, BOOL bForInput/* = TRUE*/)
{
//	DataTypeEnum Type;
//	ParameterDirectionEnum Direction;
//	switch(vtParameter.vt)
//	{
//	case VT_BSTR:
//		{
//			Type = adVarChar;
//		}
//		break;
//	case VT_I4:
//		{
//			Type = adInteger;
//		}
//		break;
//	default:
//		ASSERT(FALSE);
//		return FALSE;
//	}
//	if(bForInput)
//		Direction = adParamInput;
//	else
//		Direction = adParamOutput;
//    _ParameterPtr pprm = m_pCommand->CreateParameter(
//        _T(""), Type, Direction, nSize, vtParameter);
//    m_pCommand->Parameters->Append(pprm);
	_PROC_PARAM param;
	param.vtParameter = vtParameter;
	param.nSize = nSize;
	param.bForInput = bForInput;
	m_aryParam.Add(param);
	return TRUE;
}

void CCDHttpDatabase::ClearAllParameters()
{
//	int nCount = m_pCommand->Parameters->Count;
//	for(int i = 0; i < nCount; i ++)
//	{
//		_variant_t vtIndex = (short)0;
//		m_pCommand->Parameters->Delete(vtIndex);
//	}
	m_aryParam.RemoveAll();
}

BOOL CCDHttpDatabase::ExecStorProc(CString &strProc, CString &strParam)
{
	CString strSQL;
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

//BOOL CCDHttpDatabase::HasField(LPCTSTR lpszTableName, LPCTSTR lpszFieldName)
//{
//
//	CString str = lpszFieldName;
//	if(str == _T("APPLICATIONID") || str == _T("NEEDREPAIR") || str == _T("PATHID"))
//		return TRUE;
//	else
//		return FALSE;
//  //  return TRUE;
//}

//#endif//#ifdef _USE_HTTP