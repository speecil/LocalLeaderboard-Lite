#include "Models/CustomLeaderboard.hpp"
#include "UI/LocalLeaderboardViewController.hpp"
#include "UI/LocalLeaderboardPanel.hpp"
#include "questui/shared/BeatSaberUI.hpp"

namespace LocalLeaderboard::Models
{
    UI::ViewControllers::LocalLeaderboardPanel* CustomLeaderboard::get_panelViewController()
    {
        if (!panelViewController)
        {
            panelViewController = QuestUI::BeatSaberUI::CreateViewController<LocalLeaderboard::UI::ViewControllers::LocalLeaderboardPanel*>();
        }
        return panelViewController.ptr();
    }

    UI::ViewControllers::LocalLeaderboardViewController* CustomLeaderboard::get_leaderboardViewController()
    {
        if (!leaderboardViewController)
        {
            leaderboardViewController = QuestUI::BeatSaberUI::CreateViewController<LocalLeaderboard::UI::ViewControllers::LocalLeaderboardViewController*>();
        }
        return leaderboardViewController.ptr();
    }
}