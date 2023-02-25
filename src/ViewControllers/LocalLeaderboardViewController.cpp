// Includes
#include "UI/LocalLeaderboardViewController.hpp"
#include "leaderboardcore/shared/LeaderboardCore.hpp"
#include "questui/shared/BeatSaberUI.hpp"
#include "assets.hpp"
#include "bsml/shared/BSML.hpp"
#include "Config.hpp"
#include "GlobalNamespace/IPreviewBeatmapLevel.hpp"
#include "GlobalNamespace/IBeatmapLevel.hpp"
#include "GlobalNamespace/BeatmapCharacteristicSO.hpp"
#include "GlobalNamespace/IDifficultyBeatmapSet.hpp"
#include "Models/LeaderboardEntry.hpp"
#include "GlobalNamespace/LeaderboardTableCell.hpp"
#include "questui/shared/CustomTypes/Components/MainThreadScheduler.hpp"
#include "UI/LocalLeaderboardPanel.hpp"
#include <algorithm>
#include <string>
#include <thread>
#include "main.hpp"
#include "HMUI/IconSegmentedControl.hpp"
#include "HMUI/IconSegmentedControlCell.hpp"
#include "HMUI/IconSegmentedControl_DataItem.hpp"
#include <array>
#include "bsml/shared/BSML/Components/ButtonIconImage.hpp"
#include "bsml/shared/Helpers/utilities.hpp"
#include "logging.hpp"
// #include "GlobalNamespace/PlayerData.hpp"
// #include "GlobalNamespace/PlayerDataModel.hpp"
// #include "GlobalNamespace/PlayerDataModelHelper.hpp"
#include "GlobalNamespace/UserInfo.hpp"
#include "GlobalNamespace/OculusNetworkPlayerModel.hpp"
#include "GlobalNamespace/IPlatformUserModel.hpp"
#include "System/Threading/Tasks/Task.hpp"
#include "System/Threading/Tasks/Task_1.hpp"
// variable declaration
int totalPages;
bool Ascending;
bool shouldRefresh = false;

// Defines the custom type for use within the file (this is c# methods within c++)
DEFINE_TYPE(LocalLeaderboard::UI::ViewControllers, LocalLeaderboardViewController);

// Namespaces
using namespace QuestUI;
using namespace QuestUI::BeatSaberUI;
using namespace HMUI;
using namespace UnityEngine;
using namespace UnityEngine::UI;
using namespace BSML::Utilities;
using namespace HMUI;
using namespace GlobalNamespace;
using namespace LocalLeaderboard::Models;
using namespace LocalLeaderboard::UI::ViewControllers;
using namespace GlobalNamespace;
using namespace BSML;

// Defining Unity/Base Game Objects for use
LocalLeaderboard::UI::ViewControllers::LocalLeaderboardPanel *Panel;
LocalLeaderboard::UI::ViewControllers::LocalLeaderboardViewController *View;
// GlobalNamespace::PlayerDataModel *LLplayerDataModel;
// auto LLplayerData = LLplayerDataModel->playerData;
// UserInfo *usernameshit;
// Defining own namespace
namespace LocalLeaderboard::UI::ViewControllers
{
    PlatformLeaderboardViewController *originalplvc = NULL; // Fixes a soft restart issue with altering the leaderboard

    // DidActivate Function runs whenever my leaderboard is active
    // Takes 3 parameters with the most important being firstActivation
    void LocalLeaderboardViewController::DidActivate(bool firstActivation, bool addedToHierarchy, bool screenSystemEnabling)
    {
        // Only runs on the first activation
        if (firstActivation)
        {

            // Construct BSML file at the LocalLeaderboardViewControllers (my view controllers) position
            parse_and_construct(IncludedAssets::LocalLeaderboardViewController_bsml, this->get_transform(), this);

            // Find Unity Objects of the game objects declared above
            Panel = UnityEngine::Resources::FindObjectsOfTypeAll<LocalLeaderboardPanel *>().FirstOrDefault();
            View = UnityEngine::Resources::FindObjectsOfTypeAll<LocalLeaderboardViewController *>().FirstOrDefault();
            auto oculusNetwork = UnityEngine::Resources::FindObjectsOfTypeAll<OculusNetworkPlayerModel *>().FirstOrDefault();
            auto platformUserModel = oculusNetwork->platformUserModel;
            using namespace System::Threading::Tasks;
            Task* getInfoTask = platformUserModel->GetUserInfo();
            std::function<void(Task_1<UserInfo*>*)> delegateFunc = [this](Task_1<UserInfo*>* result){
                auto userInfo = result->get_ResultOnSuccess();
                auto playerName = userInfo->userName;
                headerText->SetText(playerName + "'s LOCAL LEADERBOARD!");
            };
            auto delegate = custom_types::MakeDelegate<System::Action_1<Task*>*>(delegateFunc);
            getInfoTask->ContinueWith(delegate);
            // reinterpret_cast<System::Threading::Tasks::Task*>(platformUserModel->GetUserInfo())->ContinueWith(custom_types::MakeDelegate<System::Action_1<System::Threading::Tasks::Task*>*>(static_cast<std::function<void(System::Threading::Tasks::Task_1<GlobalNamespace::UserInfo*>*)>>([](System::Threading::Tasks::Task_1<GlobalNamespace::UserInfo*>* result){

            // })));
            // usernameshit = UnityEngine::Resources::FindObjectsOfTypeAll<UserInfo *>().FirstOrDefault();
            // LLPlayerDataModel = UnityEngine::Resources::FindObjectsOfTypeAll<GlobalNamespace::PlayerDataModel *>().FirstOrDefault();

            // Refresh the leaderboard on first activation (fixes a crash)
            RefreshLeaderboard(currentDifficultyBeatmap);

            // Set the text used to show "Saving" to a welcome prompt
            Panel->promptText->set_richText(true);
            Panel->promptText->get_gameObject()->SetActive(true);
            Panel->promptText->SetText("<color=#28b077>Welcome to LocalLeaderboard by </color><color=blue>Speecil!</color>");

            // Create a thread to show the welcome prompt for 7 seconds
            std::thread([]()
                        {
            std::this_thread::sleep_for(std::chrono::seconds(7));
            QuestUI::MainThreadScheduler::Schedule([=](){
                Panel->promptText->get_gameObject()->SetActive(false);
                }); })
                .detach();
        }
        originalplvc = UnityEngine::Resources::FindObjectsOfTypeAll<PlatformLeaderboardViewController *>().FirstOrDefault();

        RefreshLeaderboard(currentDifficultyBeatmap);
    }

    void LocalLeaderboardViewController::DidDeactivate(bool removedFromHierarchy, bool screenSystemDisabling)
    {
        shouldRefresh = true;
        if (!originalplvc || !originalplvc->m_CachedPtr.m_value)
            return;
    }

    void RichMyText(LeaderboardTableView *tableView)
    {
        for (auto &cell : tableView->GetComponentsInChildren<LeaderboardTableCell *>())
        {
            cell->playerNameText->set_richText(true);
        }
    }

    void LocalLeaderboardViewController::PostParse()
    {
        auto icons = ArrayW<IconSegmentedControl::DataItem *>({IconSegmentedControl::DataItem::New_ctor(LoadSpriteRaw(IncludedAssets::clock_png), "DateTime"),
                                                               IconSegmentedControl::DataItem::New_ctor(LoadSpriteRaw(IncludedAssets::score_png), "Highscore")});
        scopeSegmentedControl->SetData(icons);
    }

    void LocalLeaderboardViewController::OnPageUp()
    {
        page--;
        RefreshLeaderboard(currentDifficultyBeatmap);
    }

    void LocalLeaderboardViewController::OnIconSelected(IconSegmentedControl *segmentedControl, int index)
    {
        sortMethod = index;
        RefreshLeaderboard(currentDifficultyBeatmap);
    }

    void LocalLeaderboardViewController::OnPageDown()
    {
        page++;
        RefreshLeaderboard(currentDifficultyBeatmap);
    }

    void LocalLeaderboardViewController::RefreshLeaderboard(GlobalNamespace::IDifficultyBeatmap *difficultyBeatmap)
    {
        currentDifficultyBeatmap = difficultyBeatmap;
        if (!this->isActivated)
            return;
        errorText->get_gameObject()->set_active(false);
        std::string mapId = difficultyBeatmap->get_level()->i_IPreviewBeatmapLevel()->get_levelID();

        int difficulty = difficultyBeatmap->get_difficultyRank();
        std::string mapType = difficultyBeatmap->get_parentDifficultyBeatmapSet()->get_beatmapCharacteristic()->get_serializedName();

        std::string balls = mapType + std::to_string(difficulty); // BeatMap Allocated Level Label String :lmfao:

        std::vector<LeaderboardEntry> leaderboardEntries = LocalLeaderboard::Config::LoadBeatMapInfo(mapId, balls);
        if (leaderboardEntries.size() > 0)
        {
            LeaderboardEntry recent = leaderboardEntries[leaderboardEntries.size() - 1];
            Panel->lastPlayed->SetText("Last Played: " + recent.datePlayed);
        }
        if (Ascending)
        {
            if (sortMethod == 0)
            {
            }
            else if (sortMethod == 1)
            {
                std::sort(leaderboardEntries.begin(), leaderboardEntries.end(), [](auto &first, auto &second)
                          { return second.acc > first.acc; });
            }
        }
        else
        {
            if (sortMethod == 0)
            {
                std::reverse(leaderboardEntries.begin(), leaderboardEntries.end());
            }
            else if (sortMethod == 1)
            {
                std::sort(leaderboardEntries.begin(), leaderboardEntries.end(), [](auto &first, auto &second)
                          { return first.acc > second.acc; });
            }
        }
        INFO("leaderboard size: %lu", leaderboardEntries.size());
        totalPages = leaderboardEntries.size() / 10;
        leaderboardTableView->SetScores(CreateLeaderboardData(leaderboardEntries, page), -1);
        INFO("Scores Set");
        RichMyText(leaderboardTableView);
        up_button->set_interactable(true);
        down_button->set_interactable(true);
        Panel->totalScores->get_gameObject()->set_active(true);
        Panel->lastPlayed->get_gameObject()->set_active(true);
        if (totalPages - page == totalPages)
        {
            up_button->set_interactable(false);
        }
        if (totalPages == page)
        {
            down_button->set_interactable(false);
        }
        if (leaderboardEntries.size() == 0)
        {
            errorText->get_gameObject()->set_active(true);
            errorText->set_fontSize(7);
            up_button->set_interactable(false);
            down_button->set_interactable(false);
            Panel->lastPlayed->get_gameObject()->set_active(false);
        }
        int playnumba = leaderboardEntries.size();
        std::string plays = std::to_string(playnumba);
        Panel->totalScores->SetText("Total Scores: " + plays);
    }

    List<LeaderboardTableView::ScoreData *> *LocalLeaderboardViewController::CreateLeaderboardData(std::vector<LeaderboardEntry> leaderboard, int page)
    {
        INFO("Creating Leaderboard Data");
        auto tableData = List<LeaderboardTableView::ScoreData *>::New_ctor();
        int pageIndex = page * 10;
        for (int i = pageIndex; i < leaderboard.size() && i < pageIndex + 10; i++)
        {
            int score = leaderboard.data()->score;
            INFO("creating leaderboard entry number %i", i);
            tableData->Add(CreateLeaderboardEntryData(leaderboard[i], i + 1, score));
        }
        INFO("Created Leaderboard Data");
        return tableData;
    }

    GlobalNamespace::LeaderboardTableView::ScoreData *LocalLeaderboardViewController::CreateLeaderboardEntryData(Models::LeaderboardEntry entry, int rank, int score)
    {
        INFO("Creating Entry Data");

        std::string formattedDate = string_format("<color=#28b077>%s</color></size>", entry.datePlayed.c_str());
        std::string formattedAcc = string_format(" - (<color=#ffd42a>%.2f%%</color>)", entry.acc);
        score = entry.score;
        std::string formattedCombo = "";
        //std::string formattedMods = "";
        if (entry.fullCombo)
        {
            formattedCombo = " - FC ";
        }
        else
        {
            formattedCombo = string_format(" - <color=red>x%i </color>", entry.badCutCount + entry.missCount).c_str();
        }

            std::string formattedMods = string_format("   %s</size>", entry.mods.c_str());
        

        std::string result = "<size=80%>" + formattedDate + formattedAcc + formattedCombo + formattedMods + "</size>";

        INFO("Created Entry Data");
        return GlobalNamespace::LeaderboardTableView::ScoreData::New_ctor(score, result, rank, false);
    }
}