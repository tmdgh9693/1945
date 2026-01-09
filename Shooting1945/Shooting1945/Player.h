#pragma once
#include "afxwin.h"
#include "Bullet.h"
#include <vector>

class CPlayer
{
public:
    CPlayer();
    ~CPlayer();

    void Init(int x, int y, int width, int height);
    void Update();
    void Render(CDC* pDC);

    void MoveLeft();
    void MoveRight();
    void MoveUp();
    void MoveDown();

    void Fire();

    void ApplyPowerUp(int type);

    CRect GetRect() const;
    CRect GetHitBox() const;

    bool IsAlive() const { return m_bAlive; }
    void SetAlive(bool alive) { m_bAlive = alive; }

    bool HasShield() const { return m_bHasShield; }
    void SetShield(bool value) { m_bHasShield = value; }

    int GetShotType() const { return m_nShotType; }
    int GetPowerUpRemainingTime() const;

    std::vector<CBullet>& GetBullets() { return m_bullets; }

    void SetGameArea(int width, int height);

private:
    CRect m_rect;
    std::vector<CBullet> m_bullets;

    int m_nSpeed;
    bool m_bAlive;

    int m_nGameWidth;
    int m_nGameHeight;

    int m_nFireDelay;
    int m_nFireCounter;

    int m_nShotType;
    bool m_bHasShield;

    int m_nPowerUpTimer;
    int m_nPowerUpDuration;
};
