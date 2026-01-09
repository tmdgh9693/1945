#pragma once
#include "afxwin.h"
#include "Bullet.h"
#include <vector>

class CBoss
{
public:
    CBoss();
    ~CBoss();

    void Init(int x, int y, int width, int height, int stage);
    void Update();
    void Render(CDC* pDC);
    void Fire();

    CRect GetRect() const { return m_rect; }
    bool IsActive() const { return m_bActive; }
    void SetActive(bool active) { m_bActive = active; }
    int GetHP() const { return m_nHP; }
    int GetMaxHP() const { return m_nMaxHP; }
    void Hit(int damage);

    std::vector<CBullet>& GetBullets() { return m_bullets; }

private:
    CRect m_rect;
    bool m_bActive;
    int m_nHP;
    int m_nMaxHP;
    int m_nSpeed;
    int m_nPattern;
    int m_nMoveCounter;
    int m_nFireCounter;
    int m_nFireDelay;
    int m_nGameWidth;
    int m_nGameHeight;
    int m_nInitX;
    int m_nDirection;

    std::vector<CBullet> m_bullets;
};