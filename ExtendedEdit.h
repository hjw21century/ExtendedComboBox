#include <vector>

class CExtendedEdit : public CEdit
{
protected:
    DECLARE_MESSAGE_MAP()
public:
    CExtendedEdit();
    afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
    void SetInputLimit(BOOL limited, std::vector<const char*> pattern, std::vector<const char*> reservpatterns);

private:
    BOOL m_limited;

    UINT m_nChar;

    std::vector<const char*> m_patterns;
    std::vector<const char*> m_reservpatterns;

    BOOL CExtendedEdit::IsMatchPattern(CString text);
};