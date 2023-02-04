#include "UI/LocalLeaderboardPanel.hpp"
#include "bsml/shared/BSML.hpp"
#include "questui/shared/BeatSaberUI.hpp"
#include "assets.hpp"
#include "UI/LocalLeaderboardViewController.hpp"
#include <algorithm>
#include "bsml/shared/Helpers/getters.hpp"
#include "bsml/shared/Helpers/creation.hpp"
#include "assets.hpp"
#include "bsml/shared/BSML.hpp"
#include "bsml/shared/BSML/Components/Backgroundable.hpp"
#include "HMUI/ImageView.hpp"
DEFINE_TYPE(LocalLeaderboard::UI::ViewControllers, LocalLeaderboardPanel);
//LocalLeaderboard::UI::ViewControllers::LocalLeaderboardPanel *pv;
using namespace QuestUI;
using namespace QuestUI::BeatSaberUI;
using namespace HMUI;
using namespace UnityEngine;
using namespace UnityEngine::UI;
using namespace BSML;
HMUI::ImageView* bgImage;
namespace LocalLeaderboard::UI::ViewControllers
{
    void LocalLeaderboardPanel::DidActivate(bool firstActivation, bool addedToHierarchy, bool screenSystemEnabling)
    {
        if (firstActivation)
        {
            parse_and_construct(IncludedAssets::PanelView_bsml, this->get_transform(), this);
        }
    }

    void LocalLeaderboardPanel::PostParse(){
        bgImage = container->GetComponent<BSML::Backgroundable*>()->background;
        bgImage->skew = 0.18f;
        bgImage->gradient = true;
        LocalLeaderboard_logo->skew = 0.18f;
        separator->skew = 0.18f;
    }

    void LocalLeaderboardPanel::SetSaving(bool value)
    {
        prompt_loader->set_active(value);
        promptText->get_gameObject()->set_active(value);
    }

}
