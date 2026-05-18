#include "AnimationHandler.h"
#include "ClaimLogic.h"

void AnimationHandler(float fps, Uint64& lastFrame, Uint64& lastUpdate) {
    Uint64 current = SDL_GetTicks();
    if (state == PAUSED) {
        for (int i = 0; i < gameObjects.size(); i++) {
            for (int j = 0; j < gameObjects[i].size(); j++) {
                if (gameObjects[i][j]->GetCurrentAnimation() != nullptr) {
                    gameObjects[i][j]->GetCurrentAnimation()->SetLastUpdate(current);
                }
            }
        }
        for (int i = 0; i < diceAnimations.size(); i++) {
            DiceAnimation &da = diceAnimations[i];
            if (da.finished) continue;
            da.lastTime = current;
        }
        return;
    }

    for (int i = 0; i < gameObjects.size(); i++) {
        for (int j = 0; j < gameObjects[i].size(); j++) {
            if (gameObjects[i][j]->GetCurrentAnimation() != nullptr) {
                gameObjects[i][j]->GetCurrentAnimation()->CycleFrame(current);
            }
        }
    }
    
    

    for (int i = 0; i < diceAnimations.size(); i++) {
        DiceAnimation &da = diceAnimations[i];
        if (da.finished) continue;

        da.elapsed += current - da.lastTime;
        da.lastTime = current;

        Uint64 stepReached = da.elapsed / da.stepInterval;

        if (stepReached > (Uint64)da.currentStep) {
            da.currentStep = (int)stepReached;
            if (da.currentStep >= da.totalSteps) {
                da.die->SetTexture(da.finalTexture);
                da.finished = true;
                da.elapsed = 0;
                if (da.revealMovesLeftOnFinish)
                {
                    movesLeftText->SetRendered(true);
                }
            } else {
                int randomFace = rand() % (int)da.faces.size();
                da.die->SetTexture(da.faces[randomFace]);
            }
        }
    }
    UpdateBattleSequence();
}
