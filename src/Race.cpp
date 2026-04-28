#include "Race.h"
#include <algorithm>
#include <random>
#include <utility>

struct Participant {
    RaceEntry entry;
    int currentStamina;
    float phaseBonus;   // 이전 구간 순위 메리트
};

static std::mt19937& getRng() {
    static std::mt19937 rng(std::random_device{}());
    return rng;
}

// 스테미너 상태에 따른 속도 보정 배율
static float staminaMod(int cur, int maxStam) {
    if (maxStam <= 0) return 0.5f;
    float ratio = static_cast<float>(cur) / maxStam;
    if (ratio >= 0.30f) return 1.00f;   // 정상
    if (ratio >  0.00f) return 0.75f;   // 30% 미만: 디버프
    return 0.50f;                        // 0%: 대디버프
}

// 순위 → 다음 구간 메리트 보너스
static float rankBonus(int rank) {
    switch (rank) {
    case 1: return 0.10f;
    case 2: return 0.05f;
    case 3: return 0.02f;
    default: return 0.00f;
    }
}

static float calcScore(const Participant& p, RacePhase phase) {
    const RaceEntry& e = p.entry;
    float stat = 0.f;

    switch (phase) {
    case RacePhase::EARLY:
        // 파워 주력, 스피드 보조
        stat = e.speed * 0.35f + e.power * 0.65f;
        break;
    case RacePhase::MID:
        // 스피드·파워 동등
        stat = e.speed * 0.50f + e.power * 0.50f;
        break;
    case RacePhase::LATE:
        // 스피드 주력
        stat = e.speed * 0.70f + e.power * 0.30f;
        break;
    case RacePhase::FINAL:
        // 근성 지배, 스피드 보조, 지능 소량
        stat = e.speed * 0.25f + e.guts * 0.65f + e.intelligence * 0.10f;
        break;
    }

    float stamMod  = staminaMod(p.currentStamina, e.stemina);
    float bonusMul = 1.0f + p.phaseBonus;

    std::uniform_real_distribution<float> noise(0.90f, 1.10f);
    return stat * stamMod * bonusMul * noise(getRng());
}

// 구간별 스테미너 소모량 (EARLY, MID, LATE, FINAL 순)
static const int PHASE_DRAIN[] = { 10, 20, 25, 20 };

static std::vector<PhaseRanking> resolvePhase(std::vector<Participant>& ps, RacePhase phase) {
    int n = static_cast<int>(ps.size());

    std::vector<std::pair<float, int>> scored(n);
    for (int i = 0; i < n; i++)
        scored[i] = { calcScore(ps[i], phase), i };

    std::sort(scored.begin(), scored.end(), [](const auto& a, const auto& b) {
        return a.first > b.first;
    });

    // 순위 결정 + 다음 구간 메리트 설정
    std::vector<PhaseRanking> ranks;
    for (int r = 0; r < n; r++) {
        int idx = scored[r].second;
        ps[idx].phaseBonus = rankBonus(r + 1);
        ranks.push_back({ ps[idx].entry.name, r + 1, ps[idx].entry.isPlayer });
    }

    // 스테미너 소모
    int drain = PHASE_DRAIN[static_cast<int>(phase)];
    for (auto& p : ps)
        p.currentStamina = std::max(0, p.currentStamina - drain);

    return ranks;
}

// -------------------------------------------------------

Race::Race(std::string name, std::string weather, int distance)
    : Race(std::move(name), "", "", "Turf", std::move(weather), distance)
{}

Race::Race(std::string name, std::string grade, std::string venue, std::string surface, std::string weather, int distance)
    : courseName(std::move(name))
    , weather(std::move(weather))
    , grade(std::move(grade))
    , venue(std::move(venue))
    , surface(std::move(surface))
    , distance(distance)
{}

RaceResult Race::run(const std::vector<RaceEntry>& entries) const {
    std::vector<Participant> ps;
    ps.reserve(entries.size());
    for (const auto& e : entries)
        ps.push_back({ e, e.stemina, 0.0f });

    RaceResult result;
    result.totalParticipants = static_cast<int>(ps.size());

    result.earlyRanks = resolvePhase(ps, RacePhase::EARLY);
    result.midRanks   = resolvePhase(ps, RacePhase::MID);
    result.lateRanks  = resolvePhase(ps, RacePhase::LATE);
    result.finalRanks = resolvePhase(ps, RacePhase::FINAL);

    result.playerFinalRank = -1;
    for (const auto& r : result.finalRanks) {
        if (r.isPlayer) {
            result.playerFinalRank = r.rank;
            break;
        }
    }

    return result;
}

std::string Race::getPhaseName(RacePhase phase) {
    switch (phase) {
    case RacePhase::EARLY: return "초반";
    case RacePhase::MID:   return "중반";
    case RacePhase::LATE:  return "후반";
    case RacePhase::FINAL: return "종반";
    default: return "";
    }
}
