#include "AnimationHandler.h"

void AnimationHandler(float fps, Uint64& lastFrame, Uint64& lastUpdate) {
    Uint64 current = SDL_GetTicks();
    for (int i = 0; i < gameObjects.size(); i++) {
        for (int j = 0; j < gameObjects[i].size(); j++) {
            if (gameObjects[i][j]->GetCurrentAnimation() != nullptr) {
                gameObjects[i][j]->GetCurrentAnimation()->CycleFrame(current);
            }

        }
    }
}