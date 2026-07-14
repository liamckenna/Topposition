#pragma once

enum gameState
{
    MAIN_MENU = 0,
    LOADING = 1,
    GAME = 2,
    PAUSED = 3
};

enum pauseState
{
    MAIN = 0,
    SETTINGS = 1,
    RULES = 2,
    WINNER = 3
};

enum objectType
{
    GENERIC = 0,
    TERRAIN = 1,
    PEAK = 2,
    PIECE = 3,
    ITEM = 4,
    UI_ELEMENT = 5,
    PIXEL = 6,
    OCEAN_TILE = 7,
    FLAG = 8
};

enum BattlePhase
{
    BATTLE_IDLE,
    BATTLE_START_ROUND,
    BATTLE_WAIT_ATTACK_ROLL,
    BATTLE_REVEAL_ATTACK,
    BATTLE_WAIT_DEFENSE_ROLL,
    BATTLE_WAIT_DIE_CLICKS,
    BATTLE_RESOLVE_ROUND,
    BATTLE_FATAL_ATTACK,
    BATTLE_FATAL_DEFENSE,
    BATTLE_ADVANCE,
    BATTLE_FATAL_ADVANCE,
    BATTLE_END
};

enum TeamColor
{
    RED = 1,
    BLUE = 2,
    GREEN = 3,
    YELLOW = 4,
    MAGENTA = 5,
    AQUAMARINE = 6,
    BLACK = 7,
    WHITE = 8,
    ORANGE = 9,
    CORNFLOWER = 10,
    FOREST = 11,
    MELROSE = 12,
    PURPLE = 13
};