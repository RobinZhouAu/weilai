
// MetadataCopy.h : PROJECT_NAME 应用程序的主头文件
//

#pragma once

#ifndef __AFXWIN_H__
	#error "在包含此文件之前包含“stdafx.h”以生成 PCH 文件"
#endif

#include "resource.h"		// 主符号

#include "OutPut.h"
#include "register.h"
#include "FileShare.h"
#include "DYRecordSetEx.h"

enum XC_CATALOG_STATUS
{
	///没有编目
	XC_CS_NOTSTART = 0,
	///正在编目
	XC_CS_PROCESSING,
	///结束
	XC_CS_END,
	///审核完成
	XC_AUDIT_END,


	XC_CS_COUNT,
};

enum XC_FILE_STATUS
{
	XC_FS_NOTCAPTURE = 0,		//未采集
	XC_FS_ONLINE,				//在线
	XC_FS_ONLINE_NEARLINE,		//在线+近线
	XC_FS_NEARLINE				//近线
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
// 有关此类的实现，请参阅 MetadataCopy.cpp
//

class CMetadataCopyApp : public CWinApp
{
public:
	CMetadataCopyApp();

// 重写
public:
	virtual BOOL InitInstance();

// 实现

	DECLARE_MESSAGE_MAP()
};

extern CMetadataCopyApp theApp;