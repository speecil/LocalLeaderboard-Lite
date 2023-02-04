#include "Models/CustomLeaderboard.hpp"
#include "bsml/shared/Helpers/creation.hpp"

namespace LocalLeaderboard::Models
{
    UI::ViewControllers::LocalLeaderboardPanel* CustomLeaderboard::get_panelViewController()
    {
        if (!panelViewController)
        {
            panelViewController = BSML::Helpers::CreateViewController<LocalLeaderboard::UI::ViewControllers::LocalLeaderboardPanel*>();
        }
        return panelViewController.ptr();
    }

    UI::ViewControllers::LocalLeaderboardViewController* CustomLeaderboard::get_leaderboardViewController()
    {
        if (!leaderboardViewController)
        {
            leaderboardViewController = BSML::Helpers::CreateViewController<LocalLeaderboard::UI::ViewControllers::LocalLeaderboardViewController*>();
        }
        return leaderboardViewController.ptr();
    }
}