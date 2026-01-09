#pragma once

#include <vector>
#include "Bullet.h"
#include "Player.h"
#include "Enemy.h"
#include "PowerUp.h"
#include "Boss.h"

enum DifficultyType
{
    DIFF_EASY = 0,
    DIFF_NORMAL = 1,
    DIFF_HARD = 2,
    DIFF_INFINITE = 3
};

class CShooting1945Dlg : public CDialogEx
{
public:
    CShooting1945Dlg(CWnd* pParent = nullptr);

#ifdef AFX_DESIGN_TIME
    enum { IDD = IDD_SHOOTING1945_DIALOG };
#endif

protected:
    virtual void DoDataExchange(CDataExchange* pDX);
    virtual BOOL PreTranslateMessage(MSG* pMsg);

protected:
    HICON m_hIcon;

    virtual BOOL OnInitDialog();
    afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
    afx_msg void OnPaint();
    afx_msg HCURSOR OnQueryDragIcon();
    afx_msg void OnTimer(UINT_PTR nIDEvent);
    afx_msg BOOL OnEraseBkgnd(CDC* pDC);
    afx_msg void OnDestroy();
    afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
    afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
    afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
    DECLARE_MESSAGE_MAP()

private:
    void InitGame();
    void UpdateGame();
    void RenderGame(CDC* pDC);
    void ProcessInput();
    void SpawnEnemy();
    void SpawnBoss();
    void CheckCollision();
    void SaveHighScore();
    void LoadHighScore();

    void AddItemToInventory(PowerUpType type);
    void UseSelectedItem();
    void DropPowerUp(int x, int y);

    void UpdateDifficultyParams();
    DifficultyType GetEffectiveDifficulty() const;

private:
    CDC      m_memDC;
    CBitmap  m_memBitmap;
    CBitmap* m_pOldBitmap;

    int      m_nGameWidth;
    int      m_nGameHeight;

    UINT_PTR m_nTimerID;

    bool     m_bGameRunning;
    bool     m_bPaused;
    bool     m_bInTitle;

    CPlayer               m_player;
    std::vector<CEnemy>   m_enemies;
    std::vector<CPowerUp> m_powerups;
    CBoss                 m_boss;

    bool m_bKeyLeft;
    bool m_bKeyRight;
    bool m_bKeyUp;
    bool m_bKeyDown;
    bool m_bKeySpace;

    PowerUpType m_itemSlots[2];
    bool        m_bHasItem[2];
    int         m_nSelectedItem;

    int  m_nSpawnCounter;
    int  m_nSpawnDelay;
    int  m_nScore;
    int  m_nHighScore;
    bool m_bBossSpawned;
    int  m_nKillCount;
    bool m_bGameOver;

    int  m_nStage;
    int  m_nEnemySpeedBase;

    DifficultyType m_eDifficulty;
    int            m_nSelectedDifficulty;
    int            m_nKillTarget;

    CRect m_rcDiff[4];
    CRect m_rcBtnStartTitle;
    CRect m_rcBtnExitTitle;
    CRect m_rcBtnResume;
    CRect m_rcBtnToLobby;
    CRect m_rcBtnExitPause;
};