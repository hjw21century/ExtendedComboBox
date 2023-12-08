#include "pch.h"
#include "ExtendedEdit.h"
#include "mregexp.h"

BEGIN_MESSAGE_MAP(CExtendedEdit, CEdit)
    ON_WM_CHAR()
    // ON_CONTROL_REFLECT(EN_CHANGE, OnEnChange)
END_MESSAGE_MAP()

CExtendedEdit::CExtendedEdit()
{
    m_limited = FALSE;
}

void CExtendedEdit::SetInputLimit(BOOL limited, std::vector<const char*> patterns, std::vector<const char*> reservpatterns)
{
    m_limited = limited;
    m_patterns = patterns;
    m_reservpatterns = reservpatterns;
}

BOOL CExtendedEdit::IsMatchPattern(CString text)
{
    int count = m_patterns.size();
    int reservcount = m_reservpatterns.size();
    for (int i = 0; i < count; i++) {
	    MRegexp * reg = mregexp_compile(m_patterns[i]);
	    MRegexpMatch m;
        BOOL ret = mregexp_match(reg, text.GetBuffer(), &m);
        mregexp_free(reg);
        if (ret) {
			BOOL reservMatch = FALSE;
            for (int i = 0; i < reservcount; i++) {
	            MRegexp * reg = mregexp_compile(m_reservpatterns[i]);
	            MRegexpMatch m;
                BOOL retReserv = mregexp_match(reg, text.GetBuffer(), &m);
                mregexp_free(reg);
                if (retReserv) {
					reservMatch = TRUE;
                } 
            }
			if (!reservMatch) {
				// 匹配任意一个正向法则，且不匹配任意一个反向法则，则通过
				return TRUE;
			}
        } 
    }
    
    return FALSE;
}

void CExtendedEdit::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if (!m_limited) {
		CEdit::OnChar(nChar, nRepCnt, nFlags);
		return;
	}
    int posCur = CharFromPos(GetCaretPos());

    CString strText;
    GetWindowText(strText);

    // 允许删除键
    if (nChar == VK_BACK) {
        // if (posCur > strText.GetLength() - 2) {
        //    return;
        // }
		//else 
		{
            CEdit::OnChar(nChar, nRepCnt, nFlags);
			return;
        }
    } else if (!(nChar == '.' || (nChar >= '0' && nChar <= '9') || nChar == ' ')) {
        return;
    }


   if(nChar == '.' && strText.Find('.') == -1) {
        CEdit::OnChar(nChar, nRepCnt, nFlags);
        return;
   }

   CString chchar;
   chchar.Format("%c", nChar);

   CString strTextAfer = strText;
   strTextAfer.Insert(posCur, chchar);

   if (IsMatchPattern(strTextAfer)) {
        CEdit::OnChar(nChar, nRepCnt, nFlags);
   } else {
        // 不符合模式
        MessageBeep(0);
   }
}
