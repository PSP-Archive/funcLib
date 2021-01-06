#define DEATHSTAR_WARMUP_MIN 4.0f
#define DEATHSTAR_WARMUP_VARIANCE 4.0f
#define DEATHSTAR_COOLDOWN_TIME 4.0f
#define DEATHSTAR_GATHER_TIME 2.0f
#define PLANET_EXPLODE_TIME 1.5f
#define PLANET_EXPAND_TIME 0.75f

typedef struct {
     int       lvlNumber;
     int       lvlWind, lvlGravity;
     float     lvlFriction;

     int       lvlLandingPoint;
     int       lvlPointsCount;
     int*      lvlPoints;

     u32       lvlColor;
     u32       lvlExitBorderColor;
     float     lvlFuel;
     float     lvlTime;
} Level;

Level gameLevel;

extern int    shipX, shipY;
extern int    shipLastX, shipLastY;
extern u32    shipOutlineColor;
extern float  shipFuel;
#define SHIP_FUEL_DRAINFACTOR 4.0f
extern int    shipThrust;
extern int    shipVelocityX;
extern int    shipVelocityY;
extern int    shipHealth;
extern int    shipMaxHealth;

extern bool    shipUpthrust;
extern bool    shipLeftthrust;
extern bool    shipRightthrust;
extern bool    shipHyperthrust;

extern float   gameClosestGroundHeight;
extern float   gameLandingTolerance;
extern int     gameLives;
extern u8      gameDifficulty;
extern int     gameScore;
extern int     gameHiScore;
extern char    gameHiScoreNames[40][16];
extern int     gameHiScoreLevels[40];
extern int     gameHiScores[40];
extern float   gameTimeLeft;

extern Font*   gameFont;
extern Timer*  gameTimer;
extern float   gameTickRate;
extern float   gameTickTime;

extern Texture* gameMudkipTex;
extern bool     gameMudkipUnlocked;
extern bool     gameMudkipMode;
extern bool     gameUltraUnlocked;

extern void gameStartDialog();
extern bool gameConfirmDialog(char* inMessage, bool inGame);
extern bool gameSaveDelete(int inSlot);
extern bool gameSave(int inSlot);
extern bool gameLoad(int inSlot);
extern void gameHiScoresClear();
extern void gameHiScoresLoad();
extern void gameHiScoresSave();
extern int gameHiScoresUpdate();
extern int levelPointsGet(int inX, int inY);
extern void shipDraw();
extern void levelDraw();
extern void gameRenderLoop();
extern void gameDialog(char* inMessage, bool inGame);
extern void gameHiScoreDialog(int inEntry, bool inGame);
extern void gameLevelNextDialog(float inLandingScore, int inScore);
extern int gameCalcClosestGround(int inX, int inWidth);
extern void levelCreateRandom(int inPoints, float inFraction, float inDiversity);
extern void levelRestart();
extern void levelNext();
extern void gameRestart();
extern void gameInit();
extern void gameInstructionsDialog(bool inGame);
extern void gameCreditsDialog(bool inGame);
extern int gameLoadSaveDialog(bool inLoad, char* inOperation, bool inGame);
extern void gameOptionsDialog();
extern bool gameMenu();
extern void shipCrash();
extern bool shipDamage();
extern void shipCollision(bool inForce);
extern void shipLanding();
extern void gameTick();
extern void gamePauseDialog();
extern int main();
