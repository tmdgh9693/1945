#include "pch.h"
#include "Enemy.h"
#include <cmath>
#include <algorithm>

CEnemy::CEnemy()
{
    m_bActive = false;
    m_nHP = 1;
    m_nMaxHP = 1;
    m_nSpeed = 3;
    m_nPattern = 0;
    m_nMoveCounter = 0;
    m_nInitX = 0;

    m_nGameWidth = 480;
    m_nGameHeight = 640;

    m_nStage = 1;
    m_nDifficulty = 0;

    m_nFireCounter = 0;
    m_nFireDelay = 90;
}

CEnemy::~CEnemy()
{
}

void CEnemy::Init(int x, int y,
    int width, int height,
    int speed,
    int pattern,
    int stage,
    int difficultyLevel)
{
    m_rect.SetRect(x, y, x + width, y + height);

    m_nSpeed = speed;
    m_nPattern = pattern;

    m_bActive = true;
    m_nMoveCounter = 0;
    m_nInitX = x;

    m_nStage = stage;
    if (difficultyLevel < 0) difficultyLevel = 0;
    if (difficultyLevel > 2) difficultyLevel = 2;
    m_nDifficulty = difficultyLevel;

    int factor = 2 + m_nDifficulty;
    m_nMaxHP = m_nStage * factor;
    if (m_nMaxHP < 2) m_nMaxHP = 2;
    m_nHP = m_nMaxHP;

    int baseDelay = 90;
    baseDelay -= m_nStage * 4;
    baseDelay -= m_nDifficulty * 10;
    if (baseDelay < 20) baseDelay = 20;
    if (baseDelay > 120) baseDelay = 120;
    m_nFireDelay = baseDelay;

    m_nFireCounter = 0;
    m_bullets.clear();
}

void CEnemy::Hit(int damage)
{
    m_nHP -= damage;
    if (m_nHP <= 0)
    {
        m_nHP = 0;
        m_bActive = false;
    }
}

void CEnemy::Update(const CRect& playerRect)
{
    if (!m_bActive)
        return;

    m_nMoveCounter++;

    switch (m_nPattern)
    {
    case 0:
        m_rect.top += m_nSpeed;
        m_rect.bottom += m_nSpeed;
        break;

    case 1:
    {
        m_rect.top += m_nSpeed;
        m_rect.bottom += m_nSpeed;

        double t = m_nMoveCounter * 0.12;
        int amp = 25 + m_nStage * 2;
        int offsetX = (int)(sin(t) * amp);
        int width = m_rect.Width();
        int newLeft = m_nInitX + offsetX;
        if (newLeft < 0) newLeft = 0;
        if (newLeft + width > m_nGameWidth) newLeft = m_nGameWidth - width;
        m_rect.left = newLeft;
        m_rect.right = newLeft + width;
    }
    break;

    case 2:
    {
        m_rect.top += m_nSpeed - 1;
        m_rect.bottom += m_nSpeed - 1;

        double t = m_nMoveCounter * 0.08;
        int amp = 40 + m_nStage * 3;
        int offsetX = (int)(sin(t) * amp);
        int width = m_rect.Width();
        int newLeft = m_nInitX + offsetX;
        if (newLeft < 0) newLeft = 0;
        if (newLeft + width > m_nGameWidth) newLeft = m_nGameWidth - width;
        m_rect.left = newLeft;
        m_rect.right = newLeft + width;
    }
    break;
    }

    if (m_rect.top > m_nGameHeight)
    {
        m_bActive = false;
    }

    m_nFireCounter++;

    int fireChance;
    if (m_nDifficulty == 0) fireChance = 45;
    else if (m_nDifficulty == 1) fireChance = 70;
    else fireChance = 90;

    if (m_nFireCounter >= m_nFireDelay)
    {
        m_nFireCounter = 0;

        int r = rand() % 100;
        CPoint playerCenter = playerRect.CenterPoint();

        if (m_nPattern == 0)
        {
            if (r < fireChance) FireStraight();
        }
        else if (m_nPattern == 1)
        {
            if (r < fireChance)
            {
                FireFan();
                if (m_nStage >= 4 && m_nDifficulty >= 1)
                    FireStraight();
            }
        }
        else if (m_nPattern == 2)
        {
            if (r < fireChance)
            {
                FireHoming(playerCenter);
                if (m_nStage >= 3)
                    FireFan();
                if (m_nStage >= 6 && m_nDifficulty == 2)
                    FireStraight();
            }
        }
    }

    for (auto& b : m_bullets)
    {
        b.Update();
    }

    m_bullets.erase(
        std::remove_if(m_bullets.begin(), m_bullets.end(),
            [](const CBullet& bb) { return !bb.IsActive(); }),
        m_bullets.end());
}

void CEnemy::Render(CDC* pDC)
{
    if (!m_bActive)
        return;

    CBrush brush(RGB(255, 80, 80));
    CBrush* pOldBrush = pDC->SelectObject(&brush);
    pDC->Rectangle(m_rect);
    pDC->SelectObject(pOldBrush);

    if (m_nMaxHP > 0)
    {
        int barWidth = m_rect.Width();
        int barHeight = 3;
        int barX = m_rect.left;
        int barY = m_rect.top - barHeight - 2;

        CBrush bgBrush(RGB(80, 0, 0));
        pOldBrush = pDC->SelectObject(&bgBrush);
        CRect bg(barX, barY, barX + barWidth, barY + barHeight);
        pDC->Rectangle(bg);
        pDC->SelectObject(pOldBrush);

        int curWidth = (int)(barWidth * ((float)m_nHP / (float)m_nMaxHP));
        if (curWidth < 0) curWidth = 0;
        CBrush hpBrush(RGB(255, 0, 0));
        pOldBrush = pDC->SelectObject(&hpBrush);
        CRect hp(barX, barY, barX + curWidth, barY + barHeight);
        pDC->Rectangle(hp);
        pDC->SelectObject(pOldBrush);
    }

    for (auto& b : m_bullets)
    {
        b.Render(pDC);
    }
}

void CEnemy::FireStraight()
{
    int w = 8;
    int h = 8;
    int centerX = m_rect.left + m_rect.Width() / 2;
    int bulletX = centerX - w / 2;
    int bulletY = m_rect.bottom;

    CBullet b;
    b.Init(bulletX, bulletY, w, h, -6 - m_nDifficulty);
    m_bullets.push_back(b);
}

void CEnemy::FireFan()
{
    int w = 8;
    int h = 8;
    int centerX = m_rect.left + m_rect.Width() / 2;
    int bulletY = m_rect.bottom;

    int offsets[3] = { -20, 0, 20 };

    for (int i = 0; i < 3; ++i)
    {
        int bulletX = centerX + offsets[i] - w / 2;
        CBullet b;
        b.Init(bulletX, bulletY, w, h, -5 - m_nDifficulty);
        m_bullets.push_back(b);
    }
}

void CEnemy::FireHoming(const CPoint& playerCenter)
{
    int w = 8;
    int h = 8;

    int bulletX = playerCenter.x - w / 2;
    int bulletY = m_rect.bottom;

    if (bulletX < 0) bulletX = 0;
    if (bulletX + w > m_nGameWidth) bulletX = m_nGameWidth - w;

    CBullet b;
    int spd = -6 - m_nDifficulty;
    b.Init(bulletX, bulletY, w, h, spd);
    m_bullets.push_back(b);
}