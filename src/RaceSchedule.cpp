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
            { 0, 6, false, Race("Tokyo Junior Stakes", "OP", "Tokyo", "Turf", "Sunny", 1600) },
            { 0, 7, false, Race("Hakodate Junior Stakes", "G3", "Hakodate", "Turf", "Sunny", 1200) },
            { 0, 8, false, Race("Niigata Junior Stakes", "G3", "Niigata", "Turf", "Sunny", 1600) },
            { 0, 9, true,  Race("Sapporo Junior Stakes", "G3", "Sapporo", "Turf", "Sunny", 1800) },
            { 0, 10, true, Race("Saudi Arabia Royal Cup", "G3", "Tokyo", "Turf", "Sunny", 1600) },
            { 0, 11, true, Race("Keio Hai Junior Stakes", "G2", "Tokyo", "Turf", "Sunny", 1400) },
            { 0, 11, false, Race("Tokyo Sports Hai Junior Stakes", "G2", "Tokyo", "Turf", "Sunny", 1800) },
            { 0, 12, true, Race("Hanshin Juvenile Fillies", "G1", "Hanshin", "Turf", "Sunny", 1600) },
            { 0, 12, false, Race("Asahi Hai Futurity Stakes", "G1", "Hanshin", "Turf", "Sunny", 1600) },
            { 0, 12, false, Race("Hopeful Stakes", "G1", "Nakayama", "Turf", "Sunny", 2000) },

            { 1, 1, true,  Race("Shinzan Kinen", "G3", "Kyoto", "Turf", "Sunny", 1600) },
            { 1, 2, true,  Race("Kisaragi Sho", "G3", "Kyoto", "Turf", "Sunny", 1800) },
            { 1, 3, true,  Race("Yayoi Sho", "G2", "Nakayama", "Turf", "Sunny", 2000) },
            { 1, 3, false, Race("Spring Stakes", "G2", "Nakayama", "Turf", "Sunny", 1800) },
            { 1, 4, true,  Race("Oka Sho", "G1", "Hanshin", "Turf", "Sunny", 1600) },
            { 1, 4, false, Race("Satsuki Sho", "G1", "Nakayama", "Turf", "Sunny", 2000) },
            { 1, 5, true,  Race("NHK Mile Cup", "G1", "Tokyo", "Turf", "Sunny", 1600) },
            { 1, 5, false, Race("Tokyo Yushun", "G1", "Tokyo", "Turf", "Sunny", 2400) },
            { 1, 5, false, Race("Yushun Himba", "G1", "Tokyo", "Turf", "Sunny", 2400) },
            { 1, 6, false, Race("Takarazuka Kinen", "G1", "Hanshin", "Turf", "Sunny", 2200) },
            { 1, 9, true,  Race("Rose Stakes", "G2", "Hanshin", "Turf", "Sunny", 1800) },
            { 1, 9, false, Race("Kobe Shimbun Hai", "G2", "Hanshin", "Turf", "Sunny", 2400) },
            { 1, 10, true, Race("Shuka Sho", "G1", "Kyoto", "Turf", "Sunny", 2000) },
            { 1, 10, false, Race("Kikuka Sho", "G1", "Kyoto", "Turf", "Sunny", 3000) },
            { 1, 11, true, Race("Queen Elizabeth II Cup", "G1", "Kyoto", "Turf", "Sunny", 2200) },
            { 1, 11, false, Race("Japan Cup", "G1", "Tokyo", "Turf", "Sunny", 2400) },
            { 1, 12, true, Race("Champions Cup", "G1", "Chukyo", "Dirt", "Sunny", 1800) },
            { 1, 12, false, Race("Arima Kinen", "G1", "Nakayama", "Turf", "Sunny", 2500) },

            { 2, 1, true,  Race("Kyoto Kimpai", "G3", "Kyoto", "Turf", "Sunny", 1600) },
            { 2, 2, true,  Race("Tokyo Shimbun Hai", "G3", "Tokyo", "Turf", "Sunny", 1600) },
            { 2, 2, false, Race("February Stakes", "G1", "Tokyo", "Dirt", "Sunny", 1600) },
            { 2, 3, false, Race("Osaka Hai", "G1", "Hanshin", "Turf", "Sunny", 2000) },
            { 2, 4, false, Race("Tenno Sho Spring", "G1", "Kyoto", "Turf", "Sunny", 3200) },
            { 2, 5, true,  Race("Victoria Mile", "G1", "Tokyo", "Turf", "Sunny", 1600) },
            { 2, 6, true,  Race("Yasuda Kinen", "G1", "Tokyo", "Turf", "Sunny", 1600) },
            { 2, 6, false, Race("Takarazuka Kinen", "G1", "Hanshin", "Turf", "Sunny", 2200) },
            { 2, 9, false, Race("Sprinters Stakes", "G1", "Nakayama", "Turf", "Sunny", 1200) },
            { 2, 10, false, Race("Tenno Sho Autumn", "G1", "Tokyo", "Turf", "Sunny", 2000) },
            { 2, 11, true, Race("Mile Championship", "G1", "Kyoto", "Turf", "Sunny", 1600) },
            { 2, 11, false, Race("Japan Cup", "G1", "Tokyo", "Turf", "Sunny", 2400) },
            { 2, 12, true, Race("Champions Cup", "G1", "Chukyo", "Dirt", "Sunny", 1800) },
            { 2, 12, false, Race("Arima Kinen", "G1", "Nakayama", "Turf", "Sunny", 2500) },
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
