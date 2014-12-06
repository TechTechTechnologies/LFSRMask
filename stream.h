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
#include "list.h"

using namespace std;

#include "defines.h"

sf::Clock streamClock;

float heatr(float in)
{
  if(in >= 1) in-=1;
  float result = in*3;
  if(result > 1) result = 1;
  return result;
}
float heatg(float in)
{
  if(in >= 1) in-=1;
  float result = in*3-1;
  if(result < 0) result = 0;
  if(result > 1) result = 1;
  return result;
}
float heatb(float in)
{
  if(in >= 1) in-=1;
  float result = in*3-2;
  if(result < 0) result = 0;
  if(result > 1) result = 1;
  return result; 
}



unsigned long LFSR(unsigned long taps, unsigned long state)
{

  unsigned long result, count;

  count = __builtin_popcount(taps&state) + 1;
  count&=1;

  result = ((state << 1) | count);

  return result;

}


class markStream : public sf::SoundStream
{
  virtual bool onGetData(Chunk& data);
  virtual void onSeek(sf::Time timeOffset);

public:
  markStream(unsigned long d);
  ~markStream();

  void reset();
  void draw(unsigned long xoff, unsigned long yoff);
  void drawData();

  void reinit();

  void setBox(unsigned long a,unsigned long b,unsigned long c,unsigned long d);
  void setPos(unsigned long t, unsigned long m);
  unsigned long getTaps();
  unsigned long getMask();

  void tickForward();

  unsigned long depth;
  sf::Int16* samples[2];
  sf::Int16* currSamples;
  sf::Int16* prevSamples;

  char name[256];

  unsigned long seed;
  unsigned short length;
  unsigned long taps, mask;

  unsigned short tickCounter;
  unsigned long sCounter, gCounter;

  float lastTime;
  float nextTime;

  unsigned long m1,m2,t1,t2;
  unsigned long w,h;

  unsigned long decayCounter;

  short current;

  short seeded;
  short updated;
  short direction;

};

markStream::markStream(unsigned long d)
{

  initialize(1,44100);

  depth = d;

  samples[0] = new sf::Int16[LBUFF];
  samples[1] = new sf::Int16[LBUFF];

  reset();
}

markStream::~markStream()
{
  stop();
  delete samples[0];
  delete samples[1];
}

void markStream::reinit()
{
  stop();
  initialize(1,44100);
}

void markStream::reset()
{

  decayCounter = 0;
  current = 0;
  seed = 0;

  currSamples = samples[current];
  prevSamples = samples[(current+1)&1];

  taps = START;
    t1 = START;
    t2 = START+512;
    w = t2-t1+1;
  mask = STARTM+depth;
    m1 = STARTM+depth;
    m2 = STARTM+depth;
    h = m2-m1+1;

  tickCounter=0;
  sCounter = 0;
  gCounter = 0;

  direction = 1;
  seeded = 0;

  updated = 0;

  memset(samples[0], 0, LBUFF*2);
  memset(samples[1], 0, LBUFF*2);
  memset(name, 0, 256);

}

void markStream::tickForward()
{
  if(taps >= t2)
  {
    if(stepType == M_SWITCH && direction == 1)
      ++mask;
    if(stepType == M_SWITCH);
    {
      direction = -1;
    }
  }
  if(taps <= t1)
  {
    if(stepType == M_SWITCH && direction == -1)
      ++mask;
    if(stepType == M_SWITCH);
    {
      direction = 1;
    }
  }


  if(stepType == M_STEP)
    ++taps;
  else if(stepType == M_SWITCH)
    taps+=direction;
  else if(stepType == M_WALK)
  {
    if(rand()%256 < 128)
    {
      ++taps;
    }
    else
    {
      if(rand()%256 < 128)
        ++mask;
      else
        --mask;
    }
  }

  while(taps > t2)
  {
      taps-=w;
      ++mask;
  }
  while(mask > m2)
  {
    mask -=h;
  }
  while(taps < t1)
  {
      taps+=w;
      --mask;
  }
  while(mask < m1)
  {
    mask+=h;
  }


}

bool markStream::onGetData(Chunk& data)
{

  updated = 1;

  if(!seeded) 
  {
    seeded = 1;
    srand(depth);
  }

  unsigned long i;
  unsigned char glitch = 0;
  unsigned long tapsEff;

  data.samples = samples[current];
  data.sampleCount = LBUFF;

  ++current;
  current%=2;

  currSamples = samples[current];
  prevSamples = samples[(current+1)&1];

  for(i = 0; i < LBUFF; ++i)
  {

    if(sCounter == 0 && stepType != M_HOLD)
    {
      tickForward();
    }

    ++sCounter;
    sCounter%=(SRATE*tempo);

    tapsEff = taps&mask;

    if(glitchENA)
    {
      if(gCounter < GLEN)
        tapsEff = 0;
      ++gCounter;
      gCounter%=(GRATE*glitchRate);
    }

    if(tickCounter==0)
      seed = (LFSR(tapsEff, seed));
    ++tickCounter;
    tickCounter%=LRATE;

    samples[current][i] = (10000) * ((seed&1)-.5);
    if(HPF)
    {
      if(i > 0)
        samples[current][i] -= samples[current][i-1]/FILTER;
      else
        samples[current][i] -= samples[(current+1)&1][LBUFF-1]/FILTER;
    }
#ifdef LPF
    if(i > 0)
      samples[current][i] += samples[current][i-1]/FILTER;
    else
      samples[current][i] += samples[(current+1)&1][LBUFF-1]/FILTER;
#endif
  }

  return true;
}

void markStream::draw(unsigned long xoff, unsigned long yoff)
{
  short i;
  glBegin(GL_POINTS);
    glColor4f(0,1,0, getVolume());
    glVertex2d(taps-xoff, W-(mask-yoff+W/2));
  glEnd();
}

void markStream::drawData()
{
  unsigned long i;

  glBegin(GL_LINE_STRIP);
  for(i = 0; i < LBUFF; ++i)
  {
    glVertex2d(streamX+i*streamScaleX, currSamples[i]*streamScale+streamY);
  }
  glEnd();
  if(updated)
  {
    glBegin(GL_POINTS);
      glVertex2d(streamX + 8 + streamWidth, streamY);
    glEnd();
  }

  streamY-=streamHeight*2;

  updated = 0;

}

void markStream::setBox(unsigned long a,unsigned long b,unsigned long c,unsigned long d)
{
  if(a < b)
  {
    t1 = a;
    t2 = b;
  }
  else
  {
    t1 = b;
    t2 = a;
  }
  if(c < d)
  {
    m1 = c;
    m2 = d;
  }
  else
  {
    m1 = d;
    m2 = c;
  }
  w = t2-t1+1;
  h = m2-m1+1;
}

void markStream::setPos(unsigned long t, unsigned long m)
{
  taps = t;
  mask = m;
}

unsigned long markStream::getTaps()
{
  return taps;
}
unsigned long markStream::getMask()
{
  return mask;
}


void markStream::onSeek(sf::Time timeOffset)
{

}
