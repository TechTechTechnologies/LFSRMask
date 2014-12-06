#include"stream.h"

sf::RenderWindow* glInit(short w, short h);

typedef struct
{
  short x,y,r;
  float xval, yval, scale;
}phasebox;

typedef struct
{
  short x,y,r;
  float r1,g1,b1,r2,g2,b2;
  unsigned char* target;
}indicator;


typedef struct
{
  char text[256];
  char name[256];
  short x, y, w, h, l;
  short active;
  unsigned long* target;
}textbox;

typedef struct
{
  char text[256];
  short x, y, w, h;
  short active;
  void (*target)();
}button;

/*Global Variables*/

  unsigned long nStream = 4;

  unsigned char boxing = 0;
  unsigned long t1,t2,m1,m2;
  unsigned long nm1,nm2,nt1,nt2;
  unsigned long bt1, bt2, bm1, bm2;
  unsigned long at1, at2;

  unsigned long bxoff, byoff;
  unsigned long xoff = 0;
  unsigned long yoff = 0xFC0;
  unsigned char viewChange = 1;

  float gap;

  unsigned char playing = 0;

  float tempFloat;

  phasebox pBox;
  unsigned char phasing = 0;
  markStream* pStream;
  float pFloat;

  unsigned char moving = 0;
  unsigned long xClamp, yClamp;
  markStream* sStream;

  markStream* dStream = 0;

//Text

  sf::Text text;

//Lists

  List* streamList;
  markStream* tStream;

  List* textList;
  textbox* tBox;

  List* buttList;
  button* tButt;

  List* indList;
  indicator* tInd;

/*Button Functions*/

void setButton()
{
  short i;

  if(xoff != bxoff || yoff != byoff) viewChange = 1;

  xoff = bxoff;
  yoff = byoff;

  t1 = bt1;
  t2 = bt2;
  m1 = bm1;
  m2 = bm2;

  DOLIST(streamList, tStream, markStream,
    tStream->setBox(t1, t2, m1, m2);
  );
}

void getButton()
{
  bxoff = xoff;
  byoff = yoff;

  bt1 = t1;
  bt2 = t2;
  bm1 = m1;
  bm2 = m2;

  DOLIST(textList, tBox, textbox, 
    sprintf(tBox->text, "%X", *tBox->target);    
  );


}

void pauseStreams()
{
  playing = 0;
  DOLIST(streamList, tStream, markStream,
    tStream->stop();
  );
}

void playStreams()
{
  playing = 1;
  DOLIST(streamList, tStream, markStream,
    tStream->play();
  );
}

void toggleHPF()
{
  ++HPF;
  HPF&=1;
}

void toggleGlitch()
{
  ++glitchENA;
  glitchENA&=1;
}

void changeStep()
{
  ++stepType;
  stepType%=nStepTypes;
  switch(stepType)
  {
  case 0:
    sprintf(tButt->text, "NORM"); 
    tButt->w = 44;
  break;
  case 1: 
    sprintf(tButt->text, "WALK"); 
    tButt->w = 44;
  break;
  case 2: 
    sprintf(tButt->text, "SWITCH"); 
    tButt->w = 64;
  break;
  }
}

void distStreams()
{
  short i;

  while(streamList->getLength())
  {    
    tStream = (markStream*)streamList->get(0);
    delete tStream;
    streamList->del(0);
  }

  if(nStream > 0)
  {
    gap = ((m2-m1)/(float)nStream);
    gap = floor(gap);

    for(i = 0; i < nStream; ++i)
    {
      tStream = new markStream(round(i*gap));

      tStream->setPosition(sin(2*3.1415926535*i/nStream),0, cos(2*3.1415926535*i/nStream));
      tStream->setRelativeToListener(true);
      tStream->setBox(t1,t2,m1,m2);

      tStream->setPos(t1, m1+i*gap);

      streamList->add(tStream);
    }
  }
}

/*Main*/

int main(int argc, char *argv[])
{

  short done = 0;

  long timestep = 0;

  char numString[256];

  short x, y ,i, j;

  short focus = 1;

  float maskval;

  float* mapColors = new float[3*W*W];

  GLuint mapTexture;

  char title[256];
  char cString[256];

  t1 = 0;
  t2 = 512;
  m1 = 0xFCC;
  m2 = 0xFFF;

  bt1 = t1;
  bt2 = t2;
  bm1 = m1;
  bm2 = m2;

  bxoff = xoff;
  byoff = yoff;

/*Text Initialization*/

  sf::Font font;
  font.loadFromFile("data/cour.ttf");

  text.setFont(font);
  text.setColor(sf::Color::White);

/*Textbox Setup*/

  textList = new List();
  indList = new List();

//taps

  tBox = new textbox;
  tBox->x = 0;
  tBox->y = W-16-28;
  tBox->w = 84;
  tBox->h = 16;
  tBox->active = 0;
  tBox->target = &bt1;
  tBox->l = 8;
  memset(tBox->text, 0, 256);
  memset(tBox->name, 0, 256);
  sprintf(tBox->text, "%X", *tBox->target);

  textList->add(tBox);
  
  tBox = new textbox;
  tBox->x = 96;
  tBox->y = W-16-28;
  tBox->w = 84;
  tBox->h = 16;
  tBox->active = 0;
  tBox->target = &bt2;
  tBox->l = 8;
  memset(tBox->text, 0, 256);
  sprintf(tBox->text, "%X", *tBox->target);
  sprintf(tBox->name, "Taps");

  textList->add(tBox);

//mask

  tBox = new textbox;
  tBox->x = 0;
  tBox->y = W-16-56;
  tBox->w = 84;
  tBox->h = 16;
  tBox->active = 0;
  tBox->target = &bm1;
  tBox->l = 8;
  memset(tBox->text, 0, 256);
  sprintf(tBox->text, "%X", *tBox->target);
  memset(tBox->name, 0, 256);

  textList->add(tBox);


  tBox = new textbox;
  tBox->x = 96;
  tBox->y = W-16-56;
  tBox->w = 84;
  tBox->h = 16;
  tBox->active = 0;
  tBox->target = &bm2;
  tBox->l = 8;
  memset(tBox->text, 0, 256);
  sprintf(tBox->text, "%X", *tBox->target);
  sprintf(tBox->name, "Mask");

  textList->add(tBox);

//position

  tBox = new textbox;
  tBox->x = 0;
  tBox->y = W-16;
  tBox->w = 84;
  tBox->h = 16;
  tBox->active = 0;
  tBox->target = &bxoff;
  tBox->l = 8;
  memset(tBox->text, 0, 256);
  sprintf(tBox->text, "%X", *tBox->target);
  memset(tBox->name, 0, 256);

  textList->add(tBox);


  tBox = new textbox;
  tBox->x = 96;
  tBox->y = W-16;
  tBox->w = 84;
  tBox->h = 16;
  tBox->active = 0;
  tBox->target = &byoff;
  tBox->l = 8;
  memset(tBox->text, 0, 256);
  sprintf(tBox->text, "%X", *tBox->target);
  sprintf(tBox->name, "View");

  textList->add(tBox);

/*Button Setup*/

  buttList = new List();

  tButt = new button;

  tButt->x = 0;
  tButt->y = W-16-84;
  tButt->w = 34;
  tButt->h = 16;
  tButt->target = &setButton;
  tButt->active = 0;
  sprintf(tButt->text, "SET");

  buttList->add(tButt);

  tButt = new button;

  tButt->x = 45;
  tButt->y = W-16-84;
  tButt->w = 34;
  tButt->h = 16;
  tButt->target = &getButton;
  tButt->active = 0;
  sprintf(tButt->text, "GET");

  buttList->add(tButt);

//number of streams

  tBox = new textbox;
  tBox->x = 90;
  tBox->y = W-16-84;
  tBox->w = 24;
  tBox->h = 16;
  tBox->active = 0;
  tBox->target = &nStream;
  tBox->l = 2;
  memset(tBox->text, 0, 256);
  sprintf(tBox->text, "%X", *tBox->target);
  memset(tBox->name, 0, 256);

  textList->add(tBox);

  tButt = new button;

  tButt->x = 125;
  tButt->y = W-16-84;
  tButt->w = 54;
  tButt->h = 16;
  tButt->target = &distStreams;
  tButt->active = 0;
  sprintf(tButt->text, "RESET");

  buttList->add(tButt);


//flow control

  tButt = new button;

  tButt->x = 0;
  tButt->y = W-16-112;
  tButt->w = 44;
  tButt->h = 16;
  tButt->target = &playStreams;
  tButt->active = 0;
  sprintf(tButt->text, "PLAY");

  buttList->add(tButt);

  tButt = new button;

  tButt->x = 56;
  tButt->y = W-16-112;
  tButt->w = 54;
  tButt->h = 16;
  tButt->target = &pauseStreams;
  tButt->active = 0;
  sprintf(tButt->text, "PAUSE");

  buttList->add(tButt);

//Filter control

  tButt = new button;

  tButt->x = 0;
  tButt->y = W-16-140;
  tButt->w = 34;
  tButt->h = 16;
  tButt->target = &toggleHPF;
  tButt->active = 0;
  sprintf(tButt->text, "HPF");

  buttList->add(tButt);

  tInd = new indicator;
  tInd->r = 4;
  tInd->x = 46;
  tInd->y = W-16-140+8;
  tInd->target = &HPF;
  tInd->r1 = 0;
  tInd->g1 = 1;
  tInd->b1 = 0;
  tInd->r2 = .5;
  tInd->g2 = .5;
  tInd->b2 = .5;

  indList->add(tInd);

//step type control

  tButt = new button;

  tButt->x = 58;
  tButt->y = W-16-140;
  tButt->w = 44;
  tButt->h = 16;
  tButt->target = &changeStep;
  tButt->active = 0;
  sprintf(tButt->text, "NORM");

  buttList->add(tButt);


//Glitch control

  tButt = new button;

  tButt->x = 46;
  tButt->y = W-16-168;
  tButt->w = 64;
  tButt->h = 16;
  tButt->target = &toggleGlitch;
  tButt->active = 0;
  sprintf(tButt->text, "GLITCH");

  buttList->add(tButt);

  tInd = new indicator;
  tInd->r = 4;
  tInd->x = 122;
  tInd->y = W-16-168+8;
  tInd->target = &glitchENA;
  tInd->r1 = 0;
  tInd->g1 = 1;
  tInd->b1 = 0;
  tInd->r2 = .5;
  tInd->g2 = .5;
  tInd->b2 = .5;

  indList->add(tInd);


//phasebox

  pBox.r = 80;
  pBox.scale = 2.0/80;
  pBox.x = 90;
  pBox.y = W-16-212-pBox.r;
  pBox.xval = 0;
  pBox.yval = 1;

//glitchrate

  tBox = new textbox;
  tBox->x = 0;
  tBox->y = W-16-168;
  tBox->w = 34;
  tBox->h = 16;
  tBox->active = 0;
  tBox->target = &glitchRate;
  tBox->l = 3;
  memset(tBox->text, 0, 256);
  sprintf(tBox->text, "%X", *tBox->target);
  memset(tBox->name, 0, 256);

  textList->add(tBox);


//frequency box

  tBox = new textbox;
  tBox->x = 0;
  tBox->y = W-16-196;
  tBox->w = 34;
  tBox->h = 16;
  tBox->active = 0;
  tBox->target = &LRATE;
  tBox->l = 3;
  memset(tBox->text, 0, 256);
  sprintf(tBox->text, "%X", *tBox->target);
//  memset(tBox->name, 0, 256);
  sprintf(tBox->name, "Pitch");

  textList->add(tBox);

//tempo box

  tBox = new textbox;
  tBox->x = 96;
  tBox->y = W-16-196;
  tBox->w = 34;
  tBox->h = 16;
  tBox->active = 0;
  tBox->target = &tempo;
  tBox->l = 3;
  memset(tBox->text, 0, 256);
  sprintf(tBox->text, "%X", *tBox->target);
  memset(tBox->name, 0, 256);

  sprintf(tBox->name, "Tempo");

  textList->add(tBox);


//stream draw params

  streamWidth = 234-WOFF;
  streamHeight = 8;
  streamY = W-16-214-2*pBox.r-streamHeight*2;
  streamYstart = streamY;
  streamScale = streamHeight/20000;
  streamScaleX = (float)streamWidth/LBUFF;

/*Sounds Initialization*/

  streamList = new List();

  gap = floor((m2-m1)/nStream);

  for(i = 0; i < nStream; ++i)
  {
    tStream = new markStream(i*gap);

    tStream->setPosition(sin(2*3.1415926535*i/nStream),0, cos(2*3.1415926535*i/nStream));
    tStream->setRelativeToListener(true);
    tStream->setBox(t1,t2,m1,m2);
    streamList->add(tStream);
  }

/*Graphics Initialization*/

  sf::RenderWindow* window = glInit(W+2*WOFF+234,W+2*WOFF);

  sf::Image icon;          glVertex2d(i, W-j-1);
          glVertex2d(i+1, W-j-1);
          glVertex2d(i+1, W-j);
          glVertex2d(i, W-j);

  icon.loadFromFile("bmp/icon.png");
  window->setIcon(icon.getSize().x, icon.getSize().y, icon.getPixelsPtr());

  sprintf(title, "Taps: 0x%X - 0x%X, Mask 0x%X - 0x%X", t1, t2, m1, m2);
  window->setTitle(sf::String(title));

  glEnable(GL_TEXTURE_2D);
  glGenTextures(1, &mapTexture);
  glBindTexture(GL_TEXTURE_2D, mapTexture);

  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);

  glDisable(GL_TEXTURE_2D);

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
      if(event.type == sf::Event::LostFocus)
        focus = 0;
      if(event.type == sf::Event::GainedFocus)
        focus = 1;
      if(focus)
      {
        if(event.type == sf::Event::MouseButtonPressed)
        {
          
          if(MOUSE.x >= WOFF && MOUSE.x < WOFF+W && MOUSE.y >= WOFF && MOUSE.y < WOFF+W)
          {
            if(event.mouseButton.button == sf::Mouse::Left && sf::Keyboard::isKeyPressed(sf::Keyboard::LShift))
            {
              boxing = 1;
              nt1 = MOUSE.x-WOFF;
              nm1 = MOUSE.y-WOFF-W/2+yoff;
            }
            if(event.mouseButton.button == sf::Mouse::Left && !sf::Keyboard::isKeyPressed(sf::Keyboard::LShift))
            {
              moving = 1;

              pFloat = SRAD*SRAD;
              DOLIST(streamList, tStream, markStream,
                tempFloat = (MOUSE.x-WOFF+xoff-tStream->getTaps()) * (MOUSE.x-WOFF+xoff-tStream->getTaps())+
                            (MOUSE.y-WOFF-W/2+yoff-tStream->getMask())*(MOUSE.y-WOFF-W/2+yoff-tStream->getMask());
                if(tempFloat < pFloat)
                {
                  pFloat = tempFloat;
                  sStream = tStream;
                }
              );
            }
            if(event.mouseButton.button == sf::Mouse::Right && !sf::Keyboard::isKeyPressed(sf::Keyboard::LShift))
            {
              tStream = new markStream(0);

              tStream->setPosition(pBox.xval,0, pBox.yval);
              tStream->setRelativeToListener(true);
              tStream->setBox(t1,t2,m1,m2);
              tStream->setPos(MOUSE.x-WOFF, MOUSE.y-WOFF-W/2+yoff);
              if(playing) tStream->play();
              streamList->add(tStream);
            }

            if(event.mouseButton.button == sf::Mouse::Right && sf::Keyboard::isKeyPressed(sf::Keyboard::LShift) && !moving)
            {

              sStream = 0;

              pFloat = SRAD*SRAD;
              DOLIST(streamList, tStream, markStream,
                tempFloat = (MOUSE.x-WOFF+xoff-tStream->getTaps()) * (MOUSE.x-WOFF+xoff-tStream->getTaps())+
                            (MOUSE.y-WOFF-W/2+yoff-tStream->getMask())*(MOUSE.y-WOFF-W/2+yoff-tStream->getMask());
                if(tempFloat < pFloat)
                {
                  pFloat = tempFloat;
                  sStream = tStream;
                }
              );

              if(sStream == 0)
              {
                dStream = 0;
              }
              else if(sStream == dStream)
              {
                DOLIST(streamList, tStream, markStream,
                  if(dStream == tStream)
                  {
                    delete tStream;
                    streamList->delPointer();
                    dStream = 0;
                    sStream = 0;
                    tStream = 0;
                  }
                );
              }
              else
              {
                dStream = sStream;
              }

            }

          }  

          if(event.mouseButton.button == sf::Mouse::Left)
          {
            if((MOUSE.x-W-2*WOFF-pBox.x)*(MOUSE.x-W-2*WOFF-pBox.x) + (W+WOFF-MOUSE.y-pBox.y)*(W+WOFF-MOUSE.y-pBox.y) < pBox.r*pBox.r)
            {
              phasing = 1;
              pFloat = 10000;
              DOLIST(streamList, tStream, markStream,
                tempFloat = ((MOUSE.x-W-2*WOFF-pBox.x)*pBox.scale - tStream->getPosition().x)*((MOUSE.x-W-2*WOFF-pBox.x)*pBox.scale - tStream->getPosition().x)+
                            ((W+WOFF-MOUSE.y-pBox.y)*pBox.scale - tStream->getPosition().z)*((W+WOFF-MOUSE.y-pBox.y)*pBox.scale - tStream->getPosition().z);
                if(tempFloat < pFloat)
                {
                  pFloat = tempFloat;
                  pStream = tStream;
                }
              );
            }
          }


          DOLIST(buttList, tButt, button, 
            tButt->active = 0;
            if(MOUSE.x >= tButt->x+W+2*WOFF && MOUSE.x < tButt->x+W+2*WOFF + tButt->w && 
               MOUSE.y < WOFF+W-tButt->y && MOUSE.y >= WOFF+W-(tButt->y + tButt->h))
            {
              tButt->active = 1;
            }
          );

        }
        if(event.type == sf::Event::MouseButtonReleased)
        {

          if(event.mouseButton.button == sf::Mouse::Right)
          {
            if((MOUSE.x-W-2*WOFF-pBox.x)*(MOUSE.x-W-2*WOFF-pBox.x) + (W+WOFF-MOUSE.y-pBox.y)*(W+WOFF-MOUSE.y-pBox.y) < pBox.r*pBox.r)
            {
              pBox.xval = (MOUSE.x-W-2*WOFF-pBox.x)*pBox.scale;
              pBox.yval = (W+WOFF-MOUSE.y-pBox.y)*pBox.scale;
            }
          }

          if(event.mouseButton.button == sf::Mouse::Left)
          {
            phasing = 0;
            moving = 0;
            sStream = 0;

            DOLIST(textList, tBox, textbox, 
              tBox->active = 0;
              if(MOUSE.x >= tBox->x+W+2*WOFF && MOUSE.x < tBox->x+W+2*WOFF + tBox->w && 
                 MOUSE.y < WOFF+W-tBox->y && MOUSE.y >= WOFF+W-(tBox->y + tBox->h)
                 && tBox->target)
                tBox->active = 1;
            );

            DOLIST(buttList, tButt, button, 
              tButt->active = 0;
              if(MOUSE.x >= tButt->x+W+2*WOFF && MOUSE.x < tButt->x+W+2*WOFF + tButt->w && 
                 MOUSE.y < WOFF+W-tButt->y && MOUSE.y >= WOFF+W-(tButt->y + tButt->h)
                 && tButt->target)
              {
                (*(tButt->target))();
              }
            );

            if(boxing)
            {
              boxing = 0;
              t1 = nt1+xoff;
              t2 = MOUSE.x-WOFF+xoff;
              m1 = nm1;
              m2 = MOUSE.y-WOFF+yoff-W/2;
              DOLIST(streamList, tStream, markStream, 
                tStream->setBox(t1, t2, m1, m2);
              );
            }
          }

        }

        if(event.type = sf::Event::TextEntered)
        {
          if((event.text.unicode >= '0' && event.text.unicode <= '9') || (event.text.unicode >= 'a' && event.text.unicode <= 'f') || (event.text.unicode >= 'A' && event.text.unicode <= 'F') || event.text.unicode == 8) 
          {
            DOLIST(textList, tBox, textbox,
              if(tBox->active)
              {
                if((event.text.unicode >= 'a' && event.text.unicode <= 'f'))
                {
                  event.text.unicode -= 'a'-'A';
                }
                i = 0;
                while(tBox->text[i]) ++i;
                if(i < tBox->l && event.text.unicode != 8)
                  tBox->text[i] = event.text.unicode;
                else if(i > 0 && event.text.unicode == 8)
                {
                  tBox->text[i-1] = 0;
                }
                if(tBox->text[0])
                  *(tBox->target) = strtoul(tBox->text, 0, 16);                  
              }
            );
          }


        }

      }
    }
    if(focus)
    {
      if(sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
        done = 1;

      if(sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
      {
        ++yoff;
        viewChange = 1;
      }
      if(sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
      {
        --yoff;
        viewChange = 1;
      }
      if(sf::Keyboard::isKeyPressed(sf::Keyboard::Left) && xoff > 0)
      {
        --xoff;
        viewChange = 1;
      }
      if(sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
      {
        ++xoff;
        viewChange = 1;
      }
    }

     sprintf(title, "Taps: 0x%X - 0x%X, Mask 0x%X - 0x%X, Mouse 0x%X, 0x%X", t1, t2, m1, m2, MOUSE.x-WOFF, MOUSE.y-WOFF+yoff-W/2);
     window->setTitle(sf::String(title));

    /*Do Phasing*/

    if(phasing && pStream)
    {
      if((MOUSE.x-W-2*WOFF-pBox.x)*(MOUSE.x-W-2*WOFF-pBox.x) + (W+WOFF-MOUSE.y-pBox.y)*(W+WOFF-MOUSE.y-pBox.y) < pBox.r*pBox.r)
      {
        pStream->setPosition((MOUSE.x-W-2*WOFF-pBox.x)*pBox.scale, 0, (W+WOFF-MOUSE.y-pBox.y)*pBox.scale);
      }
    }

    /*Do Moving*/

    if(moving && sStream)
    {
      xClamp = MOUSE.x-WOFF+xoff;
      if(MOUSE.x < WOFF)
        xClamp = xoff;
      if(MOUSE.x >= WOFF+W)
        xClamp = xoff+W-1;

      yClamp = MOUSE.y-WOFF-W/2+yoff;
      if(MOUSE.y < WOFF)
        yClamp = yoff-W/2;
      if(MOUSE.y >= WOFF+W)
        yClamp = W/2+yoff-1;

//      if(MOUSE.y >= WOFF && MOUSE.y < WOFF+W)
      {
        sStream->setPos(xClamp, yClamp);
      }
    }

    /*Update colors*/

    if(viewChange)
    {
      viewChange = 0;
      for(i = 0; i < W; ++i)
      {
        for(j = 0; j < W; ++j)
        {
          maskval = ((i+xoff)&(j+yoff-W/2))/(float)(W);

          mapColors[3*(j+W*i)] = heatr(maskval);
          mapColors[3*(j+W*i)+1] = heatg(maskval);
          mapColors[3*(j+W*i)+2] = heatb(maskval);
/*
          mapColors[3*(j+W*i)] = 1;
          mapColors[3*(j+W*i)+1] = 0;
          mapColors[3*(j+W*i)+2] = 0;
*/
        }
      }

      glEnable(GL_TEXTURE_2D);
      glBindTexture(GL_TEXTURE_2D, mapTexture);
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, W, W, 0, GL_RGB, GL_FLOAT, mapColors);
      glDisable(GL_TEXTURE_2D);

    }

    /*A Graphics Phase*/

    glClear(GL_DEPTH_BUFFER_BIT);
    glClear(GL_COLOR_BUFFER_BIT);
    glLoadIdentity();			         	// Reset The View
    glEnable(GL_BLEND);

    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);		


    window->pushGLStates();

/*SFML*/

    for(i = W/8; i < W; i+= W/8)
    {
      sprintf(cString, "%X", yoff+i-W/2);
      text.setString(sf::String(cString));
      text.setCharacterSize(16);
      text.setPosition(WOFF - text.getGlobalBounds().width - 8 ,WOFF+i - text.getGlobalBounds().height/2 - 6);

      window->draw(text);


      sprintf(cString, "%X", xoff+i);
      text.setString(sf::String(cString));
      text.setCharacterSize(16);
      text.setPosition(WOFF+i - text.getGlobalBounds().width/2 ,WOFF - text.getGlobalBounds().height - 6 - 4);
      window->draw(text);
    }

    /*Render Textboxes text*/
    
    DOLIST(textList, tBox, textbox,
      text.setString(sf::String(tBox->text));
      text.setCharacterSize(16);
      text.setPosition(WOFF*2 + W + tBox->x+2, ceil(W + WOFF - tBox->y - tBox->h/2 - text.getGlobalBounds().height/2 - 6) );

      window->draw(text);

      text.setString(sf::String(tBox->name));
      text.setCharacterSize(16);
      text.setPosition(WOFF*2 + W + tBox->x + tBox->w +4, ceil(W + WOFF - tBox->y - tBox->h/2 - text.getGlobalBounds().height/2 - 6) );

      window->draw(text);
    );

    /*Render buttons text*/

    DOLIST(buttList, tButt, button,
      text.setString(sf::String(tButt->text));
      text.setCharacterSize(16);
      text.setPosition(WOFF*2 + W + tButt->x+2, ceil(W + WOFF - tButt->y - tButt->h/2 - text.getGlobalBounds().height/2 - 6) );

      window->draw(text);
    );

    window->popGLStates();

/*openGL*/

    glTranslated(WOFF, WOFF, 0);

    glPointSize(2);

    /*Render map*/

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, mapTexture);
      glBegin(GL_QUADS);
        glColor3f(1,1,1);

        glTexCoord2d(0.0,0.0);
        glVertex2d(0, W);

        glTexCoord2d(1.0,0.0);
        glVertex2d(W, W);

        glTexCoord2d(1.0,1.0);
        glVertex2d(W, 0);

        glTexCoord2d(0.0,1.0);
        glVertex2d(0, 0);

      glEnd();
    glDisable(GL_TEXTURE_2D);


/*
      for(i = 0; i < W; ++i)
      {
        for(j = 0; j < W; ++j)
        {
//          maskval = ((i+xoff)&(j+yoff-W/2))/(float)(W);
//          glColor3f(heatr(maskval), heatg(maskval), heatb(maskval));
          glColor3f(mapColors[3*(j+W*i)], mapColors[3*(j+W*i)+1], mapColors[3*(j+W*i)+2]);
          glVertex2d(i, W-j-1);
          glVertex2d(i+1, W-j-1);
          glVertex2d(i+1, W-j);
          glVertex2d(i, W-j);
        }
      }
*/

    /*Draw boxes*/

    glBegin(GL_LINE_LOOP);
      glColor4f(0,1,1,.5);
      at1 = (t1 >= xoff) ? t1-xoff : 0;
      at2 = (t2 >= xoff) ? t2-xoff : 0;
      glVertex2d(at1,W/2-(m1-yoff));
      glVertex2d(at1,W/2-(m2-yoff));
      glVertex2d(at2,W/2-(m2-yoff));
      glVertex2d(at2,W/2-(m1-yoff));
    glEnd();

    glBegin(GL_LINE_LOOP);
      glColor3f(0,0,1);
      glVertex2d(-1,-1);
      glVertex2d(W,-1);
      glVertex2d(W,W);
      glVertex2d(-1,W);
    glEnd();

    if(boxing)
    {
        nt2 = MOUSE.x-WOFF;
        nm2 = MOUSE.y-WOFF+yoff-W/2;
      
      glBegin(GL_LINE_LOOP);
        glColor4f(0,1,1,.5);
        glVertex2d(nt1,W/2-(nm1-yoff));
        glVertex2d(nt1,W/2-(nm2-yoff));
        glVertex2d(nt2,W/2-(nm2-yoff));
        glVertex2d(nt2,W/2-(nm1-yoff));
      glEnd();

    }

    /*Draw grid*/

    glBegin(GL_LINES);

      glColor4f(0,0,1, .5);
      for(i = W/8; i < W; i+= W/8)
      {
        glVertex2d(-6, i);
        glVertex2d(W+6, i);

        glVertex2d(i, -6);
        glVertex2d(i, W+6);
      }

    glEnd();

    /*Draw Streams*/

    DOLIST(streamList, tStream, markStream,
      tStream->draw(xoff, yoff);
    );

    /*Draw dStream*/

    if(dStream)
    {
      glColor3f(0,0,1);
      glBegin(GL_LINES);
        glVertex2d((signed int)(dStream->getTaps()-xoff)-4, W-(dStream->getMask()-yoff+W/2)-4);
        glVertex2d((signed int)(dStream->getTaps()-xoff)+4, W-(dStream->getMask()-yoff+W/2)+4);
        glVertex2d((signed int)(dStream->getTaps()-xoff)-4, W-(dStream->getMask()-yoff+W/2)+4);
        glVertex2d((signed int)(dStream->getTaps()-xoff)+4, W-(dStream->getMask()-yoff+W/2)-4);
      glEnd();

    }

    /*Draw pStream*/

    if(phasing)
    {
      glColor3f(0,1,0);
      glBegin(GL_LINE_LOOP);
      for(i = 0; i < 32; ++i)
      {
        glVertex2d(4*cos(3.1415926535*i/16)+pStream->getTaps()-xoff, 4*sin(3.1415926535*i/16)+W-(pStream->getMask()-yoff+W/2));
      }
      glEnd();
    }

    /*Render Textboxes*/

    glTranslated(W+WOFF, 0, 0);

    DOLIST(textList, tBox, textbox,
      glColor3f(1,1,1);
      if(tBox->active) glColor3f(1,1,0);
  
      glBegin(GL_LINE_LOOP);
        glVertex2d(tBox->x, tBox->y);
        glVertex2d(tBox->x+tBox->w, tBox->y);
        glVertex2d(tBox->x+tBox->w, tBox->y+tBox->h);
        glVertex2d(tBox->x, tBox->y+tBox->h);
      glEnd();
    );

    /*Render Indicators*/

    DOLIST(indList, tInd, indicator,
      glColor3f(tInd->r1, tInd->g1, tInd->b1);
      if(*(tInd->target) == 0) glColor3f(tInd->r2, tInd->g2, tInd->b2);
  
      glBegin(GL_TRIANGLE_FAN);
        for(i = 0; i < 32; ++i)
          glVertex2d(tInd->x+tInd->r*cos(3.1415926535*i/8), tInd->y+tInd->r*sin(3.1415926535*i/8));
      glEnd();
    );


    /*Render Buttons*/

    DOLIST(buttList, tButt, button,
      glColor3f(1,1,1);
      if(tButt->active) glColor3f(1,1,0);
  
      glBegin(GL_LINE_LOOP);
        glVertex2d(tButt->x, tButt->y);
        glVertex2d(tButt->x+tButt->w, tButt->y);
        glVertex2d(tButt->x+tButt->w, tButt->y+tButt->h);
        glVertex2d(tButt->x, tButt->y+tButt->h);
      glEnd();
    );

    /*Draw Phasebox*/

    glColor3f(1,1,1);
    glBegin(GL_LINE_LOOP);
    for(i = 0; i < 32; ++i)
    {
      glVertex2d(pBox.r*cos(3.1415926535*i/16)+pBox.x, pBox.r*sin(3.1415926535*i/16)+pBox.y);
    }
    glEnd();

    glColor3f(.25,.25,.25);
    DOLIST(streamList, tStream, markStream,
      tempFloat = sqrt((tStream->getPosition().x)*(tStream->getPosition().x) + (tStream->getPosition().z)*(tStream->getPosition().z))/pBox.scale;
      glBegin(GL_LINE_LOOP);
      for(i = 0; i < 32; ++i)
      {
        glVertex2d(tempFloat*cos(3.1415926535*i/16)+pBox.x, tempFloat*sin(3.1415926535*i/16)+pBox.y);
      }
      glEnd();
    );

    glBegin(GL_POINTS);
      glVertex2d(pBox.x, pBox.y);

      glColor3f(.5,.5,.5);
      DOLIST(streamList, tStream, markStream,
        glVertex2d(pBox.x + (tStream->getPosition().x)/pBox.scale, pBox.y + (tStream->getPosition().z)/pBox.scale);        
      );

      glColor3f(0,1,0);
      glVertex2d(pBox.x + pBox.xval/pBox.scale, pBox.y + pBox.yval/pBox.scale);

    glEnd();

    /*Draw stream data*/

    streamY = streamYstart;
    DOLIST(streamList, tStream, markStream,
      glColor3f(0,1,0);
      if(tStream->getStatus() != sf::SoundSource::Playing) glColor3f(.5,.5,.5);
      tStream->drawData();
    );


    /*Display*/

    window->display();
  }

  window->close();

  DOLIST(streamList, tStream, markStream,
    delete tStream;
  );
  delete streamList;

  DOLIST(textList, tBox, textbox,
    delete tBox;
  );
  delete textList;

  DOLIST(buttList, tButt, button,
    delete tButt;
  );
  delete buttList;

  DOLIST(indList, tInd, indicator,
    delete tInd;
  );
  delete indList;

  delete mapColors;

  cout << 1000.0*time.asSeconds()/timestep << endl;

  return 0;
}


sf::RenderWindow* glInit(short w, short h)
{

  sf::ContextSettings settings;
  settings.antialiasingLevel = 8;

/*
  sf::Window* tempwindow = new sf::Window(sf::VideoMode(w, h), "sounds", sf::Style::Close, settings);
  sf::RenderWindow* window = new sf::RenderWindow();
  window->create(tempwindow->getSystemHandle(), settings);
*/
  sf::RenderWindow* window = new sf::RenderWindow(sf::VideoMode(w, h), "sounds", sf::Style::Close, settings);

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

