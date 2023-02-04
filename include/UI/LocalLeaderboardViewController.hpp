#pragma once

#include "custom-types/shared/macros.hpp"
#include "HMUI/ViewController.hpp"
#include "GlobalNamespace/IDifficultyBeatmap.hpp"
#include "GlobalNamespace/LeaderboardTableView.hpp"
#include "GlobalNamespace/LeaderboardTableView_ScoreData.hpp"
#include "Models/LeaderboardEntry.hpp"
#include <vector>
using namespace std;

DECLARE_CLASS_CODEGEN(LocalLeaderboard::UI::ViewControllers, LocalLeaderboardViewController, HMUI::ViewController,
    DECLARE_OVERRIDE_METHOD(void, DidActivate, il2cpp_utils::il2cpp_type_check::MetadataGetter<&HMUI::ViewController::DidActivate>::get(), bool firstActivation, bool addedToHierarchy, bool screenSystemEnabling);
    DECLARE_INSTANCE_METHOD(void, RefreshLeaderboard, GlobalNamespace::IDifficultyBeatmap* difficultyBeatmap);
    DECLARE_INSTANCE_FIELD(GlobalNamespace::LeaderboardTableView*, leaderboardTableView);
    //DECLARE_INSTANCE_FIELD(TMPro::TextMeshProUGUI*, errorText);

    private:
        int page;
        GlobalNamespace::IDifficultyBeatmap* currentDifficultyBeatmap;
        GlobalNamespace::LeaderboardTableView::ScoreData* CreateLeaderboardEntryData(Models::LeaderboardEntry entry, int rank);
        List<GlobalNamespace::LeaderboardTableView::ScoreData*>* CreateLeaderboardData(std::vector<Models::LeaderboardEntry> leaderboard, int page);

)
