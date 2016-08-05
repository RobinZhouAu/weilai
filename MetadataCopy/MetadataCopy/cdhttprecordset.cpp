// CDRecordset.cpp: implementation of the CCDHttpRecordset class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MsXml.h"
#include <AfxPriv.h>

#import "msxml.dll"
using namespace MSXML;

//#define _USEJPEGDLL

#ifdef _USEJPEGDLL
#include "ImageLoad.h"
#endif //_USEJPEGDLL

#include "CDHttpDatabase.h"
#include "DYHttpClient.h"


#include "CDHttpRecordset.h"
#include "unicodeStdioFile.h"
#include	"base64.h"
#include	<math.h>
#include	<AfxPriv.h>

//#ifdef _USE_HTTP

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


CCDHttpRecordset::CCDHttpRecordset(CDYDatabaseEx* pdb)
{
	m_bOpen = FALSE;
	m_pDB = (CCDHttpDatabase*)pdb;
	m_pHttpFile = NULL;
	m_strUpdateCondition = _T("");
	m_pReadBuffer = NULL;
	m_nRecordIndex = 0;
}

CCDHttpRecordset::~CCDHttpRecordset()
{
	Close();
	if(m_pReadBuffer != NULL)
	{
		free(m_pReadBuffer);
		m_pReadBuffer = NULL;
	}
	m_httpClient.m_pConnect = NULL;
	//let m_httpClient don't delete connection when destruction
}

BOOL CCDHttpRecordset::ExecuteSQL(CString &strSQL)
{
	CString strResponse;
	if(!Request(m_pDB->m_strURLExecSql,strSQL,strResponse,FALSE))
		return FALSE;		

//#ifdef _UNICODE
//	USES_CONVERSION;
//	strResponse = A2W((LPCSTR)m_pReadBuffer);
//#endif	
	strResponse.TrimLeft();
	strResponse.TrimRight();
	
	BOOL bOK = TRUE;
	if(strResponse != _T("true"))
	{
		//m_strErrorCode = strResponse;
		m_pDB->HandleError(strResponse, strSQL);
		bOK = FALSE;
	}
	return bOK;
}
BOOL CCDHttpRecordset::Request(LPCTSTR lpszURL, CString &strData, CString &strResponse, BOOL bNotTransfer)
{
	m_httpClient.m_pConnect = m_pDB->m_pConnect;
	BOOL bOK = m_httpClient.SendRequest(lpszURL,strData,strResponse,!bNotTransfer);
	//maybe http client delete the connection ptr
	if(m_httpClient.m_pConnect == NULL)
		m_pDB->m_pConnect = m_pDB->m_pSession->GetHttpConnection(
			m_pDB->m_strServerIP,m_pDB->m_nPort);
	return bOK;
}

BOOL CCDHttpRecordset::ReadString(CString& rString)
{
	rString.Empty();
	//BYTE cRead[3];
	CHAR cRead[3];
	CString strRead;
	BOOL bRead = FALSE;
	while(m_pHttpFile->Read(cRead, 1) > 0)
	{
		if(cRead[0] == '\r' || cRead[0] == '\n')
		{
			if (cRead[0] == '\r')
				m_pHttpFile->Read(cRead, 1);
			return TRUE;
		}
		else
		{
			if(::IsDBCSLeadByte(cRead[0]))
			{
				m_pHttpFile->Read(&cRead[1], 1);
				cRead[2] = '\0';
			}
			else
			{
				cRead[1] = '\0';
			}
			strRead = cRead;
			rString += strRead;
		}
		bRead = TRUE;
	}
	return bRead;
}
BOOL CCDHttpRecordset::Open(CString &strSQL,
						BOOL bUseClient/* = TRUE*/, 
						LockTypeEnum LockType/* = adLockOptimistic*/,
						CString &strResponse)
{
	Close();
	CString strSQL2 = strSQL;
	strSQL2.MakeUpper();
	int nFrom = strSQL2.Find(_T("WHERE"));
	m_strUpdateCondition = strSQL.Right(strSQL2.GetLength() - nFrom - 5);
	CString strOut;
	if(!Request(m_pDB->m_strURLOpenSql,strSQL,strResponse))
		return FALSE;
#ifdef _UNICODE
//	CStdioFile file;
//	file.Open(_T("D:\\test.xml"),CFile::modeCreate|CFile::modeWrite);
//	file.Write(m_pReadBuffer,m_nLength);
//	file.Close();
	if(m_httpClient.m_nLength == 0)
		return FALSE;
	if(!ParseRecordXML((LPCSTR)m_httpClient.m_pReadBuffer))
	{
		USES_CONVERSION;
		m_pDB->HandleError(A2W((LPCSTR)m_httpClient.m_pReadBuffer), strSQL);
		return FALSE;
	}
#else
//	CStdioFile file;
//	file.Open(_T("D:\\test.xml"),CFile::modeCreate|CFile::modeWrite);
//	file.WriteString(strResponse);
//	file.Close();
	if(!ParseRecordXML(strResponse))
		return FALSE;
#endif

//	DWORD dwTime = ::GetTickCount();	
//	DWORD dwTime2 = ::GetTickCount();
//	strOut.Format(_T("Parse Time used : %d\n"), (dwTime2 - dwTime));		
//	OutputDebugString(strOut);
	return TRUE;
}
BOOL CCDHttpRecordset::ParseRecordXML(LPCSTR lpszXML)//CString &strXml)
{
	USES_CONVERSION;
	//parse XML
	CString m_strResponse;
	MSXML::IXMLDOMDocumentPtr docPtr;
	MSXML::IXMLDOMNodePtr DOMNodePtr,recordRoot;
	MSXML::IXMLDOMNodePtr recordPtr,recordNext;
	MSXML::IXMLDOMNodePtr fieldPtr,fieldNext;
	MSXML::IXMLDOMNodePtr attributeNode;
	MSXML::IXMLDOMNodeListPtr childsRec, childsField,childCol;
	int i,j;
	HRESULT hr;
	
	hr = docPtr.CreateInstance("msxml.domdocument");
	if(hr != 0)
		return FALSE;	

	_bstr_t bstrXML = lpszXML;//strXml;
	_variant_t val;
	hr = docPtr->loadXML(bstrXML);
//	val = _T("D:\\Catalog\\test\\HTTPClient\\record.xml");	
//	hr = docPtr->load(val);	
	
	CString strValue;
	BSTR bstr;
	long records, fields;

	hr = docPtr->get_firstChild(&DOMNodePtr);//version
	if(hr != 0)
	{		
		return FALSE;
	}
			
	MSXML::IXMLDOMNamedNodeMapPtr attributeMap;	
	
	hr = DOMNodePtr->get_nextSibling(&recordRoot);//records root
	if(hr != 0) 
		return FALSE;
	hr = recordRoot->get_childNodes(&childsRec);
	if(hr != 0) return FALSE;
	hr = childsRec->get_length(&records);
	if(hr != 0) return FALSE;
	hr = recordRoot->get_firstChild(&recordPtr);//table name
	hr = recordPtr->get_text(&bstr);	
	if(hr != 0) return FALSE;
	m_strTable = bstr;	

//	hr = recordPtr->get_attributes(&attributeMap);
//	hr = attributeMap->get_item((long)0,&attributeNode);
//	hr = attributeNode->get_text(&bstr);
//	if(hr != 0) return FALSE;
//	strValue = bstr;
//	m_nKeyFieldNum = _ttol(strValue);

	hr = recordPtr->get_nextSibling(&recordNext);//cols
	hr = recordNext->get_childNodes(&childCol);//cols
	hr = childCol->get_length(&fields);
	if(hr != 0) return FALSE;
	
	CString strField;
	hr = recordNext->get_firstChild(&fieldPtr);//first col
	for(i = 0; i < fields; i++)
	{
		hr = fieldPtr->get_text(&bstr);
		strValue = bstr;
		strField = strValue;
		m_aryFields.Add(strValue);
		//m_colMap.SetAt((LPCTSTR)strValue, i);

		hr = fieldPtr->get_attributes(&attributeMap);
		hr = attributeMap->get_item((long)0,&attributeNode);
		hr = attributeNode->get_text(&bstr);
		strValue = bstr;
		m_aryFieldType.Add(_ttol((LPCTSTR)strValue));//col type
		m_aryKeyField.Add(0);		

		hr = fieldPtr->get_nextSibling(&fieldNext);
		fieldPtr = fieldNext;
	}
	
	hr = recordRoot->get_childNodes(&childsRec);
	if(hr != 0) return FALSE;
	hr = childsRec->get_length(&records);
	if(hr != 0) return FALSE;
	
	hr = recordNext->get_nextSibling(&recordRoot);//records
	hr = recordRoot->get_childNodes(&childsRec);
	if(hr != 0) return FALSE;
	hr = childsRec->get_length(&records);
	if(hr != 0) return FALSE;	
	hr = recordRoot->get_firstChild(&recordPtr);//first
	_RECORD_VALUE *pRecord;
	_FIELD_VALUE field;
	COleDateTime date;
	for(i = 0; i < (records); i++)
	{		
		pRecord = new _RECORD_VALUE;
		hr = recordPtr->get_firstChild(&fieldPtr);
		for(j = 0; j < fields; j++)
		{
			hr = fieldPtr->get_text(&bstr);
			switch(m_aryFieldType[j]) 
			{
			case FIELD_BINARY:
			case FIELD_LONGRAW:
				{
				if(!Decode(val,bstr))
					  return FALSE;
				}
				break;
			case FIELD_STRING:
				{
					val = bstr;
				}
				break;
			case FIELD_INTEGER:
				{
					strValue = bstr;
					if(strValue == _T(""))
						val.vt = VT_NULL;
					else
						val = _ttol(strValue);
				}
				break;
			case FIELD_FLOAT:
				{
					strValue = bstr;
					LPTSTR lpValue = strValue.GetBuffer(strValue.GetLength());
					LPSTR lpBuf;
#ifdef _UNICODE
					lpBuf = W2A(lpValue);
#else
					lpBuf = lpValue;
#endif
					if(strValue == _T(""))
						val.vt = VT_NULL;
					else
					{
						val.vt = VT_R8;
						val = atof(lpBuf);
					}
					strValue.ReleaseBuffer();
				}
				break;
			case FIELD_DATE:
				{
					strValue = bstr;
					if(strValue == "")
						val.vt = VT_NULL;
					else
					{
					strValue = strValue.Left(strValue.GetLength() - 2);
					date.ParseDateTime(strValue);
					val.vt = VT_DATE;
					val.date = (DATE)date;
					}
				}
				break;
			default:
				val = bstr;
			}
			field.varNew = val;
			field.varOri = val;
			pRecord->aryField.Add(field);				
			hr = fieldPtr->get_nextSibling(&fieldNext);//next col
			fieldPtr = fieldNext;				
		}				
		m_recordSet.Add(pRecord);
		hr = recordPtr->get_nextSibling(&recordNext);//next record	
		recordPtr = recordNext;
	}
	
	m_nFields  = m_aryFields.GetSize();
	m_nRecordIndex = 0;

	return TRUE;
}
BOOL CCDHttpRecordset::Decode(_variant_t &val,BSTR &bstr)
{
	USES_CONVERSION;
//#ifdef _UNICODE
//	TCHAR *pBuf2 = OLE2W(bstr);
//	int nLength = _tcslen(pBuf2)*2;
//#else
	CHAR *pBuf2 = OLE2A(bstr);
	int nLength = strlen(pBuf2);
//#endif
	BYTE *pBuffer= NULL;
	int nLength2;
	CBase64::Decode((PBYTE)pBuf2,nLength,pBuffer,nLength2);
	val.vt = (VT_ARRAY | VT_UI1);
	SAFEARRAY *pary;
	SAFEARRAYBOUND rgsabound[1];
	rgsabound[0].lLbound = 0;
	rgsabound[0].cElements = nLength2;	
	pary = SafeArrayCreate(VT_UI1,1,rgsabound);
	if(pary == NULL)
		return FALSE;
	memcpy(pary->pvData,pBuffer,nLength2);
	delete pBuffer; pBuffer = NULL;
	val.parray = pary;	
	return TRUE;
}
BOOL CCDHttpRecordset::OpenByStoredProc(CString &strProcName)
{
	if(!m_pDB->IsOpen())
		return FALSE;
	Close();
	CString strXML;
	strXML = _T("<?xml version=\"1.0\" encoding=\"GB2312\"?>");//xml declare
	strXML += _T("<spParam xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xsi:type=\"spParam\">");
	//sp name
	strXML += _T("<spName>"); strXML += strProcName; strXML += _T("</spName>");	
	//params
	strXML += _T("<params>");
	int i, nParam = m_pDB->m_aryParam.GetSize();
	CString strValue;
	_variant_t var;
	for(i = 0; i < nParam; i++)
	{
		strXML += _T("<P>"); 		
		var = m_pDB->m_aryParam[i].vtParameter; var.ChangeType(VT_BSTR);
		strValue = (LPCTSTR)(_bstr_t)var;
		strXML += _T("<v>"); strXML += strValue;; strXML += _T("</v>");		
		strValue.Format(_T("%d"),m_pDB->m_aryParam[i].nSize);
		strXML += _T("<s>"); strXML += strValue; strXML += _T("</s>");		
		strValue.Format(_T("%d"),m_pDB->m_aryParam[i].bForInput);
		strXML += _T("<d>"); strXML += strValue; strXML += _T("</d>");
		strXML += _T("</P>"); 
	}
	strXML += _T("</params>");	
	strXML += _T("</spParam>");

	
	CString strResponse;
	if(!Request(m_pDB->m_strURLOpenProcedure,strProcName,strResponse))
		return FALSE;

	CString strNew;
	int nLength = strResponse.GetLength();

#ifdef _UNICODE
	if(m_httpClient.m_nLength == 0)
		return FALSE;
	if(!ParseRecordXML((LPCSTR)m_httpClient.m_pReadBuffer))
	{
		return FALSE;
	}
#else
	if(!ParseRecordXML((LPCTSTR)strResponse))
		return FALSE;
#endif
	
	return TRUE;
}

BOOL CCDHttpRecordset::Close()
{
	int nSize = m_recordSet.GetSize();
	for(int i = 0; i < nSize; i++)
	{
		if(m_recordSet[i])
		{
			m_recordSet[i]->aryField.RemoveAll();
			delete m_recordSet[i];
			m_recordSet[i] = NULL;
		}
	}
	
	m_recordSet.RemoveAll();
//	m_colMap.RemoveAll();
	m_aryFields.RemoveAll();
	m_aryKeyField.RemoveAll();
	m_aryFieldType.RemoveAll();
	m_nFields = 0;
	m_nRecordIndex = -1;	
	m_nKeyFieldNum = 0;

	return TRUE;
}

BOOL CCDHttpRecordset::IsEOF()
{
//	if(m_pDB->m_pConnect == NULL)
//		return TRUE;
	int nSize = m_recordSet.GetSize();
	if(nSize == 0)
		return TRUE;
	return (m_nRecordIndex >=  nSize);
}

BOOL CCDHttpRecordset::IsBOF()
{
//	if(m_pDB->m_pConnect == NULL)
//		return TRUE;
//	int nSize = m_recordSet.GetSize();
//	if(nSize == 0)
//		return FALSE;
	return (m_nRecordIndex <  0);	
}

BOOL CCDHttpRecordset::MoveNext()
{
	return Move(m_nRecordIndex + 1);
}
BOOL CCDHttpRecordset::MoveFirst()
{
	m_nRecordIndex = 0;
	return TRUE;
}
BOOL CCDHttpRecordset::Move(long nIndex)
{
	int nSize = m_recordSet.GetSize();
	if(nSize == 0)//empty
		return FALSE;
	if(IsEOF() && nIndex > 0)
		return FALSE;
	if(IsBOF() && nIndex < 0)
		return FALSE;

	m_nRecordIndex = nIndex;
	if(m_nRecordIndex >= nSize)
		m_nRecordIndex = nSize;//EOF
	if(m_nRecordIndex < 0)
		m_nRecordIndex = -1;//BOF
	return TRUE;
}


int CCDHttpRecordset::GetFieldCount()
{
//	if(!m_m_pDB->m_pConnect == NULL)
//		return 0;
	return m_nFields;	
}

BOOL CCDHttpRecordset::GetFieldName(short nIndex, CString &strFieldName)
{
	strFieldName = _T("");
//	if(m_pDB->m_pConnect == NULL)
//		return FALSE;
	
	if(nIndex >= 0 && nIndex < m_aryFields.GetSize())
	{
		strFieldName = m_aryFields[nIndex];
		return TRUE;
	}

	return FALSE;
}

int CCDHttpRecordset::GetColIndex(LPCTSTR lpszFieldName)
{
	int nCol = -1;
	for(int i = 0; i < m_nFields; i++)
	{
		if(m_aryFields[i].CompareNoCase(lpszFieldName) == 0 )
		{
			nCol = i; 
			break;
		}
	}
	return nCol;

}
BOOL CCDHttpRecordset::GetFieldValue(LPCTSTR lpszFieldName, int *pValue)
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
BOOL CCDHttpRecordset::GetFieldValue(LPCTSTR lpszFieldName, CString *pValue)
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
BOOL CCDHttpRecordset::GetFieldValue(LPCTSTR lpszFieldName, double *pValue)
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
BOOL CCDHttpRecordset::GetFieldValue(LPCTSTR lpszFieldName, DWORD *pValue)
{
	return GetFieldValue(lpszFieldName,(int*)pValue);
}
BOOL CCDHttpRecordset::GetFieldValue(LPCTSTR lpszFieldName, long *pValue)
{
	return GetFieldValue(lpszFieldName,(int*)pValue);
}
BOOL CCDHttpRecordset::GetFieldValue(LPCTSTR lpszFieldName, _variant_t *pValue)
{
	int nCol = GetColIndex(lpszFieldName);
	if(nCol == -1)
		return FALSE;
	return GetFieldValue(nCol, *pValue, TRUE);
}
BOOL CCDHttpRecordset::GetFieldValue(LPCTSTR lpszFieldName, COleDateTime *pValue)
{
	if(!pValue)
		return FALSE;
	_variant_t var;
	if(!GetFieldValue(lpszFieldName,&var))
		return FALSE;	
	if(var.vt == VT_NULL)
		return TRUE;
	var.ChangeType(VT_DATE);
	*(pValue) = var.date;	
	return TRUE;
}
BOOL CCDHttpRecordset::GetFieldValue(LPCTSTR lpszFieldName, LPVOID pValue)
{
	return GetFieldValue(lpszFieldName,(int *)pValue);	
}
BOOL CCDHttpRecordset::GetFieldValue(short nIndex, CString &strValue)
{
	return GetFieldValue(nIndex, &strValue);	
}
BOOL CCDHttpRecordset::GetFieldValue(short nIndex, int *pValue)
{
	if(!pValue)
		return FALSE;	
	_variant_t val;
	if(!GetFieldValue(nIndex, &val))
		return FALSE;
	if (val.vt == VT_NULL)
		return TRUE;
	*pValue = (long)val;
	return TRUE;	
}
BOOL CCDHttpRecordset::GetFieldValue(short nIndex, long *pValue)
{
	return GetFieldValue(nIndex, (int* )pValue); 
}
BOOL CCDHttpRecordset::GetFieldValue(short nIndex, CString *pValue)
{
	if(!pValue)
		return FALSE;
	_variant_t var;
	if(!GetFieldValue(nIndex, &var))
		return FALSE;	
	if(var.vt == VT_NULL)
	{
		pValue->Empty();
		return TRUE;
	}
	return Var2String(var,(*pValue) );
}
BOOL CCDHttpRecordset::GetFieldValue(short nIndex, double *pValue)
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
BOOL CCDHttpRecordset::GetFieldValue(short nIndex, DWORD *pValue)
{
	return GetFieldValue(nIndex, (int* )pValue );
}
BOOL CCDHttpRecordset::GetFieldValue(short nIndex, COleDateTime *pValue)
{
	if(!pValue)
		return FALSE;
	_variant_t var;
	if(!GetFieldValue(nIndex, &var))
		return FALSE;	
	if(var.vt == VT_NULL)
		return TRUE;
	var.ChangeType(VT_DATE);
	*(pValue) = var.date;	
	return TRUE;
}
BOOL CCDHttpRecordset::GetFieldValue(short nIndex, _variant_t *pValue)
{
	return GetFieldValue(nIndex, *pValue, TRUE);
}
BOOL CCDHttpRecordset::GetFieldValue(short nIndex, LPVOID pValue)
{
	ASSERT(FALSE);
	return TRUE;
	_variant_t val;
	if(!GetFieldValue(nIndex,&val))
		return FALSE;
	
	switch(val.vt)
	{
	case VT_NULL:
		break;
	case VT_R8:
		*((double *)pValue) = val.dblVal;
		break;
	case VT_DECIMAL:
		{
			if(val.decVal.scale == 0)
			{
				VarI4FromDec(&val.decVal, (long *)pValue);
			}
			else
			{
				VarR8FromDec(&val.decVal, (double *)pValue);
			}
		}
		break;
	case VT_I4:
		*((int *)pValue) = val.lVal;
		break;
	case VT_DATE:
		{
			COleDateTime date = val.date;
			*((COleDateTime *)pValue) = date;
		}
		break;
	case VT_BSTR:
		{
			*((CString *)pValue) = CString((BSTR)(_bstr_t)val);
		}
		break;
	case VT_ARRAY|VT_UI1:
		//memcpy(lpValue, val.parray->pvData, val.parray->rgsabound[0].cElements);
		*((_variant_t *)pValue) = val;
		break;
	default:
		ASSERT(FALSE);
		break;
	}
	return TRUE;
}

BOOL CCDHttpRecordset::GetFieldValue(short nCol, _variant_t &val, BOOL bGetNew)
{
//	if(m_pDB->m_pConnect == NULL)
//		return FALSE;
	
	if(nCol < 0 || nCol >= m_nFields)
		return FALSE;
	
	int nSize = m_recordSet.GetSize();
	if(nSize == 0 || m_nRecordIndex < 0 || m_nRecordIndex >= nSize)
		return FALSE;	
	_FIELD_VALUE field;
	field = m_recordSet[m_nRecordIndex]->aryField.GetAt(nCol);
	if(bGetNew)
		val = field.varNew;
	else
		val = field.varOri;
	
	return TRUE;
}

BOOL CCDHttpRecordset::GetFieldValue(LPCTSTR lpszFieldName, CString &strValue)
{
	return GetFieldValue(lpszFieldName,&strValue);
	
	//returned
	_variant_t val;
	if(!GetFieldValue(lpszFieldName,&val))
		return FALSE;
	return Var2String(val,strValue);
	
}

BOOL CCDHttpRecordset::GetFieldValue(short nIndex, CString &strValue, BOOL bGetNew, VARTYPE vt)
{
	_variant_t val;
	if(!GetFieldValue(nIndex,val,bGetNew))
		return FALSE;
	if(vt != VT_NULL)
		val.ChangeType(vt);
	return Var2String(val,strValue);
}
BOOL CCDHttpRecordset::SetColType(int nCol, int nVarType)
{
	_FIELD_TYPE type = FIELD_STRING;
	switch(nVarType)
	{
	case VT_BSTR:
		type = FIELD_STRING;
		break;
	case VT_I4:
		break;
	default:
		break;		
	}
	return TRUE;
}
BOOL CCDHttpRecordset::PutFieldValue(short nCol, const _variant_t &val)
{
//	if(m_pDB->m_pConnect == NULL)
//		return FALSE;
	
	if(nCol < 0 || nCol >= m_nFields)
		return FALSE;
	
	int nSize = m_recordSet.GetSize();
	if(nSize == 0 || m_nRecordIndex < 0 || m_nRecordIndex >= nSize)
		return FALSE;	
	
	_RECORD_VALUE *pRecord = m_recordSet[m_nRecordIndex];
	_FIELD_VALUE field = pRecord->aryField.GetAt(nCol);
	field.varNew = val;
	field.bChanged = TRUE;
	pRecord->aryField.SetAt(nCol,field);
	if(pRecord->enumType == _RECORD_NORMAL)
		pRecord->enumType = _RECORD_UPDATE;
	
//	SetColType(nCol,val.vt);
	return TRUE;
}

BOOL CCDHttpRecordset::PutFieldValue(LPCTSTR lpszFieldName, const _variant_t &val)
{
//	if(m_pDB->m_pConnect == NULL)
//		return FALSE;
	
	int nCol = GetColIndex(lpszFieldName);
	if(nCol == -1)
		return FALSE;
//	if(!m_colMap.Lookup(lpszFieldName,nCol))
//		return FALSE;
	
	return PutFieldValue(nCol,val);
}

BOOL CCDHttpRecordset::AddNew()
{
//	if(m_pDB->m_pConnect == NULL)
//		return FALSE;
	_RECORD_VALUE *pRecord = new _RECORD_VALUE;
	pRecord->enumType = _RECORD_NEW;
	_FIELD_VALUE field;
	for(int i = 0; i < m_nFields; i++)
		pRecord->aryField.Add(field);
	
	m_nRecordIndex = m_recordSet.Add(pRecord);
	return TRUE;
}
BOOL CCDHttpRecordset::Encode(_variant_t &var,CString &strValue)
{
	BYTE *pBuf = (BYTE *)var.parray->pvData;
	int nLength = var.parray->rgsabound[0].cElements;
	BYTE *pBufOut = NULL;
	int nLength2;	
	CBase64::Encode(pBuf,nLength,pBufOut,nLength2);	

//#ifdef _UNICODE
//	USES_CONVERSION;
//	strValue = A2W((char*)pBufOut);
//#else
	strValue = pBufOut;
//#endif

	delete pBufOut;		
	return TRUE;	
}
CString CCDHttpRecordset::GetUpdateCondition(_RECORD_VALUE *pRecord,BOOL bInsert)
{
	CString strCondition = _T("");
//	if(pRecord->enumType == _RECORD_NEW)
//		return strCondition;
	int i;
	int nKeyField = 0;
	for(i = 0; i < m_nFields; i++)		
	{
		if(m_aryKeyField[i]) 
			nKeyField ++;
	}
	strCondition += _T("<condition>");
	BOOL bAdd;
	CString strValue, strValue2;
		
	for(i = 0; i < m_nFields; i++)
	{
		bAdd = FALSE;
		if(nKeyField != 0 && nKeyField == m_nKeyFieldNum)
		{
			if(m_aryKeyField[i] == 1)
				bAdd = TRUE;//add key frame
		}
		else if(m_aryFieldType[i] != FIELD_BINARY
			&& m_aryFieldType[i] != FIELD_LONGRAW	)
			bAdd = TRUE;//add all non binary col
		if(!bAdd) 
			continue;
		VARTYPE vt = VT_NULL;
		if(m_aryFieldType[i] == FIELD_DATE)
			vt = VT_DATE;
		GetFieldValueForXML(i,strValue2,bInsert,vt);
		strValue.Format(_T("<col type = \"%d\" name = \"%s\">%s</col>"),
			m_aryFieldType[i],m_aryFields[i],strValue2);
		strCondition += strValue; 		
	}
	strCondition += _T("</condition>");	
	return strCondition;
}

BOOL CCDHttpRecordset::GetFieldValueForXML(int i,CString &strValue2,BOOL bGetNew, VARTYPE vt)
{
	if(m_aryFieldType[i] != FIELD_BINARY && 
		m_aryFieldType[i] != FIELD_LONGRAW)
	{
		GetFieldValue(i,strValue2,bGetNew,vt);	
		strValue2.Replace(_T("&"),_T("&amp;"));	
		strValue2.Replace(_T("<"),_T("&lt;"));
		strValue2.Replace(_T(">"),_T("&gt;"));
		strValue2.Replace(_T("\""),_T("&quot;"));
//		strValue2.Replace(_T("'"),_T("&apos;"));
	}
	else
	{
		_variant_t var;
		GetFieldValue(i,var,bGetNew);
		if(var.vt == VT_BSTR)
		{
			_variant_t val;
			val.vt = (VT_ARRAY | VT_UI1);
			PBYTE pBuffer = NULL;
			CString strValue((BSTR)(_bstr_t)var);
			LPCTSTR lpszBUf = (LPCTSTR)strValue;
#ifdef _UNICODE
			USES_CONVERSION;
			pBuffer = (PBYTE)W2A(lpszBUf);
#else
			pBuffer = (PBYTE)(LPCTSTR)strValue;
#endif
			int nLength = strlen((LPCSTR)pBuffer);
			SAFEARRAY *pary;
			SAFEARRAYBOUND rgsabound[1];
			rgsabound[0].lLbound = 0;
			rgsabound[0].cElements = nLength;	
			pary = SafeArrayCreate(VT_UI1,1,rgsabound);
			if(pary == NULL)
				return FALSE;
			memcpy(pary->pvData,pBuffer,nLength);			
			val.parray = pary;	
			Encode(val,strValue2);
			
		}
		else
			Encode(var,strValue2);
	}				
	
	return TRUE;
}
BOOL CCDHttpRecordset::FillXML(CString &strXML)
{
//#ifdef _UNICODE
//	strXML = _T("<?xml version=\"1.0\" encoding=\"UTF-16LE\"?>");//xml declare
//#else
	strXML = _T("<?xml version=\"1.0\" encoding=\"GB2312\"?>");//xml declare
//#endif

	strXML += _T("<cmds>");
	int i,  k;
	int nRecUpdated = 0;
	int nRecs = m_recordSet.GetSize();
	_RECORD_VALUE *pRecord;
	_FIELD_VALUE field;
	CString strValue, strValue2;
	//MoveNext(-1);	
	MoveFirst();
	for(k = 0; k < nRecs; k++ )
	{
		pRecord = m_recordSet[k];
		if(k != 0) MoveNext();
		if(pRecord->enumType == _RECORD_NORMAL)
			continue;	
		nRecUpdated ++;
		//cmd
		strValue.Format(_T("<cmd type = \"%s\">"),
			(pRecord->enumType == _RECORD_UPDATE)? _T("UPDATE"):_T("INSERT"));
		strXML += strValue;
		//table name
		strValue.Format(_T("<table name = \"%s\" />"),m_strTable);	
		strXML += strValue; 
		//new value
		strXML += _T("<newvalue>");
		for(i = 0; i < m_nFields; i++)
		{
			field = pRecord->aryField[i];
			if( !field.bChanged )
				continue;
			field.bChanged = FALSE;
			pRecord->aryField.SetAt(i,field);
			VARTYPE vt = VT_NULL;
			if(m_aryFieldType[i] == FIELD_DATE)
				vt = VT_DATE;
			GetFieldValueForXML(i,strValue2,TRUE,vt);
			strValue.Format(_T("<col type = \"%d\" name = \"%s\">%s</col>"),
				m_aryFieldType[i],m_aryFields[i],strValue2);
			strXML += strValue; 
		}
		strXML += _T("</newvalue>");		
		//condition
		//strValue = GetUpdateCondition(pRecord,(pRecord->enumType == _RECORD_NEW));
		strValue.Format(_T("<condition>%s</condition>"),m_strUpdateCondition);
		strXML += strValue;	
		//end cmd
		strXML += _T("</cmd>");
		pRecord->enumType = _RECORD_NORMAL;//restore record status
	}
	strXML += _T("</cmds>");
	return (nRecUpdated != 0);
}
BOOL CCDHttpRecordset::Update()
{
	if(m_recordSet.GetSize() == 0)
		return FALSE;

	CString strXml;
	if(!FillXML(strXml))
		return FALSE;
//	CStdioFile file;
//	CString strFile = _T("D:\\testUpdate.xml");	
//	file.Open(strFile,CFile::modeWrite|CFile::modeCreate);
//	file.WriteString(strXml);
//	file.Close();
	
	
	CString strResponse;
	if(!Request(m_pDB->m_strURLUpdate,strXml,strResponse,FALSE) )
		return FALSE;

//#ifdef _UNICODE
//	USES_CONVERSION;
//	strResponse = A2W((LPCSTR)m_pReadBuffer);
//#endif
	
	strResponse.TrimLeft();
	strResponse.TrimRight();
		
	BOOL bOK = TRUE;
	if(strResponse != _T("true"))
	{
//		m_strErrorCode = strResponse;
		m_pDB->HandleError(strResponse, strXml);
		bOK = FALSE;
	}
	return bOK;
	
}

int  CCDHttpRecordset::GetRecordCount()
{
	return m_recordSet.GetSize();
}

//#endif//#ifdef _USE_HTTP