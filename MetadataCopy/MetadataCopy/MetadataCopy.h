
// MetadataCopy.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������

#include "OutPut.h"
#include "register.h"
#include "FileShare.h"
#include "DYRecordSetEx.h"

enum XC_CATALOG_STATUS
{
	///û�б�Ŀ
	XC_CS_NOTSTART = 0,
	///���ڱ�Ŀ
	XC_CS_PROCESSING,
	///����
	XC_CS_END,
	///������
	XC_AUDIT_END,


	XC_CS_COUNT,
};

enum XC_FILE_STATUS
{
	XC_FS_NOTCAPTURE = 0,		//δ�ɼ�
	XC_FS_ONLINE,				//����
	XC_FS_ONLINE_NEARLINE,		//����+����
	XC_FS_NEARLINE				//����
};

enum FILE_STORAGE_STATUS
{
	FSS_ABSENT = 0,
	FSS_PRESENT = 1,
	FSS_ONLINE = 2,
	FSS_ONLINE_NEARLINE = 3,
	FSS_ONLINE_NEARLINE_BACKUPED = 4,
	FSS_NEARLINE_BACKUPED = 5,
	FSS_DAMANGED = 6,

	FSS_COUNT,
};

typedef struct _OBJECT_ITEM
{
	CString strSourceObjectID;
	CString strTargetObjectID;
	CString strSourceName;
	CString strTargetName;
	CString strUserbit;
	BOOL bAlreadyExist;
	BOOL bFindInSource;
	BOOL bFindInTarget;
	BOOL bFileInTarget;
	CString strPathID;
	CString strHighFileName;
	int nSourceCatalogStatus;
	int nTargetCatalogStatus;
	int nTargetFileStatus;
	int nTargetNeedRepair;
	_OBJECT_ITEM()
	{
		bAlreadyExist = FALSE;
		bFindInSource = FALSE;
		bFindInTarget = FALSE;
		bFileInTarget = FALSE;
		nSourceCatalogStatus = 0;
		nTargetCatalogStatus = 0;
		nTargetFileStatus = 0;
		nTargetNeedRepair = 0;
	}
}OBJECTITEM;
typedef CArray<OBJECTITEM, OBJECTITEM&> CObjectItemArray;

extern CDYDatabaseEx g_dbSource;
extern CDYDatabaseEx g_dbTarget;

extern COutPut g_output;

// CMetadataCopyApp:
// �йش����ʵ�֣������ MetadataCopy.cpp
//

class CMetadataCopyApp : public CWinApp
{
public:
	CMetadataCopyApp();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CMetadataCopyApp theApp;