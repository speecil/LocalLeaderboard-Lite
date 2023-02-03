#include "UI/LocalLeaderboardPanel.hpp"

#include "questui/shared/BeatSaberUI.hpp"

DEFINE_TYPE(LocalLeaderboard::UI::ViewControllers, LocalLeaderboardPanel);

using namespace QuestUI;
using namespace QuestUI::BeatSaberUI;
using namespace HMUI;
using namespace UnityEngine;
using namespace UnityEngine::UI;

namespace LocalLeaderboard::UI::ViewControllers
{
    void LocalLeaderboardPanel::DidActivate(bool firstActivation, bool addedToHierarchy, bool screenSystemEnabling)
    {
        if (firstActivation) {
            CreateText(get_transform(), "Panel here");
        }
    }
}
