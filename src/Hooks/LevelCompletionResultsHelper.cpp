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
#include "UnityEngine/Resources.hpp"
#include "UI/LocalLeaderboardPanel.hpp"
#include "questui/shared/CustomTypes/Components/MainThreadScheduler.hpp"
#include "UI/LocalLeaderboardViewController.hpp"
#include "System/DateTime.hpp"
#include "UnityEngine/Time.hpp"
#include "Config.hpp"
using namespace GlobalNamespace;

LocalLeaderboard::UI::ViewControllers::LocalLeaderboardPanel *LLP;
LocalLeaderboard::UI::ViewControllers::LocalLeaderboardViewController *LLVC;

MAKE_AUTO_HOOK_MATCH(LevelCompletionResultsHelper, &LevelCompletionResultsHelper::ProcessScore, void, PlayerData *playerData, PlayerLevelStatsData *playerLevelStats, LevelCompletionResults *levelCompletionResults, IReadonlyBeatmapData *transformedBeatmapData, IDifficultyBeatmap *difficultyBeatmap, PlatformLeaderboardsModel *platformLeaderboardsModel)
{
    LevelCompletionResultsHelper(playerData, playerLevelStats, levelCompletionResults, transformedBeatmapData, difficultyBeatmap, platformLeaderboardsModel);
    LLP = UnityEngine::Resources::FindObjectsOfTypeAll<LocalLeaderboard::UI::ViewControllers::LocalLeaderboardPanel *>().FirstOrDefault();
    LLVC = UnityEngine::Resources::FindObjectsOfTypeAll<LocalLeaderboard::UI::ViewControllers::LocalLeaderboardViewController *>().FirstOrDefault();

    float MaxScore = ScoreModel::ComputeMaxMultipliedScoreForBeatmap(transformedBeatmapData);
    float modifiedScore = levelCompletionResults->modifiedScore;
    if (modifiedScore == 0 || MaxScore == 0)
        return;
    float acc = (modifiedScore / MaxScore) * 100;

    int badCut = levelCompletionResults->badCutsCount;
    int misses = levelCompletionResults->missedCount;
    bool FC = levelCompletionResults->fullCombo;
    std::string currentTime = System::DateTime::get_UtcNow().ToLocalTime().ToString("dd/MM/yyyy h:mm tt");

    std::string mapId = difficultyBeatmap->get_level()->i_IPreviewBeatmapLevel()->get_levelID();

    int difficulty = difficultyBeatmap->get_difficultyRank();
    std::string mapType = playerLevelStats->get_beatmapCharacteristic()->get_serializedName();

    std::string balls = mapType + std::to_string(difficulty); // BeatMap Allocated Level Label String :lmfao:

    LocalLeaderboard::Config::UpdateBeatMapInfo(mapId, balls, misses, badCut, FC, currentTime, acc);

    getLogger().info("mapId: %s", mapId.c_str());
    getLogger().info("diff: %s", balls.c_str());
    getLogger().info("bad cuts: %i", badCut);
    getLogger().info("misses: %i", misses);
    getLogger().info("Full Combo: %s", FC ? "true" : "false");
    getLogger().info("Accuracy: %.2f", acc);
    getLogger().info("Date: %s", currentTime.c_str());
    LLP->SetSaving(true);
        std::thread([difficultyBeatmap](){
            std::this_thread::sleep_for(std::chrono::seconds(5));
            QuestUI::MainThreadScheduler::Schedule([=](){
                LLP->SetSaving(false);
                LLVC->RefreshLeaderboard(difficultyBeatmap);
                });
            }).detach();
}