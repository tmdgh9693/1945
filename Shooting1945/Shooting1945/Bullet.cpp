#include "pch.h"
#include "Bullet.h"

CBullet::CBullet()
{
    m_nSpeed = 10;
    m_bActive = false;
    m_nGameHeight = 640;
}

CBullet::~CBullet()
{
}

void CBullet::Init(int x, int y, int width, int height, int speed)
{
    m_rect.SetRect(x, y, x + width, y + height);
    m_nSpeed = speed;
    m_bActive = true;
}

void CBullet::Update()
{
    if (!m_bActive)
        return;

    m_rect.top -= m_nSpeed;
    m_rect.bottom -= m_nSpeed;

    if (m_rect.bottom < 0 || m_rect.top > m_nGameHeight)
    {
        m_bActive = false;
    }
}

void CBullet::Render(CDC* pDC)
{
    if (!m_bActive)
        return;

    CBrush brush(RGB(255, 255, 0));
    CBrush* pOldBrush = pDC->SelectObject(&brush);

    pDC->Ellipse(m_rect);

    pDC->SelectObject(pOldBrush);
}
