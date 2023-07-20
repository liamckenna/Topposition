#include "TerrainGeneration.h"


void GeneratePeak() {
    //int x = (rand() % (int)(SCREEN_WIDTH/1.2)) + SCREEN_WIDTH/2 - SCREEN_WIDTH/2.4;
    int x = (rand() % SCREEN_WIDTH) * 2;
    //int y = (rand() % (int)(SCREEN_HEIGHT/1.2)) + SCREEN_HEIGHT/2 - SCREEN_HEIGHT/2.4;
    int y = (rand() % SCREEN_HEIGHT) * 2;

    int shape = rand() % (int)(shapeCount);
    int height = (rand() % rules->GetMaxHeight()) + 1;
    if (rules->GetRemainingPoints() <= rules->GetMaxHeight()) height = rules->GetRemainingPoints();
    rules->SetRemainingPoints(rules->GetRemainingPoints() - height);
    string name = to_string(height) + ", " + to_string(peaks.size() + 1);
    Peak* peak = new Peak(name, textures[to_string(shape)][height], surfaces[to_string(shape)], false, true, height, renderer);
    double rotation = rand() % 360;
    peak->SetRotation(rotation);
    peak->SetScale(0.1);
    peak->SetScale(peak->GetScale() * 2);
    peak->SetCenter(x, y);
    peaks.push_back(peak);
    peak->SetPeakID(peaks.size());
    terrain[height].push_back(peak);
    gameObjects[height].push_back(peak);

    string outlineName = "outline " + to_string(shape);
    GameObject* outline = new GameObject(outlineName, textures[outlineName][height], surfaces[outlineName], false, true);
    outline->SetScale(0.1);
    outline->SetCenter(x,y);
    terrainOutlines[height].push_back(outline);
    gameObjects[height].push_back(outline);
    peak->SetOutline(outline);

    int itemChance = (rand() % 100) + 1;
    float odds = (float)height / (float)rules->GetMaxHeight() * 100;
    if (odds > itemChance) {
        if (rules->GetRemainingItems() > 0) {
            rules->SetRemainingItems(rules->GetRemainingItems() - 1);
            int itemID = rand() % 3;
            string itemName;
            if (itemID == 0) itemName = "playerOne zipline";
            else if (itemID == 1) itemName = "parachute";
            else if (itemID == 2) itemName = "buddy rope";
            Item* item = new Item(itemName, textures[itemName][0], surfaces[itemName], true);
            pieces.push_back(item);
            item->SetScale(0.05);
            item->SetCenter(peak->GetCenter().first, peak->GetCenter().second);
            gameObjects[height].push_back(item);
            peak->SetItem(item);
        }
    }

    while (MergeTerrain(peak).size() != 0) {
        //int a = ((rand() % (int)(SCREEN_WIDTH/1.2)) + SCREEN_WIDTH/2 - SCREEN_WIDTH/2.4) * 2;
        int a = (rand() % SCREEN_WIDTH) * 2;
        //int b = ((rand() % (int)(SCREEN_HEIGHT/1.2)) + SCREEN_HEIGHT/2 - SCREEN_HEIGHT/2.4) * 2;
        int b = (rand() % SCREEN_HEIGHT) * 2;

        peak->SetCenter(a, b);
        outline->SetCenter(a, b);
        if (peak->GetItem() != nullptr) {
            peak->GetItem()->SetCenter(a, b);
        }
    }
    SDL_Color color;
    if (height == 1) {
        SDL_SetTextureColorMod(peak->GetTexture(), 246, 215, 176);
        color = {246, 215, 176};
    } else {
        SDL_SetTextureColorMod(peak->GetTexture(), 200/rules->GetMaxHeight() * peak->GetLayer(), std::min(200/rules->GetMaxHeight() * peak->GetLayer() * 2, 255), 200/rules->GetMaxHeight() * peak->GetLayer());
        color = {static_cast<Uint8>(200/rules->GetMaxHeight() * peak->GetLayer()), static_cast<Uint8>(std::min(200/rules->GetMaxHeight() * peak->GetLayer() * 2, 255)), static_cast<Uint8>(200/rules->GetMaxHeight() * peak->GetLayer())};
    }
    peak->SetColor(color);
    GenerateTerrain(peak, shape, height);
}

void GenerateTerrain(Peak* peak, int shape, int height) {
    for (int i = height - 1; i > 0; i--) {
        string name = to_string(i) + ", " + to_string(peak->GetPeakID());
        //shape = rand() % (int)(15);


        Terrain* layer = new Terrain(name, textures[to_string(shape)][i], surfaces[to_string(shape)], false, true, i, renderer);
        layer->SetScale((peak->GetScale() + 0.1 * (height - i)) * 2);
        layer->SetRotation(peak->GetRotation());
        layer->SetPeak(peak);


        Terrain* above;
        if (peak->childTerrain.size() == 0) {
            above = peak;
        } else {
            above = peak->childTerrain[peak->childTerrain.size() - 1];
        }
        int offsetX = rand() % 20;
        int offsetY = rand() % 20;
        int negativeX = rand() % 2;
        int negativeY = rand() % 2;
        if (negativeX == 1) offsetX *= -1;
        if (negativeY == 1) offsetY *= -1;
        layer->SetCenter(above->GetCenter().first + offsetX, above->GetCenter().second + offsetY);
        while (!TerrainIsSurrounded(above, layer)) {
            offsetX = rand() % 20;
            offsetY = rand() % 20;
            negativeX = rand() % 2;
            negativeY = rand() % 2;
            if (negativeX == 1) offsetX *= -1;
            if (negativeY == 1) offsetY *= -1;
            layer->SetCenter(above->GetCenter().first + offsetX, above->GetCenter().second + offsetY);
        }
        layer->SetOffsetX(offsetX);
        layer->SetOffsetY(offsetY);
        SDL_Color color;
        if (i == 1) {
            SDL_SetTextureColorMod(layer->GetTexture(), 246, 215, 176);
            color = {246, 215, 176};

        } else {
            SDL_SetTextureColorMod(layer->GetTexture(), 200/rules->GetMaxHeight() * layer->GetLayer(), std::min(200/rules->GetMaxHeight() * layer->GetLayer() * 2, 255), 200/rules->GetMaxHeight() * layer->GetLayer());
            color = {static_cast<Uint8>(200/rules->GetMaxHeight() * layer->GetLayer()), static_cast<Uint8>(std::min(200/rules->GetMaxHeight() * layer->GetLayer() * 2, 255)), static_cast<Uint8>(200/rules->GetMaxHeight() * layer->GetLayer())};
        }
        layer->SetColor(color);
        gameObjects[i].push_back(layer);
        terrain[i].push_back(layer);
        peak->childTerrain.push_back(layer);

        string outlineName = "outline " + to_string(shape);
        GameObject* outline = new GameObject(outlineName, textures[outlineName][i], surfaces[outlineName], false, true);
        outline->SetScale(peak->GetScale() + 0.1 * (height - i));
        outline->SetCenter(peak->GetCenter().first,peak->GetCenter().second);
        terrainOutlines[i].push_back(outline);
        gameObjects[i].push_back(outline);
        layer->SetOutline(outline);

    }
}

std::vector<Terrain*> MergeTerrain(Terrain* peak) {
    std::vector<Terrain*> connectedTerrain;
    for (int k = 0; k < terrain[peak->GetLayer()].size(); k++) {
        Terrain* other = terrain[peak->GetLayer()][k];
        if (peak == other) continue;
        bool intersecting = false;
        bool topLeftOne = false;
        bool topRightOne = false;
        bool bottomLeftOne = false;
        bool bottomRightOne = false;
        bool topLeftTwo = false;
        bool topRightTwo = false;
        bool bottomLeftTwo = false;
        bool bottomRightTwo = false;
        pair<int, int> offsetOne;
        pair<int, int> offsetTwo;
        int length = 0;
        int height = 0;
        SDL_Color pixelOne;
        SDL_Color pixelTwo;

        //top left
        if (peak->GetPosition().first > other->GetPosition().first &&
            peak->GetPosition().first < other->GetBottomRight().first &&
            peak->GetPosition().second > other->GetPosition().second &&
            peak->GetPosition().second < other->GetBottomRight().second) {

            topLeftOne = true;


        } if (peak->GetBottomRight().first > other->GetPosition().first &&
              peak->GetBottomRight().first < other->GetBottomRight().first &&
              peak->GetPosition().second > other->GetPosition().second &&
              peak->GetPosition().second < other->GetBottomRight().second) {

            topRightOne = true;


        }if (peak->GetPosition().first > other->GetPosition().first &&
             peak->GetPosition().first < other->GetBottomRight().first &&
             peak->GetBottomRight().second > other->GetPosition().second &&
             peak->GetBottomRight().second < other->GetBottomRight().second) {

            bottomLeftOne = true;

        } if (peak->GetBottomRight().first > other->GetPosition().first &&
              peak->GetBottomRight().first < other->GetBottomRight().first &&
              peak->GetBottomRight().second > other->GetPosition().second &&
              peak->GetBottomRight().second < other->GetBottomRight().second) {

            bottomRightOne = true;
        }


        if (other->GetPosition().first > peak->GetPosition().first &&
            other->GetPosition().first < peak->GetBottomRight().first &&
            other->GetPosition().second > peak->GetPosition().second &&
            other->GetPosition().second < peak->GetBottomRight().second) {

            topLeftTwo = true;


        } if (other->GetBottomRight().first > peak->GetPosition().first &&
              other->GetBottomRight().first < peak->GetBottomRight().first &&
              other->GetPosition().second > peak->GetPosition().second &&
              other->GetPosition().second < peak->GetBottomRight().second) {

            topRightTwo = true;

        }if (other->GetPosition().first > peak->GetPosition().first &&
             other->GetPosition().first < peak->GetBottomRight().first &&
             other->GetBottomRight().second > peak->GetPosition().second &&
             other->GetBottomRight().second < peak->GetBottomRight().second) {

            bottomLeftTwo = true;

        } if (other->GetBottomRight().first > peak->GetPosition().first &&
              other->GetBottomRight().first < peak->GetBottomRight().first &&
              other->GetBottomRight().second > peak->GetPosition().second &&
              other->GetBottomRight().second < peak->GetBottomRight().second) {

            bottomRightTwo = true;
        }

        if (!topLeftOne && !topRightOne && !bottomLeftOne && !bottomRightOne && !topLeftTwo && !topRightTwo && !bottomLeftTwo && !bottomRightTwo) {
            continue;
        } else if (topLeftOne && !topRightOne && bottomLeftOne && !bottomRightOne) {
            //std::cout << "One" << std::endl;
            length = other->GetBottomRight().first - peak->GetPosition().first;
            height = peak->GetBottomRight().second - peak->GetPosition().second;
            offsetOne.first = 0;
            offsetOne.second = 0;
            offsetTwo.first = peak->GetPosition().first - other->GetPosition().first;
            offsetTwo.second = peak->GetPosition().second - other->GetPosition().second;

        } else if (!topLeftOne && !topRightOne && bottomLeftOne && bottomRightOne) {
            //std::cout << "Two" << std::endl;
            length = peak->GetBottomRight().first - peak->GetPosition().first;
            height = peak->GetBottomRight().second - other->GetPosition().second;
            offsetOne.first = 0;
            offsetOne.second = other->GetPosition().second - peak->GetPosition().second;
            offsetTwo.first = peak->GetPosition().first - other->GetPosition().first;
            offsetTwo.second = 0;

        } else if (!topLeftOne && topRightOne && !bottomLeftOne && bottomRightOne) {
            //std::cout << "Three" << std::endl;
            length = peak->GetBottomRight().first - other->GetPosition().first;
            height = peak->GetBottomRight().second - peak->GetPosition().second;
            offsetOne.first = other->GetPosition().first - peak->GetPosition().first;
            offsetOne.second = 0;
            offsetTwo.first = 0;
            offsetTwo.second = peak->GetPosition().second - other->GetPosition().second;

        } else if (topLeftOne && topRightOne && !bottomLeftOne && !bottomRightOne) {
            //std::cout << "Four" << std::endl;
            length = peak->GetBottomRight().first - peak->GetPosition().first;
            height = other->GetBottomRight().second - peak->GetPosition().second;
            offsetOne.first = 0;
            offsetOne.second = 0;
            offsetTwo.first = peak->GetPosition().first - other->GetPosition().first;
            offsetTwo.second = peak->GetPosition().second - other->GetPosition().second;

        } else if (topLeftOne && !topRightOne && !bottomLeftOne && !bottomRightOne) {
            //std::cout << "Five" << std::endl;
            length = other->GetBottomRight().first - peak->GetPosition().first;
            height = other->GetBottomRight().second - peak->GetPosition().second;
            offsetOne.first = 0;
            offsetOne.second = 0;
            offsetTwo.first = peak->GetPosition().first - other->GetPosition().first;
            offsetTwo.second = peak->GetPosition().second - other->GetPosition().second;

        } else if (!topLeftOne && topRightOne && !bottomLeftOne && !bottomRightOne) {
            //std::cout << "Six" << std::endl;
            length = peak->GetBottomRight().first - other->GetPosition().first;
            height = other->GetBottomRight().second - peak->GetPosition().second;
            offsetOne.first = other->GetPosition().first - peak->GetPosition().first;
            offsetOne.second = 0;
            offsetTwo.first = 0;
            offsetTwo.second = peak->GetPosition().second - other->GetPosition().second;

        } else if (!topLeftOne && !topRightOne && bottomLeftOne && !bottomRightOne) {
            //std::cout << "Seven" << std::endl;
            length = other->GetBottomRight().first - peak->GetPosition().first;
            height = peak->GetBottomRight().second - other->GetPosition().second;
            offsetOne.first = 0;
            offsetOne.second = other->GetPosition().second - peak->GetPosition().second;
            offsetTwo.first = peak->GetPosition().first - other->GetPosition().first;
            offsetTwo.second = 0;

        } else if (!topLeftOne && !topRightOne && !bottomLeftOne && bottomRightOne) {
            //std::cout << "Eight" << std::endl;
            length = peak->GetBottomRight().first - other->GetPosition().first;
            height = peak->GetBottomRight().second - other->GetPosition().second;
            offsetOne.first = other->GetPosition().first - peak->GetPosition().first;
            offsetOne.second = other->GetPosition().second - peak->GetPosition().second;
            offsetTwo.first = 0;
            offsetTwo.second = 0;


        } else if (topLeftOne && topRightOne && bottomLeftOne && bottomRightOne) {
            //std::cout << "Nine" << std::endl;
            length = peak->GetBottomRight().first - peak->GetPosition().first;
            height = peak->GetBottomRight().second - peak->GetPosition().second;
            offsetOne.first = 0;
            offsetOne.second = 0;
            offsetTwo.first = peak->GetPosition().first - other->GetPosition().first;
            offsetTwo.second = peak->GetPosition().second - other->GetPosition().second;


        } else if (topLeftTwo && topRightTwo && bottomLeftTwo && bottomRightTwo) {
            //std::cout << "Ten" << std::endl;
            length = other->GetBottomRight().first - other->GetPosition().first;
            height = other->GetBottomRight().second - other->GetPosition().second;
            offsetOne.first = other->GetPosition().first - peak->GetPosition().first;
            offsetOne.second = other->GetPosition().second - peak->GetPosition().second;
            offsetTwo.first = 0;
            offsetTwo.second = 0;

        } else if (topLeftTwo && !topRightTwo && bottomLeftTwo && !bottomRightTwo) {
            //std::cout << "Eleven" << std::endl;
            length = peak->GetBottomRight().first - other->GetPosition().first;
            height = other->GetBottomRight().second - other->GetPosition().second;
            offsetOne.first = other->GetPosition().first - peak->GetPosition().first;
            offsetOne.second = other->GetPosition().second - peak->GetPosition().second;
            offsetTwo.first = 0;
            offsetTwo.second = 0;

        } else if (!topLeftTwo && !topRightTwo && bottomLeftTwo && bottomRightTwo) {
            //std::cout << "Twelve" << std::endl;
            length = other->GetBottomRight().first - other->GetPosition().first;
            height = other->GetBottomRight().second - peak->GetPosition().second;
            offsetOne.first = other->GetPosition().first - peak->GetPosition().first;
            offsetOne.second = 0;
            offsetTwo.first = 0;
            offsetTwo.second = peak->GetPosition().second - other->GetPosition().second;

        } else if (!topLeftTwo && topRightTwo && !bottomLeftTwo && bottomRightTwo) {
            //std::cout << "Thirteen" << std::endl;
            length = other->GetBottomRight().first - peak->GetPosition().first;
            height = other->GetBottomRight().second - other->GetPosition().second;
            offsetOne.first = 0;
            offsetOne.second = other->GetPosition().second - peak->GetPosition().second;
            offsetTwo.first = peak->GetPosition().first - other->GetPosition().first;
            offsetTwo.second = 0;

        } else if (topLeftTwo && topRightTwo && !bottomLeftTwo && !bottomRightTwo) {
            //std::cout << "Fourteen" << std::endl;
            length = other->GetBottomRight().first - other->GetPosition().first;
            height = peak->GetBottomRight().second - other->GetPosition().second;
            offsetOne.first = other->GetPosition().first - peak->GetPosition().first;
            offsetOne.second = other->GetPosition().second - peak->GetPosition().second;
            offsetTwo.first = 0;
            offsetTwo.second = 0;
        }

        for (int i = 0; i < length; i++) {
            for (int j = 0; j < height; j++) {
                pixelOne = GetPixelColor(peak->GetSurface(),  (offsetOne.first + i) / (peak->GetScale() * peak->GetSize()) , (offsetOne.second + j) / (peak->GetScale() * peak->GetSize()));
                pixelTwo = GetPixelColor(other->GetSurface(), (offsetTwo.first + i) / (other->GetScale() * other->GetSize()), (offsetTwo.second + j) / (other->GetScale() * other->GetSize()));



                if ((pixelOne.r == 255 && pixelOne.g == 255 && pixelOne.b == 255) && (pixelTwo.r == 255 && pixelTwo.g == 255 && pixelTwo.b == 255)) {
                    intersecting = true;
                    break;
                }
            }
            if (intersecting) break;
        }

        if (intersecting) {
            connectedTerrain.push_back(other);
        }

    }

    return connectedTerrain;
}

bool TerrainIsSurrounded(Terrain* peak, Terrain* other) {
    bool surrounded = true;
    bool topLeftOne = false;
    bool topRightOne = false;
    bool bottomLeftOne = false;
    bool bottomRightOne = false;
    bool topLeftTwo = false;
    bool topRightTwo = false;
    bool bottomLeftTwo = false;
    bool bottomRightTwo = false;
    pair<int, int> offsetOne;
    pair<int, int> offsetTwo;
    int length = 0;
    int height = 0;
    SDL_Color pixelOne;
    SDL_Color pixelTwo;

    //top left
    if (peak->GetPosition().first > other->GetPosition().first &&
        peak->GetPosition().first < other->GetBottomRight().first &&
        peak->GetPosition().second > other->GetPosition().second &&
        peak->GetPosition().second < other->GetBottomRight().second) {

        topLeftOne = true;


    } if (peak->GetBottomRight().first > other->GetPosition().first &&
          peak->GetBottomRight().first < other->GetBottomRight().first &&
          peak->GetPosition().second > other->GetPosition().second &&
          peak->GetPosition().second < other->GetBottomRight().second) {

        topRightOne = true;


    }if (peak->GetPosition().first > other->GetPosition().first &&
         peak->GetPosition().first < other->GetBottomRight().first &&
         peak->GetBottomRight().second > other->GetPosition().second &&
         peak->GetBottomRight().second < other->GetBottomRight().second) {

        bottomLeftOne = true;

    } if (peak->GetBottomRight().first > other->GetPosition().first &&
          peak->GetBottomRight().first < other->GetBottomRight().first &&
          peak->GetBottomRight().second > other->GetPosition().second &&
          peak->GetBottomRight().second < other->GetBottomRight().second) {

        bottomRightOne = true;
    }

    if (topLeftOne && topRightOne && bottomLeftOne && bottomRightOne) {
        length = peak->GetBottomRight().first - peak->GetPosition().first;
        height = peak->GetBottomRight().second - peak->GetPosition().second;
        offsetOne.first = 0;
        offsetOne.second = 0;
        offsetTwo.first = peak->GetPosition().first - other->GetPosition().first;
        offsetTwo.second = peak->GetPosition().second - other->GetPosition().second;
    } else return false;

    for (int i = 0; i < length; i++) {
        for (int j = 0; j < height; j++) {
            pixelOne = GetPixelColor(peak->GetSurface(),  (offsetOne.first + i) / (peak->GetScale() * peak->GetSize()) , (offsetOne.second + j) / (peak->GetScale() * peak->GetSize()));
            pixelTwo = GetPixelColor(other->GetSurface(), (offsetTwo.first + i) / (other->GetScale() * other->GetSize()), (offsetTwo.second + j) / (other->GetScale() * other->GetSize()));



            if ((pixelOne.r == 255 && pixelOne.g == 255 && pixelOne.b == 255) && (pixelTwo.r != 255 || pixelTwo.g != 255 || pixelTwo.b != 255)) {
                surrounded = false;
                break;
            }
        }
        if (!surrounded) break;
    }

    return surrounded;
}


void GroomTerrain() {
    int moveCount = 1;
    while (moveCount > 0) {
        moveCount = 0;
        for (int i = 0; i < peaks.size(); i++) {
            while (MergeTerrain(peaks[i]).size() != 0) {
                //int a = ((rand() % (int)(SCREEN_WIDTH/1.2)) + SCREEN_WIDTH/2 - SCREEN_WIDTH/2.4) * 2;
                int a = (rand() % SCREEN_WIDTH) * 2;
                //int b = ((rand() % (int)(SCREEN_HEIGHT/1.2)) + SCREEN_HEIGHT/2 - SCREEN_HEIGHT/2.4) * 2;
                int b = (rand() % SCREEN_HEIGHT) * 2;

                peaks[i]->SetCenter(a, b);
                if (peaks[i]->GetItem() != nullptr) {
                    peaks[i]->GetItem()->SetCenter(a, b);
                }
                moveCount++;

            }
            Terrain* above = nullptr;
            Terrain* current = nullptr;
            for (int j = 0; j < peaks[i]->childTerrain.size(); j++) {
                current = peaks[i]->childTerrain[j];
                if (j == 0) {
                    above = peaks[i];
                } else {
                    above = peaks[i]->childTerrain[j - 1];
                }
                current->SetCenter(above->GetCenter().first + current->GetOffsetX(),above->GetCenter().second + current->GetOffsetY());
            }
        }
    }
    ConnectTerrain();
    NeighborTerrain();
    SetTerrainBiome();

}

void ConnectTerrain() {
    for (int i = 0; i < terrain.size(); i++) {
        for (int j = 0; j < terrain[i].size(); j++) {
            terrain[i][j]->connectedTerrain = MergeTerrain(terrain[i][j]);
        }
    }
    for (int i = 0; i < terrain.size(); i++) {
        for (int j = 0; j < terrain[i].size(); j++) {
            std::vector<Terrain*> neighboringTerrain;
            ConnectConnectedTerrain(neighboringTerrain, terrain[i][j]);
            terrain[i][j]->connectedTerrain = neighboringTerrain;
        }
    }
}

void ConnectConnectedTerrain(std::vector<Terrain*>& connectedTerrain, Terrain* currentTerrain) {
    for (int i = 0; i < currentTerrain->connectedTerrain.size(); i++) {

        bool alreadyCovered = false;

        for (int j = 0; j < connectedTerrain.size(); j++) {
            if (connectedTerrain[j] == currentTerrain->connectedTerrain[i]) {
                alreadyCovered = true;
            }
        }
        if (alreadyCovered) continue;

        connectedTerrain.push_back(currentTerrain->connectedTerrain[i]);
        ConnectConnectedTerrain(connectedTerrain, currentTerrain->connectedTerrain[i]);
    }
}

void NeighborTerrain() {
    for (int i = 0; i < peaks.size(); i++) {
        for (int j = 0; j < peaks[i]->childTerrain.size(); j++) {
            if (j == 0) {
                peaks[i]->childTerrain[j]->SetUpperTerrain(peaks[i]);
                peaks[i]->SetLowerTerrain(peaks[i]->childTerrain[j]);
            } else {
                peaks[i]->childTerrain[j]->SetUpperTerrain(peaks[i]->childTerrain[j - 1]);
            }
            if (j == peaks[i]->childTerrain.size() - 1) {
                peaks[i]->childTerrain[j]->SetLowerTerrain(nullptr);
            } else {
                peaks[i]->childTerrain[j]->SetLowerTerrain(peaks[i]->childTerrain[j + 1]);
            }

        }
    }
}

void SetTerrainBiome() {
    for (int i = 0; i < peaks.size(); i++) {
        if (peaks[i]->GetBiome() != "") continue;
        int biomeID = rand() % 3;
        if (peaks[i]->GetLayer() == 1 && peaks[i]->connectedTerrain.size() == 0) biomeID = 0;
        string biome;
        switch (biomeID) {
            case 0:
                biome = "plains";
                break;
            case 1:
                biome = "stone";
                break;
            case 2:
                biome = "glacier";
                break;
            default:
                biome = "plains";
                break;
        }
        peaks[i]->SetBiome(biome);
        for (int j = 0; j < peaks[i]->childTerrain.size(); j++) {
            peaks[i]->childTerrain[j]->SetBiome(biome);
            for (int k = 0; k < peaks[i]->childTerrain[j]->connectedTerrain.size(); k++) {
                Terrain* tempTerrain = peaks[i]->childTerrain[j]->connectedTerrain[k];
                while (tempTerrain != nullptr) {
                    tempTerrain->SetBiome(biome);
                    tempTerrain = tempTerrain->GetUpperTerrain();
                }
            }
        }
    }
}
