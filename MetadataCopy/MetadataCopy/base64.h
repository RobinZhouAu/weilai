// Base641.h: interface for the CBase64 class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BASE641_H__0CDB47E1_F0F8_483E_8A62_ED7F500E04EE__INCLUDED_)
#define AFX_BASE641_H__0CDB47E1_F0F8_483E_8A62_ED7F500E04EE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//#ifdef _USE_HTTP

class DY_EXT_CLASS CBase64  
{
public:
	CBase64();
	virtual ~CBase64();
	static BOOL Encode(LPBYTE pbufIn, int nLength, LPBYTE &pBufOut, int &nLengthOut);
	static BOOL Decode(LPBYTE pbufIn, int nLength, LPBYTE &pBufOut, int &nLengthOut);

protected:
	static	BOOL encodeblock( unsigned char in[3], unsigned char out[4], int len );
	static BOOL decodeblock( unsigned char in[4], unsigned char out[3] );
/*
** Translation Table as described in RFC1113
*/
	static const char cb64[];//="ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
/*
** Translation Table to decode (created by author)
*/
static const char cd64[];//="|$$$}rstuvwxyz{$$$$$$$>?@ABCDEFGHIJKLMNOPQRSTUVW$$$$$$XYZ[\\]^_`abcdefghijklmnopq";

		

};
//#endif//#ifdef _USE_HTTP
void DY_EXT_CLASS g_DYITTranslate(LPCTSTR pszID, LPTSTR pszDest, int nSize, int nID = -1);

#endif // !defined(AFX_BASE641_H__0CDB47E1_F0F8_483E_8A62_ED7F500E04EE__INCLUDED_)
