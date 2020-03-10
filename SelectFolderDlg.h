
/*
�ļ���SelectFolderDlg.h
˵�����ṩһ��ѡ���ļ��еĶԻ���
*/

#ifndef SELECT_FOLDER_DLG_H
#define SELECT_FOLDER_DLG_H


#ifndef BIF_NEWDIALOGSTYLE
#define  BIF_NEWDIALOGSTYLE  0x0040
#endif

class CSelectFolderDlg
{
public:
	CSelectFolderDlg(){}
	~CSelectFolderDlg(){}
	//����һ��ѡ���ļ��еĶԻ��򣬷�����ѡ·��
	static CString Show()
	{
		TCHAR			lpszPath[MAX_PATH];
		CString			strFolder = TEXT("");
		LPMALLOC		lpMalloc;
		BROWSEINFO		sInfo;
		LPITEMIDLIST	lpidlBrowse;
		//CString			strInitFolder;

		if (::SHGetMalloc(&lpMalloc) != NOERROR)
			return FALSE;

		::ZeroMemory(&sInfo, sizeof(BROWSEINFO));
		sInfo.pidlRoot = 0;
		sInfo.pszDisplayName = lpszPath;
		sInfo.lpszTitle = _T("��ѡ����·����");
		sInfo.ulFlags = BIF_DONTGOBELOWDOMAIN | BIF_RETURNONLYFSDIRS | BIF_NEWDIALOGSTYLE | BIF_EDITBOX;
		sInfo.lpfn = NULL;
		//sInfo.lParam   = (LPARAM)strInitFolder.GetBuffer(0);
		// ��ʾ�ļ���ѡ��Ի���
		lpidlBrowse = ::SHBrowseForFolder(&sInfo);
		if (lpidlBrowse != NULL)
		{
			// ȡ���ļ�����
			if (::SHGetPathFromIDList(lpidlBrowse, lpszPath))
			{
				strFolder = lpszPath;
			}
		}
		if (lpidlBrowse != NULL)
		{
			::CoTaskMemFree(lpidlBrowse);
		}

		lpMalloc->Release();

		return strFolder;

	}

};

#endif