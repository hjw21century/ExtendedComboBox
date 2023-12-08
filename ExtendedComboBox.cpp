// ExtendedComboBox.cpp : implementation file
//

#include "pch.h"
#include "ExtendedComboBox.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CListBoxInsideComboBox

CListBoxInsideComboBox::CListBoxInsideComboBox()
{
	m_Parent = NULL;
}

CListBoxInsideComboBox::~CListBoxInsideComboBox()
{
}


BEGIN_MESSAGE_MAP(CListBoxInsideComboBox, CWnd)
	//{{AFX_MSG_MAP(CListBoxInsideComboBox)
	ON_WM_LBUTTONUP()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CListBoxInsideComboBox message handlers

void CListBoxInsideComboBox::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	CRect rect;	GetClientRect(rect);

	if (rect.PtInRect(point))
	{
		BOOL outside = FALSE;
		int index = ((CListBox *)this)->ItemFromPoint(point, outside);
		if (!outside && !m_Parent->IsItemEnabled(index))
			return;	// don't click there
	}

	CWnd::OnLButtonUp(nFlags, point);
}


void CListBoxInsideComboBox::SetParent(CExtendedComboBox *ptr)
{
	m_Parent = ptr;
}

const UINT nMessage=::RegisterWindowMessage("ComboSelEndOK");

BEGIN_MESSAGE_MAP(CExtendedComboBox, CComboBox)
	//{{AFX_MSG_MAP(CExtendedComboBox)
	ON_WM_CHARTOITEM()
    ON_WM_DRAWITEM()
    ON_WM_CHAR()
	ON_CONTROL_REFLECT(CBN_SELENDOK, OnSelendok)
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_CTLCOLORLISTBOX, OnCtlColorListBox)
	ON_MESSAGE(WM_CTLCOLOREDIT, OnCtlColorEdit)
	ON_REGISTERED_MESSAGE(nMessage, OnRealSelEndOK)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CExtendedComboBox

CExtendedComboBox::CExtendedComboBox()
{
	m_ListBox.SetParent(this);
    RECT rect;
    this->GetWindowRect(&rect);
    DWORD style = GetStyle();
    ModifyStyle(0, style | CBS_HASSTRINGS);
}

CExtendedComboBox::~CExtendedComboBox()
{
}

// default implementation
BOOL CExtendedComboBox::IsItemEnabled(UINT nIndex) const
{
	if (nIndex>=(DWORD)GetCount())
		return TRUE;	// whatever

	DWORD uData=GetItemData(nIndex);
	return !(uData&1);
}

void CExtendedComboBox::SetItemEnabled(UINT nIndex, BOOL enabled)
{
	if (nIndex >= (DWORD)GetCount())
		return;
    SetItemData(nIndex, !enabled);
}

void CExtendedComboBox::SetInputLimit(BOOL limited, std::vector<const char*> patterns, std::vector<const char*> reservpatterns)
{
	m_Edit.SetInputLimit(limited, patterns, reservpatterns);
}

/////////////////////////////////////////////////////////////////////////////
// CExtendedComboBox message handlers

void CExtendedComboBox::PreSubclassWindow() 
{
	// TODO: Add your specialized code here and/or call the base class
	ASSERT((GetStyle()&(CBS_OWNERDRAWFIXED|CBS_HASSTRINGS))==(CBS_OWNERDRAWFIXED|CBS_HASSTRINGS));
	CComboBox::PreSubclassWindow();
}

void CExtendedComboBox::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct) 
{
	// TODO: Add your code to draw the specified item
	CDC* pDC = CDC::FromHandle (lpDrawItemStruct->hDC);

	if (((LONG)(lpDrawItemStruct->itemID) >= 0) &&
		(lpDrawItemStruct->itemAction & (ODA_DRAWENTIRE | ODA_SELECT)))
	{
		BOOL fDisabled = !IsWindowEnabled () || !IsItemEnabled(lpDrawItemStruct->itemID);

		COLORREF newTextColor = fDisabled ?
			RGB(0x80, 0x80, 0x80) : GetSysColor (COLOR_WINDOWTEXT);  // light gray

		COLORREF oldTextColor = pDC->SetTextColor (newTextColor);

		COLORREF newBkColor = GetSysColor (COLOR_WINDOW);
		COLORREF oldBkColor = pDC->SetBkColor (newBkColor);

		if (newTextColor == newBkColor)
			newTextColor = RGB(0xC0, 0xC0, 0xC0);   // dark gray

		if (!fDisabled && ((lpDrawItemStruct->itemState & ODS_SELECTED) != 0))
		{
			pDC->SetTextColor (GetSysColor (COLOR_HIGHLIGHTTEXT));
			pDC->SetBkColor (GetSysColor (COLOR_HIGHLIGHT));
		}

		CString strText;
		GetLBText(lpDrawItemStruct->itemID, strText);

		const RECT &rc=lpDrawItemStruct->rcItem;

		pDC->ExtTextOut(rc.left + 2,
				  rc.top + 2,// + max(0, (cyItem - m_cyText) / 2),
				  ETO_OPAQUE, &rc,
				  strText, strText.GetLength (), NULL);

		pDC->SetTextColor (oldTextColor);
		pDC->SetBkColor (oldBkColor);
	}
	else if ((LONG)(lpDrawItemStruct->itemID)<0)	// drawing edit text
	{
		COLORREF newTextColor = GetSysColor (COLOR_WINDOWTEXT);  // light gray
		COLORREF oldTextColor = pDC->SetTextColor (newTextColor);

		COLORREF newBkColor = GetSysColor (COLOR_WINDOW);
		COLORREF oldBkColor = pDC->SetBkColor (newBkColor);

		if ((lpDrawItemStruct->itemState & ODS_SELECTED) != 0)
		{
			pDC->SetTextColor (GetSysColor (COLOR_HIGHLIGHTTEXT));
			pDC->SetBkColor (GetSysColor (COLOR_HIGHLIGHT));
		}

		CString strText;
		GetWindowText(strText);

		const RECT &rc=lpDrawItemStruct->rcItem;

		pDC->ExtTextOut(rc.left + 2,
				  rc.top + 2,// + max(0, (cyItem - m_cyText) / 2),
				  ETO_OPAQUE, &rc,
				  strText, strText.GetLength (), NULL);

		pDC->SetTextColor (oldTextColor);
		pDC->SetBkColor (oldBkColor);
	}

	if ((lpDrawItemStruct->itemAction & ODA_FOCUS) != 0)
		pDC->DrawFocusRect(&lpDrawItemStruct->rcItem);
	
}

void CExtendedComboBox::MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct) 
{
	// TODO: Add your code to determine the size of specified item
	UNREFERENCED_PARAMETER(lpMeasureItemStruct);
}

int CExtendedComboBox::OnCharToItem(UINT nChar, CListBox* pListBox, UINT nIndex) 
{
	// TODO: Add your message handler code here and/or call default
	
	int ret=CComboBox::OnCharToItem(nChar, pListBox, nIndex);
	if (ret>=0 && !IsItemEnabled(ret))
		return -2;
	else
		return ret;
}

void CExtendedComboBox::OnSelendok() 
{
	// TODO: Add your control notification handler code here
	GetWindowText(m_strSavedText);
	PostMessage(nMessage);	
}

LRESULT CExtendedComboBox::OnRealSelEndOK(WPARAM,LPARAM)
{
	CString currentText;
	GetWindowText(currentText);

	int index=FindStringExact(-1,currentText);
	if (index>=0 && !IsItemEnabled(index))
	{
		SetWindowText(m_strSavedText);
		GetParent()->SendMessage(WM_COMMAND,MAKELONG(GetWindowLong(m_hWnd,GWL_ID),CBN_SELCHANGE),(LPARAM)m_hWnd);
	}

	return 0;
}

LRESULT CExtendedComboBox::OnCtlColorListBox(WPARAM,LPARAM lParam)
{
	if (m_ListBox.m_hWnd==NULL && lParam!=0 && lParam!=(LPARAM)m_hWnd)
		m_ListBox.SubclassWindow((HWND)lParam);

	return Default();
}

LRESULT CExtendedComboBox::OnCtlColorEdit(WPARAM, LPARAM lParam)
{
	if (m_Edit.m_hWnd == NULL && lParam != 0 && lParam != (LPARAM)m_hWnd)
		m_Edit.SubclassWindow((HWND)lParam);

	return Default();
}

void CExtendedComboBox::PostNcDestroy() 
{
	// TODO: Add your specialized code here and/or call the base class
	m_ListBox.UnsubclassWindow();
	m_ListBox.Detach();

	m_Edit.UnsubclassWindow();
	m_Edit.Detach();
	
	CComboBox::PostNcDestroy();
}
