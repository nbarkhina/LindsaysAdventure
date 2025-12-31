#include <gb/gb.h>
#include <stdio.h>
#include <string.h>

#include "banked_assets.h"

#define MUSIC_BANK 1
#define LEVELS_BANK 2
#define GFX_BANK 3
#define ENGINE_BANK 4
#define BOSS_BANK 5

unsigned char currentGameState = 0;

//saucer chase tuning
const unsigned char saucerChaseLead = 24;   //how far past the player the saucer goes before turning
const unsigned char saucerMaxDrift = 160;   //hard limit from baseX to avoid runaway
const signed char bouncerBaseSpeed = 1;     //base speed for the bouncer enemy

//level stats
int levelWidthTiles = 0; //width of level
int maxCameraXLimit = 0; //stop camera at end of level

int playerX;
int playerY;
int exitReached = 0; //has the player reached the exit?
int levelNum = 1; //the level player is on
int winTimer = 0; //frames to show win message before returning to title
unsigned char playerMoving;             //is the player moving?
unsigned char playerMoveCounter;        //when to move the player (not every frame to slow it down)
const unsigned char PlayerMoveInterval = 2; //at what rate the player moves (I.E. every 2 frames)
const unsigned char AnimationInterval = 8;  //at what frame to switch animation
unsigned char animationCounter;         //a counter to keep track of when to switch animation
unsigned char animationFrame;           //which frame of animation are we on
unsigned char playerDirection;          //1 - left, 2 - right, 3 - up, 4 - down
unsigned char playerDirectionLast;      //in order to change animation immediately if switching directions
int scrollX;
int cameraX; //camera for scrolling
int newCameraX; //stop scrolling if the player is walking left
int levelChunkCurrent; //which chunk we are up to
int levelChunkDrawn; //which chunk has already been drawn
int levelChunkY; //used to draw efficiently in chunks of 4
unsigned char levelChunkInProgress; //is 4-chunk drawing finished
unsigned char waaButton; //if waa button is pressed
unsigned char waaCounter; //if baby is going waa
#define NUMDOGS 5
Dog dogs[NUMDOGS]; //array of dogs
#define NUMPLATFORMS 6
Platform platforms[NUMPLATFORMS]; //array of platforms
#define NUMSAUCERS 2
Saucer saucers[NUMSAUCERS]; //array of saucers
#define NUMBOUNCERS 4
Bouncer bouncers[NUMBOUNCERS]; //array of bouncers
#define NUMBOSSES 1
Boss bosses[NUMBOSSES]; //array of boss babysitters
unsigned char spriteIndex = 0; //track which sprite we are drawing
unsigned char hideSpriteCount = 0; //how many sprites to hide at the end of a frame

//stores dynamic copy of level after reading from ROM
//enough for 2 screens (20*14*2=560)
unsigned char level_memory[560];

//vram buffers
unsigned char bg_buffer[360]; //20*18 tiles = 160x144 pixels
unsigned char win_buffer[360];

//text printing
unsigned char text_buffer[50];
const unsigned char textTileStart = 80; //offset to where text starts in tilemap



//Waveform for channel 3 effects (4-bit samples packed per byte)
const unsigned char channel3Wave[16] = {
  0x10, 0x32, 0x54, 0x76,
  0x98, 0xBA, 0xDC, 0xFE,
  0xFE, 0xDC, 0xBA, 0x98,
  0x76, 0x54, 0x32, 0x10
};

// Quick two-note bottle pickup blip at 50% volume
const SfxStep pickupSfxSteps[] = {
  {NOTE_E4, 6, 0x20},
  {NOTE_G4, 8, 0x20},
};
const unsigned char pickupSfxStepCount = sizeof(pickupSfxSteps)/sizeof(pickupSfxSteps[0]);

// Triumphant sweep for reaching the exit door
const SfxStep winSfxSteps[] = {
  {NOTE_C3, 10, 0x30},
  {NOTE_E3, 10, 0x30},
  {NOTE_G3, 12, 0x30},
  {NOTE_C4, 16, 0x30},
};
const unsigned char winSfxStepCount = sizeof(winSfxSteps)/sizeof(winSfxSteps[0]);

// Sad descending tones for player death
const SfxStep sadSfxSteps[] = {
  {NOTE_E3, 10, 0x20},
  {NOTE_C3, 10, 0x20},
  {NOTE_G2, 14, 0x20},
};
const unsigned char sadSfxStepCount = sizeof(sadSfxSteps)/sizeof(sadSfxSteps[0]);

unsigned char soundMasterToggle = 1; //global toggle set from the title screen
unsigned char soundToggleLock = 0; //prevents rapid repeat while holding left/right
unsigned char musicPlaying = 0;
unsigned int musicPos1 = 0;
unsigned int musicPos2 = 0;
unsigned int musicTimer1 = 0;
unsigned int musicTimer2 = 0;
unsigned int musicPos3 = 0;
unsigned int musicTimer3 = 0;
unsigned int musicFrame = 0;

const MusicNote* activeChannel1Track;
const MusicNote* activeChannel2Track;
const MusicNote* activeChannel3Track;

unsigned int activeLength1;
unsigned int activeLength2;
unsigned int activeLength3;
unsigned int activeTotalFrames;
unsigned int activeLoopFrame;
unsigned int activeStartFrame;

const SfxStep* activeWaveSteps = 0;
unsigned char activeWaveStepCount = 0;
unsigned char waveSfxActive = 0;
unsigned char waveStepPos = 0;
unsigned char waveTimer = 0;

//LOCAL VARIABLES GO HERE
int i, j, k, l, x, y;
unsigned char p, q, r;
int playerScreenX; //reserved only for the player
int screenX; //local variable only used for move_camera function
int screenY; //local variable only used for move_camera function
int screenX1; //extra copies to save on number of math functions
int screenX2; //extra copies to save on number of math functions
int screenX3; //extra copies to save on number of math functions
int screenX4; //extra copies to save on number of math functions
int screenY1; //extra copies to save on number of math functions
int screenY2; //extra copies to save on number of math functions

unsigned char initalLevelDraw; //used to draw the initial 21 columns when level loads


//collision detection
unsigned char isDownColliding;
unsigned char isUpColliding;
unsigned char isLeftColliding;
unsigned char isRightColliding;
int tileCollideX; //x coordinate of tile to check against
int tileCollideXReal; //same as tileCollideX but without % 32
int tileCollideY; //y coordinate of tile to check against
int tileCollideTile; //place in the level_memory array to check against
unsigned char tileCollideValue; //the value of the tile itself
unsigned char cancelCollision; //if the collision was with a non-solid block
int score;
unsigned char drawScore; //if 1 then draw the score this frame
int lives;
unsigned char drawLives; //flag to refresh lives display
unsigned char health;
unsigned char drawHealth; //flag to refresh health bar sprites
const unsigned char maxHealth = 5;
unsigned char invincibilityTimer; //frames remaining while the player is invulnerable
const unsigned char invincibilityDuration = 60; //1 second at 60fps
const unsigned char invincibilityFlashMask = 0x04; //use bit toggling for quick flashing
unsigned char showGameOverMessage; //flag to draw game over text during reset
unsigned char disableNoise; // flag to disable noise channel temporarily

//physics
unsigned char readyToJump = 0; //gets reset when player is on the ground
unsigned char jumpButton = 0; //is the jump button being pressed
unsigned char leftButton = 0; //is the left button being pressed
unsigned char rightButton = 0; //is the right button being pressed
char downSpeed = 0; //speed player is currently falling or jumping
const char terminalVelocity = 40; //maximum falling velocity
const char jumpSpeed = -40; //vertical speed constant of player after pressing jump
// const char jumpSpeed = -25; //vertical speed constant of player after pressing jump
unsigned char jumpCounter = 0; //pressing longer jumps higher
const char gravity = 2; //gravity constant
char lateralSpeed = 0; //speed player is moving left or right
char movedAmount = 0; //used as a layer on top of lateralSpeed instead of decimals
int fallResetTimer = 0; //counts down to reset when falling out of bounds
const int fallResetDelayFrames = 90; //1.5 seconds at 60fps
const int fallDeathY = 176; //roughly off the bottom of the screen
//to reduce number of calculations
int y1 = 0;
int y2 = 0;
//for collision detection
int tileCollideCheck;

void printTextWin(const char *text, UINT8 x, UINT8 y)
{
  sprintf(text_buffer, text);
  j = strlen(text);
  for (i = 0; i < j; i++)
  {
    if (text_buffer[i] == 32)
      text_buffer[i] = 0;
    else
      text_buffer[i] += textTileStart;
  }
  set_win_tiles(x, y, j, 1, text_buffer);
}

void printText(const char *text, UINT8 x, UINT8 y)
{
  sprintf(text_buffer, text);
  j = strlen(text);
  for (i = 0; i < j; i++)
  {
    if (text_buffer[i] == 32)
      text_buffer[i] = 0;
    else
      text_buffer[i] += textTileStart;
  }
  set_bkg_tiles(x, y, j, 1, text_buffer);
}

void drawSoundOption(){
  if (soundMasterToggle)
  {
    // I had to move the position of the greater than symbol
    // because the dragon was so big - swapped it with semicolon
    printText("<SOUND ON ;", 5, 16); //pad to overwrite OFF length
  }
  else
  {
    printText("<SOUND OFF;", 5, 16);
  }

  //white tile for the spaces between lindsays adventure and press start
  bg_buffer[0] = 250;
  if (soundMasterToggle)
  {
    set_bkg_tiles(11,16,1,1,bg_buffer);
    set_bkg_tiles(14,16,1,1,bg_buffer);
  }
  else
  {
    set_bkg_tiles(11,16,1,1,bg_buffer);
  }
}

void printTextFromBuffer(UINT8 x, UINT8 y)
{
  j = strlen(text_buffer);
  for (i = 0; i < j; i++)
  {
    if (text_buffer[i] == 32)
      text_buffer[i] = 0;
    else
      text_buffer[i] += textTileStart;
  }
  set_win_tiles(x, y, j, 1, text_buffer);
}

void stopMusic(){
  //stop the background music and clear envelopes
  musicPlaying = 0;
  musicPos1 = 0;
  musicPos2 = 0;
  musicPos3 = 0;
  musicTimer1 = 0;
  musicTimer2 = 0;
  musicTimer3 = 0;
  musicFrame = 0;
  NR12_REG = 0x00;
  NR22_REG = 0x00;
  NR42_REG = 0x00;
}

void startMusic(){
  musicPlaying = 1; //resume music during gameplay
  musicPos1 = 0;
  musicPos2 = 0;
  musicPos3 = 0;
  musicTimer1 = 0;
  musicTimer2 = 0;
  musicTimer3 = 0;
  musicFrame = activeStartFrame;
}

void disableSoundEffects()
{
  NR30_REG = 0;
  NR42_REG = 0;
}

void enableSoundEffects()
{
  NR30_REG = 0x80;
}


void drawHealthBar()
{
  for (i = 0; i < maxHealth; i++)
  {
    //use tile index 0x47 for filled pip, 0 for empty
    text_buffer[i] = (i < health) ? 0x47 : 0x00;
  }
  set_win_tiles(2, 2, maxHealth, 1, text_buffer);
}

void handleExitDoorReached()
{
  //start win timer and hide player until transition
  winTimer = 180; //3 seconds at 60fps
  clearSprites(); //remove player/enemy sprites during the win screen
  printTextWin("YOU WIN",6,0);
  exitReached = 0;

  playWinSfx(); //celebrate the exit door
  stopMusic(); //silence during the win timer
}

void playNoise(){
  if (!soundMasterToggle)
    return;

  // sharp hit noise for kills
  NR42_REG = 0xF2;  // vol 15, decay, step=2
  NR43_REG = 0x15;  // medium frequency, 7-bit LFSR
  NR44_REG = 0x80;  // trigger
}

void playWaveSfx(const SfxStep* steps, unsigned char stepCount){
  activeWaveSteps = steps;
  activeWaveStepCount = stepCount;
  waveSfxActive = 1;
  waveStepPos = 0;
  waveTimer = 0;
}

void playPickupSfx(){
  // start coin-like wave arpeggio for pickups
  playWaveSfx(pickupSfxSteps, pickupSfxStepCount);
}

void playWinSfx(){
  playWaveSfx(winSfxSteps, winSfxStepCount);
}

void playSadSfx(){
  playWaveSfx(sadSfxSteps, sadSfxStepCount);
}


void damagePlayer()
{
  //ignore damage while already invincible or during a pending reset
  if (invincibilityTimer > 0 || fallResetTimer > 0)
    return;

  if (health > 0)
  {
    health--;
    drawHealth = 1;
  }

  //player death: hide sprite and start the same reset flow used for falling
  if (health == 0)
  {
    playSadSfx();
    fallResetTimer = fallResetDelayFrames;
    if (lives > 0)
    {
      stopMusic();
      lives--;
      drawLives = 1;
      if (lives == 0)
        showGameOverMessage = 1;
    }
    return;
  }

  invincibilityTimer = invincibilityDuration;
}

//reset dynamic sprites for next frame of animation 
void resetSpriteIndex()
{
  spriteIndex = 13;
  hideSpriteCount=0;
}

//move remaining unused sprites offscreen
void hideSprites(){
  for(p=0;p<hideSpriteCount;p++){
    move_sprite(spriteIndex, 0,0);spriteIndex++;
    move_sprite(spriteIndex, 0,0);spriteIndex++;
    move_sprite(spriteIndex, 0,0);spriteIndex++;
    move_sprite(spriteIndex, 0,0);spriteIndex++;
  }
}

//remove all sprites from screen
void clearSprites(){
  for(p=0;p<40;p++){
    move_sprite(p, 0,0);
  }
}


void drawLevelChunk(){
  //level data lives in a banked ROM; switch in before reading tiles
  SWITCH_ROM(LEVELS_BANK);

  //NOTE - WHEN COMPARING NEGATIVE NUMBERS
  //BOTH LEFT ANR RIGHT SIDE HAS TO BE SAME TYPE
  //IF ONE IS SIGNED AND OTHER IS UNSIGNED
  //THINGS DONT WORK

  if (levelChunkInProgress==0)
  {
    //chunk already drawn
    if (levelChunkDrawn>=levelChunkCurrent)
      return;

    //reached the end of the level
    if (levelChunkDrawn==(levelWidthTiles-1))
      return;

    //draw column in sections of 4
    levelChunkDrawn++;
    levelChunkInProgress = 1;
    levelChunkY = 0;
  }
  

  

  //wrap around if past 32
  i = levelChunkDrawn;
  i &= 31;

  //set level_memory and background
  k=0;
  for(y=levelChunkY;y<levelChunkY+4;y++){

    unsigned int levelIndex = levelChunkDrawn + (levelWidthTiles * y);

    if (levelNum == 1)
    {
      j = level1[levelIndex];
    }
    else if (levelNum == 2)
    {
      j = level2[levelIndex];
    }
    else if (levelNum == 3)
    {
      j = level3[levelIndex];
    }
    else if (levelNum == 4)
    {
      j = level4[levelIndex];
    }
  

    //SPECIAL TILES

    //spawn dog
    if (j==0x01){
      for(l=0;l<NUMDOGS;l++){
        if (dogs[l].state==StateDead){
          dogs[l].direction=MoveDirectionLeft;
          dogs[l].animationCounter=0;
          dogs[l].animationFrame=0;
          dogs[l].state=StateAlive;
          dogs[l].deathCounter=0;
          dogs[l].scatter_x=0;
          dogs[l].scatter_y=0;
          dogs[l].X = levelChunkDrawn*8;
          dogs[l].Y = y*8-8;
          break;
        }
      }
      j=0;
    }

    //spawn left/right platform
    if (j==0x02){
      for(l=0;l<NUMPLATFORMS;l++){
        if (platforms[l].state==StateDead){
          platforms[l].isUpDown=0;
          platforms[l].animationCounter=0;
          platforms[l].moveCounter=0;
          platforms[l].direction=MoveDirectionLeft;
          platforms[l].state=StateAlive;
          platforms[l].X = levelChunkDrawn*8;
          platforms[l].Y = y*8-8;
          platforms[l].isCarryingPlayer = 0;
          break;
        }
      }
      j=0;
    }

    //spawn up/down platform
    if (j==0x03){
      for(l=0;l<NUMPLATFORMS;l++){
        if (platforms[l].state==StateDead){
          platforms[l].isUpDown=0;
          platforms[l].animationCounter=0;
          platforms[l].moveCounter=0;
          platforms[l].direction=MoveDirectionUp;
          platforms[l].state=StateAlive;
          platforms[l].X = levelChunkDrawn*8;
          platforms[l].Y = y*8-8;
          platforms[l].isCarryingPlayer = 0;
          break;
        }
      }
      j=0;
    }

    //spawn saucer
    if (j==0x04){
      for(l=0;l<NUMSAUCERS;l++){
        if (saucers[l].state==StateDead){
          saucers[l].direction = MoveDirectionRight;
          saucers[l].animationCounter = 0;
          saucers[l].animationFrame = 0;
          saucers[l].state = StateAlive;
          saucers[l].scatter_x = 0;
          saucers[l].scatter_y = 0;
          saucers[l].deathCounter = 0;
          saucers[l].swoopPhase = 0;
          saucers[l].swoopTimer = 0;
          saucers[l].swoopCooldown = 90;
          saucers[l].X = levelChunkDrawn*8;
          saucers[l].Y = y*8-8;
          saucers[l].baseX = saucers[l].X;
          saucers[l].baseY = saucers[l].Y;
          break;
        }
      }
      j=0;
    }

    //spawn bouncer
    if (j==0x05){
      for(l=0;l<NUMBOUNCERS;l++){
        if (bouncers[l].state==StateDead){
          bouncers[l].animationCounter = 0;
          bouncers[l].animationFrame = 0;
          bouncers[l].state = StateAlive;
          bouncers[l].scatter_x = 0;
          bouncers[l].scatter_y = 0;
          bouncers[l].deathCounter = 0;
          bouncers[l].velX = ((levelChunkDrawn + y) & 1) ? bouncerBaseSpeed : -bouncerBaseSpeed;
          bouncers[l].velY = ((levelChunkDrawn + y) & 2) ? bouncerBaseSpeed : -bouncerBaseSpeed;
          bouncers[l].X = levelChunkDrawn*8;
          bouncers[l].Y = y*8-8;
          break;
        }
      }
      j=0;
    }

    //spawn boss babysitter
    if (j==0x06){
      for(l=0;l<NUMBOSSES;l++){
        if (bosses[l].state==StateDead){
          bosses[l].direction = MoveDirectionDown;
          bosses[l].animationCounter = 0;
          bosses[l].animationFrame = 0;
          bosses[l].state = StateAlive;
          bosses[l].scatter_x = 0;
          bosses[l].scatter_y = 0;
          bosses[l].deathCounter = 0;
          bosses[l].health = 4;
          bosses[l].invincibilityTimer = 0;
          bosses[l].shootTimer = 150;
          bosses[l].X = levelChunkDrawn*8;
          bosses[l].Y = (y*8)-16;
          break;
        }
      }
      j=0;
    }

    

    bg_buffer[k] = j;
    k++;
    level_memory[i+(32*y)] = j; 

    //finished drawing
		if (y==13){
      levelChunkInProgress = 0;
			break;
		}
  }

  //draw to the background
  if (levelChunkY==12)
    set_bkg_tiles(i,levelChunkY,1,2,bg_buffer);
  else
    set_bkg_tiles(i,levelChunkY,1,4,bg_buffer);

  //once the 4 sections are drawn reset it
  levelChunkY+=4;
  if (levelChunkY==16)
    levelChunkY=0;


}


void move_camera(){

  //after a certain threshold start moving the camera
	if (playerX>100){
		newCameraX = playerX - 100;

    //stop scrolling if the player is walking left
    if (newCameraX>cameraX)
      cameraX = newCameraX;

    //ensure camera never exceeds level bounds
    if (cameraX > maxCameraXLimit)
      cameraX = maxCameraXLimit;

    //adjust the level chunk ahead of the player reaching the edge
	  levelChunkCurrent = 22 + (cameraX>>3);

    //dont draw past the end of the level
    if (levelChunkCurrent >= levelWidthTiles)
      levelChunkCurrent = levelWidthTiles - 1;
	}


  


}

void resetGameObjects(){
  for(i=0;i<NUMDOGS;i++){
    dogs[i].state=StateDead;
  }
  for(i=0;i<NUMPLATFORMS;i++){
    platforms[i].state=StateDead;
  }
  for(i=0;i<NUMSAUCERS;i++){
    saucers[i].state=StateDead;
  }
  for(i=0;i<NUMBOUNCERS;i++){
    bouncers[i].state=StateDead;
  }
  for(i=0;i<NUMBOSSES;i++){
    bosses[i].state=StateDead;
  }
}





void drawPlayer()
{
  unsigned char hidePlayer;
  //determine player direction for animation
  if (playerDirection != playerDirectionLast)
  {
    //trigger immediate animation change if direction changes
    animationCounter = AnimationInterval;
    playerDirectionLast = playerDirection;
  }

  //process animation
  if (animationCounter == AnimationInterval)
  {
    animationCounter = 0;
    animationFrame++;

    //LEFT
    if (playerDirection == MoveDirectionLeft)
    {
      if (animationFrame == 2)
      {
        set_sprite_tile(1, BabyLeftAnim1);
        set_sprite_tile(2, BabyLeftAnim2);
        set_sprite_tile(3, BabyLeftAnim3);
        set_sprite_tile(4, BabyLeftAnim4);
      }
      else
      {
        set_sprite_tile(1, BabyLeftAnim5);
        set_sprite_tile(2, BabyLeftAnim6);
        set_sprite_tile(3, BabyLeftAnim7);
        set_sprite_tile(4, BabyLeftAnim8);
      }
    }
    //RIGHT
    if (playerDirection == MoveDirectionRight)
    {
      if (animationFrame == 2)
      {
        set_sprite_tile(1, BabyRightAnim1);
        set_sprite_tile(2, BabyRightAnim2);
        set_sprite_tile(3, BabyRightAnim3);
        set_sprite_tile(4, BabyRightAnim4);
      }
      else
      {
        set_sprite_tile(1, BabyRightAnim5);
        set_sprite_tile(2, BabyRightAnim6);
        set_sprite_tile(3, BabyRightAnim7);
        set_sprite_tile(4, BabyRightAnim8);
      }
    }

    if (animationFrame == 2)
    {
      animationFrame = 0;
    }
  }

  //draw player
	screenX = playerX-cameraX+8; //plus 8 because gameboy x sprites are -8
	screenY = playerY+17; //plus 16 because gameboy x sprites are -16
                        //plus 1 to draw lindsay a little lower
                        //so she looks like she's crawling
  
  //copy for later to make sure player stays on the screen
  playerScreenX = screenX;

  //in order to save on number of math functions
  screenX1 = screenX+8;
  screenY1 = screenY+8;
  

  //flash the player sprite while invincible by toggling visibility
  hidePlayer = 0;
  if (invincibilityTimer > 0 && (invincibilityTimer & invincibilityFlashMask))
    hidePlayer = 1;

  if (hidePlayer || health == 0){
    move_sprite(1, 0, 0);
    move_sprite(2, 0, 0);
    move_sprite(3, 0, 0);
    move_sprite(4, 0, 0);
  }
  else{
    move_sprite(1, screenX, screenY);
    move_sprite(2, screenX1, screenY);
    move_sprite(3, screenX, screenY1);
    move_sprite(4, screenX1, screenY1);
  }



  //draw waa attack
  if (waaCounter>0){
    waaCounter--;
    if (playerDirection == MoveDirectionRight){
      //in order to save on number of math functions
      screenX1 = screenX+16;
      screenX2 = screenX+24;
      screenX3 = screenX+32;
      screenX4 = screenX+40;

      set_sprite_tile(5, 0x0C);
      set_sprite_tile(6, 0x0D);
      set_sprite_tile(7, 0x0E);
      set_sprite_tile(8, 0x0F);
      set_sprite_tile(9, 0x1C);
      set_sprite_tile(10, 0x1D);
      set_sprite_tile(11, 0x1E);
      set_sprite_tile(12, 0x1F);

      move_sprite(5, screenX1,screenY);
      move_sprite(6, screenX2,screenY);
      move_sprite(7, screenX3,screenY);
      move_sprite(8, screenX4,screenY);
      move_sprite(9, screenX1,screenY1);
      move_sprite(10, screenX2,screenY1);
      move_sprite(11, screenX3,screenY1);
      move_sprite(12, screenX4,screenY1);
    }
    else{
      //in order to save on number of math functions
      screenX1 = screenX-32;
      screenX2 = screenX-24;
      screenX3 = screenX-16;
      screenX4 = screenX-8;

      set_sprite_tile(5, 0x2C);
      set_sprite_tile(6, 0x2D);
      set_sprite_tile(7, 0x2E);
      set_sprite_tile(8, 0x2F);
      set_sprite_tile(9, 0x3C);
      set_sprite_tile(10, 0x3D);
      set_sprite_tile(11, 0x3E);
      set_sprite_tile(12, 0x3F);

      move_sprite(5, screenX1,screenY);
      move_sprite(6, screenX2,screenY);
      move_sprite(7, screenX3,screenY);
      move_sprite(8, screenX4,screenY);
      move_sprite(9, screenX1,screenY1);
      move_sprite(10, screenX2,screenY1);
      move_sprite(11, screenX3,screenY1);
      move_sprite(12, screenX4,screenY1);
    }

  }
  else{
    //move sprites off screen
    move_sprite(5,0,0);
    move_sprite(6,0,0);
    move_sprite(7,0,0);
    move_sprite(8,0,0);
    move_sprite(9,0,0);
    move_sprite(10,0,0);
    move_sprite(11,0,0);
    move_sprite(12,0,0);
  }
}




void processController()
{
  //reset player moving flag
  playerMoving = 0;

  if (joypad() & J_RIGHT)
  {
    playerMoving = 1;
    playerDirection = MoveDirectionRight;
  }
  if (joypad() & J_LEFT)
  {
    playerMoving = 1;
    playerDirection = MoveDirectionLeft;
  }
  if (joypad() & J_A)
    jumpButton = 1;
  else
    jumpButton = 0;
  if (joypad() & J_B){
    if (waaCounter==0 && waaButton==0){
      waaCounter = 30;
    }
    waaButton = 1;
  }
  else
    waaButton = 0;

  

  // if (joypad() & J_UP)
  // {
  //   playerMoving = 1;
  // }
  // if (joypad() & J_DOWN)
  // {
  //   playerMoving = 1;
  // }

  //increment counters if player is moving
  if (playerMoving)
  {
    animationCounter++;
    playerMoveCounter++;
  }

  //if player move counter reached its max then move the player
  // if (playerMoveCounter == PlayerMoveInterval)
  {
    //reset counter
    playerMoveCounter = 0;

    //move player
    if (joypad() & J_LEFT)
      leftButton = 1;
    else
      leftButton = 0;
    if (joypad() & J_RIGHT)
      rightButton = 1;
    else
      rightButton = 0;
    // if (joypad() & J_UP)
    //   playerY--;
    // if (joypad() & J_DOWN)
    //   playerY++;
  }
}

clearBackground(){
  //clear screen
  for(i=0;i<360;i++)
  {
    bg_buffer[i] = 0;
  }
  set_bkg_tiles(0,0,20,18,bg_buffer);
}

showTitleScreen(){
  currentGameState = GameStateTitle;
  showGameOverMessage = 0;
  score = 0;
  winTimer = 0;
  levelNum = 1; //start on level 1

  stopMusic(); //no music on title screen
  disableSoundEffects();
  clearBackground();
  clearSprites();

  //set bgk tiles
  for(i=0;i<360;i++)
  {
    bg_buffer[i] = 44; //found a gray tile
  }
  set_bkg_tiles(0,0,20,18,bg_buffer);

  move_win(7, 150);
  SCX_REG = 0;
  printText("LINDSAYS ADVENTURE",1,1);
  printText("PRESS START",5,15);


  //white tile for the spaces between lindsays adventure and press start
  bg_buffer[0] = 250;
  set_bkg_tiles(9,1,1,1,bg_buffer);
  set_bkg_tiles(10,15,1,1,bg_buffer);

  drawSoundOption();

  SWITCH_ROM(GFX_BANK);
  set_bkg_data(0, lind1_tile_count, lind1_tile_data);
  set_bkg_tiles(4, 4, lind1_tile_map_width, lind1_tile_map_height, lind1_map_data);
}

showWinScreen(){
  currentGameState = GameStateWon;
  showGameOverMessage = 0;
  winTimer = 0;
  stopMusic(); //no music on the win screen
  disableSoundEffects();
  clearBackground();
  clearSprites();

  //set bgk tiles
  for(i=0;i<360;i++)
  {
    bg_buffer[i] = 0x10; //found a white tile
    //bg_buffer[i] = 30; //though this random one looks better
  }
  set_bkg_tiles(0,0,20,18,bg_buffer);

  SWITCH_ROM(GFX_BANK);
  //load bird graphic for the win screen
  set_bkg_data(0, bird_small_tile_count, bird_small_tile_data);
  set_bkg_tiles(5, 2, bird_small_tile_map_width, bird_small_tile_map_height, bird_small_map_data);

  move_win(7, 150);
  SCX_REG = 0;
  printText("YOU SAVED",5,14);
  printText("STRAWBERRY SWAN",2,16);

  //white tile for the spaces between you saved and strawberry swan
  bg_buffer[0] = 0x10;
  set_bkg_tiles(8,14,1,1,bg_buffer);
  set_bkg_tiles(12,16,1,1,bg_buffer);
}

initialSetup(){
  //sprite palette (2 options)
  // OBP0_REG = 224; //11100000 - last two zero's don't matter it's always transparent
  OBP0_REG = 208; //11010000 - last two zero's don't matter it's always transparent

  //set background tiles
  SWITCH_ROM(GFX_BANK);
  set_bkg_data(0, man_tile_count, man_tile_data);
  // set_bkg_tiles(0, 0, man_tile_map_width, man_tile_map_height, man_map_data);


  SHOW_WIN;
  SHOW_BKG;
  SHOW_SPRITES;
}

void setLevelTile(int worldX, int worldY, unsigned char tileValue)
{
  tileCollideX = (worldX-1)>>3;
  tileCollideY = worldY>>3;
  tileCollideX &= 31;
  tileCollideTile = tileCollideX+(tileCollideY*32);
  bg_buffer[0] = tileValue;
  set_bkg_tiles(tileCollideX,tileCollideY,1,1,bg_buffer);
  level_memory[tileCollideTile] = tileValue;
}

loadLevel(){

  // set active music track based on level
  SWITCH_ROM(MUSIC_BANK);
  if (levelNum == 1 || levelNum == 2 || levelNum == 3)
  {
    activeChannel1Track = track1Channel1;
    activeChannel2Track = track1Channel2;
    activeChannel3Track = track1Channel3;
    activeLength1 = track1Length1;
    activeLength2 = track1Length2;
    activeLength3 = track1Length3;
    activeTotalFrames = track1TotalFrames;
    activeLoopFrame = track1LoopFrame;
    activeStartFrame = track1StartFrame;
  }
  if (levelNum == 4)
  {
    activeChannel1Track = track2Channel1;
    activeChannel2Track = track2Channel2;
    activeChannel3Track = track2Channel3;
    activeLength1 = track2Length1;
    activeLength2 = track2Length2;
    activeLength3 = track2Length3;
    activeTotalFrames = track2TotalFrames;
    activeLoopFrame = track2LoopFrame;
    activeStartFrame = track2StartFrame;
  }

  //level dimensions live in the levels bank
  SWITCH_ROM(LEVELS_BANK);
  levelWidthTiles = level_lengths[levelNum - 1] / LEVEL_TILE_HEIGHT; //14 rows tall, derive width from data
  maxCameraXLimit = (levelWidthTiles * 8) - 160; //stop camera at end of level

  playerX = 50;
  playerY = 40;
  playerMoving = 0;             
  playerMoveCounter = 0;        
  animationCounter = 0;       
  animationFrame = 0;         
  playerDirection = 2;        
  playerDirectionLast = 1;    
  scrollX = 0;
  cameraX = 0; 
  newCameraX = 0; 
  levelChunkCurrent = 21; 
  levelChunkDrawn = -1; 
  levelChunkY = 0; 
  levelChunkInProgress = 0; //is 4-chunk drawing finished
  waaButton = 0; //if waa button is pressed
  waaCounter = 0; //if baby is going waa
  exitReached = 0;
  drawScore = 1;
  drawLives = 1;
  health = maxHealth;
  drawHealth = 1;
  invincibilityTimer = 0;
  showGameOverMessage = 0;
  winTimer = 0;
  fallResetTimer = 0;
  currentGameState = GameStatePlaying;
  disableNoise = 0;
  
  enableSoundEffects();
  startMusic();

  SWITCH_ROM(GFX_BANK);
  set_bkg_data(0, man_tile_count, man_tile_data);

  clearBackground();
  clearSprites();


  //clear level memory
  for(i=0;i<sizeof(level_memory);i++)
  {
    level_memory[i]=0;
  }

  resetGameObjects();

  //initial 22 columns level draw function 22x4=88
  levelChunkDrawn=-1;
  for(initalLevelDraw=0;initalLevelDraw<88;initalLevelDraw++)
  {
    drawLevelChunk();
  }

  //draw babysitter
  if (levelNum == 1)
  {
    for(j=0;j<4;j++)
    {
      for(i=0;i<2;i++)
      {
        bg_buffer[i] = 48+i+(j*16);
      }
      set_bkg_tiles(2,2+j,2,1,bg_buffer);
    }
  }


  // text_buffer[0] = 33;
  // text_buffer[1] = 'B';
  // text_buffer[2] = '\0';
  // printTextFromBuffer(5,5);

//   printText("COME BACK",5,5);
//   printText("HERE!",7,7);
  

  //setup window layer
  for (i = 0; i < 360; i++)
  {
    win_buffer[i] = 1;
  }
  set_win_tiles(0, 0, 20, 4, win_buffer);
  move_win(7, 112);
  printTextWin("SCORE", 2, 1);
  printTextWin("LIVES", 12, 1);



  //set sprite tiles
  SWITCH_ROM(GFX_BANK);
  set_sprite_data(0, man_tile_count, man_tile_data);
  set_sprite_tile(0, 2);

  // set_sprite_prop(1,S_FLIPY);
  animationCounter = AnimationInterval; //show player on screen immediately by triggering its animation counter

}



void main()
{
  
  initialSetup();

  SWITCH_ROM(MUSIC_BANK);
  soundInit();

  showTitleScreen();

  while (1)
  {
    wait_vbl_done();

    //read controller once per frame
    UINT8 pad = joypad();

    if (soundMasterToggle)
    {
      SWITCH_ROM(MUSIC_BANK);
      
      // play music
      musicUpdate();

      // play sound effects
      waveSfxUpdate();
    }
    

    if (currentGameState==GameStateTitle)
    {
      //toggle sound on the title screen with left/right
      if ((pad & (J_LEFT | J_RIGHT)) && !soundToggleLock)
      {
        soundToggleLock = 1;
        soundMasterToggle = !soundMasterToggle;
        drawSoundOption();
      }

      //prevent rapid toggling
      if (!(pad & (J_LEFT | J_RIGHT)))
        soundToggleLock = 0;

      if (pad & J_START)
      {
        lives = 3;
        drawLives = 1;
        health = maxHealth;
        drawHealth = 1;
        loadLevel();
      }
    }

    if (currentGameState==GameStateWon)
    {
      if (joypad() & J_START)
      {
        showTitleScreen();
      }
      continue;
    }
    
    if (currentGameState==GameStatePlaying)
    {
      //show win message for a fixed duration before returning to title
      if (winTimer>0){
        winTimer--;
        if (winTimer==0){
          if (levelNum == sizeof(level_lengths)/sizeof(level_lengths[0]))
          {
            showWinScreen();
          }
          else
          {
            levelNum++;
            loadLevel();
          }
          
        }
        continue; //pause gameplay updates during the win screen
      }

      //scrolling
      SCX_REG = cameraX;

      //reset sprites
      resetSpriteIndex();

      //process player movement
      processController();

      //process gravity and collision detection
      SWITCH_ROM(ENGINE_BANK);
      runPlayerPhysics();

      //scroll screen
      move_camera();

      //do player animation
      drawPlayer();

      //process enemies (these methods are all in levels bank)
      SWITCH_ROM(ENGINE_BANK);

      //process dogs
      for(i=0;i<NUMDOGS;i++){
        if (dogs[i].state!=StateDead){
          runDog(&dogs[i]);
          drawDog(&dogs[i]);
        }
      }
      //process bouncers
      for(i=0;i<NUMBOUNCERS;i++){
        if (bouncers[i].state!=StateDead){
          runBouncer(&bouncers[i]);
          drawBouncer(&bouncers[i]);
        }
      }
      //process saucers
      for(i=0;i<NUMSAUCERS;i++){
        if (saucers[i].state!=StateDead){
          runSaucer(&saucers[i]);
          drawSaucer(&saucers[i]);
        }
      }
      //process platforms
      for(i=0;i<NUMPLATFORMS;i++){
        if (platforms[i].state!=StateDead){
          runPlatform(&platforms[i]);
          drawPlatform(&platforms[i]);
        }
      }

      //process boss babysitter
      SWITCH_ROM(BOSS_BANK);
      for(i=0;i<NUMBOSSES;i++){
        if (bosses[i].state!=StateDead){
          runBoss(&bosses[i]);
          drawBoss(&bosses[i]);
        }
      }

      //move remaining unused sprites offscreen
      hideSprites();


      //draw more level as the player moves
      drawLevelChunk();

      if (exitReached==1){
        handleExitDoorReached();
      }

      //if the player falls off the level, start a delayed reset
      if (fallResetTimer==0 && playerY>fallDeathY){
        playSadSfx();
        fallResetTimer = fallResetDelayFrames;
        if (lives>0){
          stopMusic();
          lives--;
          if (lives==0)
            showGameOverMessage = 1;
        }
        drawLives = 1;
      }

      if (fallResetTimer>0){
        fallResetTimer--;
        if (fallResetTimer==0){
          if (lives==0){
            showTitleScreen();
            continue; //skip the rest of this frame with stale state
          }
          loadLevel();
          continue; //skip the rest of this frame with stale state
        }
      }

      if (joypad() & J_SELECT)
      {
        // loadLevel();
        showTitleScreen();
      }

      //tick down invincibility timer after collisions have been processed
      if (invincibilityTimer>0){
        invincibilityTimer--;
      }

      //draw score
      if (drawScore==1){
        sprintf(text_buffer,"%d",score);
        printTextFromBuffer(8, 1);
        drawScore = 0;
      }

      if (drawLives==1){
        sprintf(text_buffer,"%d",lives);
        printTextFromBuffer(18, 1);
        drawLives = 0;
      }

      if (drawHealth==1){
        drawHealthBar();
        drawHealth = 0;
      }

      //display game over message during reset countdown
      if (showGameOverMessage==1 && fallResetTimer>0){
        printTextWin("GAME OVER",6,0);
      }
    }


    

    /* DEBUG PRINTING */

    // sprintf(text_buffer,"PLAT %d  ",platforms[0].Y);
    // sprintf(text_buffer, "NEGATIVE %d ", negativeNumber );
    // sprintf(text_buffer, "X %d Y %d S %d ", saucers[0].X, saucers[0].Y, saucers[0].swoopPhase);
    // sprintf(text_buffer,"T %d V %d ",tileCollideTile, tileCollideValue);
    // sprintf(text_buffer,"X %d Y %d ",playerX,playerY);
    // sprintf(text_buffer,"%d ",playerX);
    // printTextFromBuffer(1, 3);
    // printText("GAME SCREEN",5,5);

    /* END DEBUG PRINTING */


    //FOR DEBUGGING SCANLINE
    NR41_REG = 0;
  }
}
