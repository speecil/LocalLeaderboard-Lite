#pragma once

#include "custom-types/shared/macros.hpp"
#include "HMUI/ViewController.hpp"
#include "TMPro/TextMeshProUGUI.hpp"
#include "UnityEngine/GameObject.hpp"
#include "UnityEngine/UI/HorizontalLayoutGroup.hpp"
#include "HMUI/ImageView.hpp"
#include "UnityEngine/UI/Button.hpp"

// Declaring each function field/method
// Useful for bsml

DECLARE_CLASS_CODEGEN(LocalLeaderboard::UI::ViewControllers, LocalLeaderboardPanel, HMUI::ViewController,
    DECLARE_OVERRIDE_METHOD(void, DidActivate, il2cpp_utils::il2cpp_type_check::MetadataGetter<&HMUI::ViewController::DidActivate>::get(), bool firstActivation, bool addedToHierarchy, bool screenSystemEnabling);
    DECLARE_INSTANCE_FIELD(TMPro::TextMeshProUGUI*, promptText);
    DECLARE_INSTANCE_FIELD(TMPro::TextMeshProUGUI*, totalScores);
    DECLARE_INSTANCE_FIELD(TMPro::TextMeshProUGUI*, lastPlayed);
    DECLARE_INSTANCE_FIELD(UnityEngine::GameObject*, prompt_loader);


    DECLARE_INSTANCE_METHOD(void, Update);
    DECLARE_INSTANCE_METHOD(void, SetSaving, bool value);
    DECLARE_INSTANCE_METHOD(void, PostParse);

    DECLARE_INSTANCE_FIELD(UnityEngine::UI::HorizontalLayoutGroup*, container);
    DECLARE_INSTANCE_FIELD(HMUI::ImageView*, LocalLeaderboard_logo);
    DECLARE_INSTANCE_FIELD(HMUI::ImageView*, separator);

    


)