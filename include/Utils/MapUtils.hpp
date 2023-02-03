#pragma once

using namespace std;
#include "GlobalNamespace/BeatmapDifficulty.hpp"

namespace LocalLeaderboard::Utils
{
    std::string toLower(std::string string);
    std::string GetDifficultyName(GlobalNamespace::BeatmapDifficulty difficulty)
} // namespace LocalLeaderboard::Utils
