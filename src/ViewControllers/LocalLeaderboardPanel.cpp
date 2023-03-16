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
using namespace QuestUI;
using namespace QuestUI::BeatSaberUI;
using namespace HMUI;
using namespace UnityEngine;
using namespace UnityEngine::UI;
using namespace BSML;

// define custom type
DEFINE_TYPE(LocalLeaderboard::UI::ViewControllers, LocalLeaderboardPanel);

LocalLeaderboard::UI::ViewControllers::LocalLeaderboardViewController *vc;
HMUI::ImageView *bgImage;
int colourVal = 0;
bool userIsCool;
extern bool Ascending;
namespace LocalLeaderboard::UI::ViewControllers // namespace
{
    // runs on every activation
    void LocalLeaderboardPanel::DidActivate(bool firstActivation, bool addedToHierarchy, bool screenSystemEnabling)
    {
        // runs on first activation only
        if (firstActivation)
        {
            // find the gameobject for the view controller
            vc = UnityEngine::Resources::FindObjectsOfTypeAll<LocalLeaderboard::UI::ViewControllers::LocalLeaderboardViewController *>().FirstOrDefault();
            // parse the bsml file
            parse_and_construct(IncludedAssets::PanelView_bsml, this->get_transform(), this);

            // easter egg if you have my other projects installed
            int randomNum = random() % 10;
            userIsCool = (Modloader::getMods().contains("SpeecilTweaks") || Modloader::getMods().contains("PauseRemapper") || Modloader::getMods().contains("FailButton")) && (randomNum == 0);
        }
    }

    // runs right after the UI elements are parsed
    void LocalLeaderboardPanel::PostParse()
    {
        // fix UI elements to look good on a curved and angled background
        bgImage = container->GetComponent<BSML::Backgroundable *>()->background;
        bgImage->skew = 0.18f;
        LocalLeaderboard_logo->skew = 0.18f;
        separator->skew = 0.18f;
        bgImage->set_gradient(true);
    }

    // generate a gradient for the easter egg
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

    // function that runs on every frame (used to shift the colour of the panel)
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

    // function to show the saving prompt after you have set a score
    void LocalLeaderboardPanel::SetSaving(bool value)
    {
        totalScores->get_gameObject()->set_active(value);
        prompt_loader->set_active(value);
        promptText->get_gameObject()->set_active(value);
    }

}