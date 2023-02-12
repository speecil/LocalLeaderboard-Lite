#pragma once

#include "custom-types/shared/macros.hpp"
#include "HMUI/ViewController.hpp"
#include "GlobalNamespace/IDifficultyBeatmap.hpp"
#include "GlobalNamespace/LeaderboardTableView.hpp"
#include "GlobalNamespace/LeaderboardTableView_ScoreData.hpp"
#include "Models/LeaderboardEntry.hpp"
#include "TMPro/TextMeshProUGUI.hpp"
#include "UnityEngine/UI/Button.hpp"
#include "HMUI/IconSegmentedControl.hpp"
#include "HMUI/IconSegmentedControlCell.hpp"
#include "HMUI/IconSegmentedControl_DataItem.hpp"
#include <vector>
#include "HMUI/ImageView.hpp"

// Declaring each function field/method
// Useful for bsml

DECLARE_CLASS_CODEGEN(LocalLeaderboard::UI::ViewControllers, LocalLeaderboardViewController, HMUI::ViewController,
    DECLARE_OVERRIDE_METHOD(void, DidActivate, il2cpp_utils::il2cpp_type_check::MetadataGetter<&HMUI::ViewController::DidActivate>::get(), bool firstActivation, bool addedToHierarchy, bool screenSystemEnabling);
    DECLARE_OVERRIDE_METHOD(void, DidDeactivate, il2cpp_utils::il2cpp_type_check::MetadataGetter<&HMUI::ViewController::DidDeactivate>::get(), bool removedFromHierarchy, bool screenSystemDisabling);
    DECLARE_INSTANCE_METHOD(void, RefreshLeaderboard, GlobalNamespace::IDifficultyBeatmap* difficultyBeatmap);
    DECLARE_INSTANCE_METHOD(void, PostParse);
    DECLARE_INSTANCE_FIELD(HMUI::IconSegmentedControl*, scopeSegmentedControl);
    DECLARE_INSTANCE_FIELD(GlobalNamespace::LeaderboardTableView*, leaderboardTableView);
    DECLARE_INSTANCE_FIELD(TMPro::TextMeshProUGUI*, errorText);
    DECLARE_INSTANCE_FIELD(TMPro::TextMeshProUGUI*, headerText);
    DECLARE_INSTANCE_FIELD(UnityEngine::UI::Button*, up_button);
    DECLARE_INSTANCE_FIELD(UnityEngine::UI::Button*, down_button);
    DECLARE_INSTANCE_METHOD(void, OnIconSelected, HMUI::IconSegmentedControl* segmentedControl, int index);
    DECLARE_INSTANCE_METHOD(void, OnPageUp);
    DECLARE_INSTANCE_METHOD(void, OnPageDown);
    DECLARE_INSTANCE_METHOD(void, changeSort);
    DECLARE_INSTANCE_FIELD(HMUI::ImageView*, sorter);

    
    public:
        int page;
        int sortMethod;
        GlobalNamespace::IDifficultyBeatmap* currentDifficultyBeatmap;
    private:
        GlobalNamespace::LeaderboardTableView::ScoreData* CreateLeaderboardEntryData(Models::LeaderboardEntry entry, int rank, int score);
        List<GlobalNamespace::LeaderboardTableView::ScoreData*>* CreateLeaderboardData(std::vector<Models::LeaderboardEntry> leaderboard, int page);

)