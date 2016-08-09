///////////////////////////////////////////////////////////////////////////////////////////
// 文件名：FileShare.h
// 创建者：周海滨
// 创建时间：2001/06/28
// 内容描述：共享的文件函数及宏定义
///////////////////////////////////////////////////////////////////////////////////////////
#ifndef _FILESHARE_H_
#define _FILESHARE_H_

////----------------文件类型-----------------------------------------------//
////编目系统文件类型
//#define FILE_TYPE_ALL			0x00000000		//*.*
//#define FILE_TYPE_CIS			0x00000001		//*.CIS 编目界面配置文件
//#define FILE_TYPE_LCF			0x00000002		//*.LCF 局部数据文件
//#define FILE_TYPE_OPT			0x00000004		//*.OPT 工作区选项文件
////视频文件类型
//#define FILE_TYPE_AVI			0x00000010
//#define FILE_TYPE_MPG			0x00000020
//#define FILE_TYPE_DAT			0x00000040		
////图像文件类型
//#define FILE_TYPE_DIB			0x00001000
//#define FILE_TYPE_JPG			0x00002000
////音频文件类型
//#define FILE_TYPE_WAV			0x00100000
//#define FILE_TYPE_MID			0x00200000
////其他文件类型
//#define FILE_TYPE_TXT			0x01000000
//#define FILE_TYPE_DOC			0x02000000
//#define FILE_TYPE_PDF			0x04000000
//
//#define FILE_TYPE_VIDEO		0x00000FF0		//所有视频类型
//#define FILE_TYPE_IMAGE		0x000FF000		//所有图像类型
//#define FILE_TYPE_AUDIO		0x00F00000		//所有音频类型
//#define FILE_TYPE_DOCUMENT	0xFF000000		//所有其它文档类型
//
//#define FILE_TYPE_ALL_FILTER	"所有文件(*.*)|*.*|"
//#define FILE_TYPE_CIS_FILTER	"编目界面配置文件(*.CIS)|*.CIS|"
//#define FILE_TYPE_LCF_FILTER	"编目局部数据文件(*.LCF)|*.LCF|"
//#define FILE_TYPE_OPT_FILTER	"工作区选项文件(*.OPT)|*.OPT|"
//
//#define FILE_TYPE_AVI_FILTER	"AVI视频文件(*.AVI)|*.AVI|"
//#define FILE_TYPE_MPG_FILTER	"MPG视频文件(*.MPG)|*.MPG|"
//#define FILE_TYPE_DAT_FILTER	"DAT视频文件(*.DAT)|*.DAT|"
//
//#define FILE_TYPE_DIB_FILTER	"非压缩图像文件(*.BMP;*.TIF)|*.BMP;*.TIF|"
//#define FILE_TYPE_JPG_FILTER	"压缩图像文件(*.GIF;*.JPG;*.TGA;*.PCX)|*.GIF;*.JPG;*.PCX;*.TGA|"
//
//#define FILE_TYPE_WAV_FILTER	"非压缩音频文件(*.WAV)|*.WAV|"
//#define FILE_TYPE_MID_FILTER	"压缩音频文件(*.MID;*.MP3)|*.MID;*.MP3|"
//
//#define FILE_TYPE_TXT_FILTER	"文本文件(*.TXT)|*.TXT|"
//#define FILE_TYPE_DOC_FILTER	"MS OFFICE 文档(*.DOC;*.XLS;*.MDB)|*.DOC;*.XLS;*.MDB|"
//#define FILE_TYPE_PDF_FILTER	"PDF文档(*.PDF)|*.PDF|"
//
//
//#define FILEEXT_CIS			".CIS"
//#define FILEEXT_LCF			".LCF"
//
//#define FILEEXT_BMP			".BMP"
//#define FILEEXT_JPG			".JPG"
//
//#define IS_VIDEOFILE(type)		((type & FILE_TYPE_VIDEO) != 0x00000000)
//#define IS_IMAGEFILE(type)		((type & FILE_TYPE_IMAGE) != 0x00000000)
//#define IS_AUDIOFILE(type)		((type & FILE_TYPE_AUDIO) != 0x00000000)
//#define IS_DOCUMENTFILE(type)	((type & FILE_TYPE_DOCUMENT) != 0x00000000)
//
//#define FILETYPE_SYSTEM		0
//#define FILETYPE_VIDEO		1
//#define FILETYPE_IMAGE		2
//#define FILETYPE_AUDIO		3
//#define FILETYPE_DOCUMENT	4
////--------End of 文件类型-----------------------------------------------//


BOOL DY_EXT_API g_CreateDirectory(LPCTSTR lpszDirectory);
BOOL DY_EXT_API g_EmptyDirectory(LPCTSTR lpszDirectory);
CString DY_EXT_API g_RemovePathFromFileName(LPCTSTR lpszFileName);
CString DY_EXT_API g_GetFilterByType(int nFileType);
ULONGLONG DY_EXT_API g_GetFileSize(LPCTSTR lpszFileName);
BOOL DY_EXT_API g_GetFileTime(LPCTSTR lpszFileName, COleDateTime &odtLastWrite);
void DY_EXT_API g_ReportFileError(CFileException *fe);
CString DY_EXT_API g_GetWin32ErrMsg();					//取得Win32错误的信息文本
BOOL DY_EXT_API g_IsFileExist(LPCTSTR lpszFileName);
BOOL DY_EXT_API g_SplitFile(CString &strFullFileName, CString &strPath, CString &strFileName);
ULONGLONG DY_EXT_API g_GetDirectorySize(CString strPath, CString strSearch, int& nFileCount, int& nDirCount);
ULONGLONG DY_EXT_API g_GetDiskFreeSpace(CString strDriverPath);
// add by zjq
BOOL DY_EXT_API g_FileCanbeDel(CString strFileName);
#endif _FILESHARE_H_	