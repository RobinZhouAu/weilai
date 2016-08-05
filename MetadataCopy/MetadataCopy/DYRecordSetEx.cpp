// DYRecordSetEx.cpp: implementation of the CDYRecordSetEx class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <afxdb.h>
#include "DYRecordSetEx.h"
#include "CDRecordset.h"
#include "CDHttpRecordset.h"
#include "databaseCommonFunction.h"

#include "AutoLockUnlock.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

extern BOOL g_bSQLTrace ;//是否把执行的SQL语句输出到DebugView

extern CCriticalSection g_DYRecordSetExLock;

//#ifdef _USE_CDDATABASE_
//#else	//_USE_CDDATABASE_
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDYRecordSetEx::CDYRecordSetEx(CDYDatabaseEx* pDatabase)
{
	m_pRecordSet = NULL;
	m_pDBEx = pDatabase;
	if (m_pDBEx != NULL)
	{
		if (m_pDBEx->IsHttp() )//看看这里怎么解决这个问题
		{
			m_pRecordSet = new CCDHttpRecordset(pDatabase);
			m_pRecordSet->SetDatabase(m_pDBEx->m_pDatabase);
		}
		else
		{
			m_pRecordSet = new CCDRecordset(pDatabase);
			m_pRecordSet->SetDatabase(m_pDBEx->m_pDatabase);			
		}
	}
}

CDYRecordSetEx::~CDYRecordSetEx()
{
	if (m_pRecordSet != NULL)
	{
		m_pRecordSet->Close();	//close之前是否先判断一下？
		deleteC(m_pRecordSet);
	}
}

void CDYRecordSetEx::SetDatabase(CDYDatabaseEx *pdb)
{
	if (m_pDBEx == NULL)//初始化的时候没有传数据库指针
	{
		m_pDBEx = pdb;
		if (m_pDBEx->IsHttp() )
			m_pRecordSet = new CCDHttpRecordset(pdb);
		else
			m_pRecordSet = new CCDRecordset(pdb);
		m_pRecordSet->SetDatabase(pdb->m_pDatabase);		
	}
	if ((pdb->IsHttp() == m_pDBEx->IsHttp() )&& (pdb->GetDBType() == m_pDBEx->GetDBType()) )//表示数据库类型没有改变
	{
		m_pDBEx = pdb;
		m_pRecordSet->SetDatabase(pdb->m_pDatabase);
	}
	else
	{
		m_pDBEx = pdb;
		m_pRecordSet->Close();	//close之前是否先判断一下？
		delete m_pRecordSet;
		if (m_pDBEx->IsHttp() )
			m_pRecordSet = new CCDHttpRecordset(pdb);
		else
			m_pRecordSet = new CCDRecordset(pdb);
		m_pRecordSet->SetDatabase(pdb->m_pDatabase);
	}	
}

CDYDatabaseEx* CDYRecordSetEx::GetDatabase()
{
	return m_pDBEx;
}
BOOL CDYRecordSetEx::IsOpen()
{
	return m_pRecordSet->IsOpen();
}
BOOL CDYRecordSetEx::IsEOF()
{
	return m_pRecordSet->IsEOF();
}
BOOL CDYRecordSetEx::IsBOF()
{
	return m_pRecordSet->IsBOF();
}
int  CDYRecordSetEx::GetFieldCount()
{
	return m_pRecordSet->GetFieldCount();
}
int  CDYRecordSetEx::GetRecordCount()
{
	return m_pRecordSet->GetRecordCount();
}
CString CDYRecordSetEx::GetSQLStatemenet()
{
	return m_pRecordSet->GetSQLStatemenet();
}
void CDYRecordSetEx::SetPageSize(int nPageSize)
{
	m_pRecordSet->SetPageSize(nPageSize);
}
int	 CDYRecordSetEx::GetPageCount()
{
	return m_pRecordSet->GetPageCount();
}
BOOL CDYRecordSetEx::Memory2Variant(BYTE *pBuf, DWORD dwBufSize, _variant_t &var)
{
	CLongBinary lb;
	lb.m_dwDataLength = dwBufSize;
	lb.m_hData = (HGLOBAL)pBuf;
	COleVariant oleVar(lb);
	var = oleVar;
	lb.m_hData = NULL;
	return TRUE;
}
BOOL CDYRecordSetEx::Variant2Memory(BYTE *pBuf, DWORD dwBufSize, _variant_t &var)
{
	if(var.vt != (VT_ARRAY | VT_UI1))
		return FALSE;
	if(var.parray->rgsabound[0].cElements <= dwBufSize)
	{
		memcpy(pBuf, var.parray->pvData, var.parray->rgsabound[0].cElements);
	}
	else
	{
		memcpy(pBuf, var.parray->pvData, dwBufSize);
	}
	return TRUE;
}
HBITMAP CDYRecordSetEx::DIBVariant2Bmp(int nBmpWidth, int nBmpHeight, _variant_t &var)
{
	if(var.vt != (VT_ARRAY | VT_UI1))
		return NULL;
	if(var.parray->rgsabound[0].cElements <= sizeof BITMAP)
		return NULL;
	LPBYTE lpvBits = (LPBYTE)var.parray->pvData;
	DWORD dwSize = nBmpWidth * nBmpHeight * sizeof DWORD;
	LPBYTE pBuf = new BYTE[dwSize];
	DWORD dwBytesPerLine = nBmpWidth * sizeof DWORD;
	for(int i = 0; i < nBmpHeight; i ++)
	{
		memcpy(pBuf + dwBytesPerLine * i, lpvBits + (nBmpHeight - 1 - i) * dwBytesPerLine, dwBytesPerLine);
	}
	HBITMAP hBitmap = ::CreateBitmap(nBmpWidth, nBmpHeight,
		1, 32, pBuf);
	delete pBuf;
	return hBitmap;
}
BOOL CDYRecordSetEx::Bmp2DIBVariant(HBITMAP hBitmap, _variant_t &var)
{
	BITMAP bmp;
	if(::GetObject(hBitmap, sizeof(BITMAP), &bmp) == 0)
		return FALSE;
	DWORD dwSize = bmp.bmWidth * bmp.bmHeight * bmp.bmBitsPixel / 8;
	
	BYTE *pBuf = new BYTE[dwSize];
	if(::GetBitmapBits(hBitmap, dwSize, pBuf) == 0)
	{
		delete pBuf;
		return FALSE;
	}
	CLongBinary lb;
	lb.m_dwDataLength = dwSize;
	lb.m_hData = (HGLOBAL)pBuf;
	COleVariant oleVar(lb);
	var = oleVar;
	lb.m_hData = NULL;
	delete pBuf;
	return TRUE;
}
BOOL CDYRecordSetEx::Bitmap2Variant(HBITMAP hBitmap, _variant_t &var)
{
	if(hBitmap == NULL)
		return FALSE;
	BITMAP bmp;
	if(::GetObject(hBitmap, sizeof(BITMAP), &bmp) == 0)
		return FALSE;
	DWORD dwSize = bmp.bmWidth * bmp.bmHeight * bmp.bmBitsPixel / 8;
	
	BYTE *pBuf = new BYTE[sizeof BITMAPFILEHEADER + sizeof BITMAPINFOHEADER + dwSize];
	PBITMAPFILEHEADER lpbmfh = (PBITMAPFILEHEADER)pBuf;
	PBITMAPINFOHEADER lpbmih = (PBITMAPINFOHEADER)(pBuf + sizeof BITMAPFILEHEADER);
	
	lpbmih->biSize			= sizeof BITMAPINFOHEADER;
	lpbmih->biWidth			= bmp.bmWidth;
	lpbmih->biHeight		= bmp.bmHeight;
	lpbmih->biPlanes		= bmp.bmPlanes;
	lpbmih->biBitCount		= bmp.bmBitsPixel;
	lpbmih->biCompression	= BI_RGB;
	lpbmih->biSizeImage		= dwSize;
	lpbmih->biXPelsPerMeter	= 0;
	lpbmih->biYPelsPerMeter	= 0;
	lpbmih->biClrUsed		= 0;
	lpbmih->biClrImportant	= 0;
	
	lpbmfh->bfType          = 0x4d42;
	lpbmfh->bfSize			= sizeof BITMAPFILEHEADER + sizeof BITMAPINFOHEADER + dwSize;
	lpbmfh->bfReserved1     = 0;
	lpbmfh->bfReserved2     = 0;
	lpbmfh->bfOffBits		= sizeof BITMAPFILEHEADER + sizeof BITMAPINFOHEADER;
	
	BYTE *lpBits = pBuf + lpbmfh->bfOffBits;
	BYTE *lpBits1 = new BYTE[dwSize];
	if(::GetBitmapBits(hBitmap, dwSize, lpBits1) == 0)
	{
		delete pBuf;
		delete lpBits1;
		return FALSE;
	}
	DWORD dwWidthBytes = bmp.bmWidthBytes;
	for(int i = 0; i < lpbmih->biHeight; i ++)
	{
		memcpy(lpBits + (lpbmih->biHeight - 1 - i) * dwWidthBytes,
			lpBits1 + i * dwWidthBytes, dwWidthBytes);
	}
	
	CLongBinary lb;
	lb.m_dwDataLength = lpbmfh->bfSize;
	lb.m_hData = (HGLOBAL)pBuf;
	COleVariant oleVar(lb);
	var = oleVar;
	lb.m_hData = NULL;
	delete lpBits1;
	delete pBuf;
	
	return TRUE;
}
HBITMAP CDYRecordSetEx::Variant2Bitmap(_variant_t &var)
{
	if(var.vt != (VT_ARRAY | VT_UI1))
		return NULL;
	if(var.parray->rgsabound[0].cElements <= (sizeof BITMAPFILEHEADER + sizeof BITMAPINFOHEADER))
		return NULL;
	BYTE *pBuf = (BYTE *)var.parray->pvData;
	PBITMAPINFOHEADER lpbmih = (PBITMAPINFOHEADER)(pBuf + sizeof BITMAPFILEHEADER);
	if(lpbmih->biWidth > 1024 || lpbmih->biHeight > 768)//防止无效位图造成内存不足
		return NULL;
	BYTE *lpBits1 = pBuf + sizeof BITMAPFILEHEADER + sizeof BITMAPINFOHEADER;
	HBITMAP hBitmap = ::CreateDIBitmap(::GetDC(NULL), lpbmih, CBM_INIT, lpBits1, (BITMAPINFO *)lpbmih, DIB_RGB_COLORS);
/*	BYTE *lpBits = new BYTE[lpbmih->biSizeImage];
	DWORD dwWidthBytes = lpbmih->biWidth * lpbmih->biBitCount / 8;
	for(int i = 0; i < lpbmih->biHeight; i ++)
	{
		memcpy(lpBits + (lpbmih->biHeight - 1 - i) * dwWidthBytes,
			lpBits1 + i * dwWidthBytes, dwWidthBytes);
	}
	
	HBITMAP hBitmap = ::CreateBitmap(lpbmih->biWidth, lpbmih->biHeight,
		lpbmih->biPlanes, lpbmih->biBitCount, lpBits);

	delete lpBits;
*/	return hBitmap;
}
BOOL CDYRecordSetEx::Variant2MemFile(CMemFile &file, _variant_t &var)
{
	if(var.vt != (VT_ARRAY | VT_UI1))
		return FALSE;
	file.Attach((BYTE *)var.parray->pvData, var.parray->rgsabound[0].cElements);
	return TRUE;
}
BOOL CDYRecordSetEx::MemFile2Variant(CMemFile &file, _variant_t &var)
{
	DWORD dwLength = file.GetLength();
	if(dwLength == 0)
		return FALSE;
	BYTE *pBuf = file.Detach();
	CLongBinary lb;
	lb.m_dwDataLength = dwLength;
	lb.m_hData = (HGLOBAL)pBuf;
	COleVariant oleVar(lb);
	var = oleVar;
	lb.m_hData = NULL;
	::free(pBuf);
	return TRUE;
}
BOOL CDYRecordSetEx::Var2String(_variant_t &val,CString &strValue)
{
	switch(val.vt)
	{
	case VT_NULL:
		strValue.Empty();
		break;
	case VT_DECIMAL:
		{
			if(val.decVal.scale == 0)
			{
				long lOut;
				VarI4FromDec(&val.decVal, &lOut);
				strValue.Format(_T("%d"), lOut);
			}
			else
			{
				double dOut;
				VarR8FromDec(&val.decVal, &dOut);
				strValue.Format(_T("%f"), dOut);
			}
		}
		break;
	case VT_I4:
		strValue.Format(_T("%d"), val.lVal);
		break;
	case VT_R8:
		{
			strValue.Format(_T("%f"),(double)val);
		}
		break;
	case VT_DATE:
		{
			COleDateTime date = val.date;
			strValue = date.Format(FORMAT_ODT2STRING);
		}
		break;
	case VT_BSTR:
		{
			strValue = CString((BSTR)(_bstr_t)val);
		}
		break;
	case VT_ARRAY|VT_UI1:
		{
			CString strHex;
			strValue.Empty();
			//for(WORD i = 0; i < val.parray->rgsabound[0].cElements; i ++)
			{
				//strHex.Format(_T("%02X"), ((BYTE *)val.parray->pvData)[i]);
				//strValue += strHex;
				int nLength = val.parray->rgsabound[0].cElements;
				PBYTE pBuffer = new BYTE[nLength + 1];
				memcpy(pBuffer,val.parray->pvData,nLength);
				pBuffer[nLength] = '\0';
#ifdef _UNICODE
				USES_CONVERSION;
				strValue = A2W((LPCSTR)pBuffer);
				nLength = strValue.GetLength();
#else
				strValue = pBuffer;
				nLength = strValue.GetLength();
#endif
				delete pBuffer;
			}
		}
		break;
	default:
		strValue.Empty();
	}
	strValue.TrimRight();
	return TRUE;
}
BOOL CDYRecordSetEx::AddNew()
{
	return m_pRecordSet->AddNew();
}
BOOL CDYRecordSetEx::Update()
{
	_AUTO_LOCK lock(&g_DYRecordSetExLock);

	return m_pRecordSet->Update();
}
BOOL CDYRecordSetEx::PutFieldValue(short nIndex, const _variant_t &val)
{
	return m_pRecordSet->PutFieldValue(nIndex, val);
}
BOOL CDYRecordSetEx::PutFieldValue(LPCTSTR lpszFieldName, const _variant_t &val)
{
	return m_pRecordSet->PutFieldValue(lpszFieldName, val);
}

BOOL CDYRecordSetEx::GetFieldName(short nIndex, CString &strFieldName)
{
	return m_pRecordSet->GetFieldName(nIndex, strFieldName);
}
BOOL CDYRecordSetEx::GetFieldValue(short nIndex, CString &strValue)
{
	return m_pRecordSet->GetFieldValue(nIndex, strValue);
}
BOOL CDYRecordSetEx::GetFieldValue(short nIndex, int *pValue)
{
	return m_pRecordSet->GetFieldValue(nIndex, pValue);
}
BOOL CDYRecordSetEx::GetFieldValue(short nIndex, long *pValue)
{
	return m_pRecordSet->GetFieldValue(nIndex, pValue);
}
BOOL CDYRecordSetEx::GetFieldValue(short nIndex, CString *pValue)
{
	return m_pRecordSet->GetFieldValue(nIndex, pValue);
}
BOOL CDYRecordSetEx::GetFieldValue(short nIndex, double *pValue)
{
	return m_pRecordSet->GetFieldValue(nIndex, pValue);
}
BOOL CDYRecordSetEx::GetFieldValue(short nIndex, DWORD *pValue)
{
	return m_pRecordSet->GetFieldValue(nIndex, pValue);
}
BOOL CDYRecordSetEx::GetFieldValue(short nIndex, COleDateTime *pValue)
{
	return m_pRecordSet->GetFieldValue(nIndex, pValue);
}
BOOL CDYRecordSetEx::GetFieldValue(short nIndex, _variant_t *pValue)
{
	return m_pRecordSet->GetFieldValue(nIndex, pValue);
}
BOOL CDYRecordSetEx::GetFieldValue(short nIndex, LPVOID pValue)
{
	return m_pRecordSet->GetFieldValue(nIndex, pValue);
}

BOOL CDYRecordSetEx::GetFieldValue(LPCTSTR lpszFieldName, CString &strValue)
{
	return m_pRecordSet->GetFieldValue(lpszFieldName, strValue);
}
BOOL CDYRecordSetEx::GetFieldValue(LPCTSTR lpszFieldName, int *pValue)
{
	return m_pRecordSet->GetFieldValue(lpszFieldName, pValue);
}
BOOL CDYRecordSetEx::GetFieldValue(LPCTSTR lpszFieldName, long *pValue)
{
	return m_pRecordSet->GetFieldValue(lpszFieldName, pValue);
}
BOOL CDYRecordSetEx::GetFieldValue(LPCTSTR lpszFieldName, CString *pValue)
{
	return m_pRecordSet->GetFieldValue(lpszFieldName, pValue);
}
BOOL CDYRecordSetEx::GetFieldValue(LPCTSTR lpszFieldName, double *pValue)
{
	return m_pRecordSet->GetFieldValue(lpszFieldName, pValue);
}
BOOL CDYRecordSetEx::GetFieldValue(LPCTSTR lpszFieldName, DWORD *pValue)
{
	return m_pRecordSet->GetFieldValue(lpszFieldName, pValue);
}
BOOL CDYRecordSetEx::GetFieldValue(LPCTSTR lpszFieldName, COleDateTime *pValue)
{
	return m_pRecordSet->GetFieldValue(lpszFieldName, pValue);
}
BOOL CDYRecordSetEx::GetFieldValue(LPCTSTR lpszFieldName, _variant_t *pValue)
{
	return m_pRecordSet->GetFieldValue(lpszFieldName, pValue);
}
BOOL CDYRecordSetEx::GetFieldValue(LPCTSTR lpszFieldName, LPVOID pValue)
{
	return m_pRecordSet->GetFieldValue(lpszFieldName, pValue);
}

BOOL CDYRecordSetEx::MoveNext()
{
	return m_pRecordSet->MoveNext();
}
BOOL CDYRecordSetEx::MoveFirst()
{
	return m_pRecordSet->MoveFirst();
}
BOOL CDYRecordSetEx::Move(long nIndex)
{
	return m_pRecordSet->Move(nIndex);
}
BOOL CDYRecordSetEx::Open(CString &strSQL, BOOL bUseClient, LockTypeEnum LockType, CString& strResponse)
{
	_AUTO_LOCK lock(&g_DYRecordSetExLock);

	CString strOutput;
	CString strTime;
	COleDateTime odt;
	if(g_bSQLTrace)
	{
		strTime = COleDateTime::GetCurrentTime().Format(FORMAT_ODT2STRING);
		strOutput.Format(_T("Open SQL...[%s],[%s]\n"), strSQL, strTime);
		OutputDebugString(strOutput);	
	}

	//临时增加，处理泰文检索
	//m_pDBEx->AddNWord(strSQL);
	
	DWORD dwStart = ::GetTickCount();
	
	BOOL bOK = m_pRecordSet->Open(strSQL, bUseClient, LockType, strResponse);

	DWORD dwUsed = ::GetTickCount() - dwStart;
	
	if(g_bSQLTrace)
	{
		strTime = COleDateTime::GetCurrentTime().Format(FORMAT_ODT2STRING);
		strOutput.Format(_T("Open SQL End,[%s]. Executetime used: [%d] milliseconds.\n"), strTime, dwUsed);
		OutputDebugString(strOutput);	
	}
	return bOK;
}
BOOL CDYRecordSetEx::Open(CString &strSQL, CursorLocationEnum CursorLocation,
CursorTypeEnum CursorType, LockTypeEnum LockType, CommandTypeEnum CommaonType)
{
	_AUTO_LOCK lock(&g_DYRecordSetExLock);

	CString strOutput;
	CString strTime;
	COleDateTime odt;
	if(g_bSQLTrace)
	{
		strTime = COleDateTime::GetCurrentTime().Format(FORMAT_ODT2STRING);
		strOutput.Format(_T("Open SQL...[%s],[%s]\n"), strSQL, strTime);
		OutputDebugString(strOutput);	
	}

	//临时增加，处理泰文检索
	//m_pDBEx->AddNWord(strSQL);
	
	DWORD dwStart = ::GetTickCount();

	BOOL bOK = m_pRecordSet->Open(strSQL, CursorLocation, CursorType, LockType, CommaonType);

	DWORD dwUsed = ::GetTickCount() - dwStart;

	if(g_bSQLTrace)
	{
		strTime = COleDateTime::GetCurrentTime().Format(FORMAT_ODT2STRING);
		strOutput.Format(_T("Open SQL End,[%s]. Executetime used: [%d] milliseconds.\n"), strTime, dwUsed);
		OutputDebugString(strOutput);	
	}
	return bOK;
	
}
BOOL CDYRecordSetEx::OpenByStoredProc(CString &strProcName)
{
	CString strOutput;
	CString strTime;
	COleDateTime odt;
	if(g_bSQLTrace)
	{
		strTime = COleDateTime::GetCurrentTime().Format(FORMAT_ODT2STRING);
		strOutput.Format(_T("Open by StoredProc...[%s],[%s]\n"), strProcName, strTime);
		OutputDebugString(strOutput);	
	}
	
	BOOL bOK = m_pRecordSet->OpenByStoredProc(strProcName);
	if(g_bSQLTrace)
	{
		strTime = COleDateTime::GetCurrentTime().Format(FORMAT_ODT2STRING);
		strOutput.Format(_T("Open by StoredProc End,[%s]\n"), strTime);
		OutputDebugString(strOutput);	
	}
	return bOK;
	
}
BOOL CDYRecordSetEx::Close()
{
	return m_pRecordSet->Close();
}
BOOL CDYRecordSetEx::OpenSchema(SchemaEnum QueryType)
{
	CString strOutput;
	CString strTime;
	COleDateTime odt;
	if(g_bSQLTrace)
	{
		strTime = COleDateTime::GetCurrentTime().Format(FORMAT_ODT2STRING);
		strOutput.Format(_T("Open Schema...[%d],[%s]\n"), QueryType, strTime);
		OutputDebugString(strOutput);	
	}
	
	BOOL bOK = m_pRecordSet->OpenSchema(QueryType);
	if(g_bSQLTrace)
	{
		strTime = COleDateTime::GetCurrentTime().Format(FORMAT_ODT2STRING);
		strOutput.Format(_T("Open Schema End,[%s]\n"), strTime);
		OutputDebugString(strOutput);	
	}
	return bOK;
	
}
void CDYRecordSetEx::SetAbsolutePage(int nPage)
{
	m_pRecordSet->SetAbsolutePage(nPage);
}
	
BOOL CDYRecordSetEx::OpenByPage(CString &strSQL, int nCurPage,//0 based
							  int nItemsPerPage,
							  BOOL bUseClient ,
							  LockTypeEnum LockType ,
							  CString &strResponse ) 
{
	if(m_pDBEx == NULL || m_pRecordSet == NULL)
		return FALSE;	
	CD_DB_TYPE dbType = m_pDBEx->GetDBType();
	CString strSQLTop = GetSQLSelectTop(strSQL,dbType,
		(nCurPage + 1)*nItemsPerPage,
		nCurPage*nItemsPerPage);
	BOOL bOK = Open(strSQLTop,bUseClient,LockType,strResponse);
	if(nCurPage != 0 && bOK && !IsEOF() &&  dbType != CDDT_ORACLE)
	{
		bOK = Move(nItemsPerPage*nCurPage);
	}		
	return bOK;
}

BOOL CDYRecordSetEx::OpenByPage(CString &strSQL, int nCurPage,//0 based
							  int nItemsPerPage,
							  CursorLocationEnum CursorLocation,
							  CursorTypeEnum CursorType ,//= adOpenKeyset, 
							  LockTypeEnum LockType ,//= adLockOptimistic,
							  CommandTypeEnum CommaonType// = adCmdUnspecified
							  )
{
	if(m_pDBEx == NULL || m_pRecordSet == NULL)
		return FALSE;	
	CD_DB_TYPE dbType = m_pDBEx->GetDBType();
	CString strSQLTop = GetSQLSelectTop(strSQL,dbType,
		(nCurPage + 1)*nItemsPerPage,
		nCurPage*nItemsPerPage);
	
	BOOL bOK = Open(strSQLTop,CursorLocation,CursorType,LockType,CommaonType);
	
	if(nCurPage != 0 && bOK && !IsEOF() &&  dbType != CDDT_ORACLE)
	{
		bOK = Move(nItemsPerPage*nCurPage);
	}		
	return bOK;
}
//#endif	//_USE_CDDATABASE_