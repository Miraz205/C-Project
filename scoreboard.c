#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAX 11

struct Player {
    char name[50];
    int runs;
    int balls;
    int out;
};

struct Bowler {
    char name[50];
    int runsGiven;
    int wickets;
    int balls;
};

struct Team {
    char name[50];
    struct Player players[MAX];
    struct Bowler bowlers[MAX];
    int totalRuns;
    int wickets;
};

void inputTeam(struct Team *t) {
    printf("Enter Team Name: ");
    scanf("%s", t->name);

    for (int i = 0; i < MAX; i++) {
        printf("Enter Player %d Name: ", i + 1);
        scanf("%s", t->players[i].name);

        strcpy(t->bowlers[i].name, t->players[i].name);

        t->players[i].runs = 0;
        t->players[i].balls = 0;
        t->players[i].out = 0;

        t->bowlers[i].runsGiven = 0;
        t->bowlers[i].wickets = 0;
        t->bowlers[i].balls = 0;
    }

    t->totalRuns = 0;
    t->wickets = 0;
}

void displayScore(struct Team t, int striker, int nonStriker, int over, int ball, int target) {
    printf("\n============================\n");
    printf("Team: %s\n", t.name);
    printf("Score: %d/%d\n", t.totalRuns, t.wickets);
    printf("Overs: %d.%d\n", over, ball);

    if (target > 0)
        printf("Target: %d\n", target);

    printf("----------------------------\n");

    printf("Striker: %s (%d/%d)\n",
           t.players[striker].name,
           t.players[striker].runs,
           t.players[striker].balls);

    printf("Non-Striker: %s (%d/%d)\n",
           t.players[nonStriker].name,
           t.players[nonStriker].runs,
           t.players[nonStriker].balls);

    printf("============================\n");
}

void displayBowler(struct Bowler b) {
    printf("Bowler: %s | Runs: %d | Wickets: %d | Overs: %d.%d\n",
           b.name,
           b.runsGiven,
           b.wickets,
           b.balls / 6,
           b.balls % 6);
}

void saveMatch(struct Team t1, struct Team t2, int target) {
    FILE *fp = fopen("match.txt", "w");

    fprintf(fp, "Match Summary\n");
    fprintf(fp, "%s: %d/%d\n", t1.name, t1.totalRuns, t1.wickets);
    fprintf(fp, "%s: %d/%d\n", t2.name, t2.totalRuns, t2.wickets);

    if (t2.totalRuns >= target)
        fprintf(fp, "%s wins!\n", t2.name);
    else
        fprintf(fp, "%s wins!\n", t1.name);

    fclose(fp);
    printf("\nSaved to match.txt\n");
}

int playInnings(struct Team *bat, struct Team *bowl, int maxOvers, int target) {
    int striker = 0, nonStriker = 1, nextPlayer = 2;
    int over = 0, ball = 0;
    int bowlerIndex = 0;
    char input[10];

    while (over < maxOvers && bat->wickets < 10) {

        displayScore(*bat, striker, nonStriker, over, ball, target);
        displayBowler(bowl->bowlers[bowlerIndex]);

        printf("Enter run (0-6) or W: ");
        scanf("%s", input);

        if (strcmp(input, "W") == 0 || strcmp(input, "w") == 0) {
            bat->wickets++;
            bat->players[striker].out = 1;
            bat->players[striker].balls++;

            bowl->bowlers[bowlerIndex].wickets++;
            bowl->bowlers[bowlerIndex].balls++;

            striker = nextPlayer++;
        } else {
            int run = atoi(input);

            bat->totalRuns += run;
            bat->players[striker].runs += run;
            bat->players[striker].balls++;

            bowl->bowlers[bowlerIndex].runsGiven += run;
            bowl->bowlers[bowlerIndex].balls++;

            if (run % 2 != 0) {
                int temp = striker;
                striker = nonStriker;
                nonStriker = temp;
            }
        }

        if (target > 0 && bat->totalRuns >= target) {
            printf("\nTarget achieved!\n");
            return bat->totalRuns;
        }

        ball++;

        if (ball == 6) {
            over++;
            ball = 0;

            int temp = striker;
            striker = nonStriker;
            nonStriker = temp;

            bowlerIndex = (bowlerIndex + 1) % MAX;
        }
    }

    return bat->totalRuns;
}

void startMatch() {
    struct Team t1, t2;
    int overs;

    printf("Enter number of overs: ");
    scanf("%d", &overs);

    printf("\n--- Team 1 ---\n");
    inputTeam(&t1);

    printf("\n--- Team 2 ---\n");
    inputTeam(&t2);

    int choice;
    printf("\nWhich team will bat first?\n");
    printf("1. %s\n", t1.name);
    printf("2. %s\n", t2.name);
    printf("Enter choice: ");
    scanf("%d", &choice);

    int score1, score2, target;

    if (choice == 1) {
        // Team 1 bats first
        printf("\n%s Batting...\n", t1.name);
        score1 = playInnings(&t1, &t2, overs, 0);

        target = score1 + 1;
        printf("\nTarget for %s: %d\n", t2.name, target);

        printf("\n%s Batting...\n", t2.name);
        score2 = playInnings(&t2, &t1, overs, target);

        if (score2 >= target)
            printf("\n%s Wins!\n", t2.name);
        else
            printf("\n%s Wins!\n", t1.name);
    }
    else if (choice == 2) {
        // Team 2 bats first
        printf("\n%s Batting...\n", t2.name);
        score1 = playInnings(&t2, &t1, overs, 0);

        target = score1 + 1;
        printf("\nTarget for %s: %d\n", t1.name, target);

        printf("\n%s Batting...\n", t1.name);
        score2 = playInnings(&t1, &t2, overs, target);

        if (score2 >= target)
            printf("\n%s Wins!\n", t1.name);
        else
            printf("\n%s Wins!\n", t2.name);
    }
    else {
        printf("Invalid choice!\n");
        return;
    }

    saveMatch(t1, t2, target);
}

int main() {
    int choice;

    while (1) {
        printf("\n===== MENU =====\n");
        printf("1. Start Match\n");
        printf("2. Exit\n");
        printf("Enter choice: ");
        scanf("%d", &choice);

        if (choice == 1)
            startMatch();
        else if (choice == 2)
            break;
        else
            printf("Invalid choice!\n");
    }

    return 0;
}
