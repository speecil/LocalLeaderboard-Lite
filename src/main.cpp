#include "main.hpp"
#include "bsml/shared/BSMLDataCache.hpp"
#include "custom-types/shared/register.hpp"
#include "questui/shared/QuestUI.hpp"
#include "leaderboardcore/shared/LeaderboardCore.hpp"
#include "logging.hpp"
#include "hooking.hpp"
#include "Models/CustomLeaderboard.hpp"
#include "UI/LocalLeaderboardViewController.hpp"
#include "assets.hpp"

static ModInfo modInfo; // Stores the ID and version of our mod, and is sent to the modloader upon startup
LocalLeaderboard::Models::CustomLeaderboard leaderboard;
// Loads the config from disk using our modInfo, then returns it for use
// other config tools such as config-utils don't use this config, so it can be removed if those are in use
Configuration& getConfig() {
    static Configuration config(modInfo);
    return config;
}

// Returns a logger, useful for printing debug messages
Logger& getLogger() {
    static Logger* logger = new Logger(modInfo);
    return *logger;
}

// Called at the early stages of game loading
extern "C" void setup(ModInfo& info) {
    info.id = MOD_ID;
    info.version = VERSION;
    modInfo = info;
	
    getConfig().Load();
    getLogger().info("Completed setup!");
}

void LeaderboardSet(GlobalNamespace::IDifficultyBeatmap* difficultyBeatmap){
    
    leaderboard.get_leaderboardViewController()->page = 0;
    leaderboard.get_leaderboardViewController()->RefreshLeaderboard(difficultyBeatmap);
    if(leaderboard.get_leaderboardViewController()->get_transform()->Find("HeaderPanel")){
        getLogger().info("header found lmao");
    }
    else{
        getLogger().info("header gone lmao");
    }
}

// Called later on in the game loading - a good time to install function hooks
extern "C" void load() {
    il2cpp_functions::Init();
    QuestUI::Init();
    LeaderboardCore::Register::RegisterLeaderboard(&leaderboard, modInfo);
    getLogger().info("Installing hooks...");
    Hooks::InstallHooks(getLogger());
    getLogger().info("Installed all hooks!");
    LeaderboardCore::Events::NotifyLeaderboardSet() += LeaderboardSet;
    getConfig().Load();
}

BSML_DATACACHE(LocalLeaderboard_logo_png) {
    return IncludedAssets::LocalLeaderboard_logo_png;
}

BSML_DATACACHE(pixel_png) {
    return IncludedAssets::pixel_png;
}

BSML_DATACACHE(Player_png) {
    return IncludedAssets::Player_png;
}

BSML_DATACACHE(carat_png) {
    return IncludedAssets::carat_png;
}

BSML_DATACACHE(LocalLeaderboardViewController_bsml) {
    return IncludedAssets::LocalLeaderboardViewController_bsml;
}

BSML_DATACACHE(PanelView_bsml) {
    return IncludedAssets::PanelView_bsml;
}