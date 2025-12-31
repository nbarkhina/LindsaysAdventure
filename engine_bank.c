#pragma bank 4

#include "banked_assets.h"

void runPlayerPhysics(){
  //check collisions
	isDownColliding = 0;
	isUpColliding = 0;
	isLeftColliding = 0;
	isRightColliding = 0;

  // unsigned char readyToJump = 0;
  // unsigned char jumpButton = 0;
  // char downSpeed = 0;
  // #define TERMINAL_VELOCITY = 2;

  //if the player pressed jump then set the downSpeed
  if (readyToJump==1 && jumpButton==1){
    downSpeed = jumpSpeed;
    readyToJump = 0;
    jumpCounter = 15;
  }

  //pressing longer on jump button makes you jump higher
  // if (jumpCounter>0 && jumpButton==1)
  // {
  //   downSpeed = jumpSpeed;
  //   jumpCounter--;
  // }
  // else
  //   jumpCounter=0;

  //apply gravity
  downSpeed += gravity;

  //terminal velocity
  if (downSpeed>terminalVelocity)
    downSpeed = terminalVelocity;

  // if the player didn't fall off the screen keep applying gravity
  if (playerY < 200)
  {
    playerY += downSpeed/10; //divide by 10 instead of using decimals
    // playerY += downSpeed>>4; //this is more efficient but doesnt feel as good - revisit later
  }

  /* DOWN - we test 3 points (X)
  _    _    _    _    _    _    _    _    _    _    _    _    _    _    _    _    
  |    .    .    .    .    .    .    .    .    .    .    .    .    .    .    |    
  |    .    .    .    .    .    .    .    .    .    .    .    .    .    .    |    
  |    .    .    .    .    .    .    .    .    .    .    .    .    .    .    |    
  |    .    .    .    .    .    .    .    .    .    .    .    .    .    .    |    
  |    .    .    .    .    .    .    .    .    .    .    .    .    .    .    |    
  |    .    .    .    .    .    .    .    .    .    .    .    .    .    .    |    
  |    .    .    .    .    .    .    .    .    .    .    .    .    .    .    |    
  |    .    .    .    .    .    .    .    .    .    .    .    .    .    .    |    
  |    .    .    .    .    .    .    .    .    .    .    .    .    .    .    |    
  |    .    .    .    .    .    .    .    .    .    .    .    .    .    .    |    
  |    .    .    .    .    .    .    .    .    .    .    .    .    .    .    |    
  |    .    .    .    .    .    .    .    .    .    .    .    .    .    .    |    
  |    .    .    .    .    .    .    .    .    .    .    .    .    .    .    |    
  |    .    .    .    .    .    .    .    .    .    .    .    .    .    .    |    
  X    _    _    _    _    _    _    _    X    _    _    _    _    _    _    X  
  */

  //down
  if (downSpeed>0 
    //TODO && playerY <= 96 //only check for ground collision if player is not fallling off the bottom of the screen
  ){
    tileCollideY = ((playerY+16)>>3); //metasprite is 16 high
    for(i=0;i<3;i++){ //test 3 points
      if (i==0) // we are testing the bottom left point
        tileCollideX = playerX>>3;
      if (i==1) // bottom middle point
        tileCollideX = ((playerX+8)>>3);
      if (i==2) // bottom right point
        tileCollideX = ((playerX+15)>>3);

      /*

      The reason we are doing & 31 here is that we are testing against levelMemory[] not level1[].
      levelMemory is only 32 tiles wide whereas level1 is much wider. We are trying to determine which
      tile the player is colliding with. tileCollideX is calculating the X coordinate in levelMemory
      and tileCollideY is calculating the Y coordinate in levelMemory. By doing & 31 it is 
      effectively dividing it by 32 and giving us the remainder which is the correct X coordinate.
      
      */

      tileCollideX &= 31;
      tileCollideTile = tileCollideX+(tileCollideY*32);
      tileCollideValue = level_memory[tileCollideTile];
      
      //if we hit something push player out of the block
      //0 empty space is the most common so we make a special case for it and bail out early
      if (tileCollideValue!=0){
        processCollision();
        if (cancelCollision==1)
          continue;

        isDownColliding = 1;
        playerY = (tileCollideY*8)-16;
        if (jumpButton==0)
          readyToJump = 1;
        downSpeed = 0;
        break;
      }
    }
  }

  /* UP - we test 3 points (X)
  X    _    _    _    _    _    _    _    X    _    _    _    _    _    _    X    
  |    .    .    .    .    .    .    .    .    .    .    .    .    .    .    |    
  |    .    .    .    .    .    .    .    .    .    .    .    .    .    .    |    
  |    .    .    .    .    .    .    .    .    .    .    .    .    .    .    |    
  |    .    .    .    .    .    .    .    .    .    .    .    .    .    .    |    
  |    .    .    .    .    .    .    .    .    .    .    .    .    .    .    |    
  |    .    .    .    .    .    .    .    .    .    .    .    .    .    .    |    
  |    .    .    .    .    .    .    .    .    .    .    .    .    .    .    |    
  |    .    .    .    .    .    .    .    .    .    .    .    .    .    .    |    
  |    .    .    .    .    .    .    .    .    .    .    .    .    .    .    |    
  |    .    .    .    .    .    .    .    .    .    .    .    .    .    .    |    
  |    .    .    .    .    .    .    .    .    .    .    .    .    .    .    |    
  |    .    .    .    .    .    .    .    .    .    .    .    .    .    .    |    
  |    .    .    .    .    .    .    .    .    .    .    .    .    .    .    |    
  |    .    .    .    .    .    .    .    .    .    .    .    .    .    .    |    
  |    _    _    _    _    _    _    _    _    _    _    _    _    _    _    |  
  */
  
  //up
  if (downSpeed<0){
    tileCollideY = ((playerY)>>3); //metasprite is 16 high
    for(i=0;i<3;i++){ //test 3 points
      if (i==0) // we are testing the top left point
        tileCollideX = playerX>>3;
      if (i==1) // top middle point
        tileCollideX = ((playerX+8)>>3);
      if (i==2) // top right point
        tileCollideX = ((playerX+15)>>3);
      tileCollideX &= 31;
      tileCollideTile = tileCollideX+(tileCollideY*32);
      tileCollideValue = level_memory[tileCollideTile];
      
      //if we hit something push player out of the block
      //0 empty space is the most common so we make a special case for it and bail out early
      if (tileCollideValue!=0){
        processCollision();
        if (cancelCollision==1)
          continue;

        isUpColliding = 1;
        playerY = (tileCollideY*8)+8;
        downSpeed = 0;
        break;
      }
    }
  }
  
  /* RIGHT - we test 3 points (X)
  _    _    _    _    _    _    _    _    _    _    _    _    _    _    _    X    
  |    .    .    .    .    .    .    .    .    .    .    .    .    .    .    |    
  |    .    .    .    .    .    .    .    .    .    .    .    .    .    .    |    
  |    .    .    .    .    .    .    .    .    .    .    .    .    .    .    |    
  |    .    .    .    .    .    .    .    .    .    .    .    .    .    .    |    
  |    .    .    .    .    .    .    .    .    .    .    .    .    .    .    |    
  |    .    .    .    .    .    .    .    .    .    .    .    .    .    .    |    
  |    .    .    .    .    .    .    .    .    .    .    .    .    .    .    |    
  |    .    .    .    .    .    .    .    .    .    .    .    .    .    .    X    
  |    .    .    .    .    .    .    .    .    .    .    .    .    .    .    |    
  |    .    .    .    .    .    .    .    .    .    .    .    .    .    .    |    
  |    .    .    .    .    .    .    .    .    .    .    .    .    .    .    |    
  |    .    .    .    .    .    .    .    .    .    .    .    .    .    .    |    
  |    .    .    .    .    .    .    .    .    .    .    .    .    .    .    |    
  |    .    .    .    .    .    .    .    .    .    .    .    .    .    .    |    
  |    _    _    _    _    _    _    _    _    _    _    _    _    _    _    X  
  */
  
  //right
  if (lateralSpeed>0)
  {
    tileCollideX = (playerX+16)>>3;
    tileCollideXReal = tileCollideX; //save it before it gets transformed
    for(i=0;i<3;i++){ //test 3 points
      if (i==0) // we are testing the top right point
        tileCollideY = playerY>>3;
      if (i==1) // middle right point
        tileCollideY = ((playerY+8)>>3);
      if (i==2) // bottom right point
        tileCollideY = ((playerY+15)>>3);
       
      tileCollideX &= 31;
      tileCollideTile = tileCollideX+(tileCollideY*32);
      tileCollideValue = level_memory[tileCollideTile];
      //if we hit something push player out of the block
      //0 empty space is the most common so we make a special case for it and bail out early
      if (tileCollideValue!=0)
      {
        processCollision();
        if (cancelCollision==1)
          continue;

        isRightColliding = 1;
        lateralSpeed = 0;
        playerX = (tileCollideXReal*8)-16;
        break;
      }
    }
  }
  
  /* LEFT - we test 3 points (X)
  X    _    _    _    _    _    _    _    _    _    _    _    _    _    _    |    
  |    .    .    .    .    .    .    .    .    .    .    .    .    .    .    |    
  |    .    .    .    .    .    .    .    .    .    .    .    .    .    .    |    
  |    .    .    .    .    .    .    .    .    .    .    .    .    .    .    |    
  |    .    .    .    .    .    .    .    .    .    .    .    .    .    .    |    
  |    .    .    .    .    .    .    .    .    .    .    .    .    .    .    |    
  |    .    .    .    .    .    .    .    .    .    .    .    .    .    .    |    
  |    .    .    .    .    .    .    .    .    .    .    .    .    .    .    |    
  X    .    .    .    .    .    .    .    .    .    .    .    .    .    .    |    
  |    .    .    .    .    .    .    .    .    .    .    .    .    .    .    |    
  |    .    .    .    .    .    .    .    .    .    .    .    .    .    .    |    
  |    .    .    .    .    .    .    .    .    .    .    .    .    .    .    |    
  |    .    .    .    .    .    .    .    .    .    .    .    .    .    .    |    
  |    .    .    .    .    .    .    .    .    .    .    .    .    .    .    |    
  |    .    .    .    .    .    .    .    .    .    .    .    .    .    .    |    
  X    _    _    _    _    _    _    _    _    _    _    _    _    _    _    |  
  */

  //left
  if (lateralSpeed<0)
  {
    tileCollideX = (playerX-1)>>3;
    tileCollideXReal = tileCollideX; //save it before it gets transformed
    for(i=0;i<2;i++){ //test 2 points
      if (i==0) // we are testing the top left point
        tileCollideY = playerY>>3;
      if (i==1) // middle left point
        tileCollideY = ((playerY+8)>>3);
      if (i==2) // bottom left point
        tileCollideY = ((playerY+15)>>3);
      
      tileCollideX &= 31;
      tileCollideTile = tileCollideX+(tileCollideY*32);
      tileCollideValue = level_memory[tileCollideTile];
      //if we hit something push player out of the block. 
      //0 empty space is the most common so we make a special case for it and bail out early
      if (tileCollideValue!=0)
      {
        processCollision();
        if (cancelCollision==1)
          continue;

        isLeftColliding = 1;
        lateralSpeed = 0;
        playerX = (tileCollideXReal*8)+8;
        break;
      }
    }
  }



  if (rightButton && isRightColliding==0)
  {
    lateralSpeed = 5;
  }
  if(leftButton && isLeftColliding==0)
  {
    //prevent player from walking off the left edge of screen
    if (playerScreenX>8)
      lateralSpeed = -5;
    else
      lateralSpeed = 0;
  }
  if (leftButton==0 && rightButton==0)
  {
    lateralSpeed = 0;
  }
  if (lateralSpeed == 0)
    movedAmount = 0;
  else
    movedAmount += lateralSpeed;
  

    playerX += movedAmount/10; //divide by 10 instead of using decimals

  //instead of decimals reset the movedAmount counter every 10
  if (movedAmount>=10)
    movedAmount -= 10;
  if (movedAmount<=-10)
    movedAmount+=10;

}

// this means we collided with a non zero tile
void processCollision(){
  cancelCollision = 0;

  // solid objects
  if (
    tileCollideValue == 0x2B || // wall 1
    tileCollideValue == 0x49 || // wall 2
    tileCollideValue == 0x4A || // wall 3
    tileCollideValue == 0x48 || // wall 4
    tileCollideValue == 0x2A // platform
  )
    return;

  // for non solid objects we cancel the collision
  // and determine what to do below
  cancelCollision = 1;

  //BOTTLES
  if (tileCollideValue==0x24){
    bg_buffer[0] = 0;
    set_bkg_tiles(tileCollideX,tileCollideY,1,1,bg_buffer);
    level_memory[tileCollideTile] = 0;
    score++;
    drawScore = 1;
    playPickupSfx();
  }

  //EXIT DOOR
  if (tileCollideValue==0x52 || tileCollideValue==0x62){
    //remove the exit tile once collected
    // bg_buffer[0] = 0;
    // set_bkg_tiles(tileCollideX, tileCollideY, 1, 1, bg_buffer);
    // level_memory[tileCollideTile] = 0;
    exitReached = 1;
  }

  //IF ITS ANYTHING ELSE LIKE BAKGROUND ART THAT MEANS THE PLAYER
  //DOES NOT INTERACT WITH IT AND WE CAN JUST IGNORE IT
}

void runDog(Dog* dog){

  if (dog->state==StateAlive){
    //do animation
    dog->animationCounter++;
    if(dog->animationCounter==4){
      if (dog->animationFrame==0)
        dog->animationFrame=1;
      else
        dog->animationFrame=0;
      dog->animationCounter=0;
    }

    //move dog
    if (dog->direction==MoveDirectionLeft){
      if(dog->animationCounter==0 || dog->animationCounter==2)
        dog->X--;

      tileCollideY = dog->Y+8;
      tileCollideX = dog->X>>3;
      tileCollideX &= 31;
      tileCollideTile = tileCollideX+(tileCollideY*4);
      tileCollideValue = level_memory[tileCollideTile];
      //if we hit something then reverse
      if (tileCollideValue>0x2A)
      {
        dog->direction=MoveDirectionRight;
      }

    }
    else{
      if(dog->animationCounter==0 || dog->animationCounter==2)
        dog->X++;

      tileCollideY = dog->Y+8;
      tileCollideX = (dog->X+17)>>3;
      tileCollideX &= 31;
      tileCollideTile = tileCollideX+(tileCollideY*4);
      tileCollideValue = level_memory[tileCollideTile];
      //if we hit something then reverse
      if (tileCollideValue>0x2A)
      {
        dog->direction=MoveDirectionLeft;
      }
    }

    //check collisions with player
    if (((dog->X>=playerX && dog->X<=playerX+16) || (playerX>=dog->X && playerX<=dog->X+16)) &&
    ((dog->Y>=playerY && dog->Y<=playerY+16) || (playerY>=dog->Y && playerY<=dog->Y+16))){
      damagePlayer();
    }
    
    //check collisions with waa
    if (waaCounter>0){
      if (playerDirection == MoveDirectionRight){
        screenX1 = playerX+16;
        screenX2 = playerX+48;
      }
      else{
        screenX1 = playerX-32;
        screenX2 = playerX;
      }

      if (((dog->X>=screenX1 && dog->X<=screenX2) || (screenX1>=dog->X && screenX1<=dog->X+16)) &&
      ((dog->Y>=playerY && dog->Y<=playerY+16) || (playerY>=dog->Y && playerY<=dog->Y+16))){
        dog->state=StateDying;
        dog->deathCounter=60;
        playNoise();
        // dog->state=StateDead;
        // hideSpriteCount++; //we need to mark the sprite for removal
      }
    }
    
  }else if (dog->state==StateDying)
  {
    dog->deathCounter--;
    if (dog->deathCounter==0){
        dog->state=StateDead;
        hideSpriteCount++; //we need to mark the sprite for removal
    }
  }

  if(dog->X<playerX-120){
    dog->state=StateDead;
    hideSpriteCount++; //we need to mark the sprite for removal
  }
  
}

void drawDog(Dog* dog){

    if(dog->state==StateDead)
      return;

    //set local variables
    screenX = dog->X-cameraX+8; //plus 8 because gameboy x sprites are -8
    screenY = dog->Y+16; //plus 16 because gameboy x sprites are -16
    screenX1 = screenX+8;
    screenY1 = screenY+8;

    
    if(dog->state==StateAlive){

      set_sprite_tile(spriteIndex, 0x32);
      move_sprite(spriteIndex, screenX,screenY);spriteIndex++;
      set_sprite_tile(spriteIndex, 0x33);
      move_sprite(spriteIndex, screenX1,screenY);spriteIndex++;
      set_sprite_tile(spriteIndex, 0x42);
      move_sprite(spriteIndex, screenX,screenY1);spriteIndex++;
      set_sprite_tile(spriteIndex, 0x43+dog->animationFrame);
      move_sprite(spriteIndex, screenX1,screenY1);spriteIndex++;
    }
    else if (dog->state==StateDying)
    {
      dog->scatter_x+=2;
      dog->scatter_y+=2;
      set_sprite_tile(spriteIndex, 0x03);
      move_sprite(spriteIndex, screenX-dog->scatter_x,screenY-dog->scatter_y);spriteIndex++;
      set_sprite_tile(spriteIndex, 0x03);
      move_sprite(spriteIndex, screenX1+dog->scatter_x,screenY-dog->scatter_y);spriteIndex++;
      set_sprite_tile(spriteIndex, 0x03);
      move_sprite(spriteIndex, screenX-dog->scatter_x,screenY1+dog->scatter_y);spriteIndex++;
      set_sprite_tile(spriteIndex, 0x03);
      move_sprite(spriteIndex, screenX1+dog->scatter_x,screenY1+dog->scatter_y);spriteIndex++;
    }


}

void runSaucer(Saucer* saucer){

  if (saucer->state==StateAlive){

    //animation
    saucer->animationCounter++;
    if(saucer->animationCounter==8){
      if (saucer->animationFrame==0)
        saucer->animationFrame=1;
      else
        saucer->animationFrame=0;
      saucer->animationCounter=0;
    }

    //horizontal movement that chases and overshoots the player
    if (saucer->direction==MoveDirectionLeft){
      if(saucer->animationCounter==0 || saucer->animationCounter==2)
      {
        saucer->X--;
      }

      //turn right after passing just behind the player or hitting drift limit
      if (saucer->X < playerX - saucerChaseLead || saucer->X < saucer->baseX - saucerMaxDrift)
      {
        saucer->direction = MoveDirectionRight;
      }

    }
    else{
      if(saucer->animationCounter==0 || saucer->animationCounter==2)
      {
        saucer->X++;
      }

      //turn left after passing just ahead of the player or hitting drift limit
      if (saucer->X > playerX + saucerChaseLead || saucer->X > saucer->baseX + saucerMaxDrift)
      {
        saucer->direction = MoveDirectionLeft;
      }
    }

    //vertical swoop behaviour
    if (saucer->swoopPhase==0){
      //cruising near the top
      if (saucer->Y > saucer->baseY)
        saucer->Y--;
      if (saucer->swoopCooldown>0)
        saucer->swoopCooldown--;
      else{
        saucer->swoopPhase=1;
        saucer->swoopTimer=75;
      }
    }
    else if (saucer->swoopPhase==1){
      //descending toward the player
      // if (saucer->Y < playerY-8)
      //   saucer->Y+=2;
      // else

        // this doesn't work because I think the compiler is broken
        saucer->Y++;

      if (saucer->swoopTimer>0)
        saucer->swoopTimer--;

      if (saucer->swoopTimer==0){
        saucer->swoopPhase=2;
      }
    }
    else{
      //climb back up to cruising height
      if (saucer->Y > saucer->baseY)
        saucer->Y--;
      else{
        saucer->Y = saucer->baseY;
        saucer->swoopPhase=0;
        saucer->swoopCooldown=90;
      }
    }

    //check collisions with player
    if (((saucer->X>=playerX && saucer->X<=playerX+16) || (playerX>=saucer->X && playerX<=saucer->X+16)) &&
    ((saucer->Y>=playerY && saucer->Y<=playerY+16) || (playerY>=saucer->Y && playerY<=saucer->Y+16))){
      damagePlayer();
    }
    
    //check collisions with waa
    if (waaCounter>0){
      if (playerDirection == MoveDirectionRight){
        screenX1 = playerX+16;
        screenX2 = playerX+48;
      }
      else{
        screenX1 = playerX-32;
        screenX2 = playerX;
      }

      if (((saucer->X>=screenX1 && saucer->X<=screenX2) || (screenX1>=saucer->X && screenX1<=saucer->X+16)) &&
      ((saucer->Y>=playerY && saucer->Y<=playerY+16) || (playerY>=saucer->Y && playerY<=saucer->Y+16))){
        saucer->state=StateDying;
        saucer->deathCounter=60;
        playNoise();
      }
    }
    
  }else if (saucer->state==StateDying)
  {
    saucer->deathCounter--;
    if (saucer->deathCounter==0){
        saucer->state=StateDead;
        hideSpriteCount++; //mark sprite for removal
    }
  }

  if(saucer->X<playerX-120){
    saucer->state=StateDead;
    hideSpriteCount++; //mark sprite for removal
  }
  
}

void drawSaucer(Saucer* saucer){

    if(saucer->state==StateDead)
      return;

    //set local variables
    screenX = saucer->X-cameraX+8; //plus 8 because gameboy x sprites are -8
    screenY = saucer->Y+16; //plus 16 because gameboy x sprites are -16
    screenX1 = screenX+8;
    screenY1 = screenY+8;

    if(saucer->state==StateAlive){

      set_sprite_tile(spriteIndex, 0x27);
      move_sprite(spriteIndex, screenX,screenY);spriteIndex++;
      set_sprite_tile(spriteIndex, 0x28);
      move_sprite(spriteIndex, screenX1,screenY);spriteIndex++;
      if (saucer->animationFrame)
      {
        set_sprite_tile(spriteIndex, 0x39);
        move_sprite(spriteIndex, screenX,screenY1);spriteIndex++;
        set_sprite_tile(spriteIndex, 0x3A);
        move_sprite(spriteIndex, screenX1,screenY1);spriteIndex++;
      }
      else
      {
        set_sprite_tile(spriteIndex, 0x37);
        move_sprite(spriteIndex, screenX,screenY1);spriteIndex++;
        set_sprite_tile(spriteIndex, 0x38);
        move_sprite(spriteIndex, screenX1,screenY1);spriteIndex++;
      }
    }
    else if (saucer->state==StateDying)
    {
      saucer->scatter_x+=2;
      saucer->scatter_y+=2;
      set_sprite_tile(spriteIndex, 0x03);
      move_sprite(spriteIndex, screenX-saucer->scatter_x,screenY-saucer->scatter_y);spriteIndex++;
      set_sprite_tile(spriteIndex, 0x03);
      move_sprite(spriteIndex, screenX1+saucer->scatter_x,screenY-saucer->scatter_y);spriteIndex++;
      set_sprite_tile(spriteIndex, 0x03);
      move_sprite(spriteIndex, screenX-saucer->scatter_x,screenY1+saucer->scatter_y);spriteIndex++;
      set_sprite_tile(spriteIndex, 0x03);
      move_sprite(spriteIndex, screenX1+saucer->scatter_x,screenY1+saucer->scatter_y);spriteIndex++;
    }

}

void runBouncer(Bouncer* bouncer){

  if (bouncer->state==StateAlive){

    //animation
    bouncer->animationCounter++;
    if(bouncer->animationCounter==8){
      if (bouncer->animationFrame==0)
        bouncer->animationFrame=1;
      else
        bouncer->animationFrame=0;
      bouncer->animationCounter=0;
    }

    //movement and bouncing at bounds
    bouncer->X += bouncer->velX;
    bouncer->Y += bouncer->velY;

    int maxX = playerX + 120;
    int minX = playerX - 120;
    int maxY = (14*8) - 24;
    int minY = 8;
    
    if (minX < 8)
      minX = 8;

    if (bouncer->X < minX){
      bouncer->X = minX;
      bouncer->velX = -bouncer->velX;
    }
    if (bouncer->X > maxX){
      bouncer->X = maxX;
      bouncer->velX = -bouncer->velX;
    }
    if (bouncer->Y < minY){
      bouncer->Y = minY;
      bouncer->velY = -bouncer->velY;
    }
    if (bouncer->Y > maxY){
      bouncer->Y = maxY;
      bouncer->velY = -bouncer->velY;
    }

    //check collisions with player
    if (((bouncer->X>=playerX && bouncer->X<=playerX+16) || (playerX>=bouncer->X && playerX<=bouncer->X+16)) &&
    ((bouncer->Y>=playerY && bouncer->Y<=playerY+16) || (playerY>=bouncer->Y && playerY<=bouncer->Y+16))){
      damagePlayer();
    }
    
    //check collisions with waa
    if (waaCounter>0){
      if (playerDirection == MoveDirectionRight){
        screenX1 = playerX+16;
        screenX2 = playerX+48;
      }
      else{
        screenX1 = playerX-32;
        screenX2 = playerX;
      }

      if (((bouncer->X>=screenX1 && bouncer->X<=screenX2) || (screenX1>=bouncer->X && screenX1<=bouncer->X+16)) &&
      ((bouncer->Y>=playerY && bouncer->Y<=playerY+16) || (playerY>=bouncer->Y && playerY<=bouncer->Y+16))){
        bouncer->state=StateDying;
        bouncer->deathCounter=60;
        playNoise();
      }
    }
    
  }else if (bouncer->state==StateDying)
  {
    bouncer->deathCounter--;
    if (bouncer->deathCounter==0){
        bouncer->state=StateDead;
        hideSpriteCount++; //mark sprite for removal
    }
  }

  // if(bouncer->X<playerX-120){
  //   bouncer->state=StateDead;
  //   hideSpriteCount++; //mark sprite for removal
  // }
  
}

void drawBouncer(Bouncer* bouncer){

    if(bouncer->state==StateDead)
      return;

    //set local variables
    screenX = bouncer->X-cameraX+8; //plus 8 because gameboy x sprites are -8
    screenY = bouncer->Y+16; //plus 16 because gameboy x sprites are -16
    screenX1 = screenX+8;
    screenY1 = screenY+8;

    if(bouncer->state==StateAlive){

      if (bouncer->animationFrame)
      {
        set_sprite_tile(spriteIndex, 0x4B);
        move_sprite(spriteIndex, screenX,screenY);spriteIndex++;
        set_sprite_tile(spriteIndex, 0x4C);
        move_sprite(spriteIndex, screenX1,screenY);spriteIndex++;
        set_sprite_tile(spriteIndex, 0x5B);
        move_sprite(spriteIndex, screenX,screenY1);spriteIndex++;
        set_sprite_tile(spriteIndex, 0x5C);
        move_sprite(spriteIndex, screenX1,screenY1);spriteIndex++;
      }
      else
      {
        set_sprite_tile(spriteIndex, 0x56);
        move_sprite(spriteIndex, screenX,screenY);spriteIndex++;
        set_sprite_tile(spriteIndex, 0x57);
        move_sprite(spriteIndex, screenX1,screenY);spriteIndex++;
        set_sprite_tile(spriteIndex, 0x66);
        move_sprite(spriteIndex, screenX,screenY1);spriteIndex++;
        set_sprite_tile(spriteIndex, 0x67);
        move_sprite(spriteIndex, screenX1,screenY1);spriteIndex++;
      }
    }
    else if (bouncer->state==StateDying)
    {
      bouncer->scatter_x+=2;
      bouncer->scatter_y+=2;
      set_sprite_tile(spriteIndex, 0x03);
      move_sprite(spriteIndex, screenX-bouncer->scatter_x,screenY-bouncer->scatter_y);spriteIndex++;
      set_sprite_tile(spriteIndex, 0x03);
      move_sprite(spriteIndex, screenX1+bouncer->scatter_x,screenY-bouncer->scatter_y);spriteIndex++;
      set_sprite_tile(spriteIndex, 0x03);
      move_sprite(spriteIndex, screenX-bouncer->scatter_x,screenY1+bouncer->scatter_y);spriteIndex++;
      set_sprite_tile(spriteIndex, 0x03);
      move_sprite(spriteIndex, screenX1+bouncer->scatter_x,screenY1+bouncer->scatter_y);spriteIndex++;
    }

}

void runPlatform(Platform* platform){

  if (platform->state==StateAlive){

    //test against player collision
    if (playerY+17>platform->Y &&playerY+17<platform->Y+8 &&
      playerX+16>platform->X && playerX+16<platform->X+48){
      playerY = platform->Y-16;
      platform->isCarryingPlayer=1;
      if (jumpButton==0)
        readyToJump = 1;
      downSpeed = 0;
    }
    else
      platform->isCarryingPlayer=0;

    //do animation
    platform->animationCounter++;
    platform->moveCounter++;

    //move platform
    if (platform->direction==MoveDirectionLeft){
      if(platform->moveCounter==3){
        platform->X--;
        platform->moveCounter=0;
        if (platform->isCarryingPlayer==1)
          playerX--;
      }

      if(platform->animationCounter==120){
        platform->animationCounter=0;
        platform->direction=MoveDirectionRight;
      }
    }
    if (platform->direction==MoveDirectionRight){
      if(platform->moveCounter==3){
        platform->X++;
        platform->moveCounter=0;
        if (platform->isCarryingPlayer==1)
          playerX++;
      }

      if(platform->animationCounter==120){
        platform->animationCounter=0;
        platform->direction=MoveDirectionLeft;
      }
    }
    if (platform->direction==MoveDirectionUp){
      if(platform->moveCounter==3){
        platform->Y--;
        platform->moveCounter=0;
        if (platform->isCarryingPlayer==1)
          playerY--;
      }

      if(platform->animationCounter==120){
        platform->animationCounter=0;
        platform->direction=MoveDirectionDown;
      }
    }
    if (platform->direction==MoveDirectionDown){
      if(platform->moveCounter==3){
        platform->Y++;
        //for some reason this needs to be here otherwise ++ bug in SDCC
        //even though this if statement is never true. so bizarre
        if (platform->Y < 0)
        {
          platform->Y = 200;
        }
        platform->moveCounter=0;
        if (platform->isCarryingPlayer==1)
          playerY++;
      }

      if(platform->animationCounter==120){
        platform->animationCounter=0;
        platform->direction=MoveDirectionUp;
      }
    }

    
    if(platform->X<playerX-120){
      platform->state=StateDead;
      hideSpriteCount++; //we need to mark the sprite for removal
    }
  }
  
}

void drawPlatform(Platform* platform){

    if(platform->state==StateDead)
      return;

    //set local variables
    screenX = platform->X-cameraX+8; //plus 8 because gameboy x sprites are -8
    screenY = platform->Y+16; //plus 16 because gameboy x sprites are -16

    set_sprite_tile(spriteIndex, 0x2A);
    move_sprite(spriteIndex, screenX,screenY);spriteIndex++;
    set_sprite_tile(spriteIndex, 0x2A);
    move_sprite(spriteIndex, screenX+8,screenY);spriteIndex++;
    set_sprite_tile(spriteIndex, 0x2A);
    move_sprite(spriteIndex, screenX+16,screenY);spriteIndex++;
    set_sprite_tile(spriteIndex, 0x2A);
    move_sprite(spriteIndex, screenX+24,screenY);spriteIndex++;

}
