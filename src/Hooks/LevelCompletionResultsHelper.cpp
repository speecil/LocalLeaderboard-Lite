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
    // Run the original function first
    LevelCompletionResultsHelper::ProcessScore(playerData, playerLevelStats, levelCompletionResults, transformedBeatmapData, difficultyBeatmap, platformLeaderboardsModel);

    // Retrieve the necessary objects
    auto* localLeaderboardPanel = UnityEngine::Object::FindObjectOfType<LocalLeaderboard::UI::ViewControllers::LocalLeaderboardPanel*>();
    auto* localLeaderboardViewController = UnityEngine::Object::FindObjectOfType<LocalLeaderboard::UI::ViewControllers::LocalLeaderboardViewController*>();

    if (!localLeaderboardPanel || !localLeaderboardViewController) {
        // Log an error and return if the objects could not be retrieved
        ERROR("Failed to retrieve LocalLeaderboard objects");
        return;
    }

    // Get the score data
    const float maxScore = ScoreModel::ComputeMaxMultipliedScoreForBeatmap(transformedBeatmapData);
    const float modifiedScore = levelCompletionResults->modifiedScore;
    if (modifiedScore == 0 || maxScore == 0) {
        // Log an error and return if the score data is invalid
        ERROR("Invalid score data: modifiedScore=%f, maxScore=%f", modifiedScore, maxScore);
        return;
    }

    const float accuracy = (modifiedScore / maxScore) * 100;
    const int score = levelCompletionResults->modifiedScore;
    const int badCuts = levelCompletionResults->badCutsCount;
    const int misses = levelCompletionResults->missedCount;
    const bool fullCombo = levelCompletionResults->fullCombo;

    // Get the beatmap ID and difficulty
    const std::string mapId = difficultyBeatmap->get_level()->i_IPreviewBeatmapLevel()->get_levelID();
    const int difficulty = difficultyBeatmap->get_difficultyRank();
    const std::string mapType = playerLevelStats->get_beatmapCharacteristic()->get_serializedName();
    const std::string beatmapLabel = mapType + std::to_string(difficulty);

    // Save the data and log
    LocalLeaderboard::Config::UpdateBeatMapInfo(mapId, beatmapLabel, misses, badCuts, fullCombo, System::DateTime::get_UtcNow().ToLocalTime().ToString("dd/MM/yy h:mm tt"), accuracy, score, getModifiers(levelCompletionResults));
    INFO("Saved score data for beatmap: %s, difficulty: %s, accuracy: %.2f, score: %d, bad cuts: %d, misses: %d, full combo: %s", mapId.c_str(), beatmapLabel.c_str(), accuracy, score, badCuts, misses, fullCombo ? "true" : "false");

    // Update the leaderboard UI and show a saving prompt
    localLeaderboardPanel->SetSaving(true);
    localLeaderboardPanel->promptText->SetText("Saving...");
    std::thread([=] {
        std::this_thread::sleep_for(std::chrono::seconds(5));
        QuestUI::MainThreadScheduler::Schedule([=] {
            localLeaderboardPanel->SetSaving(false);
            localLeaderboardViewController->RefreshLeaderboard(difficultyBeatmap);
        });
    }).detach();
}