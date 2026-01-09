#pragma once
#include "afxwin.h"

enum PowerUpType
{
    POWER_NONE = -1,
    POWER_DOUBLE_SHOT = 0,
    POWER_TRIPLE_SHOT = 1,
    POWER_SPEED_UP = 2,
    POWER_SHIELD = 3
};

class CPowerUp
{
public:
    CPowerUp();
    ~CPowerUp();

    void Init(int x, int y, int width, int height, PowerUpType type);
    void Update();
    void Render(CDC* pDC);

    CRect GetRect() const { return m_rect; }
    bool IsActive() const { return m_bActive; }
    void SetActive(bool active) { m_bActive = active; }
    PowerUpType GetType() const { return m_type; }

private:
    CRect m_rect;
    bool m_bActive;
    PowerUpType m_type;
    int m_nSpeed;
    int m_nGameHeight;
};
