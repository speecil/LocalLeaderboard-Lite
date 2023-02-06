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

LocalLeaderboard::Models::CustomLeaderboard leaderboard; // Defines my custom leaderboard for easy access

// Loads the config from disk using our modInfo, then returns it for use
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
    
    // define variables to the modloader variables
    info.id = MOD_ID;
    info.version = VERSION;
    modInfo = info;
	
    getConfig().Load();
    getLogger().info("Completed setup!");
}

// This function runs when a new map is selected, (requires a map and its difficulty)
void LeaderboardSet(GlobalNamespace::IDifficultyBeatmap* difficultyBeatmap){
    
    // Sets page to 0 to avoid blank page if prior page exceeds new map's pages.
    leaderboard.get_leaderboardViewController()->page = 0;
    // Refreshes the leaderboard using the RefreshLeaderboard function
    leaderboard.get_leaderboardViewController()->RefreshLeaderboard(difficultyBeatmap);
}

// Called later on in the game loading 
// This is where I install function hooks
extern "C" void load() {
    // Base game function
    il2cpp_functions::Init();
    
    // QuestUI is a GUI Utility For Modders.
    QuestUI::Init();

    // Leaderboard, Hooks and Config Init
    LeaderboardCore::Register::RegisterLeaderboard(&leaderboard, modInfo);
    LeaderboardCore::Events::NotifyLeaderboardSet() += LeaderboardSet;
    getLogger().info("Installing hooks...");
    Hooks::InstallHooks(getLogger());
    getLogger().info("Installed all hooks!");
    getConfig().Load();
}

// Beat Saber Markup Language (BSML) Data Cache
// Helps load images/bsml files quicker

BSML_DATACACHE(LocalLeaderboard_logo_png) {
    return IncludedAssets::LocalLeaderboard_logo_png;
}

BSML_DATACACHE(pixel_png) {
    return IncludedAssets::pixel_png;
}

BSML_DATACACHE(clock_png) {
    return IncludedAssets::clock_png;
}

BSML_DATACACHE(score_png) {
    return IncludedAssets::score_png;
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