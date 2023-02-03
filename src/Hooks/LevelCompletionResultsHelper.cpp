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

#include "System/DateTime.hpp"
#include "UnityEngine/Time.hpp"
using namespace GlobalNamespace;
MAKE_AUTO_HOOK_MATCH(LevelCompletionResultsHelper, &LevelCompletionResultsHelper::ProcessScore, void, PlayerData *playerData, PlayerLevelStatsData *playerLevelStats, LevelCompletionResults *levelCompletionResults, IReadonlyBeatmapData *transformedBeatmapData, IDifficultyBeatmap *difficultyBeatmap, PlatformLeaderboardsModel *platformLeaderboardsModel)
{
    LevelCompletionResultsHelper(playerData, playerLevelStats, levelCompletionResults, transformedBeatmapData, difficultyBeatmap, platformLeaderboardsModel);
    float MaxScore = ScoreModel::ComputeMaxMultipliedScoreForBeatmap(transformedBeatmapData);
    float modifiedScore = levelCompletionResults->modifiedScore;
    if (modifiedScore == 0 || MaxScore == 0) return;
    float acc = (modifiedScore / MaxScore) * 100;


    std::string badCut = std::to_string(levelCompletionResults->badCutsCount);
    std::string misses = std::to_string(levelCompletionResults->missedCount);
    std::string FC = levelCompletionResults->fullCombo ? "Full Combo" : "Not FC";
    std::string currentTime = System::DateTime::get_UtcNow().ToLocalTime().ToString("dd/MM/yyyy h:mm tt");

    getLogger().info("bad cuts: %s", badCut.c_str());
    getLogger().info("misses: %s", misses.c_str());
    getLogger().info("Full Combo: %s", FC.c_str());
    getLogger().info("Accuracy: %.2f", acc);
    getLogger().info("Date: %s", currentTime.c_str());
    
}