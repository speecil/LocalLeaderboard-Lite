#pragma once

#include "leaderboardcore/shared/Models/CustomLeaderboard.hpp"
#include "UI/LocalLeaderboardPanel.hpp"
#include "UI/LocalLeaderboardViewController.hpp"

namespace LocalLeaderboard::Models
{
    class CustomLeaderboard final : public LeaderboardCore::Models::CustomLeaderboard
    {
        public:
            UI::ViewControllers::LocalLeaderboardPanel* get_panelViewController() final override;
            UI::ViewControllers::LocalLeaderboardViewController* get_leaderboardViewController() final override;
        private:
            SafePtrUnity<HMUI::ViewController> panelViewController;
            SafePtrUnity<HMUI::ViewController> leaderboardViewController;
    };
}