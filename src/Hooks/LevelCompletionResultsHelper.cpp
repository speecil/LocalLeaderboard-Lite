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
#include "Config.hpp"
#include "logging.hpp"
#include <arpa/inet.h>
#include <ifaddrs.h>
#include <Curl/curl.h>
using namespace GlobalNamespace;

LocalLeaderboard::UI::ViewControllers::LocalLeaderboardPanel *LLP;
LocalLeaderboard::UI::ViewControllers::LocalLeaderboardViewController *LLVC;
GlobalNamespace::GameplayModifiers *modifiers;

std::string GetModifiersString(LevelCompletionResults *levelCompletionResults, GameEnergyCounter *energy)
{
    std::string mods = "";

    if (levelCompletionResults->gameplayModifiers->get_noFailOn0Energy() && levelCompletionResults->energy == 0) mods += "Fail (NF) ";
    else if (levelCompletionResults->energy == 0)
    {
        mods += "Fail";
    }
    if (levelCompletionResults->gameplayModifiers->energyType == GameplayModifiers::EnergyType::Battery)
    {
        mods += "BE ";
    }
    if (levelCompletionResults->gameplayModifiers->instaFail)
    {
        mods += "IF ";
    }
    if (levelCompletionResults->gameplayModifiers->failOnSaberClash)
    {
        mods += "SC ";
    }
    if (levelCompletionResults->gameplayModifiers->enabledObstacleType == GameplayModifiers::EnabledObstacleType::NoObstacles)
    {
        mods += "NO ";
    }
    if (levelCompletionResults->gameplayModifiers->noBombs)
    {
        mods += "NB ";
    }
    if (levelCompletionResults->gameplayModifiers->strictAngles)
    {
        mods += "SA ";
    }
    if (levelCompletionResults->gameplayModifiers->disappearingArrows)
    {
        mods += "DA ";
    }
    if (levelCompletionResults->gameplayModifiers->ghostNotes)
    {
        mods += "GN ";
    }
    if (levelCompletionResults->gameplayModifiers->songSpeed == GameplayModifiers::SongSpeed::Slower)
    {
        mods += "SS ";
    }
    if (levelCompletionResults->gameplayModifiers->songSpeed == GameplayModifiers::SongSpeed::Faster)
    {
        mods += "FS ";
    }
    if (levelCompletionResults->gameplayModifiers->songSpeed == GameplayModifiers::SongSpeed::SuperFast)
    {
        mods += "SF ";
    }
    if (levelCompletionResults->gameplayModifiers->smallCubes)
    {
        mods += "SC ";
    }
    if (levelCompletionResults->gameplayModifiers->proMode)
    {
        mods += "PM ";
    }
    if (levelCompletionResults->gameplayModifiers->noArrows)
    {
        mods += "NA ";
    }

    return mods;
}

// Hooks the base game score processor to grab the values and parse them to my config function
MAKE_AUTO_HOOK_MATCH(LevelCompletionResultsHelper, &LevelCompletionResultsHelper::ProcessScore, void, PlayerData *playerData, PlayerLevelStatsData *playerLevelStats, LevelCompletionResults *levelCompletionResults, IReadonlyBeatmapData *transformedBeatmapData, IDifficultyBeatmap *difficultyBeatmap, PlatformLeaderboardsModel *platformLeaderboardsModel)
{
    LevelCompletionResultsHelper(playerData, playerLevelStats, levelCompletionResults, transformedBeatmapData, difficultyBeatmap, platformLeaderboardsModel);
    LLP = UnityEngine::Resources::FindObjectsOfTypeAll<LocalLeaderboard::UI::ViewControllers::LocalLeaderboardPanel *>().FirstOrDefault();
    LLVC = UnityEngine::Resources::FindObjectsOfTypeAll<LocalLeaderboard::UI::ViewControllers::LocalLeaderboardViewController *>().FirstOrDefault();
    // modifiers = UnityEngine::Resources::FindObjectsOfTypeAll<GlobalNamespace::GameplayModifiers *>().FirstOrDefault();
    float MaxScore = ScoreModel::ComputeMaxMultipliedScoreForBeatmap(transformedBeatmapData);
    float modifiedScore = levelCompletionResults->modifiedScore;
    if (modifiedScore == 0 || MaxScore == 0)
        return;
    float acc = (modifiedScore / MaxScore) * 100;
    int score = levelCompletionResults->modifiedScore;
    int badCut = levelCompletionResults->badCutsCount;
    int misses = levelCompletionResults->missedCount;
    bool FC = levelCompletionResults->fullCombo;

    
    std::string currentTime = System::DateTime::get_UtcNow().ToLocalTime().ToString("dd/MM/yy h:mm tt");

    std::string mapId = difficultyBeatmap->get_level()->i_IPreviewBeatmapLevel()->get_levelID();

    int difficulty = difficultyBeatmap->get_difficultyRank();
    std::string mapType = playerLevelStats->get_beatmapCharacteristic()->get_serializedName();

    std::string balls = mapType + std::to_string(difficulty); // BeatMap Allocated Level Label String

    GameEnergyCounter *energy;
    energy = UnityEngine::Resources::FindObjectsOfTypeAll<GameEnergyCounter *>().FirstOrDefault();
    std::string mods = GetModifiersString(levelCompletionResults, energy);

    LocalLeaderboard::Config::UpdateBeatMapInfo(mapId, balls, misses, badCut, FC, currentTime, acc, score, mods);

    INFO("mapId: %s", mapId.c_str());
    INFO("diff: %s", balls.c_str());
    INFO("bad cuts: %i", badCut);
    INFO("misses: %i", misses);
    INFO("Full Combo: %s", FC ? "true" : "false");
    INFO("Accuracy: %.2f", acc);
    INFO("Date: %s", currentTime.c_str());
    INFO("Modifiers: %s", mods);
    LLP->SetSaving(true);
    LLP->promptText->SetText("Saving...");
    std::thread([difficultyBeatmap]()   
                {
            std::this_thread::sleep_for(std::chrono::seconds(5));
            QuestUI::MainThreadScheduler::Schedule([=](){
                LLP->SetSaving(false);
                LLVC->RefreshLeaderboard(difficultyBeatmap);
                }); })
        .detach();
}
