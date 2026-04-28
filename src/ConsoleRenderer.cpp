#include "ConsoleRenderer.h"
#include "Player.h"
#include "Training.h"
#include "Race.h"
#include <iostream>
#include <iomanip>
#include <string>
#include <conio.h>

#ifdef _WIN32
#include <windows.h>
#endif

static const int BOX_W = 54;

static std::string line(char fill = '=') {
    return std::string(BOX_W, fill);
}

static std::string hpBar(int hp, int maxHp) {
    const int BAR = 24;
    int filled = (maxHp > 0) ? (hp * BAR / maxHp) : 0;
    std::string bar = "[";
    for (int i = 0; i < BAR; i++)
        bar += (i < filled) ? '=' : ' ';
    bar += "]";
    return bar;
}

// -------------------------------------------------------

void ConsoleRenderer::init() {
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
#endif
}

void ConsoleRenderer::clearScreen() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

void ConsoleRenderer::waitAnyKey() {
    std::cout << "  아무 키나 누르면 계속...\n";
    _getch();
}

void ConsoleRenderer::drawCharacterSelect() {
    clearScreen();
    std::cout << line() << "\n";
    std::cout << "  캐릭터를 선택하세요\n";
    std::cout << line('-') << "\n";
    std::cout << "  [1] 아그네스 타키온";
    std::cout << "    SPD " << std::setw(3) << 72
              << "  STA " << std::setw(3) << 63
              << "  PWR " << std::setw(3) << 68
              << "  GTS " << std::setw(3) << 62
              << "  INT " << std::setw(3) << 88 << "\n";
    std::cout << "  [2] 맨하탄 카페     ";
    std::cout << "    SPD " << std::setw(3) << 66
              << "  STA " << std::setw(3) << 85
              << "  PWR " << std::setw(3) << 72
              << "  GTS " << std::setw(3) << 80
              << "  INT " << std::setw(3) << 68 << "\n";
    std::cout << line() << "\n";
    std::cout << " > ";
}

void ConsoleRenderer::drawMainScreen(const Player& uma, const std::string& dateText) {
    clearScreen();

    // ASCII Art 영역
    std::cout << line() << "\n";
    std::cout << "|" << std::string(BOX_W - 2, ' ') << "|\n";
    std::cout << "|          [ ASCII ART PLACEHOLDER ]              |\n";
    std::cout << "|" << std::string(BOX_W - 2, ' ') << "|\n";

    // 캐릭터 정보
    std::cout << line() << "\n";
    std::cout << " 날짜  : " << dateText << "\n";
    std::cout << " 이름  : " << uma.getName() << "\n";
    std::cout << " HP    : " << hpBar(uma.getHp(), uma.getMaxHp())
              << "  " << uma.getHp() << "/" << uma.getMaxHp() << "\n";

    // 스탯
    std::cout << line('-') << "\n";
    std::cout << " SPD :" << std::setw(4) << uma.getSpeed()
              << "   STA :" << std::setw(4) << uma.getStamina()
              << "   PWR :" << std::setw(4) << uma.getPower() << "\n";
    std::cout << " GTS :" << std::setw(4) << uma.getGuts()
              << "   INT :" << std::setw(4) << uma.getIntelligence() << "\n";

    // 메뉴
    std::cout << line() << "\n";
    std::cout << "   [1] 훈련      [2] 휴식      [3] 경주      [q] 종료\n";
    std::cout << line() << "\n";
    std::cout << " > ";
}

void ConsoleRenderer::drawTrainingMenu() {
    clearScreen();
    std::cout << line() << "\n";
    std::cout << "  훈련 선택\n";
    std::cout << line('-') << "\n";
    std::cout << "  [1] 스피드   [2] 스테미너   [3] 파워\n";
    std::cout << "  [4] 근성     [5] 지능       [0] 뒤로\n";
    std::cout << line() << "\n";
    std::cout << " > ";
}

void ConsoleRenderer::drawRaceMenu(const std::string& dateText, const std::vector<Race>& races) {
    clearScreen();
    std::cout << line() << "\n";
    std::cout << "  Race Entry  -  " << dateText << "\n";
    std::cout << line('-') << "\n";

    if (races.empty()) {
        std::cout << "  No available races this turn.\n";
        std::cout << line() << "\n";
        waitAnyKey();
        return;
    }

    for (size_t i = 0; i < races.size(); i++) {
        const Race& race = races[i];
        std::cout << "  [" << (i + 1) << "] ";
        if (!race.getGrade().empty()) std::cout << race.getGrade() << " ";
        std::cout << race.getCourseName()
                  << "  " << race.getVenue()
                  << "  " << race.getSurface()
                  << "  " << race.getDistance() << "m\n";
    }

    std::cout << "  [0] Back\n";
    std::cout << line() << "\n";
    std::cout << " > ";
}

void ConsoleRenderer::drawTrainingResult(const TrainingResult& result) {
    clearScreen();
    std::cout << line() << "\n";
    if (!result.success) {
        std::cout << "  훈련 실패... 체력이 너무 부족합니다.\n";
    } else {
        std::cout << "  훈련 성공!\n";
        std::cout << line('-') << "\n";
        if (result.speedGain)        std::cout << "  SPD  +" << result.speedGain << "\n";
        if (result.staminaGain)      std::cout << "  STA  +" << result.staminaGain << "\n";
        if (result.powerGain)        std::cout << "  PWR  +" << result.powerGain << "\n";
        if (result.gutsGain)         std::cout << "  GTS  +" << result.gutsGain << "\n";
        if (result.intelligenceGain) std::cout << "  INT  +" << result.intelligenceGain << "\n";
    }
    std::cout << line('-') << "\n";
    std::cout << "  HP   -" << result.hpCost << "\n";
    std::cout << line() << "\n";
    waitAnyKey();
}

void ConsoleRenderer::drawRestResult(int hpGained) {
    clearScreen();
    std::cout << line() << "\n";
    std::cout << "  휴식을 취했습니다.\n";
    std::cout << line('-') << "\n";
    std::cout << "  HP   +" << hpGained << "\n";
    std::cout << line() << "\n";
    waitAnyKey();
}

void ConsoleRenderer::drawRaceResult(const Race& course, const RaceResult& result) {
    int total = result.totalParticipants;

    auto header = [&]() {
        std::cout << line() << "\n";
        std::cout << "  경주!  " << course.getCourseName()
                  << "  " << course.getDistance() << "m  " << course.getWeather() << "\n";
        std::cout << line('-') << "\n";
    };

    auto printRanks = [&](const std::vector<PhaseRanking>& ranks) {
        for (const auto& r : ranks) {
            std::cout << "  " << r.rank << "위  " << r.name;
            if (r.isPlayer) std::cout << "  ★";
            std::cout << "\n";
        }
    };

    auto getPlayerRank = [](const std::vector<PhaseRanking>& ranks) {
        for (const auto& r : ranks)
            if (r.isPlayer) return r.rank;
        return -1;
    };

    auto trendStr = [](int rank, int prev) -> std::string {
        if (prev <= 0) return "";
        if (rank < prev) return "  (▲" + std::to_string(prev - rank) + "위 상승!)";
        if (rank > prev) return "  (▼" + std::to_string(rank - prev) + "위 하락...)";
        return "  (순위 유지)";
    };

    auto phaseComment = [&](RacePhase phase, int rank) -> std::string {
        switch (phase) {
        case RacePhase::EARLY:
            if (rank == 1)         return "완벽한 출발! 선두를 달리고 있습니다!";
            if (rank <= 3)         return "상위권! 선두 바로 뒤를 추격합니다!";
            if (rank < total)      return "중간 그룹. 아직 역전의 기회는 있습니다!";
            return "초반부터 최후미... 여기서 따라잡을 수 있을까요?";
        case RacePhase::MID:
            if (rank == 1)         return "여전히 선두! 이 페이스를 유지합니다!";
            if (rank <= 3)         return "선두를 추격! 기세가 오르고 있습니다!";
            if (rank < total)      return "아직 역전을 노릴 수 있는 위치입니다!";
            return "힘든 상황... 근성을 보여줄 시간입니다!";
        case RacePhase::LATE:
            if (rank == 1)         return "막판 스퍼트! 아무도 막을 수 없습니다!";
            if (rank <= 3)         return "결승이 눈앞! 마지막 힘을 쥐어짭니다!";
            if (rank < total)      return "대역전극을 노립니다! 포기하지 마세요!";
            return "최후미... 하지만 포기란 없습니다!";
        default: return "";
        }
    };

    auto finalComment = [&](int rank) -> std::string {
        if (rank == 1)        return "우 승!! 완벽한 레이스였습니다!!";
        if (rank == 2)        return "준우승! 아쉽지만 정말 훌륭한 레이스였습니다!";
        if (rank == 3)        return "3위 입상! 다음엔 반드시 정상에!";
        if (rank < total)     return "아쉬운 결과... 하지만 포기하지 마세요!";
        return "꼴찌... 더 열심히 훈련이 필요합니다.";
    };

    // 경기 소개 화면
    clearScreen();
    std::cout << line() << "\n";
    std::cout << "  ★ 경주 시작 ★\n";
    std::cout << line('-') << "\n";
    std::cout << "  코스   : " << course.getCourseName() << "\n";
    std::cout << "  거리   : " << course.getDistance() << "m\n";
    std::cout << "  날씨   : " << course.getWeather() << "\n";
    std::cout << "  참가수 : " << total << "명\n";
    std::cout << line('-') << "\n";
    std::cout << "  [해설] 드디어 레이스가 시작됩니다!\n";
    std::cout << "         " << total << "명의 우마무스메가 " << course.getDistance()
              << "m의 " << course.getCourseName() << " 코스에 도전합니다!\n";
    std::cout << line('-') << "\n";
    std::cout << "  출발 준비... 스타트!!\n";
    std::cout << line() << "\n";
    waitAnyKey();

    struct PhaseInfo {
        const std::string     name;
        const std::vector<PhaseRanking>& ranks;
        RacePhase             phase;
    };
    PhaseInfo phases[] = {
        { "초반", result.earlyRanks, RacePhase::EARLY },
        { "중반", result.midRanks,   RacePhase::MID   },
        { "후반", result.lateRanks,  RacePhase::LATE  },
    };

    int prevRank = -1;
    for (const auto& p : phases) {
        int rank = getPlayerRank(p.ranks);
        clearScreen();
        header();
        std::cout << "  [ " << p.name << " ]\n";
        std::cout << line('-') << "\n";
        printRanks(p.ranks);
        std::cout << line('-') << "\n";
        std::cout << "  [해설] " << phaseComment(p.phase, rank) << trendStr(rank, prevRank) << "\n";
        std::cout << line() << "\n";
        waitAnyKey();
        prevRank = rank;
    }

    int fRank = result.playerFinalRank;
    clearScreen();
    header();
    std::cout << "  [ 종반 결승 ]\n";
    std::cout << line('-') << "\n";
    printRanks(result.finalRanks);
    std::cout << line('-') << "\n";
    std::cout << "  [해설] " << finalComment(fRank) << trendStr(fRank, prevRank) << "\n";
    std::cout << line('-') << "\n";
    std::cout << "  최종 순위: " << fRank << "위 / " << total << "명\n";
    std::cout << line() << "\n";
    waitAnyKey();
}
