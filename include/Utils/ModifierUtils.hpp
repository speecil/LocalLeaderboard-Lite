#include "main.hpp"
#include "Hooks.hpp"
#include "GlobalNamespace/LevelCompletionResultsHelper.hpp"
#include "GlobalNamespace/LevelCompletionResults.hpp"
#include "GlobalNamespace/GameplayModifiers.hpp"
#include "GlobalNamespace/GameplayModifiersHelper.hpp"
#include "GlobalNamespace/GameEnergyCounter.hpp"


std::string getModifiers(GlobalNamespace::LevelCompletionResults* levelCompletionResults)
{
    std::string mods = "";
    if (levelCompletionResults->gameplayModifiers->get_noFailOn0Energy() && levelCompletionResults->energy == 0)
    {
        mods += "Fail (NF) ";
    }
    else if (levelCompletionResults->energy == 0)
    {
        mods += "Fail";
    }
    if (levelCompletionResults->gameplayModifiers->energyType == GlobalNamespace::GameplayModifiers::EnergyType::Battery)
    {
        mods += "BE ";
    }
    if (levelCompletionResults->gameplayModifiers->instaFail)
    {
        mods += "IF ";
    }
    if (levelCompletionResults->gameplayModifiers->failOnSaberClash)
    {
        mods += "SC ";
    }
    if (levelCompletionResults->gameplayModifiers->enabledObstacleType == GlobalNamespace::GameplayModifiers::EnabledObstacleType::NoObstacles)
    {
        mods += "NO ";
    }
    if (levelCompletionResults->gameplayModifiers->noBombs)
    {
        mods += "NB ";
    }
    if (levelCompletionResults->gameplayModifiers->strictAngles)
    {
        mods += "SA ";
    }
    if (levelCompletionResults->gameplayModifiers->disappearingArrows)
    {
        mods += "DA ";
    }
    if (levelCompletionResults->gameplayModifiers->ghostNotes)
    {
        mods += "GN ";
    }
    if (levelCompletionResults->gameplayModifiers->songSpeed == GlobalNamespace::GameplayModifiers::SongSpeed::Slower)
    {
        mods += "SS ";
    }
    if (levelCompletionResults->gameplayModifiers->songSpeed == GlobalNamespace::GameplayModifiers::SongSpeed::Faster)
    {
        mods += "FS ";
    }
    if (levelCompletionResults->gameplayModifiers->songSpeed == GlobalNamespace::GameplayModifiers::SongSpeed::SuperFast)
    {
        mods += "SF ";
    }
    if (levelCompletionResults->gameplayModifiers->smallCubes)
    {
        mods += "SC ";
    }
    if (levelCompletionResults->gameplayModifiers->proMode)
    {
        mods += "PM ";
    }
    if (levelCompletionResults->gameplayModifiers->noArrows)
    {
        mods += "NA ";
    }
    return mods;
}
