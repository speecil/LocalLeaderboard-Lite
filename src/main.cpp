#include "main.hpp"
#include "bsml/shared/BSMLDataCache.hpp"
#include "custom-types/shared/register.hpp"
#include "questui/shared/QuestUI.hpp"
#include "leaderboardcore/shared/LeaderboardCore.hpp"
#include "logging.hpp"
#include "hooking.hpp"
#include "Models/CustomLeaderboard.hpp"

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
// Date, Time, Acc, Mistakes