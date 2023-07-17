#include "GameLogic.h"

int Roll(){
    int rollOne = (rand() % 6) + 1;
    string rollOneStr = "die " + to_string(rollOne);
    int rollTwo = (rand() % 6) + 1;
    string rollTwoStr = "die " + to_string(rollTwo);
    for (int i = 0; i < uiElements.size(); i++) {
        if (uiElements[i]->GetName() == "dieOne") {
            uiElements[i]->SetTexture(textures[rollOneStr][0]);
        } else if (uiElements[i]->GetName() == "dieTwo") {
            uiElements[i]->SetTexture(textures[rollTwoStr][0]);
        }
    }
    movesLeft = rollOne + rollTwo;

    UpdateMovesLeft();

    return movesLeft;
}

void RotateTurn() {

    currentTurn->GetTurnText()->SetRendered(false);


    for (int i = 0; i < currentTurn->soldiers.size(); i++) {
        currentTurn->soldiers[i]->SetSelectable(false);
    }

    for (int i = 0; i < players.size(); i++) {
        if (players[i] == currentTurn) {
            if (i == players.size() - 1) {
                currentTurn = players[0];
            } else {
                currentTurn = players[i + 1];
            }
            break;
        }
    }

    for (int i = 0; i < currentTurn->soldiers.size(); i++) {
        currentTurn->soldiers[i]->SetSelectable(true);
    }

    currentTurn->GetTurnText()->SetRendered(true);



}

void UpdateMovesLeft() {
    string movesLeftStr = "moves left " + to_string(movesLeft);

    for (int i = 0; i < uiElements.size(); i++) {
        if (uiElements[i]->GetName() == "movesLeftCount") {
            uiElements[i]->SetTexture(textures[movesLeftStr][0]);
        }
    }
}

void FinishTurn() {
    RotateTurn();
    movesLeft = 0;
    UpdateMovesLeft();
    RefreshClaimNotifs();
}

void UpdateScore() {

    int peaksLeft = peaks.size();
    for (int i = 0; i < players.size(); i++) {
        for (int j = 0; j < players[i]->peaks.size(); j++) {
            peaksLeft--;
            players[i]->SetScore(players[i]->GetScore() + players[i]->peaks[j]->GetLayer());
        }
    }
    /*if (peaksLeft == 0) {
        if (playerOneScore > playerTwoScore && playerOneScore > playerThreeScore && playerOneScore > playerFourScore) {
            GameFinished("playerOne");
        } else if (playerTwoScore > playerOneScore && playerTwoScore > playerThreeScore && playerTwoScore > playerFourScore) {
            GameFinished("playerTwo");
        } else if (playerThreeScore > playerOneScore && playerThreeScore > playerTwoScore && playerThreeScore > playerFourScore) {
            GameFinished("playerThree");
        } else if (playerFourScore > playerOneScore && playerFourScore > playerTwoScore && playerFourScore > playerThreeScore) {
            GameFinished("playerFour");
        } else {
            Tiebreaker();
        }

    }*/
    for (int i = 0; i < players.size(); i++) {
        std::cout << "Player " << i + 1 << ": " << players[i]->GetScore() << endl;
    }


}

void GameFinished(Player* winner) {
    string winnerColor;
    if (winner->GetName() == "playerOne") {
        winnerColor = "winnerRed";
    } else if (winner->GetName() == "playerTwo") {
        winnerColor = "winnerGreen";
    } else if (winner->GetName() == "playerThree") {
        winnerColor = "winnerBlue";
    } else if (winner->GetName() == "playerFour") {
        winnerColor = "winnerYellow";
    }

    UIElement* winnerMessage = new UIElement("winner message", textures[winnerColor][0], surfaces[winnerColor], true);
    winnerMessage->SetCenter(SCREEN_WIDTH/2, SCREEN_HEIGHT/2);
    uiElements.push_back(winnerMessage);
    gameObjects[gameObjects.size()-1].push_back(winnerMessage);
}

void Tiebreaker() {

}
