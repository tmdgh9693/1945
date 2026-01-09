#include "pch.h"
#include "Boss.h"
#include <algorithm>

CBoss::CBoss()
{
    m_bActive = false;
    m_nHP = 50;
    m_nMaxHP = 50;
    m_nSpeed = 2;
    m_nPattern = 0;
    m_nMoveCounter = 0;
    m_nFireCounter = 0;
    m_nFireDelay = 30;
    m_nGameWidth = 480;
    m_nGameHeight = 640;
    m_nInitX = 0;
    m_nDirection = 1;
}

CBoss::~CBoss()
{
}

void CBoss::Init(int x, int y, int width, int height, int stage)
{
    m_rect.SetRect(x, y, x + width, y + height);
    m_bActive = true;

    m_nMaxHP = 50 + (stage - 1) * 20;
    m_nHP = m_nMaxHP;

    m_nMoveCounter = 0;
    m_nFireCounter = 0;
    m_nInitX = x;
    m_nDirection = 1;
    m_bullets.clear();
}

void CBoss::Update()
{
    if (!m_bActive)
        return;

    m_nMoveCounter++;
    m_nFireCounter++;

    m_rect.left += m_nSpeed * m_nDirection;
    m_rect.right += m_nSpeed * m_nDirection;

    if (m_rect.left <= 0 || m_rect.right >= m_nGameWidth)
    {
        m_nDirection *= -1;
    }

    if (m_nFireCounter >= m_nFireDelay)
    {
        Fire();
        m_nFireCounter = 0;
    }

    for (auto& bullet : m_bullets)
    {
        bullet.Update();
    }

    m_bullets.erase(
        std::remove_if(m_bullets.begin(), m_bullets.end(),
            [](const CBullet& b) { return !b.IsActive(); }),
        m_bullets.end()
    );

    if (m_nHP <= 0)
    {
        m_bActive = false;
    }
}

void CBoss::Render(CDC* pDC)
{
    if (!m_bActive)
        return;

    CBrush brush(RGB(200, 0, 0));
    CBrush* pOldBrush = pDC->SelectObject(&brush);
    pDC->Rectangle(m_rect);
    pDC->SelectObject(pOldBrush);

    int barWidth = m_rect.Width();
    int barHeight = 5;
    int barX = m_rect.left;
    int barY = m_rect.bottom + 5;

    CBrush redBrush(RGB(255, 0, 0));
    pOldBrush = pDC->SelectObject(&redBrush);
    CRect bgRect(barX, barY, barX + barWidth, barY + barHeight);
    pDC->Rectangle(bgRect);
    pDC->SelectObject(pOldBrush);

    int currentBarWidth = (int)(barWidth * ((float)m_nHP / m_nMaxHP));
    CBrush greenBrush(RGB(0, 255, 0));
    pOldBrush = pDC->SelectObject(&greenBrush);
    CRect hpRect(barX, barY, barX + currentBarWidth, barY + barHeight);
    pDC->Rectangle(hpRect);
    pDC->SelectObject(pOldBrush);

    pDC->SetTextColor(RGB(255, 255, 255));
    pDC->SetBkMode(TRANSPARENT);
    CString str;
    str.Format(L"BOSS HP: %d/%d", m_nHP, m_nMaxHP);
    pDC->TextOutW(barX, barY + 10, str);

    for (auto& bullet : m_bullets)
    {
        bullet.Render(pDC);
    }
}

void CBoss::Fire()
{
    if (!m_bActive)
        return;

    int bulletWidth = 8;
    int bulletHeight = 8;

    for (int i = -1; i <= 1; i++)
    {
        int bulletX = m_rect.left + m_rect.Width() / 2 - bulletWidth / 2 + (i * 30);
        int bulletY = m_rect.bottom;

        CBullet bullet;
        bullet.Init(bulletX, bulletY, bulletWidth, bulletHeight, -8);
        m_bullets.push_back(bullet);
    }
}

void CBoss::Hit(int damage)
{
    m_nHP -= damage;
    if (m_nHP <= 0)
    {
        m_nHP = 0;
        m_bActive = false;
    }
}
