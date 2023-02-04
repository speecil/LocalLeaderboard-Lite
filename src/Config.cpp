#include "main.hpp"
#include "Config.hpp"
#include "UI/LocalLeaderboardViewController.hpp"
#include "UI/LocalLeaderboardPanel.hpp"
#include "leaderboardcore/shared/LeaderboardCore.hpp"
#include "questui/shared/BeatSaberUI.hpp"
#include "assets.hpp"
#include "bsml/shared/BSML.hpp"
#include "Config.hpp"
#include "GlobalNamespace/IPreviewBeatmapLevel.hpp"
#include "GlobalNamespace/IBeatmapLevel.hpp"
#include "GlobalNamespace/BeatmapCharacteristicSO.hpp"
#include "GlobalNamespace/IDifficultyBeatmapSet.hpp"
#include "Models/LeaderboardEntry.hpp"
#include <algorithm>
using namespace QuestUI;
using namespace QuestUI::BeatSaberUI;
using namespace HMUI;
using namespace UnityEngine;
using namespace UnityEngine::UI;
using namespace BSML;
using namespace LocalLeaderboard::Models;
using namespace LocalLeaderboard::UI::ViewControllers;
using namespace GlobalNamespace;
using namespace rapidjson;
// LocalLeaderboard::UI::ViewControllers::LocalLeaderboardViewController* vc;
// LocalLeaderboard::UI::ViewControllers::LocalLeaderboardPanel* pv;

namespace LocalLeaderboard::Config{
void AddBeatMap(Value& obj, std::string mapID, std::string diff, int missCount, int badCutCount, bool fullCombo, std::string datePlayed, float acc) {
    Value difficulty(kObjectType); // diff string
    auto allocator = getConfig().config.GetAllocator();
    char buffer[60]; int len = sprintf(buffer, "%s", mapID.c_str());
    Document::ValueType string(kStringType);
    string.SetString(buffer, len, allocator);
    ///
    char diffType[20]; int len2 = sprintf(diffType, "%s", diff.c_str());
    Document::ValueType string2(kStringType);
    string2.SetString(diffType, len2, allocator);
    ///
    Document::ValueType diffArr(kArrayType);

    Document::ValueType diffValues(kObjectType);
    diffValues.AddMember("missCount", missCount, allocator);
    diffValues.AddMember("badCutCount", badCutCount, allocator);
    diffValues.AddMember("fullCombo", fullCombo, allocator);
    diffValues.AddMember("datePlayed", datePlayed, allocator);
    diffValues.AddMember("acc", acc, allocator);
    
    diffArr.PushBack(diffValues, allocator);
    difficulty.AddMember(string2, diffArr, allocator);
    obj.AddMember(string, difficulty, allocator);
    getConfig().Write();
}

void UpdateBeatMapInfo(std::string mapID, std::string diff, int missCount, int badCutCount, bool fullCombo, std::string datePlayed, float acc){
    auto allocator = getConfig().config.GetAllocator();
    Value& obj = getConfig().config;
    auto itr = obj.FindMember(mapID);
    if (itr != obj.MemberEnd()){

        Document::ValueType difficulty(kObjectType);
        difficulty.AddMember("missCount", missCount, allocator);
        difficulty.AddMember("badCutCount", badCutCount, allocator);
        difficulty.AddMember("fullCombo", fullCombo, allocator);
        difficulty.AddMember("datePlayed", datePlayed, allocator);
        difficulty.AddMember("acc", acc, allocator);


        auto itr2 = itr->value.GetObject().FindMember(diff);
        if (itr2 != itr->value.GetObject().MemberEnd()){
            itr2->value.GetArray().PushBack(difficulty, allocator);
            getConfig().Write();
            return;
        }
        char diffType[20]; int len2 = sprintf(diffType, "%s", diff.c_str());
        Document::ValueType string2(kStringType);
        string2.SetString(diffType, len2, allocator);

        Document::ValueType diffArr(kArrayType);
        diffArr.PushBack(difficulty, allocator);

        itr->value.AddMember(string2, diffArr, allocator);
        getConfig().Write();
    }
    else AddBeatMap(obj, mapID, diff, missCount, badCutCount, fullCombo, datePlayed, acc);
}

std::vector<Models::LeaderboardEntry> LoadBeatMapInfo(std::string mapID, std::string diff){
    std::vector<Models::LeaderboardEntry> leaderboard;
    auto allocator = getConfig().config.GetAllocator();
    Value& obj = getConfig().config;
    auto itr = obj.FindMember(mapID);
    if (itr != obj.MemberEnd()){
        auto itr2 = itr->value.GetObject().FindMember(diff);
        if (itr2 != itr->value.GetObject().MemberEnd()){
            auto array = itr2->value.GetArray();
            for (int i=0; i < array.Size(); i++){
                auto scoreData = array[i].GetObject();
                leaderboard.push_back(Models::LeaderboardEntry(
                    scoreData.FindMember("missCount")->value.GetInt(),
                    scoreData.FindMember("badCutCount")->value.GetInt(),
                    scoreData.FindMember("acc")->value.GetFloat(),
                    scoreData.FindMember("fullCombo")->value.GetBool(),
                    scoreData.FindMember("datePlayed")->value.GetString()));
            }
        }
    }
    return leaderboard;
}
}