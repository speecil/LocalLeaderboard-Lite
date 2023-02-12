#include "main.hpp"
#include "bsml/shared/BSMLDataCache.hpp"
#include "custom-types/shared/register.hpp"
#include "questui/shared/QuestUI.hpp"
#include "leaderboardcore/shared/LeaderboardCore.hpp"
#include "logging.hpp"
#include "Hooks.hpp"
#include "Models/CustomLeaderboard.hpp"
#include "UI/LocalLeaderboardViewController.hpp"
#include "GlobalNamespace/UserInfo.hpp"
#include "assets.hpp"
#include "bsml/shared/BSML.hpp"

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
    INFO("Completed setup!");
}

// This function runs when a new beatmap is selected
// Sets the leaderboard to the newly selected beatmap and runs the function to load its data
void LeaderboardSet(GlobalNamespace::IDifficultyBeatmap* difficultyBeatmap){
    
    leaderboard.get_leaderboardViewController()->page = 0; // set page to 0 to avoid blank page if new beatmap has no data on the page.
    leaderboard.get_leaderboardViewController()->RefreshLeaderboard(difficultyBeatmap); // Refreshes leaderboard with new beatmaps data.
}

// Called later on in the game loading - this is where I install function hooks
extern "C" void load() {
    il2cpp_functions::Init(); // Initialise il2cpp functions to interact with the game
    QuestUI::Init(); // Initialise QuestUI for UI elements
    BSML::Init(); // Initialise BSML for UI elements
    LeaderboardCore::Register::RegisterLeaderboard(&leaderboard, modInfo); // register the leaderboard with the multi-leaderboard handler
    INFO("Installing hooks...");
    Hooks::InstallHooks(getLogger()); // Install game hooks
    INFO("Installed all hooks!");
    LeaderboardCore::Events::NotifyLeaderboardSet() += LeaderboardSet; // Create an event listener to run my leaderboards code when a new beatmap is selected
    getConfig().Load(); // Load my mod specific config for use within the game
}

// BSML (Beat Saber Markup Language) Image Cache to load images faster

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