#define MOUSE sf::Mouse::getPosition(*window)

#define SRAD 4

#define FILTER 1.1
#define BOX

#define START 0
#define STARTM 0xFCC

#define LBUFF 4200
#define SRATE 4200
#define GRATE 4200
#define GLEN 10

#define W 512
#define WOFF 50

#define BH 28

#define M_STEP 0
#define M_WALK 1
#define M_SWITCH 2
#define M_HOLD 3

#define S_SETALL 0
#define S_SETPOS 1
#define S_SETSPACE 2
#define S_MOVPOS 3
#define B_SET 4
#define M_SET 5
#define S_CREATE 6
#define S_DELETE 7
#define S_PLAY 8
#define S_PAUSE 9
#define B_RESET 10
#define S_END 11
#define S_STARTALL 12
#define S_PAUSEALL 13
#define S_SETVOL 14
#define S_ALL "s*"


  short streamWidth;
  short streamX;
  short streamY;
  short streamYstart;
  float streamScale;
  float streamScaleX;
  float streamHeight;

  unsigned long LRATE = 5;
  unsigned long tempo = 4;
  float tempoRes = .1;
  float lastTime = 0;
  float nextTime = 0;
  unsigned long glitchRate = 6;

  unsigned char glitchENA = 0;
  unsigned char HPF = 1;
  unsigned char stepType = 0;
  unsigned char nStepTypes = 4;

