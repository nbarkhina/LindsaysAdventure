#pragma bank 5

#include "banked_assets.h"

void runBoss(Boss* boss){

  if (boss->state==StateAlive){

    //slow vertical patrolling between ceiling and floor
    boss->animationCounter++;
    if (boss->animationCounter>=4){
      boss->animationCounter=0;
      if (boss->direction==MoveDirectionDown)
        boss->Y++;
      else
        boss->Y--;
    }

    //clamp movement bounds for the tall sprite
    int minY = 8;
    int maxY = 60; //account for 4-tile height
    if (boss->Y < minY){
      boss->Y = minY;
      boss->direction = MoveDirectionDown;
    }
    if (boss->Y > maxY){
      boss->Y = maxY;
      boss->direction = MoveDirectionUp;
    }

    //brief invulnerability after being hit
    if (boss->invincibilityTimer>0)
      boss->invincibilityTimer--;

    //fire a bouncing minion periodically toward the player
    if (boss->shootTimer>0)
      boss->shootTimer--;
    if (boss->shootTimer==0){
      for(l=0;l<NUMBOUNCERS;l++){
        if (bouncers[l].state==StateDead){
          bouncers[l].animationCounter = 0;
          bouncers[l].animationFrame = 0;
          bouncers[l].state = StateAlive;
          bouncers[l].scatter_x = 0;
          bouncers[l].scatter_y = 0;
          bouncers[l].deathCounter = 0;
          signed char vx = (playerX >= boss->X) ? bouncerBaseSpeed : -bouncerBaseSpeed;
          signed char vy = (playerY >= boss->Y) ? bouncerBaseSpeed : -bouncerBaseSpeed;
          bouncers[l].velX = vx * 2;
          bouncers[l].velY = vy * 2;
          bouncers[l].X = boss->X;
          bouncers[l].Y = boss->Y + 8;
          break;
        }
      }
      boss->shootTimer = 90;
    }

    //check collisions with player
    if (((boss->X>=playerX && boss->X<=playerX+16) || (playerX>=boss->X && playerX<=boss->X+16)) &&
    ((boss->Y>=playerY && boss->Y<=playerY+32) || (playerY>=boss->Y && playerY<=boss->Y+32))){
      damagePlayer();
    }

    //check collisions with waa
    if (waaCounter>0 && boss->invincibilityTimer==0){
      if (playerDirection == MoveDirectionRight){
        screenX1 = playerX+16;
        screenX2 = playerX+48;
      }
      else{
        screenX1 = playerX-32;
        screenX2 = playerX;
      }

      if (((boss->X>=screenX1 && boss->X<=screenX2) || (screenX1>=boss->X && screenX1<=boss->X+16)) &&
      ((boss->Y>=playerY && boss->Y<=playerY+32) || (playerY>=boss->Y && playerY<=boss->Y+32))){
        if (boss->health>0)
          boss->health--;

        if (boss->health==0){
          boss->state=StateDying;
          boss->deathCounter=60;
          disableNoise = 1; //disable noise temporarily to make death sound more pronounced
          playNoise();
        }
        else{
          boss->invincibilityTimer = invincibilityDuration;
        }
      }
    }
    
  }else if (boss->state==StateDying)
  {
    boss->deathCounter--;
    if (boss->deathCounter==50){
        playNoise();
    }
    if (boss->deathCounter==40){
        playNoise();
    }
    if (boss->deathCounter==30){
        playNoise();
    }
    if (boss->deathCounter==0){
        boss->state=StateDead;
        disableNoise = 0; //re-enable noise channel
        hideSpriteCount += 2; //8-sprite boss

        //spawn up down platform and bird on top
        for(l=0;l<NUMPLATFORMS;l++){
            if (platforms[l].state==StateDead)
            {
                //platform
                platforms[l].isUpDown=0;
                platforms[l].animationCounter=0;
                platforms[l].moveCounter=0;
                platforms[l].direction=MoveDirectionUp;
                platforms[l].state=StateAlive;
                platforms[l].X = boss->X-32;
                platforms[l].Y = 80;
                platforms[l].isCarryingPlayer = 0;

                // bird on wall
                setLevelTile(boss->X+16,  32, 82);
                setLevelTile(boss->X+24,  32, 83);
                setLevelTile(boss->X+16,  40, 98);
                setLevelTile(boss->X+24,  40, 99);
                setLevelTile(boss->X,   48, 0x2B); // wall
                setLevelTile(boss->X+8,  48, 0x2B);// wall
                setLevelTile(boss->X+16,  48, 0x2B); // wall
                setLevelTile(boss->X+24,  48, 0x2B);// wall
                break;
            }
        }
    }
  }

  if(boss->state==StateAlive && boss->X<playerX-120){
    boss->state=StateDead;
    hideSpriteCount += 2;
  }
  
}

void drawBoss(Boss* boss){

    if(boss->state==StateDead)
      return;

    //set local variables
    screenX = boss->X-cameraX+8; //plus 8 because gameboy x sprites are -8
    screenY = boss->Y+16; //plus 16 because gameboy x sprites are -16

    unsigned char hideBoss = 0;
    if (boss->state==StateAlive && boss->invincibilityTimer>0 && (boss->invincibilityTimer & invincibilityFlashMask))
      hideBoss = 1;

    if(boss->state==StateAlive){

      if (hideBoss){
        move_sprite(spriteIndex, 0,0);spriteIndex++;
        move_sprite(spriteIndex, 0,0);spriteIndex++;
        move_sprite(spriteIndex, 0,0);spriteIndex++;
        move_sprite(spriteIndex, 0,0);spriteIndex++;
        move_sprite(spriteIndex, 0,0);spriteIndex++;
        move_sprite(spriteIndex, 0,0);spriteIndex++;
        move_sprite(spriteIndex, 0,0);spriteIndex++;
        move_sprite(spriteIndex, 0,0);spriteIndex++;
        move_sprite(spriteIndex, 0,0);spriteIndex++;
        move_sprite(spriteIndex, 0,0);spriteIndex++;
        move_sprite(spriteIndex, 0,0);spriteIndex++;
        move_sprite(spriteIndex, 0,0);spriteIndex++;
        move_sprite(spriteIndex, 0,0);spriteIndex++;
        move_sprite(spriteIndex, 0,0);spriteIndex++;
        move_sprite(spriteIndex, 0,0);spriteIndex++;
        return;
      }

      //5 rows by 3 columns

      //row 1
      set_sprite_tile(spriteIndex, 0x4D);
      move_sprite(spriteIndex, screenX,screenY);spriteIndex++;
      set_sprite_tile(spriteIndex, 0x4E);
      move_sprite(spriteIndex, screenX+8,screenY);spriteIndex++;
      set_sprite_tile(spriteIndex, 0x4F);
      move_sprite(spriteIndex, screenX+16,screenY);spriteIndex++;

      //row 2
      set_sprite_tile(spriteIndex, 0x5D);
      move_sprite(spriteIndex, screenX,screenY+8);spriteIndex++;
      set_sprite_tile(spriteIndex, 0x5E);
      move_sprite(spriteIndex, screenX+8,screenY+8);spriteIndex++;
      set_sprite_tile(spriteIndex, 0x5F);
      move_sprite(spriteIndex, screenX+16,screenY+8);spriteIndex++;

      //row 3
      set_sprite_tile(spriteIndex, 0x6D);
      move_sprite(spriteIndex, screenX,screenY+16);spriteIndex++;
      set_sprite_tile(spriteIndex, 0x6E);
      move_sprite(spriteIndex, screenX+8,screenY+16);spriteIndex++;
      set_sprite_tile(spriteIndex, 0x6F);
      move_sprite(spriteIndex, screenX+16,screenY+16);spriteIndex++;

      //row 4
      set_sprite_tile(spriteIndex, 0x7D);
      move_sprite(spriteIndex, screenX,screenY+24);spriteIndex++;
      set_sprite_tile(spriteIndex, 0x7E);
      move_sprite(spriteIndex, screenX+8,screenY+24);spriteIndex++;
      set_sprite_tile(spriteIndex, 0x7F);
      move_sprite(spriteIndex, screenX+16,screenY+24);spriteIndex++;

      //row 5
      set_sprite_tile(spriteIndex, 0x8D);
      move_sprite(spriteIndex, screenX,screenY+32);spriteIndex++;
      set_sprite_tile(spriteIndex, 0x8E);
      move_sprite(spriteIndex, screenX+8,screenY+32);spriteIndex++;
      set_sprite_tile(spriteIndex, 0x8F);
      move_sprite(spriteIndex, screenX+16,screenY+32);spriteIndex++;
    }
    else if (boss->state==StateDying)
    {
      boss->scatter_x+=2;
      boss->scatter_y+=2;
      set_sprite_tile(spriteIndex, 0x03);
      move_sprite(spriteIndex, screenX-boss->scatter_x,screenY-boss->scatter_y);spriteIndex++;
      set_sprite_tile(spriteIndex, 0x03);
      move_sprite(spriteIndex, screenX+boss->scatter_x,screenY-boss->scatter_y);spriteIndex++;
      set_sprite_tile(spriteIndex, 0x03);
      move_sprite(spriteIndex, screenX-boss->scatter_x,screenY+boss->scatter_y);spriteIndex++;
      set_sprite_tile(spriteIndex, 0x03);
      move_sprite(spriteIndex, screenX+boss->scatter_x,screenY+boss->scatter_y);spriteIndex++;
      set_sprite_tile(spriteIndex, 0x03);

      set_sprite_tile(spriteIndex, 0x03);
      move_sprite(spriteIndex, screenX-boss->scatter_x+16,screenY-boss->scatter_y+16);spriteIndex++;
      set_sprite_tile(spriteIndex, 0x03);
      move_sprite(spriteIndex, screenX+boss->scatter_x-16,screenY-boss->scatter_y+16);spriteIndex++;
      set_sprite_tile(spriteIndex, 0x03);
      move_sprite(spriteIndex, screenX-boss->scatter_x+16,screenY+boss->scatter_y-16);spriteIndex++;
      set_sprite_tile(spriteIndex, 0x03);
      move_sprite(spriteIndex, screenX+boss->scatter_x-16,screenY+boss->scatter_y-16);spriteIndex++;
      set_sprite_tile(spriteIndex, 0x03);

      set_sprite_tile(spriteIndex, 0x03);
      move_sprite(spriteIndex, screenX-boss->scatter_x,screenY+8-boss->scatter_y+8);spriteIndex++;
      set_sprite_tile(spriteIndex, 0x03);
      move_sprite(spriteIndex, screenX+boss->scatter_x,screenY-8-boss->scatter_y+8);spriteIndex++;
      set_sprite_tile(spriteIndex, 0x03);
      move_sprite(spriteIndex, screenX-boss->scatter_x,screenY+8+boss->scatter_y-8);spriteIndex++;
      set_sprite_tile(spriteIndex, 0x03);
      move_sprite(spriteIndex, screenX+boss->scatter_x,screenY-8+boss->scatter_y-8);spriteIndex++;
      set_sprite_tile(spriteIndex, 0x03);

      set_sprite_tile(spriteIndex, 0x03);
      move_sprite(spriteIndex, screenX-boss->scatter_x+24,screenY-boss->scatter_y+24);spriteIndex++;
      set_sprite_tile(spriteIndex, 0x03);
      move_sprite(spriteIndex, screenX+boss->scatter_x-24,screenY-boss->scatter_y+24);spriteIndex++;
      set_sprite_tile(spriteIndex, 0x03);
      move_sprite(spriteIndex, screenX-boss->scatter_x+24,screenY+boss->scatter_y+24);spriteIndex++;
      set_sprite_tile(spriteIndex, 0x03);
    }

}
