// TestServerDlg.h : ͷ�ļ�
//

#pragma once
#include "afxwin.h"


// CTestServerDlg �Ի���
class CTestServerDlg : public CDialog
{
// ����
public:
	CTestServerDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_TESTSERVER_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	CEdit mEdit;
	CComboBox mCCBClients;
public:
	afx_msg void OnBnClickedButton1();
};
