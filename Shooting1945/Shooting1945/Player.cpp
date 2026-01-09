#include "pch.h"
#include "Player.h"
#include <algorithm>

CPlayer::CPlayer()
{
    m_nSpeed = 5;
    m_bAlive = true;
    m_nGameWidth = 480;
    m_nGameHeight = 640;
    m_nFireDelay = 10;
    m_nFireCounter = 0;
    m_nShotType = 0;
    m_bHasShield = false;
    m_nPowerUpTimer = 0;
    m_nPowerUpDuration = 180;
}

CPlayer::~CPlayer()
{
}

void CPlayer::SetGameArea(int width, int height)
{
    m_nGameWidth = width;
    m_nGameHeight = height;
}

void CPlayer::Init(int x, int y, int width, int height)
{
    m_rect.SetRect(x, y, x + width, y + height);
    m_bAlive = true;
    m_bullets.clear();
    m_nShotType = 0;
    m_bHasShield = false;
    m_nPowerUpTimer = 0;
    m_nSpeed = 5;
}

int CPlayer::GetPowerUpRemainingTime() const
{
    if (m_nPowerUpTimer <= 0)
        return 0;

    return (m_nPowerUpTimer + 59) / 60;
}

void CPlayer::Update()
{
    if (m_nFireCounter > 0)
        m_nFireCounter--;

    if (m_nPowerUpTimer > 0)
    {
        m_nPowerUpTimer--;
        if (m_nPowerUpTimer == 0)
        {
            m_nShotType = 0;
            m_bHasShield = false;
        }
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

    int w = m_rect.Width();
    int h = m_rect.Height();

    if (m_rect.left < 0)
    {
        m_rect.left = 0;
        m_rect.right = m_rect.left + w;
    }
    if (m_rect.top < 0)
    {
        m_rect.top = 0;
        m_rect.bottom = m_rect.top + h;
    }
    if (m_rect.right > m_nGameWidth)
    {
        m_rect.right = m_nGameWidth;
        m_rect.left = m_rect.right - w;
    }
    if (m_rect.bottom > m_nGameHeight)
    {
        m_rect.bottom = m_nGameHeight;
        m_rect.top = m_rect.bottom - h;
    }
}

CRect CPlayer::GetRect() const
{
    return m_rect;
}

CRect CPlayer::GetHitBox() const
{
    CRect hitBox = m_rect;
    int reduceSize = 12;
    hitBox.DeflateRect(reduceSize, reduceSize);
    return hitBox;
}

void CPlayer::Render(CDC* pDC)
{
    if (!m_bAlive)
        return;

    if (m_bHasShield)
    {
        CPen pen(PS_SOLID, 3, RGB(0, 100, 255));
        CPen* pOldPen = pDC->SelectObject(&pen);
        CBrush* pOldBrush = (CBrush*)pDC->SelectStockObject(NULL_BRUSH);

        CRect shieldRect = m_rect;
        shieldRect.InflateRect(8, 8);
        pDC->Ellipse(shieldRect);

        pDC->SelectObject(pOldPen);
        pDC->SelectObject(pOldBrush);
    }

    CBrush brush(RGB(0, 255, 0));
    CBrush* pOldBrush = pDC->SelectObject(&brush);
    pDC->Rectangle(m_rect);
    pDC->SelectObject(pOldBrush);

    CRect hitBox = GetHitBox();
    CPen pen(PS_DOT, 1, RGB(255, 255, 0));
    CPen* pOldPen = pDC->SelectObject(&pen);
    CBrush* pOldBrush2 = (CBrush*)pDC->SelectStockObject(NULL_BRUSH);
    pDC->Rectangle(hitBox);
    pDC->SelectObject(pOldPen);
    pDC->SelectObject(pOldBrush2);

    for (auto& bullet : m_bullets)
    {
        bullet.Render(pDC);
    }
}

void CPlayer::Fire()
{
    if (!m_bAlive || m_nFireCounter > 0)
        return;

    int bulletWidth = 5;
    int bulletHeight = 10;
    int centerX = m_rect.left + m_rect.Width() / 2;
    int bulletY = m_rect.top - bulletHeight;

    switch (m_nShotType)
    {
    case 0:
    {
        int bulletX = centerX - bulletWidth / 2;
        CBullet bullet;
        bullet.Init(bulletX, bulletY, bulletWidth, bulletHeight, 10);
        m_bullets.push_back(bullet);
    }
    break;

    case 1:
    {
        for (int i = -1; i <= 1; i += 2)
        {
            int bulletX = centerX + (i * 15) - bulletWidth / 2;
            CBullet bullet;
            bullet.Init(bulletX, bulletY, bulletWidth, bulletHeight, 10);
            m_bullets.push_back(bullet);
        }
    }
    break;

    case 2:
    {
        for (int i = -1; i <= 1; i++)
        {
            int bulletX = centerX + (i * 15) - bulletWidth / 2;
            CBullet bullet;
            bullet.Init(bulletX, bulletY, bulletWidth, bulletHeight, 10);
            m_bullets.push_back(bullet);
        }
    }
    break;
    }

    m_nFireCounter = m_nFireDelay;
}

void CPlayer::ApplyPowerUp(int type)
{
    switch (type)
    {
    case 0:
        m_nShotType = 1;
        m_nPowerUpTimer = m_nPowerUpDuration;
        m_bHasShield = false;
        break;
    case 1:
        m_nShotType = 2;
        m_nPowerUpTimer = m_nPowerUpDuration;
        m_bHasShield = false;
        break;
    case 2:
        if (m_nSpeed < 10)
            m_nSpeed += 2;
        break;
    case 3:
        m_bHasShield = true;
        m_nPowerUpTimer = m_nPowerUpDuration;
        break;
    default:
        break;
    }
}

void CPlayer::MoveLeft()
{
    if (!m_bAlive)
        return;

    int w = m_rect.Width();
    m_rect.left -= m_nSpeed;
    m_rect.right = m_rect.left + w;

    if (m_rect.left < 0)
    {
        m_rect.left = 0;
        m_rect.right = m_rect.left + w;
    }
}

void CPlayer::MoveRight()
{
    if (!m_bAlive)
        return;

    int w = m_rect.Width();
    m_rect.left += m_nSpeed;
    m_rect.right = m_rect.left + w;

    if (m_rect.right > m_nGameWidth)
    {
        m_rect.right = m_nGameWidth;
        m_rect.left = m_rect.right - w;
    }
}

void CPlayer::MoveUp()
{
    if (!m_bAlive)
        return;

    int h = m_rect.Height();
    m_rect.top -= m_nSpeed;
    m_rect.bottom = m_rect.top + h;

    if (m_rect.top < 0)
    {
        m_rect.top = 0;
        m_rect.bottom = m_rect.top + h;
    }
}

void CPlayer::MoveDown()
{
    if (!m_bAlive)
        return;

    int h = m_rect.Height();
    m_rect.top += m_nSpeed;
    m_rect.bottom = m_rect.top + h;

    if (m_rect.bottom > m_nGameHeight)
    {
        m_rect.bottom = m_nGameHeight;
        m_rect.top = m_rect.bottom - h;
    }
}
