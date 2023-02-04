#include "UI/LocalLeaderboardPanel.hpp"
#include "bsml/shared/BSML.hpp"
#include "questui/shared/BeatSaberUI.hpp"
#include "assets.hpp"
#include "UI/LocalLeaderboardViewController.hpp"
#include <algorithm>
DEFINE_TYPE(LocalLeaderboard::UI::ViewControllers, LocalLeaderboardPanel);
//LocalLeaderboard::UI::ViewControllers::LocalLeaderboardPanel *pv;
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
        if (firstActivation)
        {
            parse_and_construct(IncludedAssets::PanelView_bsml, this->get_transform(), this);
        }
    }

    // void LocalLeaderboardPanel::SetSaving(bool value)
    // {
    //     pv->prompt_loader->set_active(value);
    //     pv->promptText->get_gameObject()->set_active(value);
    // }

}
