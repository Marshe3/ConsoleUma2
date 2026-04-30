#include "Race.h"

#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <random>
#include <string>
#include <utility>
#include <vector>

using namespace std;

namespace
{
    struct Participant
    {
        RaceEntry entry;
        int currentStamina;
        float phaseBonus;
        float distanceRun;
        float lastSpeed;
        int lane;
        bool blocked;
        bool wide;
        bool uniqueSkillUsed;
        bool uniqueSkillActive;
    };

    struct PhaseInfo
    {
        RacePhase phase;
        const char* title;
        const char* detailTitle;
        float segmentRatio;
        int staminaDrain;
    };

    mt19937& getRng()
    {
        static mt19937 rng(random_device{}());
        return rng;
    }

    float staminaMod(int cur, int maxStam)
    {
        if (maxStam <= 0) return 0.50f;

        float ratio = static_cast<float>(cur) / maxStam;
        if (ratio >= 0.50f) return 1.00f;
        if (ratio >= 0.25f) return 0.86f;
        if (ratio > 0.00f) return 0.68f;
        return 0.52f;
    }

    float rankBonus(int rank)
    {
        switch (rank)
        {
        case 1: return 0.08f;
        case 2: return 0.05f;
        case 3: return 0.02f;
        default: return 0.00f;
        }
    }

    float phaseBaseStat(const RaceEntry& e, RacePhase phase)
    {
        switch (phase)
        {
        case RacePhase::EARLY:
            return e.power * 0.55f + e.speed * 0.25f + e.intelligence * 0.20f;
        case RacePhase::MID:
            return e.speed * 0.45f + e.power * 0.25f + e.intelligence * 0.30f;
        case RacePhase::LATE:
            return e.speed * 0.62f + e.power * 0.23f + e.guts * 0.15f;
        case RacePhase::FINAL:
            return e.speed * 0.48f + e.guts * 0.38f + e.power * 0.14f;
        default:
            return 0.0f;
        }
    }

    float calcPhaseSpeed(const Participant& p, RacePhase phase)
    {
        uniform_real_distribution<float> noise(0.94f, 1.08f);
        float stamina = staminaMod(p.currentStamina, p.entry.stemina);
        float bonus = 1.0f + p.phaseBonus;
        return phaseBaseStat(p.entry, phase) * stamina * bonus * noise(getRng());
    }

    float styleSpeedMultiplier(RunningStyle style, RacePhase phase)
    {
        switch (style)
        {
        case RunningStyle::ESCAPE:
            if (phase == RacePhase::EARLY) return 1.15f;
            if (phase == RacePhase::MID) return 1.04f;
            if (phase == RacePhase::LATE) return 0.96f;
            return 0.92f;
        case RunningStyle::PACE:
            if (phase == RacePhase::EARLY) return 1.07f;
            if (phase == RacePhase::MID) return 1.06f;
            if (phase == RacePhase::LATE) return 1.02f;
            return 0.98f;
        case RunningStyle::LATE:
            if (phase == RacePhase::EARLY) return 0.92f;
            if (phase == RacePhase::MID) return 1.00f;
            if (phase == RacePhase::LATE) return 1.10f;
            return 1.08f;
        case RunningStyle::END:
            if (phase == RacePhase::EARLY) return 0.86f;
            if (phase == RacePhase::MID) return 0.95f;
            if (phase == RacePhase::LATE) return 1.08f;
            return 1.20f;
        default:
            return 1.0f;
        }
    }

    int styleStaminaDrainBonus(RunningStyle style, RacePhase phase)
    {
        if (style == RunningStyle::ESCAPE && phase == RacePhase::EARLY) return 2;
        if (style == RunningStyle::END && phase == RacePhase::FINAL) return 2;
        if (style == RunningStyle::PACE && phase == RacePhase::MID) return -1;
        if (style == RunningStyle::LATE && phase == RacePhase::EARLY) return -1;
        return 0;
    }

    int preferredLane(RunningStyle style, RacePhase phase, int rank, int field)
    {
        switch (style)
        {
        case RunningStyle::ESCAPE:
            return 0;
        case RunningStyle::PACE:
            return rank <= max(2, field / 3) ? 0 : 1;
        case RunningStyle::LATE:
            if (phase == RacePhase::LATE || phase == RacePhase::FINAL) return 2;
            return 1;
        case RunningStyle::END:
            if (phase == RacePhase::FINAL) return 3;
            if (phase == RacePhase::LATE) return 2;
            return 1;
        default:
            return 1;
        }
    }

    bool isBlockedByPack(const vector<Participant>& ps, int selfIndex)
    {
        const Participant& self = ps[selfIndex];
        if (self.lane >= 2) return false;

        for (int i = 0; i < static_cast<int>(ps.size()); i++)
        {
            if (i == selfIndex) continue;

            const Participant& other = ps[i];
            float gap = other.distanceRun - self.distanceRun;
            if (gap > 0.0f && gap < 28.0f && abs(other.lane - self.lane) <= 1)
                return true;
        }

        return false;
    }

    vector<pair<float, int>> rankByDistance(const vector<Participant>& ps)
    {
        vector<pair<float, int>> ranked;
        ranked.reserve(ps.size());

        for (int i = 0; i < static_cast<int>(ps.size()); i++)
            ranked.push_back({ ps[i].distanceRun, i });

        sort(ranked.begin(), ranked.end(), [](const auto& a, const auto& b) {
            return a.first > b.first;
        });

        return ranked;
    }

    vector<PhaseRanking> makeRankings(const vector<Participant>& ps)
    {
        vector<PhaseRanking> ranks;
        auto ranked = rankByDistance(ps);
        ranks.reserve(ranked.size());

        for (int i = 0; i < static_cast<int>(ranked.size()); i++)
        {
            const Participant& p = ps[ranked[i].second];
            ranks.push_back({ p.entry.name, i + 1, p.entry.isPlayer });
        }

        return ranks;
    }

    string makeCommentary(const vector<Participant>& ps, RacePhase phase)
    {
        auto ranked = rankByDistance(ps);
        if (ranked.empty()) return "출전자가 없습니다.";

        const Participant& leader = ps[ranked.front().second];

        int playerRank = -1;
        string playerName;
        float leaderDistance = leader.distanceRun;
        float playerGap = 0.0f;

        for (int i = 0; i < static_cast<int>(ranked.size()); i++)
        {
            const Participant& p = ps[ranked[i].second];
            if (p.entry.isPlayer)
            {
                playerRank = i + 1;
                playerName = p.entry.name;
                playerGap = leaderDistance - p.distanceRun;
                break;
            }
        }

        string text;
        switch (phase)
        {
        case RacePhase::EARLY:
            text = leader.entry.name + " 좋은 스타트! 안쪽 코스를 잡고 앞으로 나섭니다.";
            break;
        case RacePhase::MID:
            text = leader.entry.name + " 중반 페이스를 주도합니다.";
            break;
        case RacePhase::LATE:
            text = leader.entry.name + " 4코너를 돌며 스퍼트를 준비합니다.";
            break;
        case RacePhase::FINAL:
            text = leader.entry.name + " 마지막 직선! 전력으로 골을 향합니다.";
            break;
        }

        if (playerRank < 0)
            return text;
        if (playerRank == 1)
            return text + " " + playerName + " 선두입니다!";
        if (playerRank <= 3)
            return text + " " + playerName + " 선두와 " + to_string(static_cast<int>(round(playerGap))) + "m 차이, 충분히 닿습니다.";

        return text + " " + playerName + " 현재 " + to_string(playerRank) + "위, 후반 승부가 필요합니다.";
    }

    RaceMoment makeMoment(const vector<Participant>& ps, const string& title, const string& commentary, int raceDistance)
    {
        RaceMoment moment;
        moment.title = title;
        moment.commentary = commentary;

        auto ranked = rankByDistance(ps);
        moment.runners.reserve(ranked.size());

        for (int i = 0; i < static_cast<int>(ranked.size()); i++)
        {
            const Participant& p = ps[ranked[i].second];
            const int courseLapDistance = 1600;
            int totalLaps = max(1, static_cast<int>(ceil(static_cast<float>(raceDistance) / courseLapDistance)));
            int finishCoursePosition = static_cast<int>(round((fmod(static_cast<float>(raceDistance), static_cast<float>(courseLapDistance)) / courseLapDistance) * 100.0f));
            if (finishCoursePosition == 0) finishCoursePosition = 100;
            int position = static_cast<int>(round((p.distanceRun / raceDistance) * 100.0f));
            int lap = min(totalLaps, static_cast<int>(p.distanceRun / courseLapDistance) + 1);
            int coursePosition = static_cast<int>(round((fmod(p.distanceRun, static_cast<float>(courseLapDistance)) / courseLapDistance) * 100.0f));

            if (p.distanceRun >= raceDistance)
            {
                lap = totalLaps;
                coursePosition = finishCoursePosition;
            }
            int stamina = p.entry.stemina > 0
                ? static_cast<int>(round((static_cast<float>(p.currentStamina) / p.entry.stemina) * 100.0f))
                : 0;

            moment.runners.push_back({
                p.entry.name,
                p.entry.isPlayer,
                i + 1,
                max(0, min(100, position)),
                max(0, min(100, coursePosition)),
                lap,
                totalLaps,
                p.lane,
                max(0, min(100, stamina)),
                p.lastSpeed,
                p.entry.style,
                p.blocked,
                p.wide,
                p.uniqueSkillActive,
                p.entry.uniqueSkillName
            });
        }

        return moment;
    }

    const PhaseInfo PHASES[] = {
        { RacePhase::EARLY, "초반", "스타트 대시", 0.24f, 10 },
        { RacePhase::MID, "중반", "백스트레치", 0.28f, 18 },
        { RacePhase::LATE, "후반", "최종 코너", 0.25f, 23 },
        { RacePhase::FINAL, "종반", "라스트 스퍼트", 0.23f, 28 },
    };

    string makeFrameTitle(const PhaseInfo& info, int frame, int totalFrames)
    {
        return string(info.title) + " - " + info.detailTitle + " " + to_string(frame) + "/" + to_string(totalFrames);
    }

    string makeSkillCommentary(const vector<Participant>& ps)
    {
        string text;
        for (const auto& p : ps)
        {
            if (!p.uniqueSkillActive || p.entry.uniqueSkillName.empty()) continue;

            if (!text.empty()) text += " ";
            text += p.entry.name + "의 고유기 [" + p.entry.uniqueSkillName + "] 발동!";
        }

        return text;
    }

    string makeLaneCommentary(const vector<Participant>& ps)
    {
        string text;

        for (const auto& p : ps)
        {
            if (p.entry.isPlayer && p.blocked)
                return p.entry.name + " 안쪽 마군에 갇혔습니다!";
            if (p.entry.isPlayer && p.wide)
                return p.entry.name + " 바깥으로 크게 돌며 추월 라인을 잡습니다!";
        }

        for (const auto& p : ps)
        {
            if (p.blocked)
                return p.entry.name + " 앞이 막혀 잠시 주춤합니다.";
            if (p.wide)
                return p.entry.name + " 외곽으로 크게 돌며 승부를 겁니다.";
        }

        return text;
    }
}

Race::Race(string name, string weather, int distance)
    : Race(move(name), "", "", "Turf", move(weather), distance)
{}

Race::Race(string name, string grade, string venue, string surface, string weather, int distance)
    : courseName(move(name))
    , weather(move(weather))
    , grade(move(grade))
    , venue(move(venue))
    , surface(move(surface))
    , distance(distance)
{}

RaceResult Race::run(const vector<RaceEntry>& entries) const
{
    vector<Participant> ps;
    ps.reserve(entries.size());

    for (const auto& e : entries)
        ps.push_back({ e, e.stemina, 0.0f, 0.0f, 0.0f, 1, false, false, false, false });

    RaceResult result;
    result.totalParticipants = static_cast<int>(ps.size());
    result.playerFinalRank = -1;

    result.moments.push_back(makeMoment(
        ps,
        "게이트 인",
        "모든 우마무스메가 게이트에 들어갔습니다. 관중석이 조용해집니다.",
        distance
    ));

    float targetDistance = 0.0f;

    for (const PhaseInfo& info : PHASES)
    {
        targetDistance += distance * info.segmentRatio;
        const int frameCount = 4;
        float frameTargetDistance = targetDistance - distance * info.segmentRatio;

        for (int frame = 1; frame <= frameCount; frame++)
        {
            frameTargetDistance += (distance * info.segmentRatio) / frameCount;

            vector<float> speeds;
            speeds.reserve(ps.size());

            float bestSpeed = 0.0f;
            for (const auto& p : ps)
            {
                float speed = calcPhaseSpeed(p, info.phase) * styleSpeedMultiplier(p.entry.style, info.phase);
                speeds.push_back(speed);
                bestSpeed = max(bestSpeed, speed);
            }

            auto beforeSkillRanked = rankByDistance(ps);
            vector<int> currentRanks(ps.size(), 0);
            for (int i = 0; i < static_cast<int>(beforeSkillRanked.size()); i++)
                currentRanks[beforeSkillRanked[i].second] = i + 1;

            for (int i = 0; i < static_cast<int>(ps.size()); i++)
            {
                int rank = currentRanks[i] > 0 ? currentRanks[i] : static_cast<int>(ps.size());
                int targetLane = preferredLane(ps[i].entry.style, info.phase, rank, static_cast<int>(ps.size()));

                if (ps[i].lane < targetLane) ps[i].lane++;
                else if (ps[i].lane > targetLane) ps[i].lane--;

                ps[i].wide = ps[i].lane >= 3;
            }

            for (int i = 0; i < static_cast<int>(ps.size()); i++)
                ps[i].blocked = isBlockedByPack(ps, i);

            for (int i = 0; i < static_cast<int>(ps.size()); i++)
            {
                ps[i].uniqueSkillActive = false;
                if (ps[i].uniqueSkillUsed || ps[i].entry.uniqueSkillName.empty()) continue;

                int rank = currentRanks[i];
                int field = static_cast<int>(ps.size());

                if (ps[i].entry.uniqueSkillName == "U=ma2" &&
                    info.phase == RacePhase::MID &&
                    rank > 0 && rank <= max(2, field / 2))
                {
                    ps[i].currentStamina = min(ps[i].entry.stemina, ps[i].currentStamina + 18);
                    speeds[i] *= 1.05f;
                    ps[i].uniqueSkillUsed = true;
                    ps[i].uniqueSkillActive = true;
                }
                else if (ps[i].entry.uniqueSkillName == "그대를 좇아서" &&
                    (info.phase == RacePhase::LATE || info.phase == RacePhase::FINAL) &&
                    rank > 1)
                {
                    speeds[i] *= 1.16f;
                    ps[i].currentStamina = min(ps[i].entry.stemina, ps[i].currentStamina + 8);
                    ps[i].uniqueSkillUsed = true;
                    ps[i].uniqueSkillActive = true;
                }

                bestSpeed = max(bestSpeed, speeds[i]);
            }

            bestSpeed = 0.0f;
            for (int i = 0; i < static_cast<int>(ps.size()); i++)
            {
                if (ps[i].blocked)
                    speeds[i] *= 0.86f;

                if (ps[i].wide)
                {
                    speeds[i] *= (ps[i].entry.style == RunningStyle::LATE || ps[i].entry.style == RunningStyle::END)
                        ? 1.03f
                        : 0.95f;
                }

                bestSpeed = max(bestSpeed, speeds[i]);
            }

            for (int i = 0; i < static_cast<int>(ps.size()); i++)
            {
                float normalized = bestSpeed > 0.0f ? speeds[i] / bestSpeed : 0.0f;
                float segmentGain = distance * info.segmentRatio / frameCount * (0.80f + normalized * 0.28f);
                ps[i].distanceRun = max(ps[i].distanceRun, min(static_cast<float>(distance), ps[i].distanceRun + segmentGain));
                ps[i].distanceRun = min(ps[i].distanceRun, frameTargetDistance + distance * 0.035f);
                ps[i].lastSpeed = speeds[i];
                int drain = max(1, info.staminaDrain / frameCount + styleStaminaDrainBonus(ps[i].entry.style, info.phase));
                if (ps[i].wide) drain += 1;
                ps[i].currentStamina = max(0, ps[i].currentStamina - drain);
            }

            auto ranked = rankByDistance(ps);
            for (int i = 0; i < static_cast<int>(ranked.size()); i++)
                ps[ranked[i].second].phaseBonus = rankBonus(i + 1);

            result.moments.push_back(makeMoment(
                ps,
                makeFrameTitle(info, frame, frameCount),
                makeCommentary(ps, info.phase) +
                    (makeLaneCommentary(ps).empty() ? "" : " " + makeLaneCommentary(ps)) +
                    (makeSkillCommentary(ps).empty() ? "" : " " + makeSkillCommentary(ps)),
                distance
            ));
        }

        vector<PhaseRanking> ranks = makeRankings(ps);
        switch (info.phase)
        {
        case RacePhase::EARLY:
            result.earlyRanks = ranks;
            break;
        case RacePhase::MID:
            result.midRanks = ranks;
            break;
        case RacePhase::LATE:
            result.lateRanks = ranks;
            break;
        case RacePhase::FINAL:
            result.finalRanks = ranks;
            break;
        }

    }

    if (!ps.empty())
    {
        auto ranked = rankByDistance(ps);
        float winnerDistance = ps[ranked.front().second].distanceRun;
        if (winnerDistance < distance)
        {
            float push = distance - winnerDistance;
            for (auto& p : ps)
                p.distanceRun = min(static_cast<float>(distance), p.distanceRun + push);
        }

        result.finalRanks = makeRankings(ps);
        result.moments.push_back(makeMoment(
            ps,
            "결승선 통과",
            ps[ranked.front().second].entry.name + " 가장 먼저 결승선을 통과합니다!",
            distance
        ));
    }

    for (const auto& r : result.finalRanks)
    {
        if (r.isPlayer)
        {
            result.playerFinalRank = r.rank;
            break;
        }
    }

    return result;
}

string Race::getPhaseName(RacePhase phase)
{
    switch (phase)
    {
    case RacePhase::EARLY: return "초반";
    case RacePhase::MID: return "중반";
    case RacePhase::LATE: return "후반";
    case RacePhase::FINAL: return "종반";
    default: return "";
    }
}

string Race::getRunningStyleName(RunningStyle style)
{
    switch (style)
    {
    case RunningStyle::ESCAPE: return "도주";
    case RunningStyle::PACE: return "선행";
    case RunningStyle::LATE: return "선입";
    case RunningStyle::END: return "추입";
    default: return "";
    }
}
