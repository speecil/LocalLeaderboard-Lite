#pragma once

#include "leaderboardcore/shared/Models/CustomLeaderboard.hpp"

namespace LocalLeaderboard::Models
{
    class CustomLeaderboard final : public LeaderboardCore::Models::CustomLeaderboard
    {
        public:
            HMUI::ViewController* get_panelViewController() final override;
            HMUI::ViewController* get_leaderboardViewController() final override;
        private:
            SafePtrUnity<HMUI::ViewController> panelViewController;
            SafePtrUnity<HMUI::ViewController> leaderboardViewController;
    };
}