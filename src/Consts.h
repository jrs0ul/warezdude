#pragma once


const unsigned NET_HEADER_LEN             = 3;
const char NET_HEADER[NET_HEADER_LEN + 1] = "WD3";

const int SERVER_SKIP_SENDING_POS_FRAMES  = 1;

const int TILE_WIDTH                      = 32;
const int HALF_TILE_WIDTH                 = TILE_WIDTH / 2;


const float PLAYER_RADIUS                 = 8.f;
const int PLAYER_MELEE_DAMAGE             = 15;

const int PROJECTILE_BULLET_DAMAGE        = 3;
const int PROJECTILE_MINE_DAMAGE          = 6;

const int PROJECTILE_EXPLOSION_DURATION   = 5;

const int ENTITY_INITIAL_AMMO             = 20;
const int ENTITY_INITIAL_HP               = 100;


const int MONSTER_MAX_RACE                = 3;
const int MONSTER_BASE_HP                 = 30;


const int SLIME_DAMAGE                    = 2;

const int DAMAGER_NOBODY                  = -1;

const unsigned char TILE_DIRT             = 1;
const unsigned char TILE_ROCKS            = 8;
const unsigned char TILE_WALL             = 19;
const unsigned char TILE_V_WALL           = 20;
const unsigned char TILE_H_WALL           = 21;
const unsigned char TILE_EXIT             = 81;

const int MAP_COUNT                       = 8;
