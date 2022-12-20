#include <LiquidCrystal.h>
#include <EEPROM.h>
#include "LedControl.h"
#include "pitches.h"  // include a library with pitch values for musical notes

byte heartChar[8] = {
  0b00000,
  0b01010,
  0b11111,
  0b11111,
  0b01110,
  0b00100,
  0b00000,
  0b00000
};

byte arrowUpChar[8] = {
  0b00100,
  0b01110,
  0b11111,
  0b00100,
  0b00100,
  0b00100,
  0b00100,
  0b00100
};

byte arrowDownChar[8] = {
  0b00100,
  0b00100,
  0b00100,
  0b00100,
  0b00100,
  0b11111,
  0b01110,
  0b00100
};

byte verticalArrowsChar[8] = {
	0b00100,
	0b01110,
	0b11111,
	0b00000,
	0b00000,
	0b11111,
	0b01110,
	0b00100
};

const uint64_t menuImages[] = {
  //start game
  0x061e7efe7e1e0600,
  //highscore
  0x7e1818181c181800,
  //settings
  0x0347ee7c381c0e04,
  //about
  0x180018183c3c1800,
  //howtoplay
  0x1800183860663c00
  
};


char *aboutText[] = { "GH: radumihai8dasdasdasdasdasdasdasd", "Radu Mihai",  "Eat Cookies"};
char aboutTextIndex = 0;

char *howToPlayText[] = { "3 levels", "1-3 lives", "Time decreases", "Points increase", "Eat cookies", "Wall can kill",  "Watch the time"};
char howToPlayTextIndex = 0;

const int BUZZER_PIN = 3;
const byte dinPin = 12;
const byte clockPin = A4;
const byte loadPin = 10;
const byte matrixSize = 8;
 
const int pinSW = 2; // digital pin connected to switch output
const int pinX = A0; // A0 - analog pin connected to X output
const int pinY = A1; // A1 - analog pin connected to Y output

const int lcdBrightnessPin = 11;
const int minLcdBrightness = 40;
const int maxLcdBrightness = 100;

int xValue = 0;
int yValue = 0;

byte swState = LOW;
byte lastSwState = HIGH;

bool joyMoved = false;
int minThreshold = 400;
int maxThreshold = 600;

const byte rs = 9;
const byte en = 8;
const byte d4 = 7;
const byte d5 = 6;
const byte d6 = 13;
const byte d7 = 4;

LedControl lc = LedControl(dinPin, clockPin, loadPin, 1); //DIN, CLK, LOAD, No. DRIVER
byte matrixBrightness = 2;
 
const byte lcdContrastPin = 5;
 
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);
byte lcdContrast = 120;
int currentMenuOption = 1;
int currentMenuSubOption = 1;

// 0 - greeting, 1 - menu, 2 - submenu, 3 - game, 4 - ending, 5 - enter name
int currentState = 0;

int startTime = 0;

int currentDifficulty = 1;
int currentMatrixBrightness = 10;
int currentLcdBrightness = 100;
bool currentSounds = 1;

int contrastAddress = 0;
int difficultyAddress = 1;
int matrixBrightnessAddress = 2;
int soundsAddress = 3;
//int nameAddress = 4;
int lcdBrightnessAddress = 5;


// TODO: retrieve from eeprom
int highScore = 1;

int currentScore = 0;

// X, Y position
const int foodBlinkDelay = 600; // time in milliseconds to delay 

int currentPlayerPosition[2] = {0, 0};

const int MAX_X = 8; // maximum x position
const int MAX_Y = 8; // maximum y position

//Game foods
// Generate random food position and point value
const int levelNumber = 3;
const int difficultyNumber = 3;
const int pointsByLevel[levelNumber] = {1, 2, 5};
const int timeByLevel[levelNumber] = {30, 20, 10};

const int foodTimeByDifficulty[difficultyNumber] = {8000, 6500, 5000};
long long foodSpawnTime = 0;
int currentLevel = 1;

bool foodState = 1;
long long foodLastBlink = 0;
int foodPos[2] = {0, 0}; // array to hold x and y positions

// Level stuff

long long levelStartTime = 0;

int livesByDifficulty[difficultyNumber] = {3, 2, 1};
int currentLives = 3;

//Walls
// Set min_x and max_x so wall isn't too high or too low
int WALL_MIN_X = 2;
int WALL_MAX_X = 5;

int wallLengthByLevel[levelNumber] = {2,3,4};
int wallPos[2] = {3,0};
long long lastWallMovement = millis();
int wallDirection = 0;

bool playingCoinCollectSound = false;
bool playingLevelUpSound = false;

bool newHighscore = false;

// Name 

int currentLetterPosition = 0;
const int MAX_NAME_LENGTH = 6;
char playerName[MAX_NAME_LENGTH+1] = "      ";
int firstNameAddress[2] = {6, 6+MAX_NAME_LENGTH};
int firstScoreAddress = 6 + MAX_NAME_LENGTH + 1;
int secondNameAddress[2] = {6, 6+ 2 * MAX_NAME_LENGTH};
int secondScoreAddress = 6 + 2 * MAX_NAME_LENGTH + 1;
int thirdNameAddress[2] = {6, 6+ 3 * MAX_NAME_LENGTH};
int thirdScoreAddress = 6 + 3 * MAX_NAME_LENGTH + 1;
int fourthNameAddress[2] = {6, 6+ 4 * MAX_NAME_LENGTH};
int fourthScoreAddress = 6 + 4 * MAX_NAME_LENGTH + 1;
int fifthNameAddress[2] = {6, 6+ 5 * MAX_NAME_LENGTH};
int fifthScoreAddress = 6 + 5 * MAX_NAME_LENGTH + 1;

long long lastScrollTextTime = 0;
const int scrollTextTime = 500;

void setup() {

  Serial.begin(9600);
  randomSeed(analogRead(4));
  pinMode(lcdContrastPin, OUTPUT);
  pinMode(lcdBrightnessPin, OUTPUT); 
  pinMode(pinSW, INPUT_PULLUP);
 
  lcd.begin(16, 2);
  lcd.createChar(0, heartChar); // create a new custom character (index 0)
  lcd.createChar(1, arrowUpChar); // create a new custom character (index 1)
  lcd.createChar(2, arrowDownChar); // create a new custom character (index 2)
  lcd.createChar(3, verticalArrowsChar); // create a new custom character (index 3)
  lcd.print("Hello!");
  loadSettings();
  analogWrite(lcdContrastPin, lcdContrast);
  startTime = millis();
  //setLcdBrightness(currentLcdBrightness);
  lc.shutdown(0, false); // turn off power saving, enables display
  lc.setIntensity(0, matrixBrightness); // sets brightness (0~15 possible values)
  lc.clearDisplay(0);// clear screen

  foodPos[0] = random(0, MAX_X); // x position
  foodPos[1] = random(0, MAX_Y); // y position

  highScore = EEPROM.read(firstScoreAddress);
  if(highScore == 255){
    highScore = 0;
    //updateHighScore();
  }

  setLcdBrightness();

  highScore = 1;

}
 
void loop() {
  // if(currentMenuOption == 4){
  //   if(millis() - lastScrollTextTime >= scrollTextTime){
  //     lcd.scrollDisplayLeft();
  //     lastScrollTextTime = millis();
  //   }
  // }
  setLcdBrightness();



  playCoinCollectSound();
  playLevelUpSound();

  if(millis()-startTime > 2000 && currentState == 0 ){
    currentState = 1;
    currentMenuOption = 1;
    changeMenuOption();
  }

  xValue = analogRead(pinX);
  yValue = analogRead(pinY);

  if(currentState == 1){

    int oldMenuState = currentMenuOption;
    
    if(joyMovedDown()){
      currentMenuOption += 1;
      joyMoved = true;
    }
    if(joyMovedUp()){
      currentMenuOption -= 1;
      joyMoved = true;
    }

    if(joyMovedLeft()){
      switch(currentMenuOption){
        case 4:
          aboutTextIndex -= 1;
          if(aboutTextIndex < 0)
            aboutTextIndex = 0;
          break;
        case 5:
          howToPlayTextIndex -= 1;
          if(howToPlayTextIndex<0)
            howToPlayTextIndex = 0;
          break;
        default:
          Serial.println("asd");

      }
      
      joyMoved = true;
      changeMenuOption();
    }
    if(joyMovedRight()){
      switch(currentMenuOption){
        case 4:
          aboutTextIndex += 1;
          if(aboutTextIndex > 2)
            aboutTextIndex = 0;
          break;
        case 5:
          howToPlayTextIndex += 1;
          if(howToPlayTextIndex > 5)
            howToPlayTextIndex = 0;
          break;
        default:
          Serial.println("asd" + String(currentMenuOption));

      }
      
      joyMoved = true;
      changeMenuOption();
    }

    if(currentMenuOption > 5){
      currentMenuOption = 1;
    }
    if(currentMenuOption < 1){
      currentMenuOption = 5;
    }

    if(joyMoved && oldMenuState != currentMenuOption){
      changeMenuOption();
    }

    if (joyIsNeutral()) {
      joyMoved = false;
    }

    swState = digitalRead(pinSW);
    if (swState != lastSwState) {
      if (swState == LOW) {
        changeMenuState();
      }

    }

  lastSwState = swState;
  }
  else if(currentState == 2){
    int oldMenuSubState = currentMenuSubOption;
    
    if(joyMovedDown()){
      currentMenuSubOption += 1;
      joyMoved = true;
    }
    if(joyMovedUp()){
      currentMenuSubOption -= 1;
      joyMoved = true;
    }
    if(joyMovedLeft()){
      joyMoved = true;
      decreaseSetting();
    }
      
    if(joyMovedRight()){
      joyMoved = true;
      increaseSetting();
    }
      

    if(currentMenuSubOption > 6){
      currentMenuSubOption = 1;
    }
    if(currentMenuSubOption < 1){
      currentMenuSubOption = 6;
    }

    if(joyMoved && oldMenuSubState != currentMenuSubOption){
      changeMenuSubOption();
    }

    if (joyIsNeutral()) {
      joyMoved = false;
    }

    swState = digitalRead(pinSW);
    if (swState != lastSwState) {
      if (swState == LOW) {
        changeMenuState();
      }
    }

    lastSwState = swState;
  }
  else if(currentState == 4){
    swState = digitalRead(pinSW);
    if (swState != lastSwState) {
      if (swState == LOW) {
        if(newHighscore){
          Serial.println("aici");
          newHighscore = false;
          currentState = 5;
          showNameMenu();
        }
        else{
          resetToMenu();
        }

      }
    }
    lastSwState = swState;
  }
  else if(currentState == 3){
    moveWall();
    printGameTime();
    long long currentMillis = millis();

    for(int i=0; i<=wallLengthByLevel[currentLevel - 1]; i++){
      lc.setLed(0, wallPos[0], wallPos[1]+i, 1);
    }

    if (currentMillis - foodLastBlink >= foodBlinkDelay) {
      // Blink food
      foodState = !foodState;
      lc.setLed(0, foodPos[0], foodPos[1], foodState); // turns on LED at col, row
      foodLastBlink = currentMillis; // update previous time
    }


    if(millis() - foodSpawnTime >= foodTimeByDifficulty[currentDifficulty - 1]){
      //respawn food
      lc.setLed(0, foodPos[0], foodPos[1], 0); // turns off current food position
      Serial.println("Respawning food");
      spawnFood();
    }


    if (currentPlayerPosition[0] == foodPos[0] && currentPlayerPosition[1] == foodPos[1]) {
      // Player has collected food
      // Update score
      playingCoinCollectSound = true;
      currentScore += pointsByLevel[currentLevel - 1];
      // Generate new food
      spawnFood();
      printGameStats();
    }


    //current player is not blinking
    lc.setLed(0, currentPlayerPosition[0], currentPlayerPosition[1], true); // turns on LED at col, row
    if(joyMovedDown()){
      if(!checkCollision(currentPlayerPosition[0]+1, currentPlayerPosition[1])){
        lc.setLed(0, currentPlayerPosition[0], currentPlayerPosition[1], false); // turns off old pos
        currentPlayerPosition[0] += 1;
      }
      joyMoved = true;
    }
    if(joyMovedUp()){
      if(!checkCollision(currentPlayerPosition[0] - 1, currentPlayerPosition[1])){
        lc.setLed(0, currentPlayerPosition[0], currentPlayerPosition[1], false); // turns off old pos
        currentPlayerPosition[0] -= 1;
      }
      joyMoved = true;
    }
    if(joyMovedLeft()){
      if(!checkCollision(currentPlayerPosition[0], currentPlayerPosition[1] - 1)){
        lc.setLed(0, currentPlayerPosition[0], currentPlayerPosition[1], false); // turns off old pos
        currentPlayerPosition[1] -= 1;
      }
      joyMoved = true;
    }
    if(joyMovedRight()){
      if(!checkCollision(currentPlayerPosition[0], currentPlayerPosition[1] + 1)){
        lc.setLed(0, currentPlayerPosition[0], currentPlayerPosition[1], false); // turns off old pos
        currentPlayerPosition[1] += 1;
      }
      joyMoved = true;
    }
    
    if (currentPlayerPosition[0] < 0) {
      currentPlayerPosition[0] = MAX_X - 1;
    } else if (currentPlayerPosition[0] >= MAX_X) {
      currentPlayerPosition[0] = 0;
    }
    if (currentPlayerPosition[1] < 0) {
      currentPlayerPosition[1] = MAX_Y - 1;
    } else if (currentPlayerPosition[1] >= MAX_Y) {
      currentPlayerPosition[1] = 0;
    }

    if (joyIsNeutral()) {
      joyMoved = false;
    }
  
  }
  else if(currentState == 5){
    if(joyMovedRight()){
      currentLetterPosition++;
      joyMoved = true;
    }
    if(joyMovedLeft()){
      currentLetterPosition--;
      joyMoved = true;
    }


    if(joyMovedUp()){
      playerName[currentLetterPosition]++;
      joyMoved = true;
    }
    if(joyMovedDown()){
      playerName[currentLetterPosition]--;
      joyMoved = true;
    }

    if(playerName[currentLetterPosition] <= ' '){
      playerName[currentLetterPosition] = ' ';
    }

    if(playerName[currentLetterPosition] == 'A' - 1){
      playerName[currentLetterPosition] = ' ';
    }

    if(playerName[currentLetterPosition] == ' ' + 1){
      playerName[currentLetterPosition] = 'A';
    }

    if(playerName[currentLetterPosition] > 'Z')
      playerName[currentLetterPosition] = 'Z';


    if(currentLetterPosition < 0)
      currentLetterPosition = 0;
    if(currentLetterPosition >= MAX_NAME_LENGTH)
      currentLetterPosition = MAX_NAME_LENGTH - 1;

    if(joyMoved)
      showNameMenu();

    if (joyIsNeutral()) {
      joyMoved = false;
    }

    swState = digitalRead(pinSW);
    if (swState != lastSwState) {
      if (swState == LOW) {
        updateHighScore();
        resetToMenu();
      }
    }

    lastSwState = swState;
  }

  if(currentState != 5)
    lcd.noCursor();
}

void setLcdBrightness(){
  analogWrite(lcdBrightnessPin, currentLcdBrightness);
}

void resetToMenu(){
  Serial.println("Resetting");
  currentState = 1;
  currentMenuOption = 1;
  changeMenuOption();
}

void moveWall(){
  //0 - move to right, 1 to left
  if(millis() - lastWallMovement > 3000){
    lastWallMovement = millis();
    if(wallPos[1] + 1 + wallLengthByLevel[currentLevel - 1] >= MAX_X && wallDirection == 0)
      wallDirection = 1;
    if(wallPos[1] - 1 < 0 && wallDirection == 1)
      wallDirection = 0;

    if(wallDirection == 1){
      lc.setLed(0, wallPos[0], wallPos[1] + wallLengthByLevel[currentLevel - 1], 0);
      wallPos[1]--;
    }
    else{
      lc.setLed(0, wallPos[0], wallPos[1], 0);
      wallPos[1]++;
    }
    if(checkCollision(currentPlayerPosition[0], currentPlayerPosition[1]))
      die();
    
  }
}

void showNameMenu(){
  lcd.clear();
  lcd.print("Click to save");
  lcd.setCursor(0,1);
  for(int i = 0; i<MAX_NAME_LENGTH; i++)
    lcd.print(playerName[i]);

  lcd.print("<>");
  lcd.setCursor(MAX_NAME_LENGTH+3,1);
  lcd.write((byte)1);
  lcd.setCursor(MAX_NAME_LENGTH+4,1);
  lcd.write((byte)2);
  lcd.setCursor(currentLetterPosition, 1);
  lcd.cursor();
}

void spawnFood(){
  int randomX = random(0, MAX_X); // x position
  int randomY = random(0, MAX_Y); // y position

  while(checkCollision(randomX, randomY)){
    randomX = random(0, MAX_X); // x position
    randomY = random(0, MAX_Y); // y position
  }

  foodPos[0] = randomX;
  foodPos[1] = randomY;

  Serial.println("Spawned food at " + String(foodPos[0]) + " - " + String(foodPos[1]));

  foodSpawnTime = millis();
}

void die(){
  Serial.println("DIEEEEEEEEEEEE");
  lc.setLed(0, currentPlayerPosition[0], currentPlayerPosition[1], 0);

  currentLives --;
  printGameStats();
  playDeathSound();

  if(currentLives == 0)
  {
    endGame();
  }
  else
    respawn();
}

void respawn(){
  currentPlayerPosition[0] = random(0, MAX_X);
  currentPlayerPosition[1] = random(0, MAX_Y);
}

bool checkCollision(int x, int y){
  if (x < 0) {
    x = MAX_X - 1;
  } else if (x >= MAX_X) {
    x = 0;
  }
  if (y < 0) {
    y = MAX_Y - 1;
  } else if (y>= MAX_Y) {
    y = 0;
  }

  return x == wallPos[0] && y >= wallPos[1] && y <= wallPos[1]+wallLengthByLevel[currentLevel - 1];
}

void increaseSetting(){
  if(currentSounds)
    tone(3, 600, 20);
  switch(currentMenuSubOption){
      case 1:
        currentDifficulty += 1;
        if(currentDifficulty > 3)
          currentDifficulty = 3;
        break;
      case 2:
        lcdContrast += 10;
        if(lcdContrast > 170)
          lcdContrast = 170;
        analogWrite(lcdContrastPin, lcdContrast);
        break;
      case 3:
        currentMatrixBrightness += 1;
        if(currentMatrixBrightness > 15){
          currentMatrixBrightness = 15;
        }
        lc.setIntensity(0, currentMatrixBrightness);
        break;
      case 4:
        currentLcdBrightness += 10;
        if(currentLcdBrightness > 100){
          currentLcdBrightness = 100;
        }
        setLcdBrightness();
        break;
      case 5:
        currentSounds = !currentSounds;
        break;
      case 6:
        break;
  }
  changeMenuSubOption();
}

void decreaseSetting(){
  if(currentSounds)
    tone(3, 200, 20);
  switch(currentMenuSubOption){
      case 1:
        currentDifficulty -= 1;
        if(currentDifficulty < 1)
          currentDifficulty = 1;
        break;
      case 2:
        lcdContrast -= 10;
        if(lcdContrast < 80)
          lcdContrast = 80;
        analogWrite(lcdContrastPin, lcdContrast);
        break;
      case 3:
        currentMatrixBrightness -= 1;
        if(currentMatrixBrightness < 0){
          currentMatrixBrightness  = 0;
        }
        lc.setIntensity(0, currentMatrixBrightness);
        break;
      case 4:
        currentLcdBrightness -= 10;
        if(currentLcdBrightness < 0){
          currentLcdBrightness = 0;
        }
        setLcdBrightness();
        break;
      case 5:
        currentSounds = !currentSounds;
        break;
      case 6:
        break;
  }
  changeMenuSubOption();
}

void changeMenuState(){
  if(currentState == 1){
    switch(currentMenuOption){
      case 1:
        startGame();
        break;
      case 2:
        break;
      case 3:
        currentMenuSubOption = 1;
        currentState = 2;
        changeMenuSubOption();
    }
  }
  if(currentState == 2){
    switch(currentMenuSubOption){
      case 1:
        break;
      case 2:
        break;
      case 3:
        break;
      case 4:
        break;
      case 6:
        saveSettings();
        currentState = 1;
        changeMenuOption();
        break;

    }
  }
}

void loadSettings(){
  lcdContrast = EEPROM.read(contrastAddress);
  //currentDifficulty = EEPROM.read(difficultyAddress);
  //currentMatrixBrightness = EEPROM.read(matrixBrightnessAddress);
  currentLcdBrightness = EEPROM.read(lcdBrightnessAddress);
  currentSounds = EEPROM.read(soundsAddress);
}

void saveSettings(){
  Serial.println("Settings saved");
  EEPROM.update(contrastAddress, lcdContrast);
  //EEPROM.update(difficultyAddress, currentDifficulty);
  //EEPROM.update(matrixBrightnessAddress, currentMatrixBrightness);
  EEPROM.update(lcdBrightnessAddress, currentLcdBrightness);
  EEPROM.update(soundsAddress, currentSounds);
}

void updateHighScore(){
  updatePlayer(playerName, highScore, firstNameAddress, firstScoreAddress);
}

void updatePlayer(char* playerName, int score, int nameAddress[2], int scoreAddress) {
  for(int i = 0; i <= MAX_NAME_LENGTH; i++){
    EEPROM.update(nameAddress[0] + i, playerName[i]);
  }

  EEPROM.update(scoreAddress, highScore);
}

void startGame(){
  lc.clearDisplay(0);
  currentLives = livesByDifficulty[currentDifficulty - 1];
  levelStartTime = millis();
  currentState = 3;
  printGameStats();
  currentScore = 5;
}

void printGameStats(){
  lcd.clear();
  lcd.print("P: " + String(currentScore));
  lcd.setCursor(6, 0);
  lcd.write((byte)0);
  lcd.print(": " + String(currentLives));
}

void printGameTime(){
  int leftSeconds = timeByLevel[currentLevel - 1];

  lcd.setCursor(0, 1);

  leftSeconds -= (millis() - levelStartTime) / 1000;

  if(leftSeconds <= 9)
    lcd.print("Time Left: 0" + String(leftSeconds));
  else
    lcd.print("Time Left: " + String(leftSeconds));
  lcd.setCursor(0, 0);
  if(leftSeconds <= 0)
    if(currentLevel == 3)
      endGame();
    else{
      levelUp();

    }
}


void levelUp(){
  //Clear led matrix
  for(int i = 0; i <= MAX_X; i++)
    for(int j = 0; j<= MAX_Y; j++)
      lc.setLed(0, i, j, 0);

  playingLevelUpSound = true;

  currentLevel++;
  lcd.clear();
  lcd.print("Level UP --- " + String(currentLevel));
  lcd.setCursor(0, 1);
  lcd.print(String(pointsByLevel[currentLevel - 1]) + " pts / food");
  lcd.setCursor(0, 0);
  delay(5000);
  lcd.clear();
  printGameStats();

  levelStartTime = millis();
  wallPos[0] = random(WALL_MIN_X,WALL_MAX_X);
}

void endGame(){
  currentState = 4;
  currentLevel = 1;
  currentLives = 3;
  lcd.clear();
  lcd.print("Your score: ");
  lcd.print(currentScore);
  delay(5000);
  lcd.clear();
  if(currentScore > highScore){
    highScore = currentScore;
    newHighscore = true;
    //updateHighScore();
    lcd.print("You beat HS!!");
    lcd.setCursor(0, 1);
    lcd.print("Click to save the score");
    lcd.setCursor(0, 0);
  }
  else{
    lcd.print("Didn't beat HS");
    lcd.setCursor(0, 1);
    lcd.print("Click to retry");
    lcd.setCursor(0, 0);
  }
}

void changeMenuOption(){
  //Serial.println(currentSounds);
  if(currentSounds)
    tone(3, 800, 50);
  lcd.clear();
  lcd.write((byte)3);
  displayImage(menuImages[currentMenuOption-1]);
  switch (currentMenuOption) {
    case 1:
      Serial.println("changing state1111");
      lcd.print("Start game");
      break;
    case 2:
      lcd.print("Highscore: ");
      lcd.setCursor(0,1);
      if(highScore != 0){
        // for(int i = 0; i<MAX_NAME_LENGTH; i++)
        //   lcd.print(playerName[i]);
        for(int i = firstNameAddress[0]; i<=firstNameAddress[1]; i++){
          lcd.print((char)EEPROM.read(i));
        }
        lcd.print(" : " + String(highScore));
      }
      else{
        lcd.print("No highscore yet!");
      }
      break;
    case 3:
      lcd.print("Settings");
      break;
    case 4:
      lcd.print("About <>");
      lcd.setCursor(0,1);
      lcd.print(aboutText[aboutTextIndex]);
      lcd.scrollDisplayLeft();
      break;
    case 5:
      lcd.print("How to play <>");
      lcd.setCursor(0,1);
      lcd.print(howToPlayText[howToPlayTextIndex]);
      break;
    default:
      lcd.print("Default");
      break;
  }
}

void changeMenuSubOption(){
  if(currentSounds)
    tone(3, 800, 50);
  Serial.println("changing state");
  lcd.clear();
  lcd.write((byte)3);

  switch(currentMenuOption){
    case 3:
      switch (currentMenuSubOption) {
        case 1:
          Serial.println("change menu suboption 2");
          lcd.print("Difficulty ");
          lcd.print(currentDifficulty);
          lcd.print("<>");
          break;
        case 2:
          lightDownMatrix();
          Serial.println("change menu suboption 3");
          lcd.print("Contrast ");
          lcd.print(lcdContrast);
          lcd.print("<>");
          break;
        case 3:
          lightUpMatrix();
          Serial.println("change menu suboption 3");
          lcd.print("Mat BRT ");
          lcd.print(currentMatrixBrightness);
          lcd.print("<>");
          break;
        case 4:
          lightUpMatrix();
          Serial.println("change menu suboption 4");
          lcd.print("LCD BRT ");
          lcd.print(currentLcdBrightness);
          lcd.print("<>");
          break;
        case 5:
          lightDownMatrix();
          Serial.println("change menu suboption 5");
          lcd.print("Sounds ");
          lcd.print(currentSounds);
          lcd.print("<>");
          break;
        case 6:
          Serial.println("change menu suboption 6");
          lcd.print("Back to menu");
          break;  
        default:
          lcd.print("Default");
          break;
      }
  }
}

void lightUpMatrix(){
    for(int i = 0; i <= MAX_X; i++)
      for(int j = 0; j<= MAX_Y; j++)
        lc.setLed(0, i, j, 1);
}

void lightDownMatrix(){
  for(int i = 0; i <= MAX_X; i++)
    for(int j = 0; j<= MAX_Y; j++)
      lc.setLed(0, i, j, 0);
}


bool joyIsNeutral(){
  return xValue >= minThreshold && xValue <= maxThreshold && yValue >= minThreshold && yValue <= maxThreshold;
}

bool joyMovedDown(){
  if (xValue < minThreshold && joyMoved == false)
    return true;
  return false;
}

bool joyMovedUp(){
  if (xValue > maxThreshold && joyMoved == false)
    return true;
  return false;
}

bool joyMovedLeft(){
  if (yValue < minThreshold && joyMoved == false)
    return true;
  return false;
}

bool joyMovedRight(){
  if (yValue > maxThreshold && joyMoved == false)
    return true;
  return false;
}


int coinCollectSound[2][2] = {
  // note, duration
  {NOTE_B5, 100},
  {NOTE_E6, 550},
};

int levelUpSound[6][2] = {
  // note, duration
  {NOTE_E6, 125},
  {NOTE_G6, 125},
  {NOTE_E7, 125},
  {NOTE_C7, 125},
  {NOTE_D7, 125},
  {NOTE_G7, 125},
};

int currentSoundRow = 0;
int currentSoundDelay = 0;
unsigned long currentSoundTimer = 0;
long long lastNotePlay = 0;

void playCoinCollectSound(){
  if(playingCoinCollectSound && currentSounds){
    if(millis() - lastNotePlay >= currentSoundDelay){
      tone(3, coinCollectSound[currentSoundRow][0],coinCollectSound[currentSoundRow][1]);
      currentSoundDelay = coinCollectSound[currentSoundRow][1];
      currentSoundRow ++;
      lastNotePlay = millis();
    }

    if(currentSoundRow > 1){
      currentSoundRow = 0;
      currentSoundDelay = 0;
      lastNotePlay = 0;
      playingCoinCollectSound = false;
      Serial.println("SOUND ENDED");
    }
  }
}

void playLevelUpSound(){
  if(playingLevelUpSound && currentSounds){
    if(millis() - lastNotePlay >= currentSoundDelay){
      tone(3, levelUpSound[currentSoundRow][0],levelUpSound[currentSoundRow][1]);
      currentSoundDelay = levelUpSound[currentSoundRow][1];
      currentSoundRow ++;
      lastNotePlay = millis();
    }

    if(currentSoundRow > 5){
      currentSoundRow = 0;
      currentSoundDelay = 0;
      lastNotePlay = 0;
      playingLevelUpSound = false;
      Serial.println("SOUND ENDED");
    }
  }
}

void playDeathSound(){
  if(currentSounds)
    tone(3, 100, 300);
}

void displayImage(uint64_t image) {
  for (int i = 0; i < 8; i++) {
    byte row = (image >> i * 8) & 0xFF;
    for (int j = 0; j < 8; j++) {
      lc.setLed(0, i, j, bitRead(row, j));
    }
  }
}