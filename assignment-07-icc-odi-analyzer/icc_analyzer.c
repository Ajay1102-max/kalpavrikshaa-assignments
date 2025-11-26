#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "Players_data.h"

// ---------- Function Prototypes ----------
void showMenu();
void listAllPlayers();
void searchByPlayerId();
void searchByTeam();
void topBatsmenByAverage();
void topBowlersByWickets();
void analyzeAllRounders();

// ---------- Main Function ----------
int main() {
    int userChoice;

    while (1) {
        showMenu();
        printf("\nEnter your choice: ");
        scanf("%d", &userChoice);

        switch (userChoice) {
            case 1: listAllPlayers(); break;
            case 2: searchByPlayerId(); break;
            case 3: searchByTeam(); break;
            case 4: topBatsmenByAverage(); break;
            case 5: topBowlersByWickets(); break;
            case 6: analyzeAllRounders(); break;
            case 0: 
                printf("Exiting analyzer... Thank you!\n");
                return 0;
            default:
                printf("Invalid choice! Please try again.\n");
        }
    }

    return 0;
}

// ---------- Display Menu ----------
void showMenu() {
    printf("\n========= ICC ODI Player Analyzer =========\n");
    printf("1. List All Players\n");
    printf("2. Search Player by ID\n");
    printf("3. Search Players by Team\n");
    printf("4. Top Batsmen by Batting Average\n");
    printf("5. Top Bowlers by Wickets\n");
    printf("6. Best All-Rounders (Custom Ranking)\n");
    printf("0. Exit\n");
}

// ---------- 1. List All Players ----------
void listAllPlayers() {
    printf("\n---- All Players ----\n");
    for (int i = 0; i < playerCount; i++) {
        printf("%d. %s | %s | %s | Runs: %d | Avg: %.2f | SR: %.1f | Wkts: %d | Eco: %.1f\n",
               players[i].id,
               players[i].name,
               players[i].team,
               players[i].role,
               players[i].totalRuns,
               players[i].battingAverage,
               players[i].strikeRate,
               players[i].wickets,
               players[i].economyRate);
    }
}

// ---------- 2. Search Player by ID ----------
void searchByPlayerId() {
    int searchId;
    printf("\nEnter Player ID: ");
    scanf("%d", &searchId);

    printf("\n---- Player Search Result ----\n");
    for (int i = 0; i < playerCount; i++) {
        if (players[i].id == searchId) {
            printf("ID: %d\n", players[i].id);
            printf("Name: %s\n", players[i].name);
            printf("Team: %s\n", players[i].team);
            printf("Role: %s\n", players[i].role);
            printf("Runs: %d | Avg: %.2f | SR: %.1f\n",
                   players[i].totalRuns,
                   players[i].battingAverage,
                   players[i].strikeRate);
            printf("Wickets: %d | Economy: %.1f\n",
                   players[i].wickets,
                   players[i].economyRate);
            return;
        }
    }

    printf("No player found with ID %d\n", searchId);
}

// ---------- 3. Search by Team ----------
void searchByTeam() {
    char teamName[50];
    printf("\nEnter Team Name: ");
    scanf("%s", teamName);

    printf("\n---- Players from %s ----\n", teamName);
    int found = 0;

    for (int i = 0; i < playerCount; i++) {
        if (strcasecmp(players[i].team, teamName) == 0) {
            found = 1;
            printf("%s | %s | Runs: %d | Avg: %.2f | SR: %.1f | Wkts: %d | Eco: %.1f\n",
                   players[i].name,
                   players[i].role,
                   players[i].totalRuns,
                   players[i].battingAverage,
                   players[i].strikeRate,
                   players[i].wickets,
                   players[i].economyRate);
        }
    }

    if (!found) {
        printf("No players found for team: %s\n", teamName);
    }
}

// ---------- 4. Top Batsmen by Batting Average ----------
void topBatsmenByAverage() {
    printf("\n---- Top Batsmen by Batting Average ----\n");

    for (int i = 0; i < playerCount; i++) {
        if (players[i].battingAverage > 40.0 && players[i].strikeRate > 80.0) {
            printf("%s | Avg: %.2f | SR: %.1f\n",
                   players[i].name,
                   players[i].battingAverage,
                   players[i].strikeRate);
        }
    }
}

// ---------- 5. Top Bowlers by Wickets ----------
void topBowlersByWickets() {
    printf("\n---- Top Bowlers (>100 Wickets) ----\n");

    for (int i = 0; i < playerCount; i++) {
        if (players[i].wickets >= 100) {
            printf("%s | Wickets: %d | Economy: %.1f\n",
                   players[i].name,
                   players[i].wickets,
                   players[i].economyRate);
        }
    }
}

// ---------- 6. All-Rounder Ranking ----------
void analyzeAllRounders() {
    printf("\n---- Top All-Rounders ----\n");

    for (int i = 0; i < playerCount; i++) {
        if (strcasecmp(players[i].role, "All-rounder") == 0) {
            float rating = (players[i].totalRuns / 100.0) +
                           (players[i].wickets * 1.5) -
                           (players[i].economyRate);

            printf("%s | Rating: %.2f\n", players[i].name, rating);
        }
    }
}