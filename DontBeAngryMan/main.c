#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define FIGURE_COUNT 4
#define PLAYER_COUNT 4
#define PLAYER_POSITIONS 14

const int PLAYERS_TYPE[PLAYER_COUNT] = {0,0,0,0}; // 1 - player, 0 - bot

const int LAST_BOARD_POSITION = 4*PLAYER_POSITIONS-1;
const int LAST_TUNNEL_POSITION = 62;

int figure[PLAYER_COUNT][FIGURE_COUNT];

void initFigures()
{
    for(int i=0; i<PLAYER_COUNT; i++)
    {
        for(int j=0; j<FIGURE_COUNT; j++)
        {
            figure[i][j]=-1;
        }
    }
}

int getBoardPos(int player, int pos)
{
    if (pos < 0 || pos > LAST_BOARD_POSITION) return pos;
    int boardPos = pos + PLAYER_POSITIONS * player;
    if (boardPos > LAST_BOARD_POSITION) boardPos -= LAST_BOARD_POSITION + 1;
    return boardPos;
}

int getOtherPlayersFigureCount(int player, int pos)
{
    if (pos<0 || pos>LAST_BOARD_POSITION) return 0;
    int boardPos = getBoardPos(player, pos);
    for(int i=0; i<PLAYER_COUNT; i++)
    {
        if (i==player) continue;
        int count = 0;
        for(int j=0; j<FIGURE_COUNT; j++)
        {
            if (boardPos==getBoardPos(i,figure[i][j])) count++;
        }
        if (count>0) return count;
    }
    return 0;
}

int getAvailFigures(int player, int dice, int availFigure[])
{
    int count = 0;
    for(int i=0; i<FIGURE_COUNT; i++)
    {
        int available = 0;
        int pos = figure[player][i];
        if (pos==-1 && dice==6 && getOtherPlayersFigureCount(player, 0)<2) available = 1;
        else if (pos!=-1 && pos+dice<=LAST_BOARD_POSITION && getOtherPlayersFigureCount(player, pos+dice)<2) available = 1;
        else if (pos+dice>LAST_BOARD_POSITION && pos+dice<=LAST_TUNNEL_POSITION) available = 1;
        if (available) count++;
        availFigure[i] = available;
    }
    return count;
}

void printFigure(int player, int figureIndex, int pos){
    if (pos==-1) printf("%d) Figure is out of the board.\n",figureIndex+1);
    else if (pos<=LAST_BOARD_POSITION) {
        printf("%d) Figure is on position %d, board position %d.\n",figureIndex+1, pos, getBoardPos(player, pos));
    }
    else if (pos<LAST_TUNNEL_POSITION) {
        printf("%d) Figure is on position %d, tunnel position %d.\n",figureIndex+1, pos, LAST_TUNNEL_POSITION-LAST_BOARD_POSITION);
    }
    else printf("%d) Figure is on its final position %d.\n", figureIndex+1, pos);
}

void printAvailFigures(int player, int availFigure[], int count)
{
    if (count==0)
    {
        printf("There is no figure that can be moved.\n");
        return;
    }
    printf("Available figures to move:\n");
    for(int i=0; i<FIGURE_COUNT; i++)
    {
        if (!availFigure[i]) continue;
        printFigure(player, i, figure[player][i]);
    }
}

void printFigures(int player)
{
    printf("Figures:\n");
    for(int i=0; i<FIGURE_COUNT; i++)
    {
        printFigure(player, i, figure[player][i]);
    }
}

int getOtherPlayersFigure(int player, int pos, int *otherPlayer, int *figureIndex)
{
    if (pos<0 || pos>LAST_BOARD_POSITION) return 0;
    int boardPos = getBoardPos(player, pos);
    for(int i=0; i<PLAYER_COUNT; i++)
    {
        if (i==player) continue;
        for(int j=0; j<FIGURE_COUNT; j++)
        {
            if (boardPos==getBoardPos(i,figure[i][j]))
            {
                *otherPlayer = i;
                *figureIndex = j;
                return 1;
            }
        }
    }
    return 0;
}

int getFirstMoveFigureIndex(int availFigure[]){
    for(int j=0; j<FIGURE_COUNT; j++) {
        if (availFigure[j]) return j;
    }
    return -1;
}

int getBotMoveFigureIndex(int player, int dice, int availFigure[], int count)
{
    if (count==1) return getFirstMoveFigureIndex(availFigure);
    for(int i = 0; i < FIGURE_COUNT; i++)
    {
        if (!availFigure[i]) continue;
        if (getOtherPlayersFigureCount(player,figure[player][i]+dice)) return i;
    }
    int moveIndex = rand() % count;
    for(int i = 0; i < FIGURE_COUNT; i++)
    {
        if (!availFigure[i]) continue;
        if (moveIndex==0) return i;
        moveIndex--;
    }
    return -1;
}

int getHumanMoveFigureIndex(int availFigure[], int count)
{
    if (count==1) return getFirstMoveFigureIndex(availFigure);
    int index;
    do
    {
        printf("Choose a figure: ");
        scanf("%d",&index);
        index--;
    }
    while(index<0 || index>=FIGURE_COUNT || !availFigure[index]);
    return index;
}

void playerMove(int player, int isHuman)
{
    int dice;
    do
    {
        dice=rand() % 6 + 1;
        printf("Player %d, dice %d.\n", player+1,dice);
        int availFigure[FIGURE_COUNT];
        int count = getAvailFigures(player, dice, availFigure);
        printAvailFigures(player, availFigure, count);
        if (count==0)
        {
            printFigures(player);
            if (dice==6) continue;
            else break;
        }
        int index;
        if (isHuman) index=getHumanMoveFigureIndex(availFigure, count);
        else index=getBotMoveFigureIndex(player,dice,availFigure, count);
        printf("Moving figure %d.\n",index+1);
        int *pos = &figure[player][index];
        if (*pos==-1) *pos=0;
        else *pos+=dice;
        printFigures(player);
        int otherPlayer, figureIndex;
        if (getOtherPlayersFigure(player, *pos, &otherPlayer, &figureIndex))
        {
            int oldBoardPos =  getBoardPos(otherPlayer,figure[otherPlayer][figureIndex]);
            figure[otherPlayer][figureIndex]=-1;
            printf("Player %d has figure on board position %d, moving outside the board.\n",otherPlayer + 1, oldBoardPos);
        }
    }
    while(dice==6);
}

int isPlayerFinished(int player){
    for(int j=0; j<FIGURE_COUNT; j++){
        if (figure[player][j]!=LAST_TUNNEL_POSITION) return 0;
    }
    return 1;
}

void printWinners(int winners[]){
    printf("\nGame result:\n");
    for(int i=0; i<PLAYER_COUNT; i++) {
        printf("Player %d finished on %d place.\n", i+1, winners[i]);
    }
}

int main()
{
    srand(time(NULL));
    int winners[PLAYER_COUNT];
    for(int i=0; i<PLAYER_COUNT; i++) winners[i]=0;
    initFigures();
    int count = 1;
    while(count<PLAYER_COUNT)
    {
        for (int i=0; i<PLAYER_COUNT; i++)
        {
            if (winners[i]) {
                printf("Player %d finished on %d place.\n", i+1, winners[i]);
                printf("\n");
                continue;
            }
            playerMove(i,PLAYERS_TYPE[i]);
            if (isPlayerFinished(i)){
                winners[i]=count;
                printf("Player %d finished on %d place.\n", i+1, count);
                count++;
                if (count==PLAYER_COUNT) break;
            }
            printf("\n");
        }
    }
    for(int j=0; j<PLAYER_COUNT; j++) {
        if (winners[j]==0) {
            winners[j]=count;
            break;
        }
    }
    printWinners(winners);
}
