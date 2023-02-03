#include "UI/LocalLeaderboardPanel.hpp"
#include "bsml/shared/BSML.hpp"
#include "questui/shared/BeatSaberUI.hpp"
#include "assets.hpp"
DEFINE_TYPE(LocalLeaderboard::UI::ViewControllers, LocalLeaderboardPanel);

using namespace QuestUI;
using namespace QuestUI::BeatSaberUI;
using namespace HMUI;
using namespace UnityEngine;
using namespace UnityEngine::UI;
using namespace BSML;
namespace LocalLeaderboard::UI::ViewControllers
{
    void LocalLeaderboardPanel::DidActivate(bool firstActivation, bool addedToHierarchy, bool screenSystemEnabling)
    {
        if (firstActivation) {
            parse_and_construct(IncludedAssets::PanelView_bsml, this->get_transform(), this);
        }
    }
}
