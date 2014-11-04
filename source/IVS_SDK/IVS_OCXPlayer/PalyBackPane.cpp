// PalyBackPane.cpp : implementation file
//

#include "stdafx.h"
//#include "IVS_OCXPlayer.h"
#include "PalyBackPane.h"
#include "afxdialogex.h"


// CPalyBackPane dialog

IMPLEMENT_DYNAMIC(CPalyBackPane, CDialogEx)

CPalyBackPane::CPalyBackPane(CWnd* pParent /*=NULL*/)
	: CDialogEx(CPalyBackPane::IDD, pParent)
{

}

CPalyBackPane::~CPalyBackPane()
{
}

void CPalyBackPane::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_BTN_PLAYBACK, m_btnPlay);
}


BEGIN_MESSAGE_MAP(CPalyBackPane, CDialogEx)
END_MESSAGE_MAP()//lint !e751


// CPalyBackPane message handlers
