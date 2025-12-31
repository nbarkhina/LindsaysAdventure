#pragma once

#include <gb/gb.h>
#include <stdint.h>

//player animation tile constants
#define BabyLeftAnim1 0x04
#define BabyLeftAnim2 0x05
#define BabyLeftAnim3 0x14
#define BabyLeftAnim4 0x15
#define BabyLeftAnim5 0x06
#define BabyLeftAnim6 0x07
#define BabyLeftAnim7 0x16
#define BabyLeftAnim8 0x17
#define BabyRightAnim1 0x08
#define BabyRightAnim2 0x09
#define BabyRightAnim3 0x18
#define BabyRightAnim4 0x19
#define BabyRightAnim5 0x0A
#define BabyRightAnim6 0x0B
#define BabyRightAnim7 0x1A
#define BabyRightAnim8 0x1B

//move direction constants
#define MoveDirectionLeft 1
#define MoveDirectionRight 2
#define MoveDirectionUp 3
#define MoveDirectionDown 4

//state
#define StateAlive 0
#define StateDying 1
#define StateDead 2

//game state
#define GameStateTitle 0
#define GameStatePlaying 1
#define GameStateGameover 2
#define GameStateLevelName 3
#define GameStateWon 4

extern unsigned char currentGameState;


typedef struct
{
  int X;
  int Y;
  unsigned char animationFrame;
  unsigned char animationCounter;
  unsigned char direction;
  unsigned char state;
  unsigned char scatter_x;
  unsigned char scatter_y;
  unsigned char deathCounter;
} Dog;

typedef struct
{
  int X;
  int Y;
  unsigned char moveCounter;
  unsigned char animationCounter;
  unsigned char isUpDown;
  unsigned char direction;
  unsigned char state;
  unsigned char isCarryingPlayer;
} Platform;

typedef struct
{
  int X;
  int Y;
  int baseX;
  int baseY;
  unsigned char direction;
  unsigned char state;
  unsigned char animationFrame;
  unsigned char animationCounter;
  unsigned char scatter_x;
  unsigned char scatter_y;
  unsigned char deathCounter;
  unsigned char swoopPhase;
  unsigned char swoopTimer;
  unsigned char swoopCooldown;
} Saucer;

typedef struct
{
  int X;
  int Y;
  signed char velX;
  signed char velY;
  unsigned char state;
  unsigned char animationFrame;
  unsigned char animationCounter;
  unsigned char scatter_x;
  unsigned char scatter_y;
  unsigned char deathCounter;
} Bouncer;

typedef struct
{
  int X;
  int Y;
  unsigned char state;
  unsigned char direction;
  unsigned char animationFrame;
  unsigned char animationCounter;
  unsigned char scatter_x;
  unsigned char scatter_y;
  unsigned char deathCounter;
  unsigned char health;
  unsigned char invincibilityTimer;
  unsigned char shootTimer;
} Boss;

typedef struct {
  unsigned int note;
  unsigned char duration;
  unsigned char volume;
} SfxStep;

// Shared music note definition for all banked music data
typedef struct {
  unsigned int note;       // tone period or NOTE_REST
  unsigned int duration;   // frames the note lasts (duration/60 seconds)
  unsigned int timestamp;  // frame when the note should start (timestamp/60 seconds)
} MusicNote;


//saucer chase tuning
extern const unsigned char saucerChaseLead;   //how far past the player the saucer goes before turning
extern const unsigned char saucerMaxDrift;    //hard limit from baseX to avoid runaway
extern const signed char bouncerBaseSpeed;    //base speed for the bouncer enemy

//level stats
extern int levelWidthTiles; //width of level
extern int maxCameraXLimit; //stop camera at end of level

extern int playerX;
extern int playerY;
extern int exitReached; //has the player reached the exit?
extern int levelNum; //the level player is on
extern int winTimer; //frames to show win message before returning to title
extern unsigned char playerMoving;             //is the player moving?
extern unsigned char playerMoveCounter;        //when to move the player (not every frame to slow it down)
extern const unsigned char PlayerMoveInterval; //at what rate the player moves (I.E. every 2 frames)
extern const unsigned char AnimationInterval;  //at what frame to switch animation
extern unsigned char animationCounter;         //a counter to keep track of when to switch animation
extern unsigned char animationFrame;           //which frame of animation are we on
extern unsigned char playerDirection;          //1 - left, 2 - right, 3 - up, 4 - down
extern unsigned char playerDirectionLast;      //in order to change animation immediately if switching directions
extern int scrollX;
extern int cameraX; //camera for scrolling
extern int newCameraX; //stop scrolling if the player is walking left
extern int levelChunkCurrent; //which chunk we are up to
extern int levelChunkDrawn; //which chunk has already been drawn
extern int levelChunkY; //used to draw efficiently in chunks of 4
extern unsigned char levelChunkInProgress; //is 4-chunk drawing finished
extern unsigned char waaButton; //if waa button is pressed
extern unsigned char waaCounter; //if baby is going waa
#define NUMDOGS 5
extern Dog dogs[NUMDOGS]; //array of dogs
#define NUMPLATFORMS 6
extern Platform platforms[NUMPLATFORMS]; //array of platforms
#define NUMSAUCERS 2
extern Saucer saucers[NUMSAUCERS]; //array of saucers
#define NUMBOUNCERS 4
extern Bouncer bouncers[NUMBOUNCERS]; //array of bouncers
#define NUMBOSSES 1
extern Boss bosses[NUMBOSSES]; //array of boss babysitters
extern unsigned char spriteIndex; //track which sprite we are drawing
extern unsigned char hideSpriteCount; //how many sprites to hide at the end of a frame

//stores dynamic copy of level after reading from ROM
//enough for 2 screens (20*14*2=560)
extern unsigned char level_memory[560];

//vram buffers
extern unsigned char bg_buffer[360]; //20*18 tiles = 160x144 pixels
extern unsigned char win_buffer[360];

//text printing
extern unsigned char text_buffer[50];
extern const unsigned char textTileStart; //offset to where text starts in tilemap

//Waveform for channel 3 effects (4-bit samples packed per byte)
extern const unsigned char channel3Wave[16];

// Quick two-note bottle pickup blip at 50% volume
extern const SfxStep pickupSfxSteps[2];

extern const unsigned char pickupSfxStepCount;

// Triumphant sweep for reaching the exit door
extern const SfxStep winSfxSteps[4];

extern const unsigned char winSfxStepCount;

// Sad descending tones for player death
extern const SfxStep sadSfxSteps[3];

extern const unsigned char sadSfxStepCount;


// audio state
extern unsigned char soundMasterToggle; //global toggle set from the title screen
extern unsigned char soundToggleLock; //prevents rapid repeat while holding left/right
extern unsigned char musicPlaying;
extern unsigned int musicPos1;
extern unsigned int musicPos2;
extern unsigned int musicPos3;
extern unsigned int musicTimer1;
extern unsigned int musicTimer2;
extern unsigned int musicTimer3;
extern unsigned int musicFrame;

extern const SfxStep* activeWaveSteps;
extern unsigned char activeWaveStepCount;
extern unsigned char waveSfxActive;
extern unsigned char waveStepPos;
extern unsigned char waveTimer;

//LOCAL VARIABLES GO HERE
extern int i, j, k, l, x, y;
extern unsigned char p, q, r;
extern int playerScreenX; //reserved only for the player
extern int screenX; //local variable only used for move_camera function
extern int screenY; //local variable only used for move_camera function
extern int screenX1; //extra copies to save on number of math functions
extern int screenX2; //extra copies to save on number of math functions
extern int screenX3; //extra copies to save on number of math functions
extern int screenX4; //extra copies to save on number of math functions
extern int screenY1; //extra copies to save on number of math functions
extern int screenY2; //extra copies to save on number of math functions

extern unsigned char initalLevelDraw; //used to draw the initial 21 columns when level loads


//collision detection
extern unsigned char isDownColliding;
extern unsigned char isUpColliding;
extern unsigned char isLeftColliding;
extern unsigned char isRightColliding;
extern int tileCollideX; //x coordinate of tile to check against
extern int tileCollideXReal; //same as tileCollideX but without % 32
extern int tileCollideY; //y coordinate of tile to check against
extern int tileCollideTile; //place in the level_memory array to check against
extern unsigned char tileCollideValue; //the value of the tile itself
extern unsigned char cancelCollision; //if the collision was with a non-solid block
extern int score;
extern unsigned char drawScore; //if 1 then draw the score this frame
extern int lives;
extern unsigned char drawLives; //flag to refresh lives display
extern unsigned char health;
extern unsigned char drawHealth; //flag to refresh health bar sprites
extern const unsigned char maxHealth;
extern unsigned char invincibilityTimer; //frames remaining while the player is invulnerable
extern const unsigned char invincibilityDuration; //1 second at 60fps
extern const unsigned char invincibilityFlashMask; //use bit toggling for quick flashing
extern unsigned char showGameOverMessage; //flag to draw game over text during reset
extern unsigned char disableNoise; // flag to disable noise channel temporarily

//physics
extern unsigned char readyToJump; //gets reset when player is on the ground
extern unsigned char jumpButton; //is the jump button being pressed
extern unsigned char leftButton; //is the left button being pressed
extern unsigned char rightButton; //is the right button being pressed
extern char downSpeed; //speed player is currently falling or jumping
extern const char terminalVelocity; //maximum falling velocity
extern const char jumpSpeed; //vertical speed constant of player after pressing jump
// const char jumpSpeed = -25; //vertical speed constant of player after pressing jump
extern unsigned char jumpCounter; //pressing longer jumps higher
extern const char gravity; //gravity constant
extern char lateralSpeed; //speed player is moving left or right
extern char movedAmount; //used as a layer on top of lateralSpeed instead of decimals
extern int fallResetTimer; //counts down to reset when falling out of bounds
extern const int fallResetDelayFrames; //1.5 seconds at 60fps
extern const int fallDeathY; //roughly off the bottom of the screen

//to reduce number of calculations
extern int y1;
extern int y2;
//for collision detection
extern int tileCollideCheck;


// Music note constants (tone periods)
#define NOTE_C0   44
#define NOTE_Cs0  156
#define NOTE_D0   262
#define NOTE_Ds0  363
#define NOTE_E0   457
#define NOTE_F0   547
#define NOTE_Fs0  631
#define NOTE_G0   710
#define NOTE_Gs0  786
#define NOTE_A0   854
#define NOTE_As0  923
#define NOTE_B0   986

#define NOTE_C1   1046
#define NOTE_Cs1  1102
#define NOTE_D1   1155
#define NOTE_Ds1  1205
#define NOTE_E1   1253
#define NOTE_F1   1297
#define NOTE_Fs1  1339
#define NOTE_G1   1379
#define NOTE_Gs1  1417
#define NOTE_A1   1452
#define NOTE_As1  1486
#define NOTE_B1   1517

#define NOTE_C2   1546
#define NOTE_Cs2  1575
#define NOTE_D2   1602
#define NOTE_Ds2  1627
#define NOTE_E2   1650
#define NOTE_F2   1673
#define NOTE_Fs2  1694
#define NOTE_G2   1714
#define NOTE_Gs2  1732
#define NOTE_A2   1750
#define NOTE_As2  1767
#define NOTE_B2   1783

#define NOTE_C3   1798
#define NOTE_Cs3  1812
#define NOTE_D3   1825
#define NOTE_Ds3  1837
#define NOTE_E3   1849
#define NOTE_F3   1860
#define NOTE_Fs3  1871
#define NOTE_G3   1881
#define NOTE_Gs3  1890
#define NOTE_A3   1899
#define NOTE_As3  1907
#define NOTE_B3   1915

#define NOTE_C4   1923
#define NOTE_Cs4  1930
#define NOTE_D4   1936
#define NOTE_Ds4  1943
#define NOTE_E4   1949
#define NOTE_F4   1954
#define NOTE_Fs4  1959
#define NOTE_G4   1964
#define NOTE_Gs4  1969
#define NOTE_A4   1974
#define NOTE_As4  1978
#define NOTE_B4   1982

#define NOTE_C5   1990
#define NOTE_Cs5  1997
#define NOTE_D5   2003
#define NOTE_Ds5  2009
#define NOTE_E5   2014
#define NOTE_F5   2020
#define NOTE_Fs5  2025
#define NOTE_G5   2030
#define NOTE_A5   2040

#define NOTE_REST 0

// Level data (banked in levels_bank)
#define LEVEL_TILE_HEIGHT 14
extern const unsigned char level1[];
extern const unsigned char level2[];
extern const unsigned char level3[];
extern const unsigned char level4[];
extern const uint16_t level_lengths[4];

// Graphics data (banked in gfx_bank)
extern const int man_tile_map_size;
extern const int man_tile_map_width;
extern const int man_tile_map_height;
extern const int man_tile_data_size;
extern const int man_tile_count;
extern const unsigned char man_map_data[];
extern const unsigned char man_tile_data[];

extern const int lind1_tile_map_size;
extern const int lind1_tile_map_width;
extern const int lind1_tile_map_height;
extern const int lind1_tile_data_size;
extern const int lind1_tile_count;
extern const unsigned char lind1_map_data[];
extern const unsigned char lind1_tile_data[];

extern const int bird_small_tile_map_size;
extern const int bird_small_tile_map_width;
extern const int bird_small_tile_map_height;
extern const int bird_small_tile_data_size;
extern const int bird_small_tile_count;
extern const unsigned char bird_small_map_data[];
extern const unsigned char bird_small_tile_data[];

extern const MusicNote* activeChannel1Track;
extern const MusicNote* activeChannel2Track;
extern const MusicNote* activeChannel3Track;

extern unsigned int activeLength1;
extern unsigned int activeLength2;
extern unsigned int activeLength3;
extern unsigned int activeTotalFrames;
extern unsigned int activeLoopFrame;
extern unsigned int activeStartFrame;

// track1
extern const MusicNote track1Channel1[];
extern const MusicNote track1Channel2[];
extern const MusicNote track1Channel3[];
extern const unsigned int track1Length1;
extern const unsigned int track1Length2;
extern const unsigned int track1Length3;
extern const unsigned int track1TotalFrames;
extern const unsigned char track1LoopFrame;
extern const unsigned char track1StartFrame;
// track2
extern const MusicNote track2Channel1[];
extern const MusicNote track2Channel2[];
extern const MusicNote track2Channel3[];
extern const unsigned int track2Length1;
extern const unsigned int track2Length2;
extern const unsigned int track2Length3;
extern const unsigned int track2TotalFrames;
extern const unsigned char track2LoopFrame;
extern const unsigned char track2StartFrame;

// banked functions
void stopMusic();
void startMusic();
void soundInit();
void waveSfxUpdate();
void musicUpdate();
void runPlayerPhysics();
void runDog(Dog* dog);
void drawDog(Dog* dog);
void runSaucer(Saucer* saucer);
void drawSaucer(Saucer* saucer);
void runBouncer(Bouncer* bouncer);
void drawBouncer(Bouncer* bouncer);
void runBoss(Boss* boss);
void drawBoss(Boss* boss);
void runPlatform(Platform* platform);
void drawPlatform(Platform* platform);

//global functions used in banked functions
void playNoise();
void damagePlayer();
void setLevelTile(int worldX, int worldY, unsigned char tileValue); // sets a tile in the level at world coordinates