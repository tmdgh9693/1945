#include "pch.h"
#include "framework.h"
#include "Shooting1945.h"
#include "Shooting1945Dlg.h"
#include "afxdialogex.h"
#include <ctime>
#include <algorithm>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

class CAboutDlg : public CDialogEx
{
public:
    CAboutDlg();

#ifdef AFX_DESIGN_TIME
    enum { IDD = IDD_ABOUTBOX };
#endif

protected:
    virtual void DoDataExchange(CDataExchange* pDX);

protected:
    DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()

CShooting1945Dlg::CShooting1945Dlg(CWnd* pParent)
    : CDialogEx(IDD_SHOOTING1945_DIALOG, pParent)
{
    m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

    m_nGameWidth = 480;
    m_nGameHeight = 640;

    m_pOldBitmap = nullptr;
    m_nTimerID = 0;
    m_bGameRunning = false;
    m_bPaused = false;
    m_bInTitle = true;

    m_bKeyLeft = false;
    m_bKeyRight = false;
    m_bKeyUp = false;
    m_bKeyDown = false;
    m_bKeySpace = false;

    m_nSpawnCounter = 0;
    m_nSpawnDelay = 60;
    m_nScore = 0;
    m_nHighScore = 0;
    m_bBossSpawned = false;
    m_nKillCount = 0;
    m_bGameOver = false;

    m_nStage = 1;
    m_nEnemySpeedBase = 3;

    m_itemSlots[0] = POWER_DOUBLE_SHOT;
    m_itemSlots[1] = POWER_DOUBLE_SHOT;
    m_bHasItem[0] = false;
    m_bHasItem[1] = false;
    m_nSelectedItem = 0;

    m_eDifficulty = DIFF_EASY;
    m_nSelectedDifficulty = 0;
    m_nKillTarget = 10;

    for (int i = 0; i < 4; ++i)
        m_rcDiff[i].SetRectEmpty();
    m_rcBtnStartTitle.SetRectEmpty();
    m_rcBtnExitTitle.SetRectEmpty();
    m_rcBtnResume.SetRectEmpty();
    m_rcBtnToLobby.SetRectEmpty();
    m_rcBtnExitPause.SetRectEmpty();
}

void CShooting1945Dlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CShooting1945Dlg, CDialogEx)
    ON_WM_SYSCOMMAND()
    ON_WM_PAINT()
    ON_WM_QUERYDRAGICON()
    ON_WM_TIMER()
    ON_WM_ERASEBKGND()
    ON_WM_DESTROY()
    ON_WM_KEYDOWN()
    ON_WM_KEYUP()
    ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()

BOOL CShooting1945Dlg::OnInitDialog()
{
    CDialogEx::OnInitDialog();

    ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
    ASSERT(IDM_ABOUTBOX < 0xF000);

    CMenu* pSysMenu = GetSystemMenu(FALSE);
    if (pSysMenu != nullptr)
    {
        BOOL bNameValid;
        CString strAboutMenu;
        bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
        ASSERT(bNameValid);
        if (!strAboutMenu.IsEmpty())
        {
            pSysMenu->AppendMenu(MF_SEPARATOR);
            pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
        }
    }

    SetIcon(m_hIcon, TRUE);
    SetIcon(m_hIcon, FALSE);

    CRect rect;
    rect.SetRect(0, 0, m_nGameWidth, m_nGameHeight);
    AdjustWindowRect(&rect, GetStyle(), FALSE);
    SetWindowPos(NULL, 0, 0, rect.Width(), rect.Height(),
        SWP_NOMOVE | SWP_NOZORDER);

    CClientDC dc(this);
    m_memDC.CreateCompatibleDC(&dc);
    m_memBitmap.CreateCompatibleBitmap(&dc, m_nGameWidth, m_nGameHeight);
    m_pOldBitmap = m_memDC.SelectObject(&m_memBitmap);

    LoadHighScore();

    srand((unsigned)time(nullptr));

    InitGame();
    m_bGameRunning = false;
    m_bPaused = false;
    m_bInTitle = true;

    m_nTimerID = SetTimer(1, 16, NULL);

    return TRUE;
}

BOOL CShooting1945Dlg::PreTranslateMessage(MSG* pMsg)
{
    if (pMsg->message == WM_KEYDOWN)
    {
        WPARAM key = pMsg->wParam;

        if (key == VK_RETURN)
        {
            if (m_bInTitle)
            {
                if (m_nSelectedDifficulty == 0)      m_eDifficulty = DIFF_EASY;
                else if (m_nSelectedDifficulty == 1) m_eDifficulty = DIFF_NORMAL;
                else if (m_nSelectedDifficulty == 2) m_eDifficulty = DIFF_HARD;
                else                                 m_eDifficulty = DIFF_INFINITE;

                m_nStage = 1;
                m_nScore = 0;
                InitGame();

                m_bInTitle = false;
                m_bGameRunning = true;
                m_bPaused = false;
            }
            return TRUE;
        }

        if (key == VK_ESCAPE)
        {
            if (m_bInTitle)
            {
            }
            else if (!m_bGameOver)
            {
                m_bPaused = !m_bPaused;
            }
            return TRUE;
        }

        if (key == 'Q')
        {
            if (m_bInTitle)
            {
                EndDialog(IDOK);
            }
            else if (m_bPaused || m_bGameOver)
            {
                EndDialog(IDOK);
            }
            return TRUE;
        }

        if (m_bInTitle)
        {
            if (key == VK_UP)
            {
                m_nSelectedDifficulty--;
                if (m_nSelectedDifficulty < 0) m_nSelectedDifficulty = 3;
                return TRUE;
            }
            if (key == VK_DOWN)
            {
                m_nSelectedDifficulty++;
                if (m_nSelectedDifficulty > 3) m_nSelectedDifficulty = 0;
                return TRUE;
            }
            if (key == VK_SPACE)
            {
                if (m_nSelectedDifficulty == 0)      m_eDifficulty = DIFF_EASY;
                else if (m_nSelectedDifficulty == 1) m_eDifficulty = DIFF_NORMAL;
                else if (m_nSelectedDifficulty == 2) m_eDifficulty = DIFF_HARD;
                else                                 m_eDifficulty = DIFF_INFINITE;

                m_nStage = 1;
                m_nScore = 0;
                InitGame();

                m_bInTitle = false;
                m_bGameRunning = true;
                m_bPaused = false;
                return TRUE;
            }
        }
        else
        {
            switch (key)
            {
            case VK_LEFT:
                m_bKeyLeft = true;
                return TRUE;
            case VK_RIGHT:
                m_bKeyRight = true;
                return TRUE;
            case VK_UP:
                m_bKeyUp = true;
                return TRUE;
            case VK_DOWN:
                m_bKeyDown = true;
                return TRUE;
            case VK_SPACE:
                m_bKeySpace = true;
                return TRUE;

            case VK_TAB:
                if (!m_bGameOver && !m_bPaused)
                {
                    m_nSelectedItem = (m_nSelectedItem + 1) % 2;
                }
                return TRUE;

            case VK_CONTROL:
                if (!m_bGameOver && !m_bPaused)
                {
                    UseSelectedItem();
                }
                return TRUE;

            case 'R':
                if (m_bGameOver)
                {
                    m_bInTitle = true;
                    m_bGameRunning = false;
                    m_bPaused = false;
                }
                return TRUE;

            case 'T':
                if (m_bPaused && !m_bGameOver)
                {
                    m_bInTitle = true;
                    m_bGameRunning = false;
                    m_bPaused = false;
                    m_nStage = 1;
                    m_nScore = 0;
                    InitGame();
                }
                return TRUE;
            }
        }
    }
    else if (pMsg->message == WM_KEYUP)
    {
        switch (pMsg->wParam)
        {
        case VK_LEFT:   m_bKeyLeft = false; return TRUE;
        case VK_RIGHT:  m_bKeyRight = false; return TRUE;
        case VK_UP:     m_bKeyUp = false; return TRUE;
        case VK_DOWN:   m_bKeyDown = false; return TRUE;
        case VK_SPACE:  m_bKeySpace = false; return TRUE;
        }
    }

    return CDialogEx::PreTranslateMessage(pMsg);
}

void CShooting1945Dlg::LoadHighScore()
{
    CFile file;
    if (file.Open(L"highscore.dat", CFile::modeRead))
    {
        file.Read(&m_nHighScore, sizeof(int));
        file.Close();
    }
    else
    {
        m_nHighScore = 0;
    }
}

void CShooting1945Dlg::SaveHighScore()
{
    if (m_nScore > m_nHighScore)
    {
        m_nHighScore = m_nScore;
        CFile file;
        if (file.Open(L"highscore.dat", CFile::modeCreate | CFile::modeWrite))
        {
            file.Write(&m_nHighScore, sizeof(int));
            file.Close();
        }
    }
}

DifficultyType CShooting1945Dlg::GetEffectiveDifficulty() const
{
    if (m_eDifficulty != DIFF_INFINITE)
        return m_eDifficulty;

    if (m_nStage <= 5)  return DIFF_EASY;
    if (m_nStage <= 10) return DIFF_NORMAL;
    return DIFF_HARD;
}

void CShooting1945Dlg::UpdateDifficultyParams()
{
    DifficultyType diff = GetEffectiveDifficulty();

    if (diff == DIFF_EASY)
    {
        m_nSpawnDelay = 60 - (m_nStage - 1) * 3;
        if (m_nSpawnDelay < 25) m_nSpawnDelay = 25;

        m_nEnemySpeedBase = 2 + (m_nStage - 1) / 3;

        m_nKillTarget = 8 + (m_nStage - 1) * 2;
    }
    else if (diff == DIFF_NORMAL)
    {
        m_nSpawnDelay = 50 - (m_nStage - 1) * 3;
        if (m_nSpawnDelay < 20) m_nSpawnDelay = 20;

        m_nEnemySpeedBase = 3 + (m_nStage - 1) / 2;

        m_nKillTarget = 10 + (m_nStage - 1) * 3;
    }
    else if (diff == DIFF_HARD)
    {
        m_nSpawnDelay = 45 - (m_nStage - 1) * 4;
        if (m_nSpawnDelay < 15) m_nSpawnDelay = 15;

        m_nEnemySpeedBase = 4 + (m_nStage - 1) / 2;

        m_nKillTarget = 12 + (m_nStage - 1) * 4;
    }
}

void CShooting1945Dlg::InitGame()
{
    int playerWidth = 40;
    int playerHeight = 40;
    int playerX = (m_nGameWidth - playerWidth) / 2;
    int playerY = m_nGameHeight - 100;

    m_player.Init(playerX, playerY, playerWidth, playerHeight);

    m_enemies.clear();
    m_powerups.clear();
    m_bBossSpawned = false;
    m_nKillCount = 0;
    m_bGameOver = false;

    m_nSpawnCounter = 0;

    UpdateDifficultyParams();
}

void CShooting1945Dlg::OnSysCommand(UINT nID, LPARAM lParam)
{
    if ((nID & 0xFFF0) == IDM_ABOUTBOX)
    {
        CAboutDlg dlgAbout;
        dlgAbout.DoModal();
    }
    else
    {
        CDialogEx::OnSysCommand(nID, lParam);
    }
}

void CShooting1945Dlg::OnPaint()
{
    if (IsIconic())
    {
        CPaintDC dc(this);

        SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

        int cxIcon = GetSystemMetrics(SM_CXICON);
        int cyIcon = GetSystemMetrics(SM_CYICON);
        CRect rect;
        GetClientRect(&rect);
        int x = (rect.Width() - cxIcon + 1) / 2;
        int y = (rect.Height() - cyIcon + 1) / 2;

        dc.DrawIcon(x, y, m_hIcon);
    }
    else
    {
        CPaintDC dc(this);

        m_memDC.FillSolidRect(0, 0, m_nGameWidth, m_nGameHeight, RGB(0, 0, 0));

        RenderGame(&m_memDC);

        dc.BitBlt(0, 0, m_nGameWidth, m_nGameHeight, &m_memDC, 0, 0, SRCCOPY);
    }
}

void CShooting1945Dlg::OnTimer(UINT_PTR nIDEvent)
{
    if (nIDEvent == 1)
    {
        if (m_bGameRunning && !m_bPaused && !m_bInTitle && !m_bGameOver)
        {
            UpdateGame();
        }
        Invalidate(FALSE);
    }

    CDialogEx::OnTimer(nIDEvent);
}

void CShooting1945Dlg::UpdateGame()
{
    ProcessInput();

    m_player.Update();

    if (!m_bBossSpawned)
    {
        SpawnEnemy();

        if (m_nKillCount >= m_nKillTarget)
        {
            SpawnBoss();
        }
    }

    CRect playerRect = m_player.GetRect();
    for (auto& enemy : m_enemies)
    {
        enemy.Update(playerRect);
    }

    for (auto& powerup : m_powerups)
    {
        powerup.Update();
    }

    m_enemies.erase(
        std::remove_if(m_enemies.begin(), m_enemies.end(),
            [](const CEnemy& e) { return !e.IsActive(); }),
        m_enemies.end()
    );

    m_powerups.erase(
        std::remove_if(m_powerups.begin(), m_powerups.end(),
            [](const CPowerUp& p) { return !p.IsActive(); }),
        m_powerups.end()
    );

    auto& bullets = m_player.GetBullets();

    for (auto& bullet : bullets)
    {
        if (!bullet.IsActive())
            continue;

        CRect bulletRect = bullet.GetRect();

        for (auto& enemy : m_enemies)
        {
            if (!enemy.IsActive())
                continue;

            CRect enemyRect = enemy.GetRect();
            CRect intersectRect;

            if (intersectRect.IntersectRect(bulletRect, enemyRect))
            {
                bullet.SetActive(false);
                enemy.Hit(m_nStage);

                if (!enemy.IsActive())
                {
                    m_nScore += 100;
                    m_nKillCount++;

                    int dropX = enemyRect.left + enemyRect.Width() / 2;
                    int dropY = enemyRect.top;
                    DropPowerUp(dropX, dropY);
                }
                break;
            }
        }

        if (m_bBossSpawned && m_bBossSpawned && m_boss.IsActive())
        {
            CRect bossRect = m_boss.GetRect();
            CRect intersectRect2;
            if (intersectRect2.IntersectRect(bulletRect, bossRect))
            {
                bullet.SetActive(false);
                m_boss.Hit(m_nStage);

                if (!m_boss.IsActive())
                {
                    m_nScore += 5000;
                }
            }
        }
    }

    bullets.erase(
        std::remove_if(bullets.begin(), bullets.end(),
            [](const CBullet& b) { return !b.IsActive(); }),
        bullets.end()
    );

    if (m_bBossSpawned)
    {
        m_boss.Update();

        if (!m_boss.IsActive())
        {
            m_bBossSpawned = false;
            m_nKillCount = 0;
            m_nStage++;
            InitGame();
        }
    }

    CheckCollision();
}

void CShooting1945Dlg::SpawnEnemy()
{
    m_nSpawnCounter++;

    if (m_nSpawnCounter >= m_nSpawnDelay)
    {
        m_nSpawnCounter = 0;

        int enemyWidth = 30;
        int enemyHeight = 30;
        int enemyX = rand() % (m_nGameWidth - enemyWidth);
        int enemyY = -enemyHeight;

        DifficultyType diff = GetEffectiveDifficulty();
        int diffLevel = 0;
        if (diff == DIFF_NORMAL) diffLevel = 1;
        else if (diff == DIFF_HARD) diffLevel = 2;

        int r = rand() % 100;
        int pattern;

        if (diffLevel == 0)
        {
            if (r < 60) pattern = 0;
            else if (r < 90) pattern = 1;
            else             pattern = 2;
        }
        else if (diffLevel == 1)
        {
            if (r < 35) pattern = 0;
            else if (r < 75) pattern = 1;
            else             pattern = 2;
        }
        else
        {
            if (r < 20) pattern = 0;
            else if (r < 55) pattern = 1;
            else             pattern = 2;
        }

        int enemySpeed = m_nEnemySpeedBase;
        if (enemySpeed > 10) enemySpeed = 10;

        CEnemy enemy;
        enemy.Init(enemyX, enemyY,
            enemyWidth, enemyHeight,
            enemySpeed,
            pattern,
            m_nStage,
            diffLevel);
        m_enemies.push_back(enemy);
    }
}

void CShooting1945Dlg::SpawnBoss()
{
    if (m_bBossSpawned)
        return;

    int bossWidth = 100;
    int bossHeight = 80;
    int bossX = (m_nGameWidth - bossWidth) / 2;
    int bossY = 50;

    m_boss.Init(bossX, bossY, bossWidth, bossHeight, m_nStage);
    m_bBossSpawned = true;

    m_enemies.clear();
}

void CShooting1945Dlg::DropPowerUp(int x, int y)
{
    if (rand() % 100 >= 40)
        return;

    int powerupWidth = 25;
    int powerupHeight = 25;
    int powerupX = x - powerupWidth / 2;
    int powerupY = y;

    PowerUpType type = (PowerUpType)(rand() % 4);

    CPowerUp powerup;
    powerup.Init(powerupX, powerupY, powerupWidth, powerupHeight, type);
    m_powerups.push_back(powerup);
}

void CShooting1945Dlg::CheckCollision()
{
    CRect playerHitBox = m_player.GetHitBox();

    for (auto& enemy : m_enemies)
    {
        if (!enemy.IsActive())
            continue;

        CRect enemyRect = enemy.GetRect();
        CRect intersectRect;

        if (intersectRect.IntersectRect(playerHitBox, enemyRect))
        {
            if (m_player.HasShield())
            {
                m_player.SetShield(false);
                enemy.SetActive(false);
            }
            else
            {
                m_player.SetAlive(false);
                m_bGameRunning = false;
                m_bGameOver = true;
                SaveHighScore();
            }
            break;
        }
    }

    for (auto& powerup : m_powerups)
    {
        if (!powerup.IsActive())
            continue;

        CRect powerupRect = powerup.GetRect();
        CRect intersectRect;

        if (intersectRect.IntersectRect(playerHitBox, powerupRect))
        {
            AddItemToInventory(powerup.GetType());
            powerup.SetActive(false);
            m_nScore += 50;
        }
    }

    if (m_bBossSpawned && m_boss.IsActive())
    {
        auto& bossBullets = m_boss.GetBullets();

        for (auto& bullet : bossBullets)
        {
            if (!bullet.IsActive())
                continue;

            CRect bulletRect = bullet.GetRect();
            CRect intersectRect;

            if (intersectRect.IntersectRect(playerHitBox, bulletRect))
            {
                bullet.SetActive(false);

                if (m_player.HasShield())
                {
                    m_player.SetShield(false);
                }
                else
                {
                    m_player.SetAlive(false);
                    m_bGameRunning = false;
                    m_bGameOver = true;
                    SaveHighScore();
                }
                break;
            }
        }
    }

    for (auto& enemy : m_enemies)
    {
        if (!enemy.IsActive())
            continue;

        auto& enemyBullets = enemy.GetBullets();
        for (auto& bullet : enemyBullets)
        {
            if (!bullet.IsActive())
                continue;

            CRect bulletRect = bullet.GetRect();
            CRect intersectRect;

            if (intersectRect.IntersectRect(playerHitBox, bulletRect))
            {
                bullet.SetActive(false);

                if (m_player.HasShield())
                {
                    m_player.SetShield(false);
                }
                else
                {
                    m_player.SetAlive(false);
                    m_bGameRunning = false;
                    m_bGameOver = true;
                    SaveHighScore();
                }
                break;
            }
        }
    }

    if (m_bBossSpawned && m_boss.IsActive())
    {
        CRect bossRect = m_boss.GetRect();
        CRect intersectRect;

        if (intersectRect.IntersectRect(playerHitBox, bossRect))
        {
            if (m_player.HasShield())
            {
                m_player.SetShield(false);
            }
            else
            {
                m_player.SetAlive(false);
                m_bGameRunning = false;
                m_bGameOver = true;
                SaveHighScore();
            }
        }
    }
}

void CShooting1945Dlg::RenderGame(CDC* pDC)
{
    CString str;

    if (m_bInTitle)
    {
        pDC->SetTextColor(RGB(255, 255, 255));
        pDC->SetBkMode(TRANSPARENT);

        CFont titleFont;
        titleFont.CreatePointFont(400, L"Arial");
        CFont* pOldFont = pDC->SelectObject(&titleFont);

        str.Format(L"SHOOTING 1945");
        pDC->TextOutW(35, 80, str);

        pDC->SelectObject(pOldFont);

        CFont font;
        font.CreatePointFont(160, L"Arial");
        pOldFont = pDC->SelectObject(&font);

        str.Format(L"↑/↓/마우스로 선택 가능");
        pDC->TextOutW(145, 140, str);

        str.Format(L"SPACE/ENTER로 시작");
        pDC->TextOutW(140, 180, str);

        CString diffNames[4] = { L"쉬움", L"보통", L"어려움", L"무한" };

        int baseY = 250;
        int gapY = 50;

        for (int i = 0; i < 4; ++i)
        {
            int y = baseY + i * gapY;
            CString line;
            if (i == m_nSelectedDifficulty)
                line.Format(L"> %s", diffNames[i]);
            else
                line.Format(L"  %s", diffNames[i]);

            pDC->TextOutW(190, y, line);

            m_rcDiff[i].SetRect(140, y - 4, 340, y + 24);
        }

        CBrush startBrush(RGB(0, 120, 0));
        CBrush exitBrush(RGB(120, 0, 0));
        CBrush* pOldBrush = nullptr;

        m_rcBtnStartTitle.SetRect(300, 550, 400, 580);
        pOldBrush = pDC->SelectObject(&startBrush);
        pDC->Rectangle(m_rcBtnStartTitle);
        pDC->SelectObject(pOldBrush);
        str.Format(L"시작");
        pDC->TextOutW(322, 557, str);

        m_rcBtnExitTitle.SetRect(300, 590, 400, 620);
        pOldBrush = pDC->SelectObject(&exitBrush);
        pDC->Rectangle(m_rcBtnExitTitle);
        pDC->SelectObject(pOldBrush);
        str.Format(L"종료");
        pDC->TextOutW(322, 597, str);

        str.Format(L"최고 점수: %d", m_nHighScore);
        pDC->TextOutW(150, 480, str);

        pDC->SelectObject(pOldFont);
        return;
    }

    for (auto& enemy : m_enemies)
    {
        enemy.Render(pDC);
    }

    for (auto& powerup : m_powerups)
    {
        powerup.Render(pDC);
    }

    if (m_bBossSpawned)
    {
        m_boss.Render(pDC);
    }

    m_player.Render(pDC);

    pDC->SetTextColor(RGB(255, 255, 255));
    pDC->SetBkMode(TRANSPARENT);

    str.Format(L"점수: %d", m_nScore);
    pDC->TextOutW(10, 10, str);

    str.Format(L"최고 점수: %d", m_nHighScore);
    pDC->TextOutW(10, 30, str);

    str.Format(L"스테이지: %d  |  공격력: %d", m_nStage, m_nStage);
    pDC->TextOutW(10, 50, str);

    str.Format(L"처치: %d/%d (보스 출현)", m_nKillCount, m_nKillTarget);
    pDC->TextOutW(10, 70, str);

    CString slot1Name, slot2Name;

    if (m_bHasItem[0])
    {
        switch (m_itemSlots[0])
        {
        case POWER_DOUBLE_SHOT: slot1Name = L"x2"; break;
        case POWER_TRIPLE_SHOT: slot1Name = L"x3"; break;
        case POWER_SPEED_UP:    slot1Name = L"SPEED"; break;
        case POWER_SHIELD:      slot1Name = L"SHIELD"; break;
        default:                slot1Name = L"Unknown"; break;
        }
    }
    else
    {
        slot1Name = L"(빈 슬롯)";
    }

    if (m_bHasItem[1])
    {
        switch (m_itemSlots[1])
        {
        case POWER_DOUBLE_SHOT: slot2Name = L"x2"; break;
        case POWER_TRIPLE_SHOT: slot2Name = L"x3"; break;
        case POWER_SPEED_UP:    slot2Name = L"SPEED"; break;
        case POWER_SHIELD:      slot2Name = L"SHIELD"; break;
        default:                slot2Name = L"Unknown"; break;
        }
    }
    else
    {
        slot2Name = L"(빈 슬롯)";
    }

    CString slotStr;
    slotStr.Format(L"[슬롯1%s] %s",
        (m_nSelectedItem == 0 ? L" *" : L""),
        slot1Name);
    pDC->TextOutW(10, 90, slotStr);

    slotStr.Format(L"[슬롯2%s] %s",
        (m_nSelectedItem == 1 ? L" *" : L""),
        slot2Name);
    pDC->TextOutW(10, 110, slotStr);

    if (m_player.GetShotType() > 0 || m_player.HasShield())
    {
        int remainingTime = m_player.GetPowerUpRemainingTime();
        if (remainingTime > 0)
        {
            str.Format(L"파워업 남은 시간: %d초", remainingTime);
            pDC->TextOutW(10, 130, str);
        }
    }

    if (m_bGameOver)
    {
        pDC->SetTextColor(RGB(255, 0, 0));
        CFont font;
        font.CreatePointFont(300, L"Arial");
        CFont* pOldFont2 = pDC->SelectObject(&font);

        str.Format(L"GAME OVER");
        pDC->TextOutW(80, 250, str);

        pDC->SetTextColor(RGB(255, 255, 255));
        str.Format(L"최종 점수: %d", m_nScore);
        pDC->TextOutW(120, 320, str);

        str.Format(L"R 키: 로비로 돌아가기");
        pDC->TextOutW(120, 350, str);

        pDC->SelectObject(pOldFont2);
    }

    if (m_bPaused && !m_bGameOver)
    {
        pDC->SetTextColor(RGB(255, 255, 0));
        CFont font;
        font.CreatePointFont(220, L"Arial");
        CFont* pOldFont3 = pDC->SelectObject(&font);

        str.Format(L"PAUSED");
        pDC->TextOutW(160, 220, str);

        CBrush resumeBrush(RGB(0, 120, 0));
        CBrush lobbyBrush(RGB(0, 0, 120));
        CBrush exitBrush(RGB(120, 0, 0));
        CBrush* pOldBrush = nullptr;

        m_rcBtnResume.SetRect(140, 280, 340, 315);
        pOldBrush = pDC->SelectObject(&resumeBrush);
        pDC->Rectangle(m_rcBtnResume);
        pDC->SelectObject(pOldBrush);
        str.Format(L"계속하기 (ESC)");
        pDC->TextOutW(165, 287, str);

        m_rcBtnToLobby.SetRect(140, 320, 340, 355);
        pOldBrush = pDC->SelectObject(&lobbyBrush);
        pDC->Rectangle(m_rcBtnToLobby);
        pDC->SelectObject(pOldBrush);
        str.Format(L"로비로 돌아가기 (T)");
        pDC->TextOutW(155, 327, str);

        m_rcBtnExitPause.SetRect(140, 360, 340, 395);
        pOldBrush = pDC->SelectObject(&exitBrush);
        pDC->Rectangle(m_rcBtnExitPause);
        pDC->SelectObject(pOldBrush);
        str.Format(L"게임 종료 (Q)");
        pDC->TextOutW(175, 367, str);

        pDC->SelectObject(pOldFont3);
    }
}

BOOL CShooting1945Dlg::OnEraseBkgnd(CDC* pDC)
{
    return TRUE;
}

void CShooting1945Dlg::OnDestroy()
{
    CDialogEx::OnDestroy();

    if (m_nTimerID)
    {
        KillTimer(m_nTimerID);
    }

    if (m_pOldBitmap)
    {
        m_memDC.SelectObject(m_pOldBitmap);
    }
    m_memBitmap.DeleteObject();
    m_memDC.DeleteDC();
}

void CShooting1945Dlg::ProcessInput()
{
    if (m_bKeyLeft)
        m_player.MoveLeft();

    if (m_bKeyRight)
        m_player.MoveRight();

    if (m_bKeyUp)
        m_player.MoveUp();

    if (m_bKeyDown)
        m_player.MoveDown();

    if (m_bKeySpace)
        m_player.Fire();
}

void CShooting1945Dlg::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
    CDialogEx::OnKeyDown(nChar, nRepCnt, nFlags);
}

void CShooting1945Dlg::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{
    CDialogEx::OnKeyUp(nChar, nRepCnt, nFlags);
}

HCURSOR CShooting1945Dlg::OnQueryDragIcon()
{
    return static_cast<HCURSOR>(m_hIcon);
}

void CShooting1945Dlg::AddItemToInventory(PowerUpType type)
{
    if (!m_bHasItem[0])
    {
        m_itemSlots[0] = type;
        m_bHasItem[0] = true;
        return;
    }

    if (!m_bHasItem[1])
    {
        m_itemSlots[1] = type;
        m_bHasItem[1] = true;
        return;
    }

    m_itemSlots[m_nSelectedItem] = type;
    m_bHasItem[m_nSelectedItem] = true;
}

void CShooting1945Dlg::UseSelectedItem()
{
    int idx = m_nSelectedItem;
    if (!m_bHasItem[idx])
        return;

    PowerUpType type = m_itemSlots[idx];
    m_bHasItem[idx] = false;

    m_player.ApplyPowerUp((int)type);
}

void CShooting1945Dlg::OnLButtonDown(UINT nFlags, CPoint point)
{
    if (m_bInTitle)
    {
        for (int i = 0; i < 4; ++i)
        {
            if (m_rcDiff[i].PtInRect(point))
            {
                m_nSelectedDifficulty = i;
                Invalidate(FALSE);
                return;
            }
        }

        if (m_rcBtnStartTitle.PtInRect(point))
        {
            if (m_nSelectedDifficulty == 0)      m_eDifficulty = DIFF_EASY;
            else if (m_nSelectedDifficulty == 1) m_eDifficulty = DIFF_NORMAL;
            else if (m_nSelectedDifficulty == 2) m_eDifficulty = DIFF_HARD;
            else                                 m_eDifficulty = DIFF_INFINITE;

            m_nStage = 1;
            m_nScore = 0;
            InitGame();

            m_bInTitle = false;
            m_bGameRunning = true;
            m_bPaused = false;
            Invalidate(FALSE);
            return;
        }

        if (m_rcBtnExitTitle.PtInRect(point))
        {
            EndDialog(IDOK);
            return;
        }
    }
    else if (m_bPaused && !m_bGameOver)
    {
        if (m_rcBtnResume.PtInRect(point))
        {
            m_bPaused = false;
            Invalidate(FALSE);
            return;
        }

        if (m_rcBtnToLobby.PtInRect(point))
        {
            m_bInTitle = true;
            m_bGameRunning = false;
            m_bPaused = false;
            m_nStage = 1;
            m_nScore = 0;
            InitGame();
            Invalidate(FALSE);
            return;
        }

        if (m_rcBtnExitPause.PtInRect(point))
        {
            EndDialog(IDOK);
            return;
        }
    }

    CDialogEx::OnLButtonDown(nFlags, point);
}