#include "Rendering.h"

void RenderScreen(){
    GPU_ClearRGBA( window, 51, 169, 255, 255 );
    GPU_Clear(window);
    //Render texture to screen
    //refreshClaimNotifs();
    //gameObjects[0][0]->RenderGameObject(renderer);
    renderTerrain();
    //renderPixels();
    renderClaimNotifs();
    renderPieces();
    renderUI();
    renderText();
    renderInventory();


    //Update screen
    GPU_Flip(window);

}

void renderObjects(SDL_Renderer* renderer) {
    for (int i = 0; i < gameObjects.size(); i++) {
        for (int j = 0; j < gameObjects[i].size(); j++) {
            if (gameObjects[i][j]->GetRendered()) {
                gameObjects[i][j]->RenderGameObject(window);
            }
        }
    }
}

void renderUI() {
    for (int i = 0; i < uiElements.size(); i++) {
        if (uiElements[i]->GetAssociatedPeak() == nullptr) {
            uiElements[i]->RenderGameObject(window);
        }
    }
}

void renderClaimNotifs() {
    for (int i = 0; i < uiElements.size(); i++) {
        if (uiElements[i]->GetAssociatedPeak() != nullptr) {
            uiElements[i]->RenderGameObject(window);
        }
    }
}

void renderTerrain() {
    for (int i = 0; i < terrain.size(); i++) {
        for (int j = 0; j < terrain[i].size(); j++) {
            terrain[i][j]->RenderGameObject(window, hoveringTerrain);
        }
    }
}

void renderPieces() {
    for (int i = pieces.size() - 1; i >= 0; i--) {
        if (pieces[i]->type != GameObject::ITEM) {
            pieces[i]->RenderGameObject(window);
        }
    }
}

void renderInventory(){
    for (int i = 0; i < currentTurn->inventory.size(); i++) {
        currentTurn->inventory[i]->RenderGameObject(window);
    }
}

void renderText(){
    for(int i = 0; i < text.size(); i++) {
        text[i]->RenderText(window);
    }
}
