#pragma once
#include <string>
#include "GlobalNamespace/IDifficultyBeatmap.hpp"
#include <optional>

namespace LocalLeaderboard::Models{
    class APISong{
        public:
            std::string songHash;  
            std::string difficulty;

            APISong(std::string songHash, std::string difficulty):
                songHash(songHash),
                difficulty(difficulty)
                {}
            APISong();
            static std::optional<APISong> GetDataForBeatmap(GlobalNamespace::IDifficultyBeatmap* difficultyBeatmap);


    }
}