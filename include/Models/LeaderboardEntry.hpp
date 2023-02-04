#pragma once
#include <string>

namespace LocalLeaderboard::Models
{
    struct LeaderboardEntry
    {
        int missCount;
        int badCutCount;
        float acc;
        bool fullCombo;
        std::string datePlayed;
        LeaderboardEntry(int missCount, int badCutCount, float acc, bool fullCombo, std::string datePlayed) : missCount(missCount),
                                                                                                              badCutCount(badCutCount),
                                                                                                              acc(acc),
                                                                                                              fullCombo(fullCombo),
                                                                                                              datePlayed(datePlayed) {}
    };
}