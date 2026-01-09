#include "pch.h"
#include "PowerUp.h"

CPowerUp::CPowerUp()
{
    m_bActive = false;
    m_type = POWER_DOUBLE_SHOT;
    m_nSpeed = 2;
    m_nGameHeight = 640;
}

CPowerUp::~CPowerUp()
{
}

void CPowerUp::Init(int x, int y, int width, int height, PowerUpType type)
{
    m_rect.SetRect(x, y, x + width, y + height);
    m_type = type;
    m_bActive = true;
}

void CPowerUp::Update()
{
    if (!m_bActive)
        return;

    m_rect.top += m_nSpeed;
    m_rect.bottom += m_nSpeed;

    if (m_rect.top > m_nGameHeight)
    {
        m_bActive = false;
    }
}

void CPowerUp::Render(CDC* pDC)
{
    if (!m_bActive)
        return;

    COLORREF color;
    switch (m_type)
    {
    case POWER_DOUBLE_SHOT:
        color = RGB(0, 255, 255);
        break;
    case POWER_TRIPLE_SHOT:
        color = RGB(255, 0, 255);
        break;
    case POWER_SPEED_UP:
        color = RGB(255, 165, 0);
        break;
    case POWER_SHIELD:
        color = RGB(0, 0, 255);
        break;
    default:
        color = RGB(255, 255, 255);
        break;
    }

    CBrush brush(color);
    CBrush* pOldBrush = pDC->SelectObject(&brush);

    pDC->Ellipse(m_rect);

    pDC->SelectObject(pOldBrush);

    pDC->SetTextColor(RGB(255, 255, 255));
    pDC->SetBkMode(TRANSPARENT);
    CString str;
    switch (m_type)
    {
    case POWER_DOUBLE_SHOT: str = L"x2"; break;
    case POWER_TRIPLE_SHOT: str = L"x3"; break;
    case POWER_SPEED_UP:    str = L"S";  break;
    case POWER_SHIELD:      str = L"D";  break;
    }
    pDC->TextOutW(m_rect.left + 5, m_rect.top + 3, str);
}
