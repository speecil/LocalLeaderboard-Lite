#pragma once

#include "custom-types/shared/macros.hpp"
#include "HMUI/ViewController.hpp"
#include "GlobalNamespace/IDifficultyBeatmap.hpp"


DECLARE_CLASS_CODEGEN(LocalLeaderboard::UI::ViewControllers, LocalLeaderboardViewController, HMUI::ViewController,
    DECLARE_OVERRIDE_METHOD(void, DidActivate, il2cpp_utils::il2cpp_type_check::MetadataGetter<&HMUI::ViewController::DidActivate>::get(), bool firstActivation, bool addedToHierarchy, bool screenSystemEnabling);
    DECLARE_INSTANCE_METHOD(void, RefreshLeaderboard, GlobalNamespace::IDifficultyBeatmap* difficultyBeatmap);

    private:
        int page;
        GlobalNamespace::IDifficultyBeatmap* currentDifficultyBeatmap;
)
