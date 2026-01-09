#pragma once
#include "afxwin.h"

class CBullet
{
public:
    CBullet();
    ~CBullet();

    void Init(int x, int y, int width, int height, int speed);
    void Update();
    void Render(CDC* pDC);

    CRect GetRect() const { return m_rect; }
    bool IsActive() const { return m_bActive; }
    void SetActive(bool active) { m_bActive = active; }

private:
    CRect m_rect;
    int m_nSpeed;
    bool m_bActive;
    int m_nGameHeight;
};
