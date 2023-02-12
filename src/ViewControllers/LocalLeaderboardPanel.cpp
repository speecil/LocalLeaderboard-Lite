#include "main.hpp"
#include "UI/LocalLeaderboardPanel.hpp"
#include "bsml/shared/BSML.hpp"
#include "questui/shared/BeatSaberUI.hpp"
#include "assets.hpp"
#include "UI/LocalLeaderboardViewController.hpp"
#include <algorithm>
#include "bsml/shared/Helpers/getters.hpp"
#include "bsml/shared/Helpers/creation.hpp"
#include "assets.hpp"
#include "logging.hpp"
#include "bsml/shared/BSML.hpp"
#include "bsml/shared/BSML/Components/Backgroundable.hpp"
#include "HMUI/ImageView.hpp"
#include "GlobalNamespace/IDifficultyBeatmap.hpp"
#include "UnityEngine/Time.hpp"
#include <random>
DEFINE_TYPE(LocalLeaderboard::UI::ViewControllers, LocalLeaderboardPanel);
LocalLeaderboard::UI::ViewControllers::LocalLeaderboardViewController *vc;
using namespace QuestUI;
using namespace QuestUI::BeatSaberUI;
using namespace HMUI;
using namespace UnityEngine;
using namespace UnityEngine::UI;
using namespace BSML;
HMUI::ImageView *bgImage;
int colourVal = 0;
bool userIsCool;
extern bool Ascending;
namespace LocalLeaderboard::UI::ViewControllers
{
    void LocalLeaderboardPanel::DidActivate(bool firstActivation, bool addedToHierarchy, bool screenSystemEnabling)
    {
        if (firstActivation)
        {
            vc = UnityEngine::Resources::FindObjectsOfTypeAll<LocalLeaderboard::UI::ViewControllers::LocalLeaderboardViewController *>().FirstOrDefault();
            parse_and_construct(IncludedAssets::PanelView_bsml, this->get_transform(), this);
            int randomNum = random() %10;
            userIsCool = (Modloader::getMods().contains("SpeecilTweaks") || Modloader::getMods().contains("PauseRemapper") || Modloader::getMods().contains("FailButton")) || (randomNum == 0);
        }
    }

    void LocalLeaderboardPanel::PostParse()
    {
        bgImage = container->GetComponent<BSML::Backgroundable *>()->background;
        bgImage->skew = 0.18f;
        LocalLeaderboard_logo->skew = 0.18f;
        separator->skew = 0.18f;
        bgImage->set_gradient(true);

    }

    UnityEngine::Color GradientGen(int ColourPos)
    {
        static int c[3];
        static float a[3];
        UnityEngine::Color OutputColour;

        if (ColourPos < 85)
        {
            c[0] = ColourPos * 3;
            c[1] = 255 - ColourPos * 3;
            c[2] = 0;
        }
        else if (ColourPos < 170)
        {
            ColourPos -= 85;
            c[0] = 255 - ColourPos * 3;
            c[1] = 0;
            c[2] = ColourPos * 3;
        }
        else
        {
            ColourPos -= 170;
            c[0] = 0;
            c[1] = ColourPos * 3;
            c[2] = 255 - ColourPos * 3;
        }

        a[0] = (float)c[0] / 255.0;
        a[1] = (float)c[1] / 255.0;
        a[2] = (float)c[2] / 255.0;

        OutputColour = UnityEngine::Color(a[0], a[1], a[2], 1.0f);

        return OutputColour;
    }

    void LocalLeaderboardPanel::Update()
    {
        if (userIsCool)
        {
            bgImage->set_color({GradientGen(colourVal)});
            colourVal += (UnityEngine::Time::get_deltaTime());
            if (colourVal > 255)
            {
                colourVal = 0;
            }
        }
    }

    void LocalLeaderboardPanel::SetSaving(bool value)
    {
        totalScores->get_gameObject()->set_active(value);
        prompt_loader->set_active(value);
        promptText->get_gameObject()->set_active(value);
    }

}