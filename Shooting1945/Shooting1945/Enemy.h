#pragma once
#include <vector>
#include <afxwin.h>
#include "Bullet.h"

class CEnemy
{
public:
    CEnemy();
    ~CEnemy();

    void Init(int x, int y,
        int width, int height,
        int speed,
        int pattern,
        int stage,
        int difficultyLevel);

    void Update(const CRect& playerRect);
    void Render(CDC* pDC);

    CRect GetRect() const { return m_rect; }
    bool IsActive() const { return m_bActive; }
    void SetActive(bool active) { m_bActive = active; }

    void Hit(int damage);
    std::vector<CBullet>& GetBullets() { return m_bullets; }

private:
    void FireStraight();
    void FireFan();
    void FireHoming(const CPoint& playerCenter);

private:
    CRect m_rect;
    bool  m_bActive;

    int   m_nHP;
    int   m_nMaxHP;

    int   m_nSpeed;
    int   m_nPattern;
    int   m_nMoveCounter;
    int   m_nInitX;

    int   m_nGameWidth;
    int   m_nGameHeight;

    int   m_nStage;
    int   m_nDifficulty;

    int   m_nFireCounter;
    int   m_nFireDelay;

    std::vector<CBullet> m_bullets;
};
