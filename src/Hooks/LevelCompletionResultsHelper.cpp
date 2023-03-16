#include "main.hpp"
#include "Hooks.hpp"
#include "GlobalNamespace/LevelCompletionResultsHelper.hpp"
#include "GlobalNamespace/LevelCompletionResults.hpp"
#include "GlobalNamespace/PlayerData.hpp"
#include "GlobalNamespace/PlayerLevelStatsData.hpp"
#include "GlobalNamespace/IReadonlyBeatmapData.hpp"
#include "GlobalNamespace/IDifficultyBeatmap.hpp"
#include "GlobalNamespace/PlatformLeaderboardsModel.hpp"
#include "GlobalNamespace/ScoreModel.hpp"
#include "GlobalNamespace/IPreviewBeatmapLevel.hpp"
#include "GlobalNamespace/IBeatmapLevel.hpp"
#include "GlobalNamespace/BeatmapCharacteristicSO.hpp"
#include "GlobalNamespace/BeatmapCharacteristicSO.hpp"
#include "GlobalNamespace/BeatmapDifficulty.hpp"
#include "GlobalNamespace/BeatmapDifficultyMethods.hpp"
#include "GlobalNamespace/BeatmapDifficultySerializedMethods.hpp"
#include "GlobalNamespace/IDifficultyBeatmapSet.hpp"
#include "GlobalNamespace/IPreviewBeatmapLevel.hpp"
#include "GlobalNamespace/PlatformLeaderboardsModel_ScoresScope.hpp"
#include "UnityEngine/Resources.hpp"
#include "UI/LocalLeaderboardPanel.hpp"
#include "questui/shared/CustomTypes/Components/MainThreadScheduler.hpp"
#include "GlobalNamespace/GameplayModifiers.hpp"
#include "GlobalNamespace/GameplayModifiersHelper.hpp"
#include "UI/LocalLeaderboardViewController.hpp"
#include "GlobalNamespace/GameEnergyCounter.hpp"
#include "System/DateTime.hpp"
#include "UnityEngine/Time.hpp"
#include "UnityEngine/Transform.hpp"
#include "Config.hpp"
#include "logging.hpp"
#include <vector>

#include "Utils/ModifierUtils.hpp"
using namespace GlobalNamespace;

LocalLeaderboard::UI::ViewControllers::LocalLeaderboardPanel *LLP;
LocalLeaderboard::UI::ViewControllers::LocalLeaderboardViewController *LLVC;

// Hooks the base game score processor to grab the values and parse them to my config function
MAKE_AUTO_HOOK_MATCH(LevelCompletionResultsHelper, &LevelCompletionResultsHelper::ProcessScore, void, PlayerData *playerData, PlayerLevelStatsData *playerLevelStats, LevelCompletionResults *levelCompletionResults, IReadonlyBeatmapData *transformedBeatmapData, IDifficultyBeatmap *difficultyBeatmap, PlatformLeaderboardsModel *platformLeaderboardsModel)
{
    LevelCompletionResultsHelper(playerData, playerLevelStats, levelCompletionResults, transformedBeatmapData, difficultyBeatmap, platformLeaderboardsModel); // run orig

    // access my gameobjects
    LLP = UnityEngine::Resources::FindObjectsOfTypeAll<LocalLeaderboard::UI::ViewControllers::LocalLeaderboardPanel *>().FirstOrDefault();
    LLVC = UnityEngine::Resources::FindObjectsOfTypeAll<LocalLeaderboard::UI::ViewControllers::LocalLeaderboardViewController *>().FirstOrDefault();

    // define score variables
    float MaxScore = ScoreModel::ComputeMaxMultipliedScoreForBeatmap(transformedBeatmapData);
    float modifiedScore = levelCompletionResults->modifiedScore;
    if (modifiedScore == 0 || MaxScore == 0)
        return;
    float acc = (modifiedScore / MaxScore) * 100;
    int score = levelCompletionResults->modifiedScore;
    int badCut = levelCompletionResults->badCutsCount;
    int misses = levelCompletionResults->missedCount;
    bool FC = levelCompletionResults->fullCombo;

    // get the current time
    std::string currentTime = System::DateTime::get_UtcNow().ToLocalTime().ToString("dd/MM/yy h:mm tt");

    // get the beatmap ID and difficulty
    std::string mapId = difficultyBeatmap->get_level()->i_IPreviewBeatmapLevel()->get_levelID();
    int difficulty = difficultyBeatmap->get_difficultyRank();
    std::string mapType = playerLevelStats->get_beatmapCharacteristic()->get_serializedName();

    // concatenate to one string
    std::string balls = mapType + std::to_string(difficulty); // BeatMap Allocated Level Label String

    // run the function to save the data
    LocalLeaderboard::Config::UpdateBeatMapInfo(mapId, balls, misses, badCut, FC, currentTime, acc, score, getModifiers(levelCompletionResults));

    // logging
    INFO("mapId: %s", mapId.c_str());
    INFO("diff: %s", balls.c_str());
    INFO("bad cuts: %i", badCut);
    INFO("misses: %i", misses);
    INFO("Full Combo: %s", FC ? "true" : "false");
    INFO("Accuracy: %.2f", acc);
    INFO("Date: %s", currentTime.c_str());
    INFO("Modifiers: %s", getModifiers(levelCompletionResults));

    // sets the saving prompt to be true
    LLP->SetSaving(true);
    LLP->promptText->SetText("Saving...");

    // thread to wait before refreshing
    std::thread([difficultyBeatmap]()
                {
            std::this_thread::sleep_for(std::chrono::seconds(5));
            QuestUI::MainThreadScheduler::Schedule([=](){
                LLP->SetSaving(false);
                LLVC->RefreshLeaderboard(difficultyBeatmap);
                }); })
        .detach();
}