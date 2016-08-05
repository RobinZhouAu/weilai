// DYHttpClient.h: interface for the CDYHttpClient class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DYHTTPCLIENT_H__48706A97_FAE0_45F1_BF7E_EA2898E549A6__INCLUDED_)
#define AFX_DYHTTPCLIENT_H__48706A97_FAE0_45F1_BF7E_EA2898E549A6__INCLUDED_

#include <afxinet.h>//mfc internet class

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class DY_EXT_CLASS CDYHttpClient  
{
public:
	CDYHttpClient();
	virtual ~CDYHttpClient();
	CString m_strServerIP;
	INTERNET_PORT m_nPort;
	BOOL SendRequest(LPCTSTR lpszURL, CString &strSend, CString &strResp,
		BOOL bTransfer = TRUE//whether transfer the response buffer to Unicode strResp
		,BOOL bPost = TRUE//method : post or get
		);	
	CHttpConnection* m_pConnect;

	PBYTE m_pReadBuffer;//response buffer , valid only when UNICODE defined
	int m_nLength;//response buffer length, valid only when UNICODE defined and bTransfer = TRUE

	//called before SendRequest
	void SetResponseCodePage(DWORD dwResponseCodePage);
	DWORD GetResponseCodePage();//default is 936: GBK

	static BOOL ParseHttpInfo(
		LPCTSTR lpszFullUrl,// http://www.google.com:80/search?q=VC 端口信息':80'可以没有，默认80
		CString &strIP,//www.google.com
		INTERNET_PORT &nPort,//80
		CString &strPartUrl///search?q=VC
		);

protected:
	CInternetSession m_session;
	DWORD m_dwResponseCodePage;
	

};

#endif // !defined(AFX_DYHTTPCLIENT_H__48706A97_FAE0_45F1_BF7E_EA2898E549A6__INCLUDED_)
