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
int totalPages;

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
LocalLeaderboard::UI::ViewControllers::LocalLeaderboardPanel *Panel;
LocalLeaderboard::UI::ViewControllers::LocalLeaderboardViewController *View;
namespace LocalLeaderboard::UI::ViewControllers
{
    PlatformLeaderboardViewController *originalplvc = NULL;
    void LocalLeaderboardViewController::DidActivate(bool firstActivation, bool addedToHierarchy, bool screenSystemEnabling)
    {
        if (firstActivation)
        {
            if(!originalplvc || !originalplvc->m_CachedPtr.m_value) return;
            parse_and_construct(IncludedAssets::LocalLeaderboardViewController_bsml, this->get_transform(), this);
            Panel = UnityEngine::Resources::FindObjectsOfTypeAll<LocalLeaderboard::UI::ViewControllers::LocalLeaderboardPanel *>().FirstOrDefault();
            View = UnityEngine::Resources::FindObjectsOfTypeAll<LocalLeaderboard::UI::ViewControllers::LocalLeaderboardViewController *>().FirstOrDefault();
            RefreshLeaderboard(currentDifficultyBeatmap);
        }
        originalplvc = UnityEngine::Resources::FindObjectsOfTypeAll<PlatformLeaderboardViewController *>().FirstOrDefault();

        if (originalplvc->get_transform()->Find("HeaderPanel"))
        {
            getLogger().info("found header panel ACTIVATE");
            TMPro::TextMeshProUGUI *headertext = originalplvc->get_gameObject()->GetComponentInChildren<TMPro::TextMeshProUGUI *>();
            if (headertext)
            {
                getLogger().info("found header text ACTIVATE");
                headertext->SetText("Local Leaderboard");
                getLogger().info("set header text ACTIVATE");
            }
        }
    }

    void LocalLeaderboardViewController::DidDeactivate(bool removedFromHierarchy, bool screenSystemDisabling)
    {
        if (originalplvc->get_transform()->Find("HeaderPanel"))
        {
            getLogger().info("found header panel DEACTIVATE");
            TMPro::TextMeshProUGUI *headertext = originalplvc->get_gameObject()->GetComponentInChildren<TMPro::TextMeshProUGUI *>();
            if (headertext)
            {
                getLogger().info("found header text DEACTIVATE");
                headertext->SetText("HIGHSCORES");
                getLogger().info("set header text DEACTIVATE");
            }
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
        if (leaderboardEntries.size() > 0)
        {
            LeaderboardEntry recent = leaderboardEntries[leaderboardEntries.size() - 1];
            Panel->lastPlayed->SetText("Last Played: " + recent.datePlayed);
        }
        std::sort(leaderboardEntries.begin(), leaderboardEntries.end(), [](auto &first, auto &second)
                  { return first.acc > second.acc; });
        getLogger().info("leaderboard size: %lu", leaderboardEntries.size());
        totalPages = leaderboardEntries.size() / 10;
        leaderboardTableView->SetScores(CreateLeaderboardData(leaderboardEntries, page), -1);
        getLogger().info("Scores Set");
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
            errorText->set_fontSize(6);
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
        getLogger().info("Creating Leaderboard Data");
        auto tableData = List<LeaderboardTableView::ScoreData *>::New_ctor();
        int pageIndex = page * 10;
        for (int i = pageIndex; i < leaderboard.size() && i < pageIndex + 10; i++)
        {
            int score = leaderboard.data()->score;
            getLogger().info("creating leaderboard entry number %i", i);
            tableData->Add(CreateLeaderboardEntryData(leaderboard[i], i + 1, score));
        }
        getLogger().info("Created Leaderboard Data");
        return tableData;
    }

    GlobalNamespace::LeaderboardTableView::ScoreData *LocalLeaderboardViewController::CreateLeaderboardEntryData(Models::LeaderboardEntry entry, int rank, int score)
    {
        getLogger().info("Creating Entry Data");

        std::string formattedDate = string_format("<color=#28b077>%s</color></size>", entry.datePlayed.c_str());
        std::string formattedAcc = string_format(" - (<color=#ffd42a>%.2f%%</color>)", entry.acc);
        score = entry.score;
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
        return GlobalNamespace::LeaderboardTableView::ScoreData::New_ctor(score, result, rank, false);
    }
}
