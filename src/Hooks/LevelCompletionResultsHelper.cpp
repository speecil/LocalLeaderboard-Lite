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

/*
std::string ipapi_url = "http://ip-api.com/json/";

size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

std::string GetCountryFromIP(std::string ip_address)
{
    CURL *curl;
    CURLcode res;
    std::string read_buffer;

    curl = curl_easy_init();
    if (curl) {
        std::string request_url = ipapi_url + ip_address;
        curl_easy_setopt(curl, CURLOPT_URL, request_url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &read_buffer);
        res = curl_easy_perform(curl);
        curl_easy_cleanup(curl);

        if(res != CURLE_OK) {
            return "";
        }

        // Parse the JSON response to get the country information
        // Example JSON response:
        // {
        //     "status": "success",
        //     "country": "United States",
        //     "countryCode": "US",
        //     "region": "CA",
        //     "regionName": "California",
        //     "city": "Los Angeles",
        //     "zip": "90001",
        //     "lat": 34.0522,
        //     "lon": -118.2437,
        //     "timezone": "America/Los_Angeles",
        //     "isp": "Comcast Cable",
        //     "org": "Comcast Cable",
        //     "as": "AS7922 Comcast Cable Communications, LLC",
        //     "query": "98.136.243.47"
        // }
        size_t pos = read_buffer.find("\"country\":\"");
        if (pos == std::string::npos) {
            return "";
        }
        pos += std::strlen("\"country\":\"");
        size_t end_pos = read_buffer.find("\"", pos);
        if (end_pos == std::string::npos) {
            return "";
        }
        std::string country = read_buffer.substr(pos, end_pos - pos);
        return country;
    }

    return "";
}


std::string GetLocalIpAddress() {
    struct ifaddrs *addrs, *tmp;
    getifaddrs(&addrs);
    tmp = addrs;
    char addrBuf[INET_ADDRSTRLEN];
    std::string ipAddress;

    while (tmp) {
        if (tmp->ifa_addr && tmp->ifa_addr->sa_family == AF_INET) {
            struct sockaddr_in *pAddr = (struct sockaddr_in *)tmp->ifa_addr;
            // Convert the IP address from network byte order to a string
            inet_ntop(AF_INET, &pAddr->sin_addr, addrBuf, INET_ADDRSTRLEN);
            if (strcmp(tmp->ifa_name, "lo") != 0) { // ignore localhost
                ipAddress = addrBuf;
                break;
            }
        }
        tmp = tmp->ifa_next;
    }

    freeifaddrs(addrs);
    return ipAddress;
}
*/

std::string GetModifiersString(LevelCompletionResults *levelCompletionResults, GameEnergyCounter *energy)
{
    std::string mods = "";

    if (levelCompletionResults->gameplayModifiers->get_noFailOn0Energy() && levelCompletionResults->energy == 0)
    {
        mods += "Fail (NF) ";
    }
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

    
    // std::string localIP = GetLocalIpAddress();
    // auto countryCode = GetCountryFromIP(localIP);
    // getLogger().info(countryCode.c_str());
    //DEBUG(countryCode);

    
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