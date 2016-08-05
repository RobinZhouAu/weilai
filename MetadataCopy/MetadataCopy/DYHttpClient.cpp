// DYHttpClient.cpp: implementation of the CDYHttpClient class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "DYHttpClient.h"
#include "AfxPriv.h"
#include "CharSetTransferMacro.h"
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDYHttpClient::CDYHttpClient()
{
	m_strServerIP = "";
	m_nPort = 80;
	m_pConnect = NULL;
	m_pReadBuffer = NULL;
	m_nLength = 0;
	m_dwResponseCodePage = CP_GBK;
}

CDYHttpClient::~CDYHttpClient()
{
	if(m_pConnect)
	{
		delete m_pConnect;
		m_pConnect = NULL;
	}	
	if(m_pReadBuffer)
	{
		free(m_pReadBuffer);
		m_pReadBuffer = NULL;
	}
	
}

void CDYHttpClient::SetResponseCodePage(DWORD dwResponseCodePage)
{
	m_dwResponseCodePage = dwResponseCodePage;
}
DWORD CDYHttpClient::GetResponseCodePage()
{
	return m_dwResponseCodePage;
}

BOOL CDYHttpClient::SendRequest(LPCTSTR lpszURL, CString &strSend, CString &strResponse,
								BOOL bTransfer, BOOL bPost)
{
	strResponse = _T("");
	if(!m_pConnect) 
		m_pConnect = m_session.GetHttpConnection(m_strServerIP,	m_nPort);
	if(!m_pConnect)
	{
		OutputDebugString(_T("m_session.GetHttpConnection return NULL \r\n"));
		return FALSE;
	}
	CInternetSession* pSession = m_pConnect->GetSession();
	HINTERNET hIntnet = NULL;
	if(pSession)
		hIntnet = (HINTERNET)(*pSession);
	if(pSession == NULL || hIntnet == NULL)
	{
		if(pSession == NULL)
			OutputDebugString(_T("m_pConnect->GetSession return NULL \r\n"));
		else
			OutputDebugString(_T("m_pConnect->GetSession return NULL handle \r\n"));
		deleteC(m_pConnect);
		m_pConnect = m_session.GetHttpConnection(m_strServerIP,	m_nPort);
		if(!m_pConnect)
		{
			OutputDebugString(_T("m_session.GetHttpConnection return NULL \r\n"));
			return FALSE;
		}
	}
	
	CString strOut;
	CHttpFile *pHttpFile = m_pConnect->OpenRequest(
							 bPost? CHttpConnection::HTTP_VERB_POST : CHttpConnection::HTTP_VERB_GET,
							 lpszURL,
							 NULL,1,NULL,NULL,
							 INTERNET_FLAG_EXISTING_CONNECT|INTERNET_FLAG_NO_CACHE_WRITE); 
	if(pHttpFile == NULL)
	{
		OutputDebugString(_T("OpenRequest return NULL CHttpFile pointer\r\n"));
		return FALSE;
	}
	CString strHeader = _T("Accept: */*");
	BOOL bOK;
	bOK = pHttpFile->AddRequestHeaders(strHeader);
	strHeader = _T("Content-Type: text/html; charset=GB2312");
	bOK = pHttpFile->AddRequestHeaders(strHeader);		
	//send request
	try
	{
		int nLength;
#ifdef _UNICODE
		//transfer unicode as ansi
		USES_MYCP_CONVERSION;
		LPSTR lpszData = W2A(strSend);
		nLength = strlen(lpszData);
		bOK = pHttpFile->SendRequestEx(nLength);		
		if(nLength > 0)
			pHttpFile->Write((LPCTSTR)lpszData,nLength);   
		bOK = pHttpFile->EndRequest();		
#else
		nLength = strSend.GetLength();
		bOK = pHttpFile->SendRequestEx(nLength);		
		if(nLength > 0)
			pHttpFile->WriteString(strSend);   
		bOK = pHttpFile->EndRequest();	
#endif	
		
		CString strLine;
		bOK = pHttpFile->SetReadBufferSize(4096);
		
		if(m_pReadBuffer)
		{
			free(m_pReadBuffer);
			m_pReadBuffer = NULL;
		}
		m_pReadBuffer = (PBYTE)malloc(4097);
		m_pReadBuffer[0] = '\0';
		PBYTE pBuffer = m_pReadBuffer;
		m_nLength = 0;
		while( (nLength = pHttpFile->Read(pBuffer,4096) ) > 0 )
		{		
			m_nLength += nLength;
			m_pReadBuffer = (PBYTE)realloc(m_pReadBuffer, m_nLength + 4097);
			pBuffer = m_pReadBuffer + m_nLength;
		}	
		m_pReadBuffer[m_nLength] = '\0';
		
#ifdef _UNICODE
		if(bTransfer)
		{
			DWORD dwNum = MultiByteToWideChar (m_dwResponseCodePage, 0, (LPCSTR)m_pReadBuffer, -1, NULL, 0);
			wchar_t *pwText;
			pwText = new wchar_t[dwNum];
			if(!pwText)
				delete []pwText;
			MultiByteToWideChar (m_dwResponseCodePage, 0, (LPCSTR)m_pReadBuffer, -1, pwText, dwNum);
			strResponse = pwText;
			delete []pwText;
		}
			//strResponse = CP2W((LPCSTR)m_pReadBuffer,m_dwResponseCodePage);
		else
			strResponse = m_pReadBuffer;
#else
		strResponse = m_pReadBuffer;
#endif	
//		while(pHttpFile->ReadString(strLine))//cannot read big response
//		{
//			strLine.TrimLeft();
//			strResponse += strLine;
//		}	
	}
	catch (CException *e) 
	{
		TCHAR msg[1024];
		e->GetErrorMessage(msg,1024);
		strOut.Format(_T("DB: %s"),msg);	
		OutputDebugString(strOut);	
		pHttpFile->Close();
		delete pHttpFile;
		pHttpFile = NULL;
		e->Delete();
		return FALSE;
	}
	
	pHttpFile->Close();
	delete pHttpFile;
	pHttpFile = NULL;
	strResponse.TrimLeft();
	strResponse.TrimRight();
	if(m_nLength == 0)
	{
		deleteC(m_pConnect);//get new connect next time
		OutputDebugString(_T("strResponse is empty, get new connect next time  \r\n"));
	}
	return TRUE;	
}

BOOL CDYHttpClient::ParseHttpInfo(
						  LPCTSTR lpszFullUrl,// http://www.google.com:80/search?q=VC 端口信息':80'可以没有，默认80
						  CString &strIP,//www.google.com
						  INTERNET_PORT &nPort,//80
						  CString &strPartUrl///search?q=VC
						  )
{
	strIP.Empty();
	strPartUrl.Empty();

	CString strUrl = lpszFullUrl;
	strUrl.TrimLeft();
	strUrl.TrimRight();
	CString strUrlOld = strUrl;
	strUrl.MakeUpper();
	CString strHttp = _T("HTTP://");
	int nLength = strHttp.GetLength();
	if(strUrl.Find(strHttp) < 0)
	{
		return FALSE;
	}
	int nPos1 = strUrl.Find(TCHAR(':'),nLength);
	int nPos2 = strUrl.Find(TCHAR('/'),nLength );
	if(nPos2 < 0)
		nPos2 = strUrl.GetLength();
	CString strPort;
	if(nPos1 > 0)// has port
	{
		strIP = strUrlOld.Mid(nLength ,nPos1 - nLength );
		strPort = strUrlOld.Mid(nPos1+1,nPos2 - nPos1 - 1);
		nPort = _ttol(strPort);
	}
	else//no port
	{
		strIP = strUrlOld.Mid(7,nPos2 - 7);
	}
	strPartUrl = strUrlOld.Mid(nPos2);
	return TRUE;
}
