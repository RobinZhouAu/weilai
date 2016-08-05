G// CDRecordset.cpp: implementation of the CCDRecordset class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "CDRecordset.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//#define BUGFIX_RELEASE_IERRORINFO(error) \ 
//           IErrorInfo * pErr = error.ErrorInfo();  \ 
//           pErr->Release();                        \ 
//           pErr->Release(); 
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CCDRecordset::CCDRecordset(CDYDatabaseEx* pdb)
{
	m_bOpen = FALSE;
	m_nUpdateField = 0;
}

CCDRecordset::~CCDRecordset()
{
	Close();
}

BOOL CCDRecordset::Open(CString &strSQL, BOOL bUseClient/* = TRUE*/, LockTypeEnum LockType/* = adLockOptimistic*/, CString &strResponse/*兼容HTTPRecordSet*/)
{
	if(m_pDB == NULL)
		return FALSE;
	if(!m_pDB->IsOpen())
	{		
		OutputDebugString(_T("m_pDB is null!\n"));
		return FALSE;
	}
	Close();
	if(m_pDB->m_bExecEmptySQL)
		m_pDB->ExecEmptySQL();


	_bstr_t bstrQuery;
	bstrQuery = _bstr_t(LPCTSTR(strSQL));
	
	HRESULT hr;
	try
	{
		hr = m_pRecordSet.CreateInstance(__uuidof(Recordset));
		if (!SUCCEEDED(hr))
		{
			_com_error e(hr);
			m_pDB->HandleError(e, _T("Create Recordset Instance"));
			return FALSE;
		}
		if(bUseClient)
			m_pRecordSet->CursorLocation = adUseClient;
		_variant_t ActiveConnection = _variant_t((IDispatch *)m_pDB->GetConnection(), true);
		hr = m_pRecordSet->raw_Open((_variant_t)bstrQuery, ActiveConnection, adOpenKeyset, LockType, adCmdUnspecified);
//		hr = m_pRecordSet->Open((_variant_t)bstrQuery, ActiveConnection, adOpenKeyset, LockType, adCmdUnspecified);
		if (!SUCCEEDED(hr))
		{
			_com_error e(hr);
			m_pDB->HandleError(e, (LPCTSTR)bstrQuery);
			return FALSE;
		}
	}
	catch(_com_error &e)
	{ 
		m_pDB->HandleError(e, (LPCTSTR)bstrQuery);//strSQL);
        return FALSE;
	}
	catch(...)
	{
		_com_error e(0x80004005);//to reconnect
		m_pDB->HandleError(e, (LPCTSTR)bstrQuery);
        return FALSE;
	}
	m_bOpen = TRUE;
	m_nUpdateField = 0;
	return TRUE;
}

BOOL CCDRecordset::Open(CString &strSQL, CursorLocationEnum CursorLocation,
	CursorTypeEnum CursorType/* = adOpenKeyset*/, LockTypeEnum LockType/* = adLockOptimistic*/,
	CommandTypeEnum CommaonType/* = adCmdUnspecified*/)	//打开记录集
{
	if(!m_pDB->IsOpen())
	{		
		OutputDebugString(_T("m_pDB is null!\n"));
		return FALSE;
	}
	Close();
	if(m_pDB->m_bExecEmptySQL)
		m_pDB->ExecEmptySQL();

	_bstr_t bstrQuery;
	bstrQuery = _bstr_t(LPCTSTR(strSQL));
	
	HRESULT hr;
	try
	{
		hr = m_pRecordSet.CreateInstance(__uuidof(Recordset));
		if (!SUCCEEDED(hr))
		{
			_com_error e(hr);
			m_pDB->HandleError(e, _T("Create Recordset Instance"));
			return FALSE;
		}
		m_pRecordSet->CursorLocation = CursorLocation;
		_variant_t ActiveConnection = _variant_t((IDispatch *)m_pDB->GetConnection(), true);
		hr = m_pRecordSet->Open(bstrQuery, ActiveConnection, CursorType, LockType, CommaonType);
		if (!SUCCEEDED(hr))
		{
			_com_error e(hr);
			m_pDB->HandleError(e, _T("Open Recordset"));
			return FALSE;
		}
	}
	catch(_com_error &e)
	{ 
		m_pDB->HandleError(e, (LPCTSTR)bstrQuery);//strSQL);
        return FALSE;
	}
	catch(...)
	{
		_com_error e(0x80004005);//to reconnect
		m_pDB->HandleError(e, (LPCTSTR)bstrQuery);
        return FALSE;
	}
	
	m_bOpen = TRUE;
	m_nUpdateField = 0;
	return TRUE;
}


BOOL CCDRecordset::OpenByStoredProc(CString &strProcName)
{
	if(!m_pDB->IsOpen())
		return FALSE;
	Close();
	if(m_pDB->m_bExecEmptySQL)
		m_pDB->ExecEmptySQL();

	_bstr_t bstrQuery;
	bstrQuery = _bstr_t(LPCTSTR(strProcName));
	HRESULT hr;
	try
	{
		hr = m_pRecordSet.CreateInstance(__uuidof(Recordset));
		if (!SUCCEEDED(hr))
		{
			_com_error e(hr);
			m_pDB->HandleError(e, _T("Create Recordset Instance"));
			return FALSE;
		}
		m_pRecordSet->CursorLocation = adUseClient;
		_CommandPtr &pCommand = m_pDB->GetCommand();
		pCommand->CommandText = bstrQuery;
		pCommand->CommandType = adCmdStoredProc;
		m_pRecordSet = pCommand->Execute(NULL, NULL, adCmdStoredProc);
		
	}
	catch(_com_error &e)
	{ 
		m_pDB->HandleError(e, strProcName);
        return FALSE;
	}
	m_bOpen = TRUE;
	return TRUE;
}

BOOL CCDRecordset::Close()
{
	if(m_pRecordSet != NULL)
	{
		try
		{
			m_pRecordSet->Close();
		}
		catch(_com_error &)
		{ 
		}
		m_pRecordSet.Release();
	}
	return TRUE;
}

BOOL CCDRecordset::IsEOF()
{
	try
	{
		if(m_pRecordSet)
			return m_pRecordSet->GetadoEOF();
	}
	catch(_com_error &e)
	{ 
		m_pDB->HandleError(e);
	}
	return TRUE;
}

BOOL CCDRecordset::IsBOF()
{
	if(m_pRecordSet)
		return m_pRecordSet->GetBOF();
	return TRUE;
}

BOOL CCDRecordset::MoveFirst()
{
	try
	{
		if(m_pRecordSet)
		{
			m_pRecordSet->MoveFirst();
		}
	}
	catch(_com_error &e)
	{ 
		CString strCmdLine;
		strCmdLine.Format(_T("MoveFirst@[%s]"), GetSQLStatemenet());
		m_pDB->HandleError(e, strCmdLine);
		return FALSE;
	}
	return TRUE;
}

BOOL CCDRecordset::MoveNext()
{
	try
	{
		if(m_pRecordSet)
		{
			m_pRecordSet->MoveNext();
		}
	}
	catch(_com_error &e)
	{ 
		CString strCmdLine;
		strCmdLine.Format(_T("MoveNext@[%s]"), GetSQLStatemenet());
		m_pDB->HandleError(e, strCmdLine);
		return FALSE;
	}
	return TRUE;
}

int CCDRecordset::GetFieldCount()
{
	if(m_pRecordSet == NULL)
		return 0;
	int nCount;
	try
	{
		nCount = m_pRecordSet->GetFields()->GetCount();
	}
	catch(_com_error &e)
	{
		m_pDB->HandleError(e);
		return 0;
	}
	return nCount;
}

BOOL CCDRecordset::GetFieldName(short nIndex, CString &strFieldName)
{
	_variant_t Index = nIndex;
	if(m_pRecordSet == NULL)
		return FALSE;
	try
	{
		strFieldName = CString(BSTR(m_pRecordSet->GetFields()->GetItem(Index)->GetName()));	
	}
	catch(_com_error &e)
	{
		m_pDB->HandleError(e);
		return FALSE;
	}
	return TRUE;
}

//BOOL CCDRecordset::GetFieldValue(LPCTSTR lpszFieldName, LPVOID lpValue)
//{
//	if(m_pRecordSet == NULL)
//		return FALSE;
//	_variant_t val;
//	try
//	{
//		val = m_pRecordSet->GetCollect(lpszFieldName);
//	}
//	catch(_com_error &e)
//	{
//		CString strCmdLine;
//		strCmdLine.Format(_T("GetField[%s]@[%s]"), lpszFieldName, GetSQLStatemenet());
//		m_pDB->HandleError(e, strCmdLine);
//		return FALSE;
//	}
//
//	switch(val.vt)
//	{
//	case VT_NULL:
//		break;
//	case VT_R8:
//		*((double *)lpValue) = val.dblVal;
//		break;
//	case VT_DECIMAL:
//		{
//			if(val.decVal.scale == 0)
//			{
//				VarI4FromDec(&val.decVal, (long *)lpValue);
//			}
//			else
//			{
//				VarR8FromDec(&val.decVal, (double *)lpValue);
//			}
//		}
//		break;
//	case VT_I4:
//		*((int *)lpValue) = val.lVal;
//		break;
//	case VT_DATE:
//		{
//			COleDateTime date = val.date;
//			*((COleDateTime *)lpValue) = date;
//		}
//		break;
//	case VT_BSTR:
//		*((CString *)lpValue) = CString((BSTR)(_bstr_t)val);
//		break;
//	case VT_ARRAY|VT_UI1:
//		//memcpy(lpValue, val.parray->pvData, val.parray->rgsabound[0].cElements);
//		*((_variant_t *)lpValue) = val;
//		break;
//	default:
//		ASSERT(FALSE);
//		break;
//	}
//	return TRUE;
//}
//
//BOOL CCDRecordset::GetFieldValue(short nIndex, LPVOID lpValue)
//{
//	if(m_pRecordSet == NULL)
//		return FALSE;
//	_variant_t val;
//	_variant_t Index = nIndex;
//	try
//	{
//		val = m_pRecordSet->GetCollect(Index);
//	}
//	catch(_com_error &e)
//	{
//		CString strCmdLine;
//		strCmdLine.Format(_T("GetField[%d]@[%s]"), nIndex, GetSQLStatemenet());
//		m_pDB->HandleError(e, strCmdLine);
//		return FALSE;
//	}
//
//	switch(val.vt)
//	{
//	case VT_NULL:
//		break;
//	case VT_R8:
//		*((double *)lpValue) = val.dblVal;
//		break;
//	case VT_DECIMAL:
//		{
//			if(val.decVal.scale == 0)
//			{
//				VarI4FromDec(&val.decVal, (long *)lpValue);
//			}
//			else
//			{
//				VarR8FromDec(&val.decVal, (double *)lpValue);
//			}
//		}
//		break;
//	case VT_I4:
//		*((int *)lpValue) = val.lVal;
//		break;
//	case VT_DATE:
//		{
//			COleDateTime date = val.date;
//			*((COleDateTime *)lpValue) = date;
//		}
//		break;
//	case VT_BSTR:
//		*((CString *)lpValue) = CString((BSTR)(_bstr_t)val);
//		break;
//	case VT_ARRAY|VT_UI1:
//		*((_variant_t *)lpValue) = val;
//		break;
//	}
//	return TRUE;
//}

BOOL CCDRecordset::GetFieldValue(short nIndex, _variant_t *val)
{
	if(m_pRecordSet == NULL)
		return FALSE;
	_variant_t Index = nIndex;
	try
	{
		*val = m_pRecordSet->GetCollect(Index);
	}
	catch(_com_error &e)
	{
		CString strCmdLine;
		strCmdLine.Format(_T("GetField[%d]@[%s]"), nIndex, GetSQLStatemenet());
		m_pDB->HandleError(e, strCmdLine);
		return FALSE;
	}
	return TRUE;
}
BOOL CCDRecordset::GetFieldValue(short nIndex, CString &strValue)
{
	return GetFieldValue(nIndex, &strValue);
}
BOOL CCDRecordset::GetFieldValue(short nIndex, int *pValue)
{
	if(!pValue)
		return FALSE;
	_variant_t var;
	if(!GetFieldValue(nIndex,&var))
		return FALSE;	
	if(var.vt == VT_NULL)
		return TRUE;
	*pValue = (long)var;
	return TRUE;	
}
BOOL CCDRecordset::GetFieldValue(short nIndex, long *pValue)
{
	return GetFieldValue(nIndex, (int* )pValue );
}
BOOL CCDRecordset::GetFieldValue(short nIndex, CString *pValue)
{
	if(!pValue)
		return FALSE;
	_variant_t var;
	if(!GetFieldValue(nIndex,&var))
		return FALSE;	
	if(var.vt == VT_NULL) //这里应该给pValue赋值
	{
		pValue->Empty();
		return TRUE;
	}
	return Var2String(var,(*pValue) );

}
BOOL CCDRecordset::GetFieldValue(short nIndex, double *pValue)
{
	if(!pValue)
		return FALSE;
	_variant_t var;
	if(!GetFieldValue(nIndex, &var))
		return FALSE;	
	if(var.vt == VT_NULL)
		return TRUE;
	*pValue = (double)var;
	return TRUE;	
}
BOOL CCDRecordset::GetFieldValue(short nIndex, DWORD *pValue)
{
	return GetFieldValue(nIndex, (int* )pValue );
}
BOOL CCDRecordset::GetFieldValue(short nIndex, COleDateTime *pValue)
{
	if(!pValue)
		return FALSE;
	_variant_t var;
	if(!GetFieldValue(nIndex, &var))
	{
		if(m_pDB && 
				m_pDB->GetDBType() == CDDT_ORACLE &&
				m_pDB->m_hrError == 0x80040e21)//special process
			return TRUE;	
		else
			return FALSE;
	}
	if(var.vt == VT_NULL)
		return TRUE;
	var.ChangeType(VT_DATE);
	*(pValue) = var.date;	
	return TRUE;	
}
BOOL CCDRecordset::GetFieldValue(short nIndex, LPVOID lpValue)
{
		if(m_pRecordSet == NULL)
		return FALSE;
	_variant_t val;
	_variant_t Index = nIndex;
	try
	{
		val = m_pRecordSet->GetCollect(Index);
	}
	catch(_com_error &e)
	{
		CString strCmdLine;
		strCmdLine.Format(_T("GetField[%d]@[%s]"), nIndex, GetSQLStatemenet());
		m_pDB->HandleError(e, strCmdLine);
		return FALSE;
	}

	switch(val.vt)
	{
	case VT_NULL:
		break;
	case VT_R8:
		*((double *)lpValue) = val.dblVal;
		break;
	case VT_DECIMAL:
		{
			if(val.decVal.scale == 0)
			{
				VarI4FromDec(&val.decVal, (long *)lpValue);
			}
			else
			{
				VarR8FromDec(&val.decVal, (double *)lpValue);
			}
		}
		break;
	case VT_I4:
		*((int *)lpValue) = val.lVal;
		break;
	case VT_DATE:
		{
			COleDateTime date = val.date;
			*((COleDateTime *)lpValue) = date;
		}
		break;
	case VT_BSTR:
		*((CString *)lpValue) = CString((BSTR)(_bstr_t)val);
		break;
	case VT_ARRAY|VT_UI1:
		*((_variant_t *)lpValue) = val;
		break;
	}
	return TRUE;
}
BOOL CCDRecordset::GetFieldValue(LPCTSTR lpszFieldName, _variant_t *val)
{
	if(m_pRecordSet == NULL)
		return FALSE;
	try
	{
		*val = m_pRecordSet->GetCollect(lpszFieldName);
	}
	catch(_com_error &e)
	{
		CString strCmdLine;
		strCmdLine.Format(_T("GetField[%s]@[%s]"), lpszFieldName, GetSQLStatemenet());
		m_pDB->HandleError(e, strCmdLine);
		return FALSE;
	}
	return TRUE;
}
BOOL CCDRecordset::GetFieldValue(LPCTSTR lpszFieldName, CString &strValue)
{
	return GetFieldValue(lpszFieldName,&strValue);
	
	//return
	if(m_pRecordSet == NULL)
		return FALSE;
	_variant_t val;
	try
	{
		val = m_pRecordSet->GetCollect(lpszFieldName);
	}
	catch(_com_error &e)
	{
		CString strCmdLine;
		strCmdLine.Format(_T("GetField[%s]@[%s]"), lpszFieldName, GetSQLStatemenet());
		m_pDB->HandleError(e, strCmdLine);
		return FALSE;
	}
	Var2String(val, strValue);
	return TRUE;
}

BOOL CCDRecordset::GetFieldValue(LPCTSTR lpszFieldName, double *pValue)
{
	if(!pValue)
		return FALSE;
	_variant_t var;
	if(!GetFieldValue(lpszFieldName,&var))
		return FALSE;	
	if(var.vt == VT_NULL)
		return TRUE;
	*pValue = (double)var;
	return TRUE;
}
BOOL CCDRecordset::GetFieldValue(LPCTSTR lpszFieldName, int *pValue)
{
	if(!pValue)
		return FALSE;
	_variant_t var;
	if(!GetFieldValue(lpszFieldName,&var))
		return FALSE;	
	if(var.vt == VT_NULL)
		return TRUE;
	*pValue = (long)var;
	return TRUE;	
}
BOOL CCDRecordset::GetFieldValue(LPCTSTR lpszFieldName, long *pValue)
{
	return GetFieldValue(lpszFieldName, (int* )pValue );
}
BOOL CCDRecordset::GetFieldValue(LPCTSTR lpszFieldName, CString *pValue)
{
	if(!pValue)
		return FALSE;
	_variant_t var;
	if(!GetFieldValue(lpszFieldName,&var))
		return FALSE;	
	if(var.vt == VT_NULL)
	{
		pValue->Empty();
		return TRUE;
	}
	return Var2String(var,(*pValue) );	
}
BOOL CCDRecordset::GetFieldValue(LPCTSTR lpszFieldName, DWORD *pValue)
{
	return GetFieldValue(lpszFieldName, (int* )pValue );
}
BOOL CCDRecordset::GetFieldValue(LPCTSTR lpszFieldName, COleDateTime *pValue)
{
	if(!pValue)
		return FALSE;
	_variant_t var;
	if(!GetFieldValue(lpszFieldName,&var))
	{
		if(m_pDB && 
			m_pDB->GetDBType() == CDDT_ORACLE &&
			m_pDB->m_hrError == 0x80040e21)//special process
			return TRUE;	
		else			
			return FALSE;	
	}
	if(var.vt == VT_NULL)
		return TRUE;
	var.ChangeType(VT_DATE);
	*(pValue) = var.date;	
	return TRUE;	
}
BOOL CCDRecordset::GetFieldValue(LPCTSTR lpszFieldName, LPVOID lpValue)
{
	if(m_pRecordSet == NULL)
		return FALSE;
	_variant_t val;
	try
	{
		val = m_pRecordSet->GetCollect(lpszFieldName);
	}
	catch(_com_error &e)
	{
		CString strCmdLine;
		strCmdLine.Format(_T("GetField[%s]@[%s]"), lpszFieldName, GetSQLStatemenet());
		m_pDB->HandleError(e, strCmdLine);
		return FALSE;
	}
	
	switch(val.vt)
	{
	case VT_NULL:
		break;
	case VT_R8:
		*((double *)lpValue) = val.dblVal;
		break;
	case VT_DECIMAL:
		{
			if(val.decVal.scale == 0)
			{
				VarI4FromDec(&val.decVal, (long *)lpValue);
			}
			else
			{
				VarR8FromDec(&val.decVal, (double *)lpValue);
			}
		}
		break;
	case VT_I4:
		*((int *)lpValue) = val.lVal;
		break;
	case VT_DATE:
		{
			COleDateTime date = val.date;
			*((COleDateTime *)lpValue) = date;
		}
		break;
	case VT_BSTR:
		*((CString *)lpValue) = CString((BSTR)(_bstr_t)val);
		break;
	case VT_ARRAY|VT_UI1:
		//memcpy(lpValue, val.parray->pvData, val.parray->rgsabound[0].cElements);
		*((_variant_t *)lpValue) = val;
		break;
	default:
		ASSERT(FALSE);
		break;
	}
	return TRUE;
}

BOOL CCDRecordset::PutFieldValue(short nIndex, const _variant_t &val)
{
	_variant_t Index = nIndex;
	if(m_pRecordSet == NULL)
		return FALSE;
	try
	{
		if(IsEmptyString(val))
		{
			_variant_t var;
			var.ChangeType(VT_NULL);
			m_pRecordSet->PutCollect(Index, var);
		}
		else if(val.vt == VT_BSTR)
		{
			_variant_t var;
			CString strValue = (LPCTSTR)(_bstr_t)val;
			strValue.Trim();
			var = strValue;
			m_pRecordSet->PutCollect(Index, var);
		}
		else			
			m_pRecordSet->PutCollect(Index, val);
	}
	catch(_com_error &e)
	{
		CString strCmdLine;
		strCmdLine.Format(_T("PutField[%d]@[%s]"), nIndex, GetSQLStatemenet());
		m_pDB->HandleError(e, strCmdLine);
		return FALSE;
	}
	m_nUpdateField ++;
	return TRUE;
}

BOOL CCDRecordset::IsEmptyString(const _variant_t &val)
{
	if(m_pDB && m_pDB->GetDBType() == CDDT_DB2//only for db2
		&& val.vt == VT_BSTR)
	{
		CString strValue = (LPCTSTR)(_bstr_t)val;
		if(strValue.IsEmpty())
		{
			return TRUE;
		}
	}
	return FALSE;
}
BOOL CCDRecordset::PutFieldValue(LPCTSTR lpszFieldName, const _variant_t &val)
{
	if(m_pRecordSet == NULL)
		return FALSE;
	if (val.vt == VT_EMPTY)
		return TRUE;
	CString strFieldName = lpszFieldName;
	try
	{
		
		if(IsEmptyString(val))
		{
			_variant_t var;
			var.ChangeType(VT_NULL);
			m_pRecordSet->PutCollect(lpszFieldName, var);
		}
		else if(val.vt == VT_BSTR)
		{
			_variant_t var;
			CString strValue = (LPCTSTR)(_bstr_t)val;
			strValue.Trim();
			var = strValue;
			m_pRecordSet->PutCollect(lpszFieldName, var);
		}
		else
			m_pRecordSet->PutCollect(lpszFieldName, val);
	}
	catch(_com_error &e)
	{
		CString strCmdLine;
		CString strValue;
		_variant_t varValue = val;
		Var2String(varValue,strValue);
		strCmdLine.Format(_T("PutField[%s]@%s,Value[%s]"), lpszFieldName, GetSQLStatemenet(),strValue);
		m_pDB->HandleError(e, strCmdLine);
		return FALSE;
	}
	m_nUpdateField ++;
	return TRUE;
}

BOOL CCDRecordset::AddNew()
{
	if(m_pRecordSet == NULL)
		return FALSE;
	try
	{
		HRESULT hr = m_pRecordSet->AddNew();
		if (!SUCCEEDED(hr))
		{
			_com_error e(hr);
			m_pDB->HandleError(e, _T("Add New"));
			return FALSE;
		}
	}
	catch(_com_error &e)
	{
		m_pDB->HandleError(e, _T("Add New"));
		return FALSE;
	}
	return TRUE;
}

BOOL CCDRecordset::Update()
{
	if(m_pRecordSet == NULL)
		return FALSE;
	if(m_nUpdateField == 0)
		return TRUE;
	try
	{
		//Added by dzz 2003.7.28
		if( m_pDB->GetDBType() == CDDT_ORACLE )
		{
			HRESULT hr = m_pRecordSet->Resync( adAffectCurrent, adResyncUnderlyingValues );
			if (!SUCCEEDED(hr))
			{
				_com_error e(hr);
				m_pDB->HandleError(e, _T("Resync"));
				return FALSE;
			}
		}
		//Added by dzz 2003.7.28
		HRESULT hr = m_pRecordSet->Update();
		if (!SUCCEEDED(hr))
		{
			_com_error e(hr);
			CString strCmd;
			strCmd.Format(_T("Update:[%s]"), GetSQLStatemenet());
			m_pDB->HandleError(e, strCmd);
			return FALSE;
		}
	}
	catch(_com_error &e)
	{
		CString strCmd;
		strCmd.Format(_T("Update:[%s]"), GetSQLStatemenet());
		m_pDB->HandleError(e, strCmd);
		return FALSE;
	}
	return TRUE;
}

int  CCDRecordset::GetRecordCount()
{
	return m_pRecordSet->RecordCount;
}

BOOL CCDRecordset::OpenSchema(SchemaEnum QueryType)
{
	if(!m_pDB->IsOpen())
		return FALSE;
	Close();
	if(m_pDB->m_bExecEmptySQL)
		m_pDB->ExecEmptySQL();

	try
	{
		m_pRecordSet = m_pDB->GetConnection()->OpenSchema(QueryType);
	}
	catch(_com_error &e)
	{ 
		m_pDB->HandleError(e, _T("Open Schema"));
        return FALSE;
	}
	catch(...)
	{
		_com_error e(0x80004005);//to reconnect
		m_pDB->HandleError(e,_T("Open Schema"));
        return FALSE;
	}
	
	m_bOpen = TRUE;
	m_nUpdateField = 0;
	return TRUE;
}

void CCDRecordset::SetPageSize(int nPageSize)
{
	if(m_pRecordSet != NULL)
		m_pRecordSet->PageSize = nPageSize;
}

int	CCDRecordset::GetPageCount()
{
	if(m_pRecordSet != NULL)
		return m_pRecordSet->PageCount;
	return -1;
}

void CCDRecordset::SetAbsolutePage(int nPage)
{
	if(m_pRecordSet != NULL)
		m_pRecordSet->AbsolutePage = (PositionEnum)nPage;
}

BOOL CCDRecordset::Move(long nMoveTo)
{
	if(m_pRecordSet == NULL)
		return FALSE;

	try
	{
		_variant_t var = (long)adBookmarkFirst;
		m_pRecordSet->Move(nMoveTo,var);
	}
	catch(_com_error &e)
	{ 
		CString strCmdLine;
		strCmdLine.Format(_T("Move[%d]@[%s]"), nMoveTo, GetSQLStatemenet());
		m_pDB->HandleError(e, strCmdLine);
        return FALSE;
	}
	return TRUE;
}
