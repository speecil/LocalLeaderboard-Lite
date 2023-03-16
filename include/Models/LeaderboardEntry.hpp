#pragma once
#include <string>

// Defines the struct each leaderboard entry must follow to output the correct format
namespace LocalLeaderboard::Models
{
    struct LeaderboardEntry
    {
        int missCount;
        int badCutCount;
        float acc;
        bool fullCombo;
        std::string datePlayed;
        int score;
        std::string mods;
        LeaderboardEntry(int missCount, int badCutCount, float acc, bool fullCombo, std::string datePlayed, int score, std::string mods) : missCount(missCount),
                                                                                                                                           badCutCount(badCutCount),
                                                                                                                                           acc(acc),
                                                                                                                                           fullCombo(fullCombo),
                                                                                                                                           datePlayed(datePlayed),
                                                                                                                                           score(score),
                                                                                                                                           mods(mods) {}
    };
}