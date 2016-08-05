// UnicodeStdioFile.h: interface for the CUnicodeStdioFile class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_UNICODESTDIOFILE_H__4A558472_DA8D_4CCB_8F6D_DDB2EEB9D509__INCLUDED_)
#define AFX_UNICODESTDIOFILE_H__4A558472_DA8D_4CCB_8F6D_DDB2EEB9D509__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class DY_EXT_CLASS CUnicodeStdioFile : public CStdioFile  
{
public:
	CUnicodeStdioFile();
	virtual ~CUnicodeStdioFile();
public:
	// reading and writing strings
	virtual void WriteString(LPCTSTR lpsz);
	virtual LPTSTR ReadString(LPTSTR lpsz, UINT nMax);
	virtual BOOL ReadString(CString& rString);
};
/*
#ifdef _UNICODE
	#define CStdioFile CUnicodeStdioFile
#endif
*/
#endif // !defined(AFX_UNICODESTDIOFILE_H__4A558472_DA8D_4CCB_8F6D_DDB2EEB9D509__INCLUDED_)
