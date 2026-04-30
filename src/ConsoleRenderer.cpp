#include "ConsoleRenderer.h"
#include "Player.h"
#include "Training.h"

#include <algorithm>
#include <chrono>
#include <conio.h>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <thread>
#include <utility>
#include <vector>

#ifdef _WIN32
#include <windows.h>
#endif

using namespace std;

namespace
{
    const int BOX_W = 78;
    const int TRACK_W = 76;

    enum class UiColor
    {
        Normal,
        Dim,
        Accent,
        Highlight,
        Good,
        Warning,
        Danger,
        Title
    };

    void setColor(UiColor color)
    {
#ifdef _WIN32
        static HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
        WORD attr = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;
        switch (color)
        {
        case UiColor::Dim:       attr = FOREGROUND_BLUE | FOREGROUND_GREEN; break;
        case UiColor::Accent:    attr = FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY; break;
        case UiColor::Highlight: attr = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY; break;
        case UiColor::Good:      attr = FOREGROUND_GREEN | FOREGROUND_INTENSITY; break;
        case UiColor::Warning:   attr = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY; break;
        case UiColor::Danger:    attr = FOREGROUND_RED | FOREGROUND_INTENSITY; break;
        case UiColor::Title:     attr = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY; break;
        default:                 attr = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE; break;
        }
        SetConsoleTextAttribute(handle, attr);
#else
        switch (color)
        {
        case UiColor::Dim:       cout << "\033[36m"; break;
        case UiColor::Accent:    cout << "\033[96m"; break;
        case UiColor::Highlight: cout << "\033[93m"; break;
        case UiColor::Good:      cout << "\033[92m"; break;
        case UiColor::Warning:   cout << "\033[33m"; break;
        case UiColor::Danger:    cout << "\033[91m"; break;
        case UiColor::Title:     cout << "\033[97m"; break;
        default:                 cout << "\033[0m"; break;
        }
#endif
    }

    void resetColor()
    {
        setColor(UiColor::Normal);
    }

#ifdef _WIN32
    void configureGameConsole()
    {
        HANDLE output = GetStdHandle(STD_OUTPUT_HANDLE);
        if (output == INVALID_HANDLE_VALUE) return;

        CONSOLE_SCREEN_BUFFER_INFO info;
        if (!GetConsoleScreenBufferInfo(output, &info)) return;

        COORD largest = GetLargestConsoleWindowSize(output);
        SHORT columns = min<SHORT>(100, largest.X);
        SHORT rows = min<SHORT>(36, largest.Y);

        SMALL_RECT shrinkRect = { 0, 0, min<SHORT>(info.srWindow.Right, columns - 1), min<SHORT>(info.srWindow.Bottom, rows - 1) };
        SetConsoleWindowInfo(output, TRUE, &shrinkRect);

        COORD bufferSize = { columns, rows };
        SetConsoleScreenBufferSize(output, bufferSize);

        SMALL_RECT windowRect = { 0, 0, static_cast<SHORT>(columns - 1), static_cast<SHORT>(rows - 1) };
        SetConsoleWindowInfo(output, TRUE, &windowRect);

        CONSOLE_CURSOR_INFO cursorInfo;
        if (GetConsoleCursorInfo(output, &cursorInfo))
        {
            cursorInfo.bVisible = FALSE;
            SetConsoleCursorInfo(output, &cursorInfo);
        }

        DWORD mode = 0;
        HANDLE input = GetStdHandle(STD_INPUT_HANDLE);
        if (input != INVALID_HANDLE_VALUE && GetConsoleMode(input, &mode))
        {
            mode &= ~ENABLE_QUICK_EDIT_MODE;
            mode |= ENABLE_EXTENDED_FLAGS;
            SetConsoleMode(input, mode);
        }
    }
#endif

    string repeat(char ch, int count)
    {
        return string(max(0, count), ch);
    }

    string line(char fill = '=')
    {
        return string(BOX_W, fill);
    }

    void printLine(char fill = '=')
    {
        setColor(fill == '=' ? UiColor::Accent : UiColor::Dim);
        cout << line(fill) << "\n";
        resetColor();
    }

    void printTitle(const string& title, const string& subTitle = "")
    {
        printLine('=');
        setColor(UiColor::Title);
        cout << "  " << left << setw(48) << title;
        resetColor();
        if (!subTitle.empty())
        {
            setColor(UiColor::Highlight);
            cout << right << setw(24) << subTitle;
            resetColor();
        }
        cout << "\n";
        cout << right;
        printLine('-');
    }

    void printPrompt()
    {
        setColor(UiColor::Highlight);
        cout << "\n  입력 > ";
        resetColor();
    }

    string gauge(int value, int maxValue, int width, char fill = '#')
    {
        int filled = (maxValue > 0) ? value * width / maxValue : 0;
        filled = max(0, min(width, filled));
        return "[" + repeat(fill, filled) + repeat('.', width - filled) + "]";
    }

    string hpBar(int hp, int maxHp)
    {
        return gauge(hp, maxHp, 24, '#');
    }

    string statBar(int value)
    {
        return gauge(min(value, 120), 120, 18, '#');
    }

    string shortName(const string& name)
    {
        if (name.size() <= 16) return name;
        return name.substr(0, 15) + ".";
    }

    string displaySurface(const string& surface)
    {
        if (surface == "Turf") return "잔디";
        if (surface == "Dirt") return "더트";
        return surface;
    }

    string displayWeather(const string& weather)
    {
        if (weather == "Sunny") return "맑음";
        return weather;
    }

    string phaseIcon(const string& title)
    {
        if (title.find("초반") != string::npos) return "출발";
        if (title.find("중반") != string::npos) return "중반";
        if (title.find("후반") != string::npos) return "추격";
        if (title.find("최종") != string::npos) return "최종";
        return "경주";
    }

    void sleepRaceFrame()
    {
        this_thread::sleep_for(chrono::milliseconds(700));
    }

    vector<pair<int, int>> buildOvalPath(int lane)
    {
        vector<pair<int, int>> path;
        int top = 5 - lane;
        int bottom = 11 + lane;
        int left = 16 - lane * 2;
        int right = 55 + lane * 2;

        for (int x = left; x <= right; x++) path.push_back({ top, x });
        path.push_back({ top + 1, right + 4 });
        path.push_back({ top + 2, right + 6 });
        for (int y = top + 3; y <= bottom - 3; y++) path.push_back({ y, right + 7 });
        path.push_back({ bottom - 2, right + 6 });
        path.push_back({ bottom - 1, right + 4 });
        for (int x = right; x >= left; x--) path.push_back({ bottom, x });
        path.push_back({ bottom - 1, left - 4 });
        path.push_back({ bottom - 2, left - 6 });
        for (int y = bottom - 3; y >= top + 3; y--) path.push_back({ y, left - 7 });
        path.push_back({ top + 2, left - 6 });
        path.push_back({ top + 1, left - 4 });

        return path;
    }

    void drawTopViewTrack(const vector<RaceRunnerState>& runners, int raceDistance)
    {
        const int H = 18;
        const int courseLapDistance = 1600;
        int finishCoursePosition = static_cast<int>((raceDistance % courseLapDistance) * 100 / courseLapDistance);
        if (finishCoursePosition == 0) finishCoursePosition = 100;
        vector<string> field(H, string(TRACK_W, ' '));
        vector<vector<int>> runnerCount(H, vector<int>(TRACK_W, 0));
        vector<vector<bool>> playerInCell(H, vector<bool>(TRACK_W, false));
        vector<vector<int>> frontRankInCell(H, vector<int>(TRACK_W, 99));
        vector<vector<pair<int, int>>> lanes;

        for (int lane = 0; lane < 4; lane++)
        {
            lanes.push_back(buildOvalPath(lane));
            for (const auto& p : lanes.back())
            {
                if (p.first >= 0 && p.first < H && p.second >= 0 && p.second < TRACK_W)
                    field[p.first][p.second] = '.';
            }
        }

        for (int y = 5; y <= 11; y++)
            for (int x = 17; x <= 54; x++)
                field[y][x] = (y == 8 ? '-' : ' ');

        for (const auto& lanePath : lanes)
        {
            const int finishIndex = finishCoursePosition * (static_cast<int>(lanePath.size()) - 1) / 100;
            field[lanePath[finishIndex].first][lanePath[finishIndex].second] = '|';
        }

        for (const auto& runner : runners)
        {
            int lane = max(0, min(3, runner.lane));
            const auto& path = lanes[lane];
            if (path.empty()) continue;

            int base = runner.coursePosition * (static_cast<int>(path.size()) - 1) / 100;
            int idx = max(0, min(static_cast<int>(path.size()) - 1, base));
            int y = path[idx].first;
            int x = path[idx].second;
            if (y < 0 || y >= H || x < 0 || x >= TRACK_W) continue;

            runnerCount[y][x]++;
            playerInCell[y][x] = playerInCell[y][x] || runner.isPlayer;
            frontRankInCell[y][x] = min(frontRankInCell[y][x], runner.rank);
        }

        for (int y = 0; y < H; y++)
        {
            for (int x = 0; x < TRACK_W; x++)
            {
                if (runnerCount[y][x] <= 0) continue;

                if (runnerCount[y][x] >= 2)
                {
                    field[y][x] = playerInCell[y][x] ? '@' : '*';
                }
                else if (playerInCell[y][x])
                {
                    field[y][x] = 'P';
                }
                else
                {
                    field[y][x] = static_cast<char>('0' + min(9, frontRankInCell[y][x]));
                }
            }
        }

        setColor(UiColor::Dim);
        cout << "  +" << string(TRACK_W, '-') << "+\n";
        resetColor();

        for (const auto& row : field)
        {
            setColor(UiColor::Dim);
            cout << "  |";
            resetColor();
            for (char ch : row)
            {
                if (ch == 'P' || ch == '@') setColor(UiColor::Highlight);
                else if (ch == '*') setColor(UiColor::Warning);
                else if (ch >= '1' && ch <= '9') setColor(UiColor::Danger);
                else if (ch == '|') setColor(UiColor::Good);
                else if (ch == '.') setColor(UiColor::Dim);
                else resetColor();
                cout << ch;
            }
            resetColor();
            setColor(UiColor::Dim);
            cout << "|\n";
            resetColor();
        }

        setColor(UiColor::Dim);
        cout << "  +" << string(TRACK_W, '-') << "+\n";
        resetColor();

        cout << "   트랙 1바퀴 기준: 1600m / 레이스 거리: " << raceDistance << "m\n";
        cout << "   P: 선택 캐릭터  숫자: 상대 현재 순위  @: 선택 캐릭터 포함 마군  *: 마군  |: 결승선\n";
        cout << "   안쪽은 마군 위험, 바깥은 거리 손해 대신 추월 라인을 잡습니다.\n";
    }

    void printCommandCard(char key, const string& title, const string& desc)
    {
        setColor(UiColor::Highlight);
        cout << "  [" << key << "] ";
        resetColor();
        cout << left << setw(10) << title;
        setColor(UiColor::Dim);
        cout << "  " << desc << "\n";
        resetColor();
        cout << right;
    }

    void printStatRow(const string& label, int value)
    {
        cout << "  " << left << setw(4) << label << " " << statBar(value)
             << " " << right << setw(3) << value << "\n";
    }

    void printGain(const string& label, int value)
    {
        if (value <= 0) return;
        cout << "  " << left << setw(4) << label << " ";
        setColor(UiColor::Good);
        cout << "+" << value;
        resetColor();
        cout << "\n";
    }
}

void ConsoleRenderer::init()
{
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
    SetConsoleTitleA("Console Uma Training");
    configureGameConsole();
#endif
}

void ConsoleRenderer::clearScreen()
{
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

void ConsoleRenderer::waitAnyKey()
{
    setColor(UiColor::Dim);
    cout << "\n  아무 키나 누르면 계속...\n";
    resetColor();
    _getch();
}

void ConsoleRenderer::drawCharacterSelect()
{
    clearScreen();
    printTitle("콘솔 우마 트레이닝", "캐릭터 선택");
    cout << "  출전할 우마무스메를 선택하세요.\n\n";

    setColor(UiColor::Highlight);
    cout << "  [1] 아그네스 타키온\n";
    resetColor();
    cout << "      각질 도주   고유기 U=ma2\n";
    cout << "      SPD 72  STA 63  PWR 68  GTS 62  INT 88\n";
    cout << "      빠른 전개와 지능으로 초중반 흐름을 잡습니다.\n\n";

    setColor(UiColor::Highlight);
    cout << "  [2] 맨하탄 카페\n";
    resetColor();
    cout << "      각질 추입   고유기 그대를 좇아서\n";
    cout << "      SPD 66  STA 85  PWR 72  GTS 80  INT 68\n";
    cout << "      체력과 뒷심으로 마지막 직선에서 추격합니다.\n";

    printLine('=');
    printPrompt();
}

void ConsoleRenderer::drawMainScreen(const Player& uma, const string& dateText)
{
    clearScreen();
    printTitle("콘솔 우마 트레이닝", dateText);

    setColor(UiColor::Highlight);
    cout << "  " << uma.getName() << "\n";
    resetColor();
    cout << "  각질 " << Race::getRunningStyleName(uma.getRunningStyle())
         << "  |  고유기 " << uma.getUniqueSkillName() << "\n\n";

    cout << "  컨디션  " << hpBar(uma.getHp(), uma.getMaxHp())
         << "  " << setw(3) << uma.getHp() << "/" << uma.getMaxHp() << "\n";
    printLine('-');

    cout << "  능력치\n";
    printStatRow("SPD", uma.getSpeed());
    printStatRow("STA", uma.getStamina());
    printStatRow("PWR", uma.getPower());
    printStatRow("GTS", uma.getGuts());
    printStatRow("INT", uma.getIntelligence());
    printLine('-');

    cout << "  행동\n";
    printCommandCard('1', "훈련", "능력치를 올리고 체력을 소모합니다.");
    printCommandCard('2', "휴식", "체력을 회복하고 다음 턴으로 넘어갑니다.");
    printCommandCard('3', "레이스", "현재 일정의 레이스에 출전합니다.");
    printCommandCard('q', "종료", "게임을 종료합니다.");
    printLine('=');
    printPrompt();
}

void ConsoleRenderer::drawTrainingMenu()
{
    clearScreen();
    printTitle("트레이닝 센터", "훈련 선택");
    printCommandCard('1', "스피드", "최고 속도와 포지션 유지에 영향을 줍니다.");
    printCommandCard('2', "스태미나", "긴 거리와 후반 체력 유지에 중요합니다.");
    printCommandCard('3', "파워", "추월, 가속, 마군 돌파에 영향을 줍니다.");
    printCommandCard('4', "근성", "최종 직선에서 버티는 힘을 올립니다.");
    printCommandCard('5', "지능", "스킬 발동과 위치 선정에 영향을 줍니다.");
    printCommandCard('0', "뒤로", "메인 화면으로 돌아갑니다.");
    printLine('=');
    printPrompt();
}

void ConsoleRenderer::drawRaceMenu(const string& dateText, const vector<Race>& races)
{
    clearScreen();
    printTitle("레이스 일정", dateText);

    if (races.empty())
    {
        cout << "  이번 턴에 출전 가능한 레이스가 없습니다.\n";
        printLine('=');
        waitAnyKey();
        return;
    }

    cout << "  번호  등급   레이스명                       경기장     코스   거리\n";
    printLine('-');

    for (size_t i = 0; i < races.size(); i++)
    {
        const Race& race = races[i];
        setColor(UiColor::Highlight);
        cout << "  [" << (i + 1) << "] ";
        resetColor();
        cout << left << setw(5) << (race.getGrade().empty() ? "-" : race.getGrade())
             << setw(30) << race.getCourseName()
             << setw(10) << race.getVenue()
             << setw(7) << displaySurface(race.getSurface())
             << race.getDistance() << "m\n";
    }

    printLine('-');
    printCommandCard('0', "뒤로", "출전하지 않고 메인 화면으로 돌아갑니다.");
    printLine('=');
    printPrompt();
}

void ConsoleRenderer::drawTrainingResult(const TrainingResult& result)
{
    clearScreen();
    printTitle("훈련 결과");

    if (!result.success)
    {
        setColor(UiColor::Danger);
        cout << "  훈련 실패\n";
        resetColor();
        cout << "  체력이 부족해 훈련을 제대로 소화하지 못했습니다.\n";
    }
    else
    {
        setColor(UiColor::Good);
        cout << "  훈련 성공\n";
        resetColor();
        printLine('-');
        printGain("SPD", result.speedGain);
        printGain("STA", result.staminaGain);
        printGain("PWR", result.powerGain);
        printGain("GTS", result.gutsGain);
        printGain("INT", result.intelligenceGain);
    }

    printLine('-');
    cout << "  체력 ";
    setColor(UiColor::Warning);
    cout << "-" << result.hpCost << "\n";
    resetColor();
    printLine('=');
    waitAnyKey();
}

void ConsoleRenderer::drawRestResult(int hpGained)
{
    clearScreen();
    printTitle("휴식");
    cout << "  휴식을 취했습니다.\n";
    printLine('-');
    cout << "  체력 ";
    setColor(UiColor::Good);
    cout << "+" << hpGained << "\n";
    resetColor();
    printLine('=');
    waitAnyKey();
}

void ConsoleRenderer::drawRaceResult(const Race& course, const RaceResult& result)
{
    auto drawTrack = [&](const RaceMoment& moment)
    {
        clearScreen();
        ostringstream meta;
        meta << course.getVenue() << " " << displaySurface(course.getSurface())
             << " " << course.getDistance() << "m";
        printTitle(course.getCourseName() + (course.getGrade().empty() ? "" : " " + course.getGrade()), meta.str());

        setColor(UiColor::Highlight);
        cout << "  [" << phaseIcon(moment.title) << "] ";
        resetColor();
        cout << moment.title << "\n";
        cout << "  해설: " << moment.commentary << "\n";
        printLine('-');

        drawTopViewTrack(moment.runners, course.getDistance());
        printLine('-');

        cout << "  현재 순위\n";
        for (const auto& runner : moment.runners)
        {
            setColor(runner.isPlayer ? UiColor::Highlight : UiColor::Normal);
            cout << "  " << setw(2) << runner.rank << " ";
            cout << (runner.isPlayer ? ">" : " ");
            cout << left << setw(16) << shortName(runner.name) << right;
            resetColor();

            cout << " " << setw(4) << Race::getRunningStyleName(runner.style);
            cout << " L" << runner.lane + 1;
            cout << " " << runner.lap << "/" << runner.totalLaps << "바퀴";
            cout << " 진행 " << setw(3) << runner.trackPosition << "%";
            cout << " 체력 " << setw(3) << runner.staminaPercent << "%";
            cout << " 속도 " << setw(4) << static_cast<int>(runner.speed) << "\n";

            if (runner.blocked)
            {
                setColor(UiColor::Warning);
                cout << "      ! 마군에 막힘: 앞이 막혀 속도가 떨어집니다.\n";
                resetColor();
            }
            if (runner.wide)
            {
                setColor(UiColor::Accent);
                cout << "      ! 외곽 주행: 크게 돌며 추월 라인을 잡습니다.\n";
                resetColor();
            }
            if (runner.uniqueSkillActive)
            {
                setColor(UiColor::Good);
                cout << "      ! 고유기 발동: " << runner.uniqueSkillName << "\n";
                resetColor();
            }
        }

        printLine('=');
    };

    clearScreen();
    printTitle("레이스 데이", course.getCourseName());
    cout << "  레이스 : " << course.getCourseName();
    if (!course.getGrade().empty()) cout << " " << course.getGrade();
    cout << "\n";
    cout << "  경기장 : " << course.getVenue() << "\n";
    cout << "  코스   : " << displaySurface(course.getSurface()) << " / " << displayWeather(course.getWeather()) << "\n";
    cout << "  거리   : " << course.getDistance() << "m\n";
    cout << "  출전   : " << result.totalParticipants << "명\n";
    printLine('-');
    cout << "  탑뷰 트랙에서 P는 선택 캐릭터, 숫자는 상대의 현재 순위입니다.\n";
    cout << "  아무 키나 누르면 게이트가 열립니다.\n";
    printLine('=');
    _getch();

    for (const auto& moment : result.moments)
    {
        drawTrack(moment);
        sleepRaceFrame();
    }

    clearScreen();
    printTitle("결승", "결승 결과");

    for (const auto& r : result.finalRanks)
    {
        if (r.rank == 1) setColor(UiColor::Highlight);
        else if (r.isPlayer) setColor(UiColor::Accent);
        else resetColor();

        cout << "  " << setw(2) << r.rank << "  " << left << setw(20) << r.name << right;
        resetColor();
        if (r.isPlayer) cout << "  <선택 캐릭터>";
        cout << "\n";
    }

    printLine('-');
    if (result.playerFinalRank < 0)
    {
        cout << "  선택 캐릭터 결과가 기록되지 않았습니다.\n";
    }
    else if (result.playerFinalRank == 1)
    {
        setColor(UiColor::Highlight);
        cout << "  우승! 결승선 앞에서 관중석이 터져 나옵니다.\n";
        resetColor();
    }
    else if (result.playerFinalRank <= 3)
    {
        setColor(UiColor::Good);
        cout << "  " << result.playerFinalRank << "위 입상! 마지막까지 좋은 스퍼트였습니다.\n";
        resetColor();
    }
    else
    {
        cout << "  최종 " << result.playerFinalRank << "위. 다음 레이스를 위해 더 단련합시다.\n";
    }

    printLine('=');
    waitAnyKey();
}
