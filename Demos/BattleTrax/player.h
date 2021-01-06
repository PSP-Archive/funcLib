#ifndef PLAYER_H
#define PLAYER_H

#include "././Code/flGlobal.h"
#include "././Code/flGraphicsSimpleSprite.h"

#define TANK_TURRETTYPE_NONE     0
#define TANK_TURRETTYPE_CANNON   1
#define TANK_TURRETTYPE_CHAINGUN 2 // Armor Piercing Chain Gun
#define TANK_TURRETTYPE_MISSILE  3

#define TANK_ARMORTYPE_NONE       0 // All damage goes to hull.
#define TANK_ARMORTYPE_NORMAL     1 // Armor protects hull by percentage.
#define TANK_ARMORTYPE_REACTIVE   2 // Hull only takes damage after armor is halved.
#define TANK_ARMORTYPE_REFLECTIVE 3 // A certain amount of damage is reflected dependant on armor level.

#define TANK_HULLTYPE_NONE   0
#define TANK_HULLTYPE_NORMAL 1
#define TANK_HULLTYPE_HEAVY  2

#define TANK_STATE_NOTHRUST    0
#define TANK_STATE_THRUST      1
#define TANK_STATE_STRAFERIGHT 2
#define TANK_STATE_STRAFELEFT  3

typedef u8 dir4;
#define DIRECTION_UP    0
#define DIRECTION_RIGHT 1
#define DIRECTION_DOWN  2
#define DIRECTION_LEFT  3

typedef struct {
     int tankArmorType;
     int tankArmorCur;
     int tankArmorMax;
} TankArmor;

typedef struct {
     int     tankHullType;
     int     tankHullCur;
     int     tankHullMax;
     Sprite* tankHullSprite;
} TankHull;

typedef struct {
     int     tankTurretType;
     int     tankTurretAmmo;
     dir4    tankTurretDir;
     Sprite* tankTurretSprite;
} TankTurret;

typedef struct {
     TankArmor*  tankArmor;
     TankHull*   tankHull;
     TankTurret* tankTurret[3];
     u8          tankTurretCur;
     int         tankX, tankY;
     dir4        tankDir;
     u8          tankState;
} Tank;

typedef struct {
     unsigned int playerLives;
     unsigned int playerScore;
     Tank*        playerTank;
} Player;

extern Player* playerCreate();
extern void    playerFree(Player* inPlayer);
extern void    playerDraw(Player* inPlayer, int inX, int inY);
extern void    playerHit(Player* inPlayer, int inDamage);
extern void    playerDestroy(Player* inPlayer);
extern void    playerInput(Player* inPlayer);

inline void tankTurretRotateL(TankTurret* inTurret);
inline void tankTurretRotateR(TankTurret* inTurret);

inline void tankRotateL(Tank* inTank);
inline void tankRotateR(Tank* inTank);

#endif
