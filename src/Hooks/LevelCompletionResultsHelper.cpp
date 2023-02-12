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
#include "UI/LocalLeaderboardViewController.hpp"
#include "System/DateTime.hpp"
#include "UnityEngine/Time.hpp"
#include "Config.hpp"
#include "logging.hpp"
#include <vector>
using namespace GlobalNamespace;

LocalLeaderboard::UI::ViewControllers::LocalLeaderboardPanel *LLP;
LocalLeaderboard::UI::ViewControllers::LocalLeaderboardViewController *LLVC;
GlobalNamespace::GameplayModifiers *modifiers;
std::string mods = "";

// function to get the active modifiers
void getMods()
{
    if (modifiers->energyType == GameplayModifiers::EnergyType::Battery)
    {
        mods += "BE";
    }
    if (modifiers->noFailOn0Energy)
    {
        mods += "NF";
    }
    if (modifiers->instaFail)
    {
        mods += "IF";
    }
    if (modifiers->failOnSaberClash)
    {
        mods += "SC";
    }
    if (modifiers->enabledObstacleType == GameplayModifiers::EnabledObstacleType::NoObstacles)
    {
        mods += "NO";
    }
    if (modifiers->noBombs)
    {
        mods += "NB";
    }
    if (modifiers->strictAngles)
    {
        mods += "SA";
    }
    if (modifiers->disappearingArrows)
    {
        mods += "DA";
    }
    if (modifiers->ghostNotes)
    {
        mods += "GN";
    }
    if (modifiers->songSpeed == GameplayModifiers::SongSpeed::Slower)
    {
        mods += "SS";
    }
    if (modifiers->songSpeed == GameplayModifiers::SongSpeed::Faster)
    {
        mods += "FS";
    }
    if (modifiers->songSpeed == GameplayModifiers::SongSpeed::SuperFast)
    {
        mods += "SF";
    }
    if (modifiers->smallCubes)
    {
        mods += "SC";
    }
    if (modifiers->strictAngles)
    {
        mods += "SA";
    }
    if (modifiers->proMode)
    {
        mods += "PM";
    }
    if (modifiers->noArrows)
    {
        mods += "NA";
    }
}
// Hooks the base game score processor to grab the values and parse them to my config function
MAKE_AUTO_HOOK_MATCH(LevelCompletionResultsHelper, &LevelCompletionResultsHelper::ProcessScore, void, PlayerData *playerData, PlayerLevelStatsData *playerLevelStats, LevelCompletionResults *levelCompletionResults, IReadonlyBeatmapData *transformedBeatmapData, IDifficultyBeatmap *difficultyBeatmap, PlatformLeaderboardsModel *platformLeaderboardsModel)
{
    LevelCompletionResultsHelper(playerData, playerLevelStats, levelCompletionResults, transformedBeatmapData, difficultyBeatmap, platformLeaderboardsModel);
    LLP = UnityEngine::Resources::FindObjectsOfTypeAll<LocalLeaderboard::UI::ViewControllers::LocalLeaderboardPanel *>().FirstOrDefault();
    LLVC = UnityEngine::Resources::FindObjectsOfTypeAll<LocalLeaderboard::UI::ViewControllers::LocalLeaderboardViewController *>().FirstOrDefault();
    
    // Score values
    float MaxScore = ScoreModel::ComputeMaxMultipliedScoreForBeatmap(transformedBeatmapData);
    float modifiedScore = levelCompletionResults->modifiedScore;
    if (modifiedScore == 0 || MaxScore == 0)
        return;
    float acc = (modifiedScore / MaxScore) * 100;
    int score = levelCompletionResults->modifiedScore;
    int badCut = levelCompletionResults->badCutsCount;
    int misses = levelCompletionResults->missedCount;
    bool FC = levelCompletionResults->fullCombo;

    // get the current time in dd/MM/yy h:mm tt
    std::string currentTime = System::DateTime::get_UtcNow().ToLocalTime().ToString("dd/MM/yy h:mm tt");

    // Beatmap identifier modules
    std::string mapId = difficultyBeatmap->get_level()->i_IPreviewBeatmapLevel()->get_levelID();
    int difficulty = difficultyBeatmap->get_difficultyRank();
    std::string mapType = playerLevelStats->get_beatmapCharacteristic()->get_serializedName();
    
    // BeatMap Allocated Level Label String
    std::string balls = mapType + std::to_string(difficulty);

    // Get the active mods
    getMods();

    // Run the void to update the config with the new score data
    LocalLeaderboard::Config::UpdateBeatMapInfo(mapId, balls, misses, badCut, FC, currentTime, acc, score, mods);

    // Debugging to logss
    INFO("mapId: %s", mapId.c_str());
    INFO("diff: %s", balls.c_str());
    INFO("bad cuts: %i", badCut);
    INFO("misses: %i", misses);
    INFO("Full Combo: %s", FC ? "true" : "false");
    INFO("Accuracy: %.2f", acc);
    INFO("Date: %s", currentTime.c_str());
    INFO("Modifiers: %s", mods);

    // Show the "Saving..." text in UI
    LLP->SetSaving(true);

    // Thread to wait 5 seconds before refreshing
    std::thread([difficultyBeatmap]()
                {
            std::this_thread::sleep_for(std::chrono::seconds(5));
            QuestUI::MainThreadScheduler::Schedule([=](){
                LLP->SetSaving(false);
                LLVC->RefreshLeaderboard(difficultyBeatmap);
                }); })
        .detach();
}