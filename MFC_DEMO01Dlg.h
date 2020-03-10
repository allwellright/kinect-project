
// MFC_DEMO01Dlg.h : ͷ�ļ�
//

#pragma once
#include "cv.h"
#include "highgui.h"
#include "CvvImage.h"
#include "afxwin.h"

class CBodyBasics;
// CMFC_DEMO01Dlg �Ի���
class CMFC_DEMO01Dlg : public CDialogEx
{
// ����
public:
	CMFC_DEMO01Dlg(CWnd* pParent = NULL);	// ��׼���캯��
	//CMFC_DEMO01Dlg(CBodyBasics* ckinect);
// �Ի�������
	enum { IDD = IDD_MFC_DEMO01_DIALOG };

	

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
	//afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedYes();
	afx_msg void OnBnClickedOk();
    void DrawPicToHDC(IplImage *img, UINT ID);
public:
 
	static CMFC_DEMO01Dlg  *s_pDlg;
	afx_msg void OnBnClickedStartButton1();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnBnClickedChooseButton2();
	afx_msg void OnBnClickedSaveButton1();
	afx_msg void OnCbnSelchangePicCombo1();
	CComboBox m_comboPic;
	CEdit m_outedit;
	afx_msg void OnBnClickedSave2Button1();
	afx_msg void OnBnClickedSave3Button2();
	CString m_edit;
	int m_numedit;
	afx_msg void OnBnClickedStopButton3();
	afx_msg void OnBnClickedGetButton3();
	afx_msg void OnEnChangePathEdit1();
	afx_msg void OnStnClickedPicStatic();
	afx_msg void OnEnChangeOutEdit1();
	afx_msg void OnBnClickedStartButton2();
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButton2();
	afx_msg void OnBnClickedStartButton3();
};
