//CONNECT_FUCTION.CPP
/*************************************
/从文件中获取数据库信息
*************************************/
#include "stdafx.h"
#include "DYDatabaseEx.h"
#include "base64.h"
#include "ConnectFuc.h"

CString Encode(LPCTSTR lpszWord)
{
	CString strNew = lpszWord;

#ifdef _UNICODE
	USES_CONVERSION;
	char *pBuf = W2A(lpszWord);
#else
	char *pBuf = (char*)lpszWord;
#endif
	int nLength = strlen(pBuf);
	BYTE *pBufOut = NULL;
	int nLength2;	
	CBase64::Encode((BYTE*)pBuf,nLength,pBufOut,nLength2);	
#ifdef _UNICODE
	char *pBuf2 = (char*)pBufOut;
	strNew = A2W(pBuf2);
#else	
	strNew = pBufOut;
#endif
	delete pBufOut;		
	strNew.TrimRight(TCHAR('='));
	return strNew;
}
CString Decode(LPCTSTR lpszWord)
{
	CString strNew = lpszWord;
#ifdef _UNICODE
	USES_CONVERSION;
	char *pBuf = W2A(lpszWord);
#else
	char *pBuf = (char*)lpszWord;
#endif
	int nLength = strlen(pBuf);
	BYTE *pBufOut = NULL;
	int nLength2;	
	CBase64::Decode((BYTE*)pBuf,nLength,pBufOut,nLength2);	
#ifdef _UNICODE
	char *pBuf2 = (char*)pBufOut;
	strNew = A2W(pBuf2);
#else	
	strNew = pBufOut;
#endif
	delete pBufOut;	
	strNew.TrimRight(1);
	return strNew;
}
DY_EXT_API CString EncodeString(LPCTSTR lpszWord)
{
	CString strNew = Encode(lpszWord);
	strNew = Encode(strNew);
	return strNew;
}
DY_EXT_API CString DecodeString(LPCTSTR lpszWord)
{
	CString strNew = Decode(lpszWord);
	strNew = Decode(strNew);
	return strNew;
}
BOOL GetDBConnectionFromFile(CDDBCONNECT *pConnection,
							CString lpszRegFile
							)
{
	//get con file info from register
	if(pConnection == NULL)
		return FALSE;	
	if(lpszRegFile.IsEmpty())
		return FALSE;
	//read file
	CFile file;
	if(!file.Open( lpszRegFile, CFile::modeRead|CFile::typeBinary))
		return FALSE;
	CArchive ar( &file, CArchive::load);
	int nVersion;
	try{
		ar >> nVersion;
		ar >> pConnection->strDataSource;
		ar >> pConnection->strDatabase;
		int nType = 0;
		ar >> nType;//
		pConnection->enumType = (CD_DB_TYPE)nType;
		ar >> pConnection->strUID;
		ar >> pConnection->strPWD;
	}
	catch (CException *e) {
		e->Delete();
		return FALSE;
	}
	catch (...){
		AfxMessageBox(_T("Wrong file format"));
		//DYMessageBox(
		//	DYTranslateEx(_T("IDS_ERROR_FILE_TYPE"),_T("Wrong File Type!"),XC_DICTIONARY_ID_CONNECTIONMANAGER)
		//	);
	}
	//decode
	pConnection->strDataSource = DecodeString(pConnection->strDataSource);
	pConnection->strDatabase = DecodeString(pConnection->strDatabase);
	pConnection->strUID = DecodeString(pConnection->strUID);
	pConnection->strPWD = DecodeString(pConnection->strPWD);
	return TRUE;
}

BOOL SetDBConnectionToFile(CDDBCONNECT *pConnection,
						   CString lpszRegFile 
						   )
{
	CDDBCONNECT con;
	if(pConnection != NULL)
		con = *pConnection;
	//save into file
	CFile file;
	if(!file.Open(lpszRegFile, CFile::modeCreate|CFile::modeWrite|CFile::typeBinary))
	{
		CString strMsg;
		strMsg.Format(_T("Cannot open file [%s] to write!"),lpszRegFile);
		AfxMessageBox(strMsg);				
		return FALSE;
	}
	CArchive ar( &file, CArchive::store);
	//encode
	con.strDataSource = EncodeString(con.strDataSource);
	con.strDatabase = EncodeString(con.strDatabase);
	con.strUID = EncodeString(con.strUID);
	con.strPWD = EncodeString(con.strPWD);
	
	ar << (int)0;//version
	ar << con.strDataSource;
	ar << con.strDatabase;
	ar << (int)con.enumType;
	ar << con.strUID;
	ar << con.strPWD;
	ar.Close();

	file.Close();

	return TRUE;
}
