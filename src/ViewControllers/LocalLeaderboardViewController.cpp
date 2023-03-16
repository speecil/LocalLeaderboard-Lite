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
#include "GlobalNamespace/UserInfo.hpp"
#include "GlobalNamespace/OculusNetworkPlayerModel.hpp"
#include "GlobalNamespace/IPlatformUserModel.hpp"
#include "System/Threading/Tasks/Task.hpp"
#include "System/Threading/Tasks/Task_1.hpp"
#include "HMUI/ImageView.hpp"
#include <iostream>
#include <cstring>
#include "bsml/shared/BSML/Animations/AnimationController.hpp"

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

            // c++ delegate to use c# methods in order to get the username of the player
            auto platformUserModel = oculusNetwork->platformUserModel;
            using namespace System::Threading::Tasks;
            Task *getInfoTask = platformUserModel->GetUserInfo();
            std::function<void(Task_1<UserInfo *> *)> delegateFunc = [this](Task_1<UserInfo *> *result)
            {
                // set the prompt to the welcome text
                auto userInfo = result->get_ResultOnSuccess();
                auto playerName = userInfo->userName;
                Panel->promptText->SetText("Hey there " + playerName + ", <color=#28b077>Welcome to LocalLeaderboard by </color><color=blue>Speecil!</color>");
                // set the header text to be personalised to the user
                headerText->SetText(playerName->ToUpper() + "'S LOCAL LEADERBOARD!");
            };

            // run delegate
            auto delegate = custom_types::MakeDelegate<System::Action_1<Task *> *>(delegateFunc);
            getInfoTask->ContinueWith(delegate);

            // Refresh the leaderboard on leaderboard first activation (fixes a crash)
            RefreshLeaderboard(currentDifficultyBeatmap);

            // Set the text used to show "Saving" to a welcome prompt
            Panel->promptText->set_richText(true);
            Panel->promptText->get_gameObject()->SetActive(true);

            // Create a thread to show the welcome prompt for 7 seconds
            std::thread([]()
                        {
            std::this_thread::sleep_for(std::chrono::seconds(7));
            QuestUI::MainThreadScheduler::Schedule([=](){
                Panel->promptText->get_gameObject()->SetActive(false);
                }); })
                .detach();
        }
        originalplvc = UnityEngine::Resources::FindObjectsOfTypeAll<PlatformLeaderboardViewController *>().FirstOrDefault(); // maybe not needed

        RefreshLeaderboard(currentDifficultyBeatmap);
    }

    // this function runs on every deactivation of the leaderboard
    void LocalLeaderboardViewController::DidDeactivate(bool removedFromHierarchy, bool screenSystemDisabling)
    {
        shouldRefresh = true;
        if (!originalplvc || !originalplvc->m_CachedPtr.m_value)
            return;
    }

    // macro to set rich text to true of every leaderboard cell
    void RichMyText(LeaderboardTableView *tableView)
    {
        for (auto &cell : tableView->GetComponentsInChildren<LeaderboardTableCell *>())
        {
            cell->playerNameText->set_richText(true);
        }
    }

    // this function runs right after the UI elements are parsed, allowing to edit parsed elements
    // right before the player sees it
    void LocalLeaderboardViewController::PostParse()
    {
        // load the icons and set them to the segmented control
        auto icons = ArrayW<IconSegmentedControl::DataItem *>({IconSegmentedControl::DataItem::New_ctor(LoadSpriteRaw(IncludedAssets::clock_png), "DateTime"),
                                                               IconSegmentedControl::DataItem::New_ctor(LoadSpriteRaw(IncludedAssets::score_png), "Highscore")});
        scopeSegmentedControl->SetData(icons);
    }

    // function that runs when the player clicks on the change sort button
    void LocalLeaderboardViewController::changeSort()
    {
        // checks the status and flips it accordingly
        if (Ascending)
        {
            Ascending = false;
            sorter->get_gameObject()->GetComponentInChildren<HMUI::ImageView *>()->get_transform()->Rotate(0, 0, 180);
        }
        else
        {
            Ascending = true;
            sorter->get_gameObject()->GetComponentInChildren<HMUI::ImageView *>()->get_transform()->Rotate(0, 0, 180);
        }
        RefreshLeaderboard(currentDifficultyBeatmap);
    }

    // changes the page index variable and refreshes the leaderboard to show new data
    void LocalLeaderboardViewController::OnPageUp()
    {
        page--;
        RefreshLeaderboard(currentDifficultyBeatmap);
    }

    // changes the sort method based on what the user has selected
    void LocalLeaderboardViewController::OnIconSelected(IconSegmentedControl *segmentedControl, int index)
    {
        sortMethod = index;
        RefreshLeaderboard(currentDifficultyBeatmap);
    }

    // changes the page index variable and refreshes the leaderboard to show new data
    void LocalLeaderboardViewController::OnPageDown()
    {
        page++;
        RefreshLeaderboard(currentDifficultyBeatmap);
    }

    // function that runs to refresh the leaderboard data
    void LocalLeaderboardViewController::RefreshLeaderboard(GlobalNamespace::IDifficultyBeatmap *difficultyBeatmap)
    {
        currentDifficultyBeatmap = difficultyBeatmap;
        // check that the leaderboard is active before refreshing
        if (!this->isActivated)
            return;
        errorText->get_gameObject()->set_active(false);

        // get the level ID of the beatmap
        std::string mapId = difficultyBeatmap->get_level()->i_IPreviewBeatmapLevel()->get_levelID();

        // get the difficulty selected
        int difficulty = difficultyBeatmap->get_difficultyRank();

        // get the map specific type (used for custom made characteristics made by the community)
        std::string mapType = difficultyBeatmap->get_parentDifficultyBeatmapSet()->get_beatmapCharacteristic()->get_serializedName();

        std::string balls = mapType + std::to_string(difficulty); // BeatMap Allocated Level Label String

        // load the beatmap info using the function in the config.cpp using the data from the selected beatmap
        std::vector<LeaderboardEntry> leaderboardEntries = LocalLeaderboard::Config::LoadBeatMapInfo(mapId, balls);

        // check that there are leaderboard entries
        if (leaderboardEntries.size() > 0)
        {
            // create a copy of the leaderboard data as it is already sorted by last played
            LeaderboardEntry recent = leaderboardEntries[leaderboardEntries.size() - 1];

            // set the lastPlayed text to the most recent play
            Panel->lastPlayed->SetText("Last Played: " + recent.datePlayed);
        }

        /*
        SORTING
        - checks if the user wants it to sort ascending or descending
        - get the sort method from the user (highscore or date played)
        - sort accordingly
        */
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

        INFO("leaderboard size: %lu", leaderboardEntries.size());                             // logging
        totalPages = leaderboardEntries.size() / 10;                                          // get the total pages for the beatmap
        leaderboardTableView->SetScores(CreateLeaderboardData(leaderboardEntries, page), -1); // create the data
        INFO("Scores Set");                                                                   // logging
        RichMyText(leaderboardTableView);                                                     // sets rich text
        up_button->set_interactable(true);
        down_button->set_interactable(true);
        Panel->totalScores->get_gameObject()->set_active(true);
        Panel->lastPlayed->get_gameObject()->set_active(true);

        // check that the user is at the top of the page list
        if (totalPages - page == totalPages)
        {
            up_button->set_interactable(false);
        }

        // check that the user isnt at the bottom of the page list
        if (totalPages == page)
        {
            down_button->set_interactable(false);
        }

        // check that there is no entries
        if (leaderboardEntries.size() == 0)
        {
            errorText->get_gameObject()->set_active(true);
            errorText->set_fontSize(7);
            up_button->set_interactable(false);
            down_button->set_interactable(false);
            Panel->lastPlayed->get_gameObject()->set_active(false);
        }

        int playnumba = leaderboardEntries.size();             // check the amount of plays
        std::string plays = std::to_string(playnumba);         // set the int to a string
        Panel->totalScores->SetText("Total Scores: " + plays); // set the total scores text to show the total scores for the given beatmap
    }

    // list that gets the returned data from the config then runs a function to create the pretty leaderboard entry
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

    /*
    format the data from the list and make it pretty
    then display it using rich text
    then return it
    */
    GlobalNamespace::LeaderboardTableView::ScoreData *LocalLeaderboardViewController::CreateLeaderboardEntryData(Models::LeaderboardEntry entry, int rank, int score)
    {
        INFO("Creating Entry Data");

        std::string formattedDate = string_format("<color=#28b077>%s</color></size>", entry.datePlayed.c_str());
        std::string formattedAcc = string_format(" - (<color=#ffd42a>%.2f%%</color>)", entry.acc);
        score = entry.score;
        std::string formattedCombo = "";
        if (entry.fullCombo)
        {
            formattedCombo = " -<color=green> FC </color>";
        }
        else
        {
            formattedCombo = string_format(" - <color=red>x%i </color>", entry.badCutCount + entry.missCount).c_str();
        }

        std::string formattedMods = string_format("   %s</size>", entry.mods.c_str());

        std::string result = "<size=85%>" + formattedDate + formattedAcc + formattedCombo + formattedMods + "</size>";

        INFO("Created Entry Data");
        return GlobalNamespace::LeaderboardTableView::ScoreData::New_ctor(score, result, rank, false);
    }
}