#ifndef _AUTO_LOCK_UNLOCK_H__
#define _AUTO_LOCK_UNLOCK_H__
   
#include <afxMt.h>
struct _AUTO_LOCK
{
	_AUTO_LOCK(CCriticalSection *pLock)
	{
		m_pLock = NULL;
		if(pLock)
		{
			m_pLock = pLock;
			m_pLock->Lock();
		}
	};
	~_AUTO_LOCK()
	{
		if(m_pLock)
		{
			m_pLock->Unlock();
		}
	};

	CCriticalSection *m_pLock;

};

template <class PtrType>
inline void g_DelPtrArray(CArray<PtrType*,PtrType*&> &aryPtr)
{
	int nItems = aryPtr.GetSize();
	PtrType *pPtr = NULL;
	for(int i = 0; i < nItems; i++)
	{		
		deleteC(aryPtr[i]);
	}
	aryPtr.RemoveAll();
}

inline void g_ParseStrings(LPCTSTR lpszString, 
					CStringArray &aryParsedString,
					LPCTSTR lpszSep) 
{
	aryParsedString.RemoveAll();
	CString strValues = lpszString;
	CString strValue;
	strValue = _tcstok(strValues.GetBuffer(strValues.GetLength()), lpszSep);
	while (!strValue.IsEmpty()) 
	{
		aryParsedString.Add(strValue);
		strValue = _tcstok(NULL, lpszSep);
	}			
}

#endif