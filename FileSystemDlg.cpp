
// FileSystemDlg.cpp: 实现文件
//

#include "pch.h"
#include "framework.h"
#include "FileSystem.h"
#include "FileSystemDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CFileSystemDlg 对话框


CFileSystemDlg::CFileSystemDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_FILESYSTEM_DIALOG, pParent)
	, m_IsDocFileSelected(FALSE)
	, m_IsXlsFileSelected(FALSE)
	, m_IsJpgFileSelected(FALSE)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CFileSystemDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TREE, m_tree);
	DDX_Control(pDX, IDC_LIST, m_list);
	DDX_Control(pDX, IDC_FILENAME_EDIT, m_edit);
	//DDX_Control(pDX, IDC_DocFile_RADIO, m_DocFileRadio);
	//DDX_Control(pDX, IDC_XlsFile_RADIO, m_XlsFileRadio);
	//DDX_Control(pDX, IDC_JpgFile_RADIO, m_JpgFileRadio);
	DDX_Control(pDX, IDC_FIND_BUTTON, m_FindButton);
	DDX_Radio(pDX, IDC_DocFile_RADIO, m_IsDocFileSelected);
	DDX_Radio(pDX, IDC_XlsFile_RADIO, m_IsXlsFileSelected);
	DDX_Radio(pDX, IDC_JpgFile_RADIO, m_IsJpgFileSelected);
}

BEGIN_MESSAGE_MAP(CFileSystemDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()

//	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE, &CFileSystemDlg::OnTvnSelchangedTree)
ON_NOTIFY(TVN_SELCHANGED, IDC_TREE, &CFileSystemDlg::OnTvnSelchangedTree)
ON_NOTIFY(NM_RCLICK, IDC_LIST, &CFileSystemDlg::OnNMRClickList)
ON_COMMAND(ID_LARGE_ICON, &CFileSystemDlg::OnLargeIcon)
ON_COMMAND(ID_SMALL_ICON, &CFileSystemDlg::OnSmallIcon)
ON_COMMAND(ID_DETAILS, &CFileSystemDlg::OnDetails)
ON_BN_CLICKED(IDC_FIND_BUTTON, &CFileSystemDlg::OnBnClickedFindButton)
ON_BN_CLICKED(IDC_DocFile_RADIO, &CFileSystemDlg::OnBnClickedDocfileRadio)
ON_BN_CLICKED(IDC_XlsFile_RADIO, &CFileSystemDlg::OnBnClickedXlsfileRadio)
ON_BN_CLICKED(IDC_JpgFile_RADIO, &CFileSystemDlg::OnBnClickedJpgfileRadio)
//ON_NOTIFY(LVN_ITEMACTIVATE, IDC_LIST, &CFileSystemDlg::OnLvnItemActivateList)
ON_BN_CLICKED(IDC_RENAME_BUTTON, &CFileSystemDlg::OnBnClickedRenameButton)
ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST, &CFileSystemDlg::OnLvnItemchangedList)
ON_EN_CHANGE(IDC_FILENAME_EDIT, &CFileSystemDlg::OnEnChangeFilenameEdit)
END_MESSAGE_MAP()


// CFileSystemDlg 消息处理程序

BOOL CFileSystemDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码

	m_tree.ModifyStyle(0, TVS_HASBUTTONS | TVS_HASLINES | TVS_LINESATROOT | TVS_SHOWSELALWAYS | TVS_DISABLEDRAGDROP);
	m_tree.InsertItem(_T("Dummy"));  // 添加一个虚拟根节点，以便能够接收 TVN_SELCHANGED 消息
	m_tree.SelectItem(m_tree.GetRootItem());  // 选中虚拟根节点

	FillDiskTree();

	m_list.ModifyStyle(LVS_TYPEMASK, LVS_REPORT);

	// 初始化文件类型变量
	m_IsDocFileSelected = false;
	m_IsXlsFileSelected = false;
	m_IsJpgFileSelected = false;

	// 初始化为无效的索引
	m_SelectedFileIndex = -1; 
	// 初始化当前路径
	m_CurrentFolderPath = _T("C:\\");
	
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CFileSystemDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
HCURSOR CFileSystemDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CFileSystemDlg::FillDiskTree()
{
	m_tree.DeleteAllItems();  // 清空 TreeView

	// 使用 Shell API 获取磁盘信息
	SHFILEINFO shfi;
	HIMAGELIST himlSysSmall = (HIMAGELIST)SHGetFileInfo(L"C:\\", 0, &shfi, sizeof(SHFILEINFO), SHGFI_SYSICONINDEX | SHGFI_SMALLICON);

	// 设置图标列表
	m_tree.SetImageList(CImageList::FromHandle(himlSysSmall), TVSIL_NORMAL);

	// 获取逻辑驱动器的位掩码
	DWORD dwDrives = GetLogicalDrives();

	// 遍历每个驱动器并插入到 TreeView 中
	for (int i = 0; i < 26; ++i) {
		if (dwDrives & (1 << i)) {
			CString strDrive;
			strDrive.Format(_T("%c:\\"), _T('A') + i);

			// 添加驱动器到 TreeView
			HTREEITEM hDriveItem = m_tree.InsertItem(strDrive, shfi.iIcon, shfi.iIcon);
			m_tree.SetItemData(hDriveItem, (DWORD_PTR)i);  // 将驱动器索引作为数据存储

			// 递归获取驱动器下的目录
			FillDirectories(hDriveItem, strDrive);
		}
	}
}

void CFileSystemDlg::FillDirectories(HTREEITEM hParentItem, const CString& strPath)
{
	 //只显示一级文件夹，资源消耗量小
	WIN32_FIND_DATA findFileData;
	HANDLE hFind = FindFirstFile(strPath + _T("*"), &findFileData);

	if (hFind != INVALID_HANDLE_VALUE) {
		do {
			if (findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY &&
				_tcscmp(findFileData.cFileName, _T(".")) != 0 &&
				_tcscmp(findFileData.cFileName, _T("..")) != 0) {
				CString* pPath = new CString(strPath + findFileData.cFileName + _T("\\"));

				HTREEITEM hDirItem = m_tree.InsertItem(findFileData.cFileName, 1, 1, hParentItem);
				m_tree.SetItemData(hDirItem, reinterpret_cast<DWORD_PTR>(pPath));
			}
		} while (FindNextFile(hFind, &findFileData) != 0);

		FindClose(hFind);
	}
}

//void CFileSystemDlg::FillDirectories(HTREEITEM hParentItem, const CString& strPath)
//{
// 
// //递归显示所有文件夹，资源消耗量大
//	WIN32_FIND_DATA findFileData;
//	HANDLE hFind = FindFirstFile((strPath + _T("*.*")).GetString(), &findFileData);
//
//	if (hFind == INVALID_HANDLE_VALUE)
//	{
//		return;
//	}
//
//	do
//	{
//		// 忽略当前目录和上级目录
//		if (_tcscmp(findFileData.cFileName, _T(".")) == 0 || _tcscmp(findFileData.cFileName, _T("..")) == 0)
//			continue;
//
//		// 如果是子目录，则递归添加到 TreeView
//		if (findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
//		{
//			CString strDirName = findFileData.cFileName;
//			CString strFullPath = strPath + strDirName + _T("\\");
//
//			// 添加子目录节点
//			HTREEITEM hChildItem = m_tree.InsertItem(strDirName, hParentItem);
//
//			// 设置图标
//			m_tree.SetItemImage(hChildItem, 1, 1);
//
//			// 递归填充子目录
//			FillDirectories(hChildItem, strFullPath);
//		}
//
//	} while (FindNextFile(hFind, &findFileData) != 0);
//
//	FindClose(hFind);
//
//}

void CFileSystemDlg::OnDestroy()
{
	// TODO: 在此处添加实现代码.
	// 清理树控件的存储
	HTREEITEM hItem = m_tree.GetRootItem();
	while (hItem != NULL) {
		CString* pPath = reinterpret_cast<CString*>(m_tree.GetItemData(hItem));
		delete pPath;
		hItem = m_tree.GetNextItem(hItem, TVGN_NEXT);
	}

	CDialogEx::OnDestroy();
}

HICON GetFileIcon(const CString& filePath, bool largeIcon)
{
	SHFILEINFO shFileInfo = {};

	// 获取文件图标
	DWORD flags = SHGFI_ICON | (largeIcon ? SHGFI_LARGEICON : SHGFI_SMALLICON);
	SHGetFileInfo(filePath, 0, &shFileInfo, sizeof(shFileInfo), flags);

	return shFileInfo.hIcon;
}


void CFileSystemDlg::OnTvnSelchangedTree(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码

	// 获取 Tree View 控件
	CTreeCtrl* pTreeCtrl = &m_tree;

	// 获取所选节点
	HTREEITEM hSelectedItem = pTreeCtrl->GetSelectedItem();
	if (hSelectedItem == nullptr)
		return;

	// 获取节点的完整路径
	m_strPath = GetItemPath(pTreeCtrl, hSelectedItem);
	TRACE(_T("m_strPath: %s\n"), m_strPath);

	CString strSelectedPath = GetItemPath(pTreeCtrl, hSelectedItem);

	// 填充 List Control
	FillFileList(strSelectedPath);

	
	*pResult = 0;
}


void CFileSystemDlg::FillFileList(const CString& strPath)
{
	// TODO: 在此处添加实现代码.

	m_CurrentFolderPath = strPath;

	//清空 List Control
	m_list.DeleteAllItems();
	m_list.InsertColumn(0, _T("文件名"), LVCFMT_CENTER, 120);
	m_list.InsertColumn(1, _T("文件类型"), LVCFMT_CENTER, 100);

	// 使用 FindFirstFile 和 FindNextFile 遍历文件
	WIN32_FIND_DATA findFileData;
	HANDLE hFind = FindFirstFile((strPath + _T("*.*")).GetString(), &findFileData);

	if (hFind != INVALID_HANDLE_VALUE)
	{
		do
		{
			// 忽略当前目录和上级目录
			if (_tcscmp(findFileData.cFileName, _T(".")) == 0 || _tcscmp(findFileData.cFileName, _T("..")) == 0)
				continue;

			// 添加文件信息到 List Control
			int nIndex = m_list.InsertItem(0, findFileData.cFileName);
			m_list.SetItemText(nIndex, 1, (findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) ? _T("文件夹") : _T("文件"));


		} while (FindNextFile(hFind, &findFileData) != 0);

		FindClose(hFind);
	}

}


CString CFileSystemDlg::GetItemPath(CTreeCtrl* pTreeCtrl, HTREEITEM hItem)
{
	// TODO: 在此处添加实现代码.

	CString strPath;

	// 获取所选节点的文本
	CString strItemText = pTreeCtrl->GetItemText(hItem);

	// 递归获取节点的父节点
	HTREEITEM hParentItem = pTreeCtrl->GetParentItem(hItem);
	if (hParentItem != nullptr)
	{
		// 如果不是根节点，继续递归
		strPath = GetItemPath(pTreeCtrl, hParentItem);
	}

	// 添加当前节点的文本到路径中
	strPath += strItemText;

	// 如果不是根节点，添加路径分隔符
	if (hParentItem != nullptr)
	{
		strPath += _T("\\");
	}

	// 调试输出
	//TRACE(_T("Selected Folder Path: %s\n"), strPath);

	return strPath;
}


void CFileSystemDlg::OnNMRClickList(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码

	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	if (pNMListView->iItem != -1)
	{
		CPoint pt; GetCursorPos(&pt);
		CMenu menu; menu.LoadMenu(IDR_MENU1);
		CMenu* popmenu; popmenu = menu.GetSubMenu(0);
		popmenu->TrackPopupMenu(TPM_RIGHTBUTTON, pt.x, pt.y, this);
	}

	*pResult = 0;
}

//右键菜单-大图标
void CFileSystemDlg::OnLargeIcon()
{
	// TODO: 在此添加命令处理程序代码

	m_list.ModifyStyle(LVS_TYPEMASK, LVS_ICON);

}

//右键菜单-小图标
void CFileSystemDlg::OnSmallIcon()
{
	// TODO: 在此添加命令处理程序代码

	//设置List Control的显示方式为小图标	
	m_list.ModifyStyle(LVS_TYPEMASK, LVS_SMALLICON);


}

//右键菜单-详细信息
void CFileSystemDlg::OnDetails()
{
	// TODO: 在此添加命令处理程序代码

	//设置List Control的显示方式为详细信息
	m_list.ModifyStyle(LVS_TYPEMASK, LVS_REPORT);

}


// 在 OnBnClickedFindButton 函数中处理查询按钮点击事件
void CFileSystemDlg::OnBnClickedFindButton()
{
	// 获取当前选定的目录
	HTREEITEM hSelectedItem = m_tree.GetSelectedItem();
	CString strPath = GetItemPath(&m_tree, hSelectedItem);

	// 查询当前目录下的指定文件类型
	QueryFiles(strPath);
}

// 在 QueryFiles 函数中执行文件查询，并更新 List Control
void CFileSystemDlg::QueryFiles(const CString& strPath)
{
	// 清空 List Control
	m_list.DeleteAllItems();

	// 使用 FindFirstFile 和 FindNextFile 遍历文件
	WIN32_FIND_DATA findFileData;
	HANDLE hFind = FindFirstFile((strPath + _T("*.*")).GetString(), &findFileData);

	if (hFind != INVALID_HANDLE_VALUE)
	{
		do
		{
			// 忽略当前目录和上级目录
			if (_tcscmp(findFileData.cFileName, _T(".")) == 0 || _tcscmp(findFileData.cFileName, _T("..")) == 0)
				continue;

			// 根据文件类型过滤文件
			if ((m_IsDocFileSelected && IsDocFile(findFileData.cFileName)) ||
				(m_IsXlsFileSelected && IsXlsFile(findFileData.cFileName)) ||
				(m_IsJpgFileSelected && IsJpgFile(findFileData.cFileName)))
			{
				// 添加文件信息到 List Control
				int nIndex = m_list.InsertItem(0, findFileData.cFileName);
				m_list.SetItem(nIndex, 0, LVIF_IMAGE | LVIF_TEXT, findFileData.cFileName, 0, 0, 0, 0);
			}

		} while (FindNextFile(hFind, &findFileData) != 0);

		FindClose(hFind);
	}
}

//判断文件类型
bool CFileSystemDlg::IsDocFile(const CString& fileName)
{
	return fileName.Right(4).CompareNoCase(_T(".doc")) == 0;
}

bool CFileSystemDlg::IsXlsFile(const CString& fileName)
{
	return fileName.Right(4).CompareNoCase(_T(".xls")) == 0;
}

bool CFileSystemDlg::IsJpgFile(const CString& fileName)
{
	return fileName.Right(4).CompareNoCase(_T(".jpg")) == 0;
}


void CFileSystemDlg::OnBnClickedDocfileRadio()
{
	// TODO: 在此添加控件通知处理程序代码
	m_IsDocFileSelected = true;
	m_IsXlsFileSelected = false;
	m_IsJpgFileSelected = false;
}


void CFileSystemDlg::OnBnClickedXlsfileRadio()
{
	// TODO: 在此添加控件通知处理程序代码
	m_IsDocFileSelected = false;
	m_IsXlsFileSelected = true;
	m_IsJpgFileSelected = false;
}


void CFileSystemDlg::OnBnClickedJpgfileRadio()
{
	// TODO: 在此添加控件通知处理程序代码
	m_IsDocFileSelected = false;
	m_IsXlsFileSelected = false;
	m_IsJpgFileSelected = true;
}


void CFileSystemDlg::OnBnClickedRenameButton()
{
	// TODO: 在此添加控件通知处理程序代码

	if (m_SelectedFileIndex != -1)
	{
		// 获取选中文件的旧名称
		CString oldFileName = m_list.GetItemText(m_SelectedFileIndex, 0);


		// 获取当前文件的完整路径
		CString currentFilePath = m_CurrentFolderPath + oldFileName;

		// 将 List Control 中选中文件的文本设置为新文件名
		m_list.SetItemText(m_SelectedFileIndex, 0, m_NewFileName);

		// 构建新文件的完整路径
		CString newFilePath = m_CurrentFolderPath + m_NewFileName;

		TRACE(_T("currentFilePath：%s    newFilePath:%s    \n"), currentFilePath, newFilePath);

		if (MoveFile(currentFilePath, newFilePath))
		{
			// 更新 List Control 中的显示
			m_list.SetItemText(m_SelectedFileIndex, 0, m_NewFileName);
		}
		else
		{
			MessageBox(_T("重命名失败"), _T("错误"), MB_OK | MB_ICONERROR);
		}
	}
	else
	{
		TRACE(_T("No item selected in List Control.\n"));
	}

}

void CFileSystemDlg::OnLvnItemchangedList(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);

	// 检查是否选中了新的项
	if ((pNMLV->uChanged & LVIF_STATE) && (pNMLV->uNewState & LVIS_SELECTED))
	{
		m_SelectedFileIndex = pNMLV->iItem;  // 更新选中文件的索引
		CString selectedFileName = m_list.GetItemText(m_SelectedFileIndex, 0);

		// 将选中文件的名称显示在 EditControl 中
		SetDlgItemText(IDC_FILENAME_EDIT, selectedFileName);

		// 将编辑框中的文本保存到变量中
		GetDlgItemText(IDC_FILENAME_EDIT, m_NewFileName);

		TRACE(_T("m_NewFileName:%s   m_CurrentFolderPath:%s \n"), m_NewFileName,m_CurrentFolderPath);
	}
	else
	{
		TRACE(_T("EditControl Display error!\n"));
	}

	*pResult = 0;
}


void CFileSystemDlg::OnEnChangeFilenameEdit()
{
	// TODO:  如果该控件是 RICHEDIT 控件，它将不
	// 发送此通知，除非重写 CDialogEx::OnInitDialog()
	// 函数并调用 CRichEditCtrl().SetEventMask()，
	// 同时将 ENM_CHANGE 标志“或”运算到掩码中。

	GetDlgItemText(IDC_FILENAME_EDIT, m_NewFileName);
	TRACE(_T("m_NewFileName 更新：%s\n"), m_NewFileName);
}
