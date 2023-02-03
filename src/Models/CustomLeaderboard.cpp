#include "Models/CustomLeaderboard.hpp"
#include "UI/LocalLeaderboardViewController.hpp"
#include "UI/LocalLeaderboardPanel.hpp"
#include "questui/shared/BeatSaberUI.hpp"

namespace LocalLeaderboard::Models
{
    HMUI::ViewController* CustomLeaderboard::get_panelViewController()
    {
        if (!panelViewController)
        {
            panelViewController = QuestUI::BeatSaberUI::CreateViewController<LocalLeaderboard::UI::ViewControllers::LocalLeaderboardPanel*>();
        }
        return panelViewController.ptr();
    }

    HMUI::ViewController* CustomLeaderboard::get_leaderboardViewController()
    {
        if (!leaderboardViewController)
        {
            leaderboardViewController = QuestUI::BeatSaberUI::CreateViewController<LocalLeaderboard::UI::ViewControllers::LocalLeaderboardViewController*>();
        }
        return leaderboardViewController.ptr();
    }
}