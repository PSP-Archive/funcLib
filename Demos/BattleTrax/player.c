#include "player.h"
#include "././Code/flInput.h"
#include <stdlib.h>
#include <malloc.h>
#include "main.h"

Player* playerCreate() {
     Player* tempOut = (Player*)malloc(sizeof(Player));
     tempOut->playerLives = 5;
     tempOut->playerScore = 0;
     tempOut->playerTank = (Tank*)malloc(sizeof(Tank));
     tempOut->playerTank->tankDir = DIRECTION_UP;
     tempOut->playerTank->tankState = TANK_STATE_NOTHRUST;
     tempOut->playerTank->tankTurretCur = 0;
     tempOut->playerTank->tankX = 0;
     tempOut->playerTank->tankY = 0;
     tempOut->playerTank->tankArmor = (TankArmor*)malloc(sizeof(TankArmor));
     tempOut->playerTank->tankArmor->tankArmorType = TANK_ARMORTYPE_NORMAL;
     tempOut->playerTank->tankArmor->tankArmorMax = 100;
     tempOut->playerTank->tankArmor->tankArmorCur = 100;
     tempOut->playerTank->tankHull = (TankHull*)malloc(sizeof(TankHull));
     tempOut->playerTank->tankHull->tankHullType = TANK_HULLTYPE_NORMAL;
     tempOut->playerTank->tankHull->tankHullMax = 100;
     tempOut->playerTank->tankHull->tankHullCur = 100;
     tempOut->playerTank->tankHull->tankHullSprite = spriteLoadAuto("./sprites/tankHull0.png", 32, 32);
     tempOut->playerTank->tankTurret[0] = (TankTurret*)malloc(sizeof(TankTurret));
     tempOut->playerTank->tankTurret[1] = (TankTurret*)malloc(sizeof(TankTurret));
     tempOut->playerTank->tankTurret[2] = (TankTurret*)malloc(sizeof(TankTurret));
     tempOut->playerTank->tankTurret[0]->tankTurretDir = DIRECTION_UP;
     tempOut->playerTank->tankTurret[1]->tankTurretDir = DIRECTION_UP;
     tempOut->playerTank->tankTurret[2]->tankTurretDir = DIRECTION_UP;
     tempOut->playerTank->tankTurret[0]->tankTurretAmmo = 30;
     tempOut->playerTank->tankTurret[1]->tankTurretAmmo = 500;
     tempOut->playerTank->tankTurret[2]->tankTurretAmmo = 5;
     tempOut->playerTank->tankTurret[0]->tankTurretType = TANK_TURRETTYPE_CANNON;
     tempOut->playerTank->tankTurret[1]->tankTurretType = TANK_TURRETTYPE_CHAINGUN;
     tempOut->playerTank->tankTurret[2]->tankTurretType = TANK_TURRETTYPE_MISSILE;
     tempOut->playerTank->tankTurret[0]->tankTurretSprite = spriteLoadAuto("./sprites/tankTurret0.png", 32, 32);
     tempOut->playerTank->tankTurret[1]->tankTurretSprite = spriteLoadAuto("./sprites/tankTurret1.png", 32, 32);
     tempOut->playerTank->tankTurret[2]->tankTurretSprite = spriteLoadAuto("./sprites/tankTurret2.png", 32, 32);
     return tempOut;
}

void playerFree(Player* inPlayer) {
     if(inPlayer) {
          if(inPlayer->playerTank) {
               if(inPlayer->playerTank->tankArmor)
                    free(inPlayer->playerTank->tankArmor);
               if(inPlayer->playerTank->tankHull) {
                    spriteFree(inPlayer->playerTank->tankHull->tankHullSprite);
                    free(inPlayer->playerTank->tankHull);
               }
               int i;
               for(i = 0; i < 3; i++) {
                    if(inPlayer->playerTank->tankTurret[i]) {
                         spriteFree(inPlayer->playerTank->tankTurret[i]->tankTurretSprite);
                         free(inPlayer->playerTank->tankTurret[i]);
                    }
               }
               free(inPlayer->playerTank);
          }
          free(inPlayer);
     }
}

void playerDraw(Player* inPlayer, int inX, int inY) {
     if(!inPlayer)
          return;
     if(!inPlayer->playerTank)
          return;
     if(inPlayer->playerTank->tankHull) {
          spriteDrawFrame(inPlayer->playerTank->tankHull->tankHullSprite, inX, inY, (((inPlayer->playerTank->tankState & 3) << 2) + (inPlayer->playerTank->tankDir & 3)));
     }
     if(inPlayer->playerTank->tankTurret[inPlayer->playerTank->tankTurretCur]) {
          spriteDrawFrame(inPlayer->playerTank->tankTurret[inPlayer->playerTank->tankTurretCur]->tankTurretSprite, inX, inY, (((0) << 0) + (inPlayer->playerTank->tankTurret[inPlayer->playerTank->tankTurretCur]->tankTurretDir & 3)));
     }
}

void playerHit(Player* inPlayer, int inDamage) {
     int tempHullDamage = 0;
     int tempArmorDamage = 0;
     if(inPlayer->playerTank->tankArmor->tankArmorType== TANK_ARMORTYPE_NONE) {
          tempHullDamage = inDamage;
     } else if(inPlayer->playerTank->tankArmor->tankArmorType== TANK_ARMORTYPE_NORMAL) {
          if(((rand() * inPlayer->playerTank->tankArmor->tankArmorMax) / RAND_MAX) > inPlayer->playerTank->tankArmor->tankArmorCur)
               tempHullDamage = ((rand() * ((inPlayer->playerTank->tankArmor->tankArmorMax - inPlayer->playerTank->tankArmor->tankArmorCur) / inPlayer->playerTank->tankArmor->tankArmorMax)) / RAND_MAX) * inDamage;
          tempArmorDamage = inDamage - tempHullDamage;
     } else if(inPlayer->playerTank->tankArmor->tankArmorType== TANK_ARMORTYPE_REACTIVE) {
          if(((rand() * (inPlayer->playerTank->tankArmor->tankArmorMax >> 1)) / RAND_MAX) > inPlayer->playerTank->tankArmor->tankArmorCur)
               tempHullDamage = ((rand() * ((inPlayer->playerTank->tankArmor->tankArmorMax - inPlayer->playerTank->tankArmor->tankArmorCur) / inPlayer->playerTank->tankArmor->tankArmorMax)) / RAND_MAX) * inDamage;
          tempArmorDamage = inDamage - tempHullDamage;
     }
     inPlayer->playerTank->tankHull->tankHullCur -= tempHullDamage;
     inPlayer->playerTank->tankArmor->tankArmorCur -= tempArmorDamage;
     if(inPlayer->playerTank->tankArmor->tankArmorCur < 0) {
          inPlayer->playerTank->tankHull->tankHullCur += inPlayer->playerTank->tankArmor->tankArmorCur;
          inPlayer->playerTank->tankArmor->tankArmorCur = 0;
     }
     if(inPlayer->playerTank->tankHull->tankHullCur <= 0)
          playerDestroy(inPlayer);
}

void playerDestroy(Player* inPlayer) {
     inPlayer->playerLives--;
     if(inPlayer->playerLives < 0)
          gameState = GAMESTATE_GAMEOVER;
     inPlayer->playerTank->tankHull->tankHullCur = inPlayer->playerTank->tankHull->tankHullMax;
     inPlayer->playerTank->tankArmor->tankArmorCur = inPlayer->playerTank->tankArmor->tankArmorMax;
     inPlayer->playerTank->tankTurret[0]->tankTurretAmmo = 30;
     inPlayer->playerTank->tankTurret[1]->tankTurretAmmo = 500;
     inPlayer->playerTank->tankTurret[2]->tankTurretAmmo = 5;
}

void playerInput(Player* inPlayer) {
     if(inputCheck(INPUT_DPAD_LEFT) == INPUT_JUST_DOWN) {
          tankRotateL(inPlayer->playerTank);
     } else if(inputCheck(INPUT_DPAD_RIGHT) == INPUT_JUST_DOWN) {
          tankRotateR(inPlayer->playerTank);
     }
     
     if(inputCheck(INPUT_BUTTON_CIRCLE) == INPUT_JUST_DOWN) {
          tankTurretRotateR(inPlayer->playerTank->tankTurret[inPlayer->playerTank->tankTurretCur]);
     } else if(inputCheck(INPUT_BUTTON_SQUARE) == INPUT_JUST_DOWN) {
          tankTurretRotateL(inPlayer->playerTank->tankTurret[inPlayer->playerTank->tankTurretCur]);
     }
}

inline void tankTurretRotateL(TankTurret* inTurret) {
     inTurret->tankTurretDir = (inTurret->tankTurretDir - 1) & 3;
}

inline void tankTurretRotateR(TankTurret* inTurret) {
     inTurret->tankTurretDir = (inTurret->tankTurretDir + 1) & 3;
}

inline void tankRotateL(Tank* inTank) {
     inTank->tankDir = (inTank->tankDir - 1) & 3;
}

inline void tankRotateR(Tank* inTank) {
     inTank->tankDir = (inTank->tankDir + 1) & 3;
}
