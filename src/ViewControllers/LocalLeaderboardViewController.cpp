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
#include <algorithm>
#include <thread>
#include "main.hpp"

DEFINE_TYPE(LocalLeaderboard::UI::ViewControllers, LocalLeaderboardViewController);

using namespace QuestUI;
using namespace QuestUI::BeatSaberUI;
using namespace HMUI;
using namespace UnityEngine;
using namespace UnityEngine::UI;
using namespace BSML;
using namespace LocalLeaderboard::Models;
using namespace LocalLeaderboard::UI::ViewControllers;
using namespace GlobalNamespace;

namespace LocalLeaderboard::UI::ViewControllers
{
    void LocalLeaderboardViewController::DidActivate(bool firstActivation, bool addedToHierarchy, bool screenSystemEnabling)
    {
        if (firstActivation)
        {
            parse_and_construct(IncludedAssets::LocalLeaderboardViewController_bsml, this->get_transform(), this);
            RefreshLeaderboard(currentDifficultyBeatmap);
        }
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
    }

    void LocalLeaderboardViewController::OnPageUp()
    {
        page--;
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
        std::sort(leaderboardEntries.begin(), leaderboardEntries.end(), [](auto &first, auto &second)
                  { return first.acc > second.acc; });
        getLogger().info("leaderboard size: %lu", leaderboardEntries.size());
        leaderboardTableView->SetScores(CreateLeaderboardData(leaderboardEntries, page), -1);
        getLogger().info("Scores Set");
        RichMyText(leaderboardTableView);
        if (leaderboardEntries.size() == 0)
        {
            errorText->get_gameObject()->set_active(true);
        }
    }

    List<LeaderboardTableView::ScoreData *> *LocalLeaderboardViewController::CreateLeaderboardData(std::vector<LeaderboardEntry> leaderboard, int page)
    {
        getLogger().info("Creating Leaderboard Data");
        auto tableData = List<LeaderboardTableView::ScoreData *>::New_ctor();
        int pageIndex = page * 10;
        for (int i = pageIndex; i < leaderboard.size() && i < pageIndex + 10; i++)
        {
            getLogger().info("creating leaderboard entry number %i", i);
            tableData->Add(CreateLeaderboardEntryData(leaderboard[i], i + 1));
        }
        getLogger().info("Created Leaderboard Data");
        return tableData;
    }

    GlobalNamespace::LeaderboardTableView::ScoreData *LocalLeaderboardViewController::CreateLeaderboardEntryData(Models::LeaderboardEntry entry, int rank)
    {
        getLogger().info("Creating Entry Data");

        std::string formattedDate = string_format("<color=#00FFAE>%s</color></size>", entry.datePlayed.c_str());
        std::string formattedAcc = string_format(" - (<color=#ffd42a>%.2f%%</color>)", entry.acc);
        std::string formattedCombo = "";
        if (entry.fullCombo)
        {
            formattedCombo = " - FC";
        }
        else
        {
            formattedCombo = string_format(" - <color=red>x %i</color></size>", entry.badCutCount + entry.missCount).c_str();
        }
        std::string result = "<size=100%>" + formattedDate + formattedAcc + formattedCombo + "</size>";

        // uh oh this could cause a lot of problems given the format we want
        getLogger().info("Created Entry Data");
        return GlobalNamespace::LeaderboardTableView::ScoreData::New_ctor((int)entry.acc, result, rank, false);
    }
}
