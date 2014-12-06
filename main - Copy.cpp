#include"stream.h"

sf::RenderWindow* glInit(short w, short h);

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

  unsigned char boxing = 0;
  unsigned long t1,t2,m1,m2;
  unsigned long nm1,nm2,nt1,nt2;
  unsigned long at1, at2;

  unsigned long xoff = 0;
  unsigned long yoff = 0xFC0;

  unsigned short gap;

//Text

  sf::Text text;

//Lists

  List* streamList;
  markStream* tStream;

  List* textList;
  textbox* tBox;

  List* buttList;
  button* tButt;

//streams

  markStream* stream[NSTREAM];

/*Button Functions*/

void setButton()
{
  short i;

  DOLIST(textList, tBox, textbox,
    if(tBox->target)
    {
      *(tBox->target) = strtoul(tBox->text, 0, 16);
    }
  );

/**********************************************************************************************************/

  DOLIST(streamList, tStream, markStream,
    tStream->setBox(t1, t2, m1, m2);
  );

  for(i=0; i < NSTREAM; ++i)
    stream[i]->setBox(t1, t2, m1, m2); 
}

void distStreams()
{
  short i;
  gap = floor((m2-m1)/NSTREAM);

/**********************************************************************************************************/

  for(i = 0; i < NSTREAM; ++i)
  {
    delete stream[i];
    stream[i] = new markStream(i*gap);

    stream[i]->setPosition(sin(2*3.1415926535*i/NSTREAM),0, cos(2*3.1415926535*i/NSTREAM));
    stream[i]->setRelativeToListener(true);
    stream[i]->play();
    stream[i]->setBox(t1,t2,m1,m2);
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

  char title[256];
  char cString[256];

  t1 = 0;
  t2 = 512;
  m1 = 0xFCC;
  m2 = 0xFFF;

/*Text Initialization*/

  sf::Font font;
  font.loadFromFile("data/cour.ttf");

  text.setFont(font);
  text.setColor(sf::Color::White);

/*Textbox Setup*/

  textList = new List();

//taps

  tBox = new textbox;
  tBox->x = 0;
  tBox->y = W-16-28;
  tBox->w = 84;
  tBox->h = 16;
  tBox->active = 0;
  tBox->target = &t1;
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
  tBox->target = &t2;
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
  tBox->target = &m1;
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
  tBox->target = &m2;
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
  tBox->target = &xoff;
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
  tBox->target = &yoff;
  tBox->l = 8;
  memset(tBox->text, 0, 256);
  sprintf(tBox->text, "%X", *tBox->target);
  sprintf(tBox->name, "View");

  textList->add(tBox);

//  tBox = new textbox;

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

  tButt->x = 46;
  tButt->y = W-16-84;
  tButt->w = 54;
  tButt->h = 16;
  tButt->target = &distStreams;
  tButt->active = 0;
  sprintf(tButt->text, "RESET");

  buttList->add(tButt);


/*Sounds Initialization*/

/**********************************************************************************************************/

  gap = floor((m2-m1)/NSTREAM);

  for(i = 0; i < NSTREAM; ++i)
  {
    stream[i] = new markStream(i*gap);

    stream[i]->setPosition(sin(2*3.1415926535*i/NSTREAM),0, cos(2*3.1415926535*i/NSTREAM));
    stream[i]->setRelativeToListener(true);
    stream[i]->play();
    stream[i]->setBox(t1,t2,m1,m2);
  }

/*Graphics Initialization*/

  sf::RenderWindow* window = glInit(W+2*WOFF+234,W+2*WOFF);

  sf::Image icon;
  icon.loadFromFile("bmp/icon.png");
  window->setIcon(icon.getSize().x, icon.getSize().y, icon.getPixelsPtr());

  sprintf(title, "Taps: 0x%X - 0x%X, Mask 0x%X - 0x%X", t1, t2, m1, m2);
  window->setTitle(sf::String(title));


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
            boxing = 1;
            nt1 = MOUSE.x-WOFF;
            nm1 = MOUSE.y-WOFF-W/2+yoff;
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
/**********************************************************************************************************/
            for(i=0; i < NSTREAM; ++i)
              stream[i]->setBox(t1, t2, m1, m2);
          }
        }

        if(event.type = sf::Event::TextEntered)
        {
          if((event.text.unicode >= '0' && event.text.unicode <= '9') || (event.text.unicode >= 'a' && event.text.unicode <= 'f') || event.text.unicode == 8) 
          {
            DOLIST(textList, tBox, textbox,
              if(tBox->active)
              {
                i = 0;
                while(tBox->text[i]) ++i;
                if(i < tBox->l && event.text.unicode != 8)
                  tBox->text[i] = event.text.unicode;
                else if(i > 0)
                {
                  tBox->text[i-1] = 0;
                }
                  
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
        ++yoff;
      if(sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
        --yoff;
      if(sf::Keyboard::isKeyPressed(sf::Keyboard::Left) && xoff > 0)
        --xoff;
      if(sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
        ++xoff;
    }

     sprintf(title, "Taps: 0x%X - 0x%X, Mask 0x%X - 0x%X, Mouse 0x%X, 0x%X", t1, t2, m1, m2, MOUSE.x-WOFF, MOUSE.y-WOFF+yoff-W/2);
     window->setTitle(sf::String(title));


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

    glBegin(GL_QUADS);

      for(i = 0; i < W; ++i)
      {
        for(j = 0; j < W; ++j)
        {
          maskval = ((i+xoff)&(j+yoff-W/2))/(float)(W);
          glColor3f(heatr(maskval), heatg(maskval), heatb(maskval));
          glVertex2d(i, W-j-1);
          glVertex2d(i+1, W-j-1);
          glVertex2d(i+1, W-j);
          glVertex2d(i, W-j);
        }
      }

    glEnd();

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
      glColor4f(0,0,1,1);
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
/**********************************************************************************************************/
    for(i = 0; i < NSTREAM; ++i)
    {
      stream[i]->draw(xoff, yoff);
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

    /*Display*/

    window->display();
  }

  window->close();

  for(i = 0; i < NSTREAM; ++i)
  {
    delete stream[i];
  }

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

