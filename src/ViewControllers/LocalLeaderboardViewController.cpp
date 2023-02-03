#include "UI/LocalLeaderboardViewController.hpp"
#include "leaderboardcore/shared/LeaderboardCore.hpp"
#include "questui/shared/BeatSaberUI.hpp"
#include "assets.hpp"
#include "bsml/shared/BSML.hpp"
#include "Config.hpp"
#include "GlobalNamespace/IPreviewBeatmapLevel.hpp"
#include "GlobalNamespace/IBeatmapLevel.hpp"
#include "GlobalNamespace/BeatmapCharacteristicSO.hpp"
#include "GlobalNamespace/IDifficultyBeatmapSet.hpp"
#include <algorithm>
DEFINE_TYPE(LocalLeaderboard::UI::ViewControllers, LocalLeaderboardViewController);

using namespace QuestUI;
using namespace QuestUI::BeatSaberUI;
using namespace HMUI;
using namespace UnityEngine;
using namespace UnityEngine::UI;
using namespace BSML;


namespace LocalLeaderboard::UI::ViewControllers
{
    void LocalLeaderboardViewController::DidActivate(bool firstActivation, bool addedToHierarchy, bool screenSystemEnabling)
    {
        if (firstActivation)
        {
            parse_and_construct(IncludedAssets::LocalLeaderboardViewController_bsml, this->get_transform(), this);
        }
    }

    void LocalLeaderboard::UI::ViewControllers::LocalLeaderboardViewController::RefreshLeaderboard(GlobalNamespace::IDifficultyBeatmap *difficultyBeatmap)
    {
        currentDifficultyBeatmap = difficultyBeatmap;
        std::string mapId = difficultyBeatmap->get_level()->i_IPreviewBeatmapLevel()->get_levelID();

        int difficulty = difficultyBeatmap->get_difficultyRank();
        std::string mapType = difficultyBeatmap->get_parentDifficultyBeatmapSet()->get_beatmapCharacteristic()->get_serializedName();
        
        std::string balls = mapType + std::to_string(difficulty); // BeatMap Allocated Level Label String :lmfao:
        
        std::vector<LeaderboardEntry> leaderboardEntries = LocalLeaderboard::Config::LoadBeatMapInfo(mapId, balls);
        std::sort(leaderboardEntries.begin(), leaderboardEntries.end(), [](auto& first, auto& second){return first.acc > second.acc;});
        
        // leaderboardTableView->SetScores()
    }

}
