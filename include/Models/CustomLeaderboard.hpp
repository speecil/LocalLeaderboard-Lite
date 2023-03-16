#pragma once

#include "leaderboardcore/shared/Models/CustomLeaderboard.hpp"
#include "UI/LocalLeaderboardPanel.hpp"
#include "UI/LocalLeaderboardViewController.hpp"

// Definitions for view controllers and their pointers using safeptr (makes it harder to mess up)
namespace LocalLeaderboard::Models
{
    class CustomLeaderboard final : public LeaderboardCore::Models::CustomLeaderboard
    {
    public:
        UI::ViewControllers::LocalLeaderboardPanel *get_panelViewController() final override;
        UI::ViewControllers::LocalLeaderboardViewController *get_leaderboardViewController() final override;

    private:
        SafePtrUnity<UI::ViewControllers::LocalLeaderboardPanel> panelViewController;
        SafePtrUnity<UI::ViewControllers::LocalLeaderboardViewController> leaderboardViewController;
    };
}