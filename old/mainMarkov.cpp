#include <cstdlib>
#include <cstring>
#include <memory>
#include <iostream>
#include <fstream>
#include <sstream>
#include <cmath>
#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/System.hpp>
#include <GL/gl.h>
#include <GL/glu.h>

using namespace std;

#define LBUFF 420
#define TICK 2
#define DEPTH 12
#define SCALE 10

unsigned short LFSR(unsigned short taps, unsigned short state)
{

  unsigned short next, result, count;
  count = 1;
  next = (taps & state);
  
  while(next != 0)
  {
    count += (next&1);
    next>>=1;
  }

  count%=2;

  result = ((state << 1) | count);

  return result;

}


class markStream : public sf::SoundStream
{
  virtual bool onGetData(Chunk& data);
  virtual void onSeek(sf::Time timeOffset);

public:
  markStream(unsigned char d);
  ~markStream();

  void reset();
  void draw();

  unsigned char* next;
  char* update;

  unsigned char depth;
  sf::Int16* samples[2];
  unsigned long seed;
  unsigned long length;
  unsigned char current;
  unsigned char tickCounter;
  unsigned long updateCounter;

  float variance;
  float oldVariance;
  float varDiff;
  unsigned long varianceCounter;
};

markStream::markStream(unsigned char d)
{

  initialize(1,44100);

  unsigned long i;
  length = pow(2, d);
  depth = d;

  next = new unsigned char[length];
  update = new char[length];
  samples[0] = new sf::Int16[LBUFF];
  samples[1] = new sf::Int16[LBUFF];

  reset();
}

markStream::~markStream()
{
  delete next;
  delete update;
  delete samples[0];
  delete samples[1];
}
void markStream::reset()
{

  unsigned long i;

  seed = 0;

  for(i = 0; i < length; ++i)
  {
    next[i] = rand()%256;
    update[i] = rand()%8 - 4;
//    next[i] = 255*(i%2);
//    next[i] = i%256;
  }
  for(i = 0; i < LBUFF; ++i)
  {
    samples[0][i] = 0;
    samples[1][i] = 0;
  }

  updateCounter=0;

  current=0;
  tickCounter=0;
  variance = 0;
  varianceCounter = 0;
  varDiff = 0;
  oldVariance = 0;
}

bool markStream::onGetData(Chunk& data)
{

  short i;
  unsigned char testVal;
  unsigned char randVal;

  data.samples = samples[current];
  data.sampleCount = LBUFF;

  ++current;
  current%=2;

  for(i = 0; i < LBUFF; ++i)
  {
    testVal = next[seed];
    randVal = rand()%256;
    seed <<=1;
    seed &= (~(0x1<<depth));
    if(randVal >= testVal)
    {
      samples[current][i] = 10000;
      seed |= 1;
      variance += (256-testVal)*(256-testVal)/(256.0*256);
    }
    else
    {
      samples[current][i] = 0; 
      variance += (testVal)*(testVal)/(256.0*256);   
    }

//    ++varianceCounter;
  }

//  varDiff += (variance - oldVariance);
//  cout << variance/LBUFF << endl;

  variance/=(TICK*LBUFF);

  ++tickCounter;
  tickCounter%=TICK;

  if(tickCounter == 0)
  {
   
    varDiff = variance - oldVariance;
//  cout << varDiff << endl;
//    cout << variance << endl;
    update[updateCounter] -= varDiff*SCALE  + rand()%4 - 2; 

cout << seed << "\t" << updateCounter << "\t" << (int)next[updateCounter] << "\t" << (int)update[updateCounter] << "\t" << variance << endl;


    ++updateCounter;
    if(updateCounter > length)
    {
      cout << endl;
      updateCounter = 1;
    }
    
    varDiff = 0;

    if(update[updateCounter] < 0  && -update[updateCounter] > next[updateCounter])
      next[updateCounter] = 0;
    else if(update[updateCounter] > 0 && update[updateCounter] > 255-next[updateCounter])
      next[updateCounter] = 255;
    else
      next[updateCounter] += update[updateCounter];

    oldVariance = variance;
    variance = 0;


  }


  return true;
}

void markStream::draw()
{
  short i;
  glBegin(GL_LINES);
    for(i = 0; i < 420; ++i)
    {
      glColor3f(samples[current][i]/10000, samples[current][i]/10000, samples[current][i]/10000);
      glVertex2d(i,0);
      glVertex2d(i,420);
    }
  glEnd();
}

void markStream::onSeek(sf::Time timeOffset)
{

}

sf::Window* glInit(short w, short h);

int main(int argc, char *argv[])
{

  short done = 0;

  long timestep = 0;

  char numString[256];

  short x, y ,i, j;

/*Sounds Initialization*/

  markStream* stream = new markStream(DEPTH);
  stream->play();

/*Graphics Initialization*/

  sf::Window* window = glInit(420,420);

/*Main Initialization*/

  sf::Event event;
  sf::Clock clock;
  sf::Time time;

/*Main Loop*/

  clock.restart();
  while(!done)
  {
    ++timestep;

    time = clock.getElapsedTime();


    while(window->pollEvent(event))
    {
      if(event.type == sf::Event::Closed)
        done = 1;
      if(event.type == sf::Event::MouseButtonPressed)
      {
        stream->reset(); 
      }
    }
    if(sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
      done = 1;

    /*A Graphics Phase*/

    glClear(GL_DEPTH_BUFFER_BIT);
    glClear(GL_COLOR_BUFFER_BIT);
    glLoadIdentity();			         	// Reset The View
    glEnable(GL_BLEND);

    glClearColor(0.0f, 0.0f, 1.0f, 0.0f);		

    stream->draw();

    window->display();
  }

  window->close();

  delete stream;

  cout << 1000.0*time.asSeconds()/timestep << endl;

  return 0;
}


sf::Window* glInit(short w, short h)
{

  sf::ContextSettings settings;
  settings.antialiasingLevel = 8;

  sf::Window* window = new sf::Window(sf::VideoMode(w, h), "sounds", sf::Style::Close, settings);
  window->setVerticalSyncEnabled(true);

  glViewport(0, 0, w, h);
  glClearColor(0.0f, 0.0f, 0.0f, 0.0f);		
  glClearDepth(1.0);
  glDepthFunc(GL_LESS);
//  glEnable(GL_DEPTH_TEST);
  glShadeModel(GL_SMOOTH); 

  glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();

  gluOrtho2D(0, w, 0, h);

  glMatrixMode(GL_MODELVIEW);

  glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
//  glBlendFunc(GL_ONE,GL_ONE);

  return window;


}

