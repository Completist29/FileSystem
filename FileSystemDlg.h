// FileSystemDlg.h: 头文件
//

#pragma once
#include<vector>


enum FileType
{
	FileType_Doc,
	FileType_Xls,
	FileType_Jpg,
	FileType_Unknown
};

// CFileSystemDlg 对话框
class CFileSystemDlg : public CDialogEx
{
// 构造
public:
	CFileSystemDlg(CWnd* pParent = nullptr);	// 标准构造函数

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_FILESYSTEM_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持

// 实现
protected:
	HICON m_hIcon;
	CImageList m_ImageList;  //图像列表(相同大小图像集合) 
	HTREEITEM m_hRoot;   


	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	// 磁盘节点
	CTreeCtrl m_tree;
	
	CListCtrl m_list;
	// 编辑框	
	CEdit m_edit;
	//当前文件夹路径
	CString m_strPath;  

	CButton m_FindButton;
//	CButton m_DocFileRadio;
//	CButton m_XlsFileRadio;
//	CButton m_JpgFileRadio;


	BOOL m_IsDocFileSelected;
	BOOL m_IsXlsFileSelected;
	BOOL m_IsJpgFileSelected;

	int m_SelectedFileIndex;
	CString m_NewFileName;  
	CString m_CurrentFolderPath;


	// 获取系统图标遍历逻辑驱动器，插入到 TreeView 控件
	void FillDiskTree();
	//递归获取指定目录下的子目录
	void FillDirectories(HTREEITEM hParentItem, const CString& strPath);

	void OnDestroy();

	afx_msg void OnTvnSelchangedTree(NMHDR* pNMHDR, LRESULT* pResult);

	void FillFileList(const CString& strPath);
	CString GetItemPath(CTreeCtrl* pTreeCtrl, HTREEITEM hItem);



	afx_msg void OnNMRClickList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnLargeIcon();
	afx_msg void OnSmallIcon();
	afx_msg void OnDetails();
	afx_msg void OnBnClickedFindButton();

	


	void QueryFiles(const CString& strPath);
	bool IsDocFile(const CString& fileName);
	bool IsXlsFile(const CString& fileName);
	bool IsJpgFile(const CString& fileName);
	afx_msg void OnBnClickedDocfileRadio();
	afx_msg void OnBnClickedXlsfileRadio();
	afx_msg void OnBnClickedJpgfileRadio();
//	afx_msg void OnLvnItemActivateList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnBnClickedRenameButton();
	afx_msg void OnLvnItemchangedList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnEnChangeFilenameEdit();
};
