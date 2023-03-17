#include "main.hpp"
#include "Hooks.hpp"
#include "GlobalNamespace/LevelCompletionResultsHelper.hpp"
#include "GlobalNamespace/LevelCompletionResults.hpp"
#include "GlobalNamespace/GameplayModifiers.hpp"
#include "GlobalNamespace/GameplayModifiersHelper.hpp"
#include "GlobalNamespace/GameEnergyCounter.hpp"



std::string getModifiers(const GlobalNamespace::LevelCompletionResults* levelCompletionResults) {
    std::string mods = "";
    GlobalNamespace::GameplayModifiers* gameplayModifiers = levelCompletionResults->gameplayModifiers;
    std::vector<std::pair<std::string, bool>> modifierChecks = {
        {"Fail (NF) ", gameplayModifiers->get_noFailOn0Energy() && levelCompletionResults->energy == 0},
        {"Fail", !gameplayModifiers->get_noFailOn0Energy() && levelCompletionResults->energy == 0},
        {"BE ", gameplayModifiers->energyType == GlobalNamespace::GameplayModifiers::EnergyType::Battery},
        {"IF ", gameplayModifiers->instaFail},
        {"SC ", gameplayModifiers->failOnSaberClash},
        {"NO ", gameplayModifiers->enabledObstacleType == GlobalNamespace::GameplayModifiers::EnabledObstacleType::NoObstacles},
        {"NB ", gameplayModifiers->noBombs},
        {"SA ", gameplayModifiers->strictAngles},
        {"DA ", gameplayModifiers->disappearingArrows},
        {"GN ", gameplayModifiers->ghostNotes},
        {"SS ", gameplayModifiers->songSpeed == GlobalNamespace::GameplayModifiers::SongSpeed::Slower},
        {"FS ", gameplayModifiers->songSpeed == GlobalNamespace::GameplayModifiers::SongSpeed::Faster},
        {"SF ", gameplayModifiers->songSpeed == GlobalNamespace::GameplayModifiers::SongSpeed::SuperFast},
        {"SC ", gameplayModifiers->smallCubes},
        {"PM ", gameplayModifiers->proMode},
        {"NA ", gameplayModifiers->noArrows}
    };

    for (const auto& modifierCheck : modifierChecks) {
        if (modifierCheck.second) {
            mods += modifierCheck.first;
        }
    }

    return mods;
}











