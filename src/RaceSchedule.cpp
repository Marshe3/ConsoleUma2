#include "RaceSchedule.h"

#include <string>
#include <vector>

using namespace std;

namespace
{
    struct ScheduledRace
    {
        int year;
        int month;
        bool firstHalf;
        Race race;
    };

    const vector<ScheduledRace>& schedule()
    {
        static const vector<ScheduledRace> races = {
            { 0, 6, false, Race("도쿄 주니어 스테이크스", "OP", "도쿄", "Turf", "Sunny", 1600) },
            { 0, 7, false, Race("하코다테 주니어 스테이크스", "G3", "하코다테", "Turf", "Sunny", 1200) },
            { 0, 8, false, Race("니가타 주니어 스테이크스", "G3", "니가타", "Turf", "Sunny", 1600) },
            { 0, 9, true,  Race("삿포로 주니어 스테이크스", "G3", "삿포로", "Turf", "Sunny", 1800) },
            { 0, 10, true, Race("사우디아라비아 로열컵", "G3", "도쿄", "Turf", "Sunny", 1600) },
            { 0, 11, true, Race("케이오배 주니어 스테이크스", "G2", "도쿄", "Turf", "Sunny", 1400) },
            { 0, 11, false, Race("도쿄 스포츠배 주니어 스테이크스", "G2", "도쿄", "Turf", "Sunny", 1800) },
            { 0, 12, true, Race("한신 주버나일 필리즈", "G1", "한신", "Turf", "Sunny", 1600) },
            { 0, 12, false, Race("아사히배 퓨처리티 스테이크스", "G1", "한신", "Turf", "Sunny", 1600) },
            { 0, 12, false, Race("호프풀 스테이크스", "G1", "나카야마", "Turf", "Sunny", 2000) },

            { 1, 1, true,  Race("신잔 기념", "G3", "교토", "Turf", "Sunny", 1600) },
            { 1, 2, true,  Race("키사라기상", "G3", "교토", "Turf", "Sunny", 1800) },
            { 1, 3, true,  Race("야요이상", "G2", "나카야마", "Turf", "Sunny", 2000) },
            { 1, 3, false, Race("스프링 스테이크스", "G2", "나카야마", "Turf", "Sunny", 1800) },
            { 1, 4, true,  Race("오카상", "G1", "한신", "Turf", "Sunny", 1600) },
            { 1, 4, false, Race("사츠키상", "G1", "나카야마", "Turf", "Sunny", 2000) },
            { 1, 5, true,  Race("NHK 마일컵", "G1", "도쿄", "Turf", "Sunny", 1600) },
            { 1, 5, false, Race("일본 더비", "G1", "도쿄", "Turf", "Sunny", 2400) },
            { 1, 5, false, Race("오크스", "G1", "도쿄", "Turf", "Sunny", 2400) },
            { 1, 6, false, Race("타카라즈카 기념", "G1", "한신", "Turf", "Sunny", 2200) },
            { 1, 9, true,  Race("로즈 스테이크스", "G2", "한신", "Turf", "Sunny", 1800) },
            { 1, 9, false, Race("고베신문배", "G2", "한신", "Turf", "Sunny", 2400) },
            { 1, 10, true, Race("슈카상", "G1", "교토", "Turf", "Sunny", 2000) },
            { 1, 10, false, Race("킷카상", "G1", "교토", "Turf", "Sunny", 3000) },
            { 1, 11, true, Race("엘리자베스 여왕배", "G1", "교토", "Turf", "Sunny", 2200) },
            { 1, 11, false, Race("재팬컵", "G1", "도쿄", "Turf", "Sunny", 2400) },
            { 1, 12, true, Race("챔피언스컵", "G1", "주쿄", "Dirt", "Sunny", 1800) },
            { 1, 12, false, Race("아리마 기념", "G1", "나카야마", "Turf", "Sunny", 2500) },

            { 2, 1, true,  Race("교토 금배", "G3", "교토", "Turf", "Sunny", 1600) },
            { 2, 2, true,  Race("도쿄신문배", "G3", "도쿄", "Turf", "Sunny", 1600) },
            { 2, 2, false, Race("페브러리 스테이크스", "G1", "도쿄", "Dirt", "Sunny", 1600) },
            { 2, 3, false, Race("오사카배", "G1", "한신", "Turf", "Sunny", 2000) },
            { 2, 4, false, Race("텐노상 봄", "G1", "교토", "Turf", "Sunny", 3200) },
            { 2, 5, true,  Race("빅토리아 마일", "G1", "도쿄", "Turf", "Sunny", 1600) },
            { 2, 6, true,  Race("야스다 기념", "G1", "도쿄", "Turf", "Sunny", 1600) },
            { 2, 6, false, Race("타카라즈카 기념", "G1", "한신", "Turf", "Sunny", 2200) },
            { 2, 9, false, Race("스프린터스 스테이크스", "G1", "나카야마", "Turf", "Sunny", 1200) },
            { 2, 10, false, Race("텐노상 가을", "G1", "도쿄", "Turf", "Sunny", 2000) },
            { 2, 11, true, Race("마일 챔피언십", "G1", "교토", "Turf", "Sunny", 1600) },
            { 2, 11, false, Race("재팬컵", "G1", "도쿄", "Turf", "Sunny", 2400) },
            { 2, 12, true, Race("챔피언스컵", "G1", "주쿄", "Dirt", "Sunny", 1800) },
            { 2, 12, false, Race("아리마 기념", "G1", "나카야마", "Turf", "Sunny", 2500) },
            { 0, 7, true,  Race("Junior Dirt Sprint", "OP", "Funabashi", "Dirt", "Sunny", 1200) },
            { 0, 9, false, Race("Autumn Dirt Dash", "OP", "Tokyo", "Dirt", "Cloudy", 1400) },
            { 0, 10, false, Race("Junior Challenge Cup", "OP", "Kyoto", "Turf", "Sunny", 1800) },

            { 1, 1, false, Race("New Year Dirt OP", "OP", "Nakayama", "Dirt", "Sunny", 1800) },
            { 1, 2, false, Race("Classic Dirt Sprint", "OP", "Hanshin", "Dirt", "Cloudy", 1400) },
            { 1, 4, true,  Race("Spring Dirt Trial", "OP", "Tokyo", "Dirt", "Sunny", 1600) },
            { 1, 6, true,  Race("June Open Mile", "OP", "Tokyo", "Turf", "Rainy", 1600) },
            { 1, 7, true,  Race("Summer Dirt Cup", "OP", "Chukyo", "Dirt", "Sunny", 1700) },
            { 1, 8, false, Race("Sapporo Open", "OP", "Sapporo", "Turf", "Cloudy", 2000) },
            { 1, 10, true, Race("October Dirt OP", "OP", "Kyoto", "Dirt", "Sunny", 1800) },

            { 2, 1, false, Race("Senior Dirt OP", "OP", "Nakayama", "Dirt", "Sunny", 1800) },
            { 2, 3, true,  Race("March Open Stakes", "OP", "Hanshin", "Turf", "Cloudy", 1800) },
            { 2, 4, true,  Race("April Dirt Mile", "OP", "Tokyo", "Dirt", "Rainy", 1600) },
            { 2, 5, false, Race("May Open Cup", "OP", "Kyoto", "Turf", "Sunny", 2000) },
            { 2, 7, true,  Race("Procyon Dirt OP", "OP", "Chukyo", "Dirt", "Sunny", 1400) },
            { 2, 8, false, Race("Summer Open Stakes", "OP", "Sapporo", "Turf", "Sunny", 2000) },
            { 2, 10, true, Race("Autumn Dirt Classic", "OP", "Tokyo", "Dirt", "Cloudy", 2100) },
        };

        return races;
    }
}

vector<Race> RaceSchedule::getAvailableRaces(const GameDate& date)
{
    vector<Race> available;
    int scheduleYear = date.getYear() > 2 ? 2 : date.getYear();

    for (const auto& item : schedule())
    {
        if (item.year == scheduleYear &&
            item.month == date.getMonth() &&
            item.firstHalf == date.isFirstHalf())
        {
            available.push_back(item.race);
        }
    }

    return available;
}
