#pragma once
#include <string>
#include "beatsaber-hook/shared/config/rapidjson-utils.hpp"
#include "Models/LeaderboardEntry.hpp"
#include <vector>
// Defines the functions for the config and their parameters
namespace LocalLeaderboard::Config
{
    void AddBeatMap(rapidjson::Value &obj, std::string mapID, std::string diff, int missCount, int badCutCount, bool fullCombo, std::string datePlayed, float acc, int score, std::string mods);
    void UpdateBeatMapInfo(std::string mapID, std::string diff, int missCount, int badCutCount, bool fullCombo, std::string datePlayed, float acc, int score, std::string mods);
    std::vector<Models::LeaderboardEntry> LoadBeatMapInfo(std::string mapID, std::string diff);
}