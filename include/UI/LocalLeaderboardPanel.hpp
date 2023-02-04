#pragma once

#include "custom-types/shared/macros.hpp"
#include "HMUI/ViewController.hpp"

DECLARE_CLASS_CODEGEN(LocalLeaderboard::UI::ViewControllers, LocalLeaderboardPanel, HMUI::ViewController,
    DECLARE_OVERRIDE_METHOD(void, DidActivate, il2cpp_utils::il2cpp_type_check::MetadataGetter<&HMUI::ViewController::DidActivate>::get(), bool firstActivation, bool addedToHierarchy, bool screenSystemEnabling);
    //DECLARE_INSTANCE_FIELD(TMPro::TextMeshProUGUI*, promptText);
    //DECLARE_INSTANCE_FIELD(UnityEngine::GameObject*, prompt_loader);
    //DECLARE_INSTANCE_METHOD(void, SetSaving, bool value);

)
