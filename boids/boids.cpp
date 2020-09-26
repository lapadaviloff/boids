/*
 Mitroshin Aleksey
lapadaviloff@yandex.ru
(miam.devsoft@gmail.com)
алгоритм движения юнитов)))
ver 1.0

исходный код,с которого все началось: 
FamTrinli: Asteroids   https://www.youtube.com/watch?v=rWaSo2usU4A&t=24s
*/
#include <SFML/Graphics.hpp>
#include <time.h>
#include <list>
#include<iostream>
using namespace sf;

const int W = 1200;
const int H = 800;

float DEGTORAD = 0.017453f;

class Animation
{
public:
    float Frame, speed;
    Sprite sprite;
    std::vector<IntRect> frames;
    Animation():Frame (0),speed (0) {}
    Animation(Texture& t, int x, int y, int w, int h, int count, float Speed)
    {
        Frame = 0;
        speed = Speed;

        for (int i = 0; i < count; i++)
            frames.push_back(IntRect(x + i * w, y, w, h));

        sprite.setTexture(t);
        sprite.setOrigin(w / 2, h / 2);
        sprite.setTextureRect(frames[0]);
    }

    void update()
    {
        Frame += speed;
        int n = frames.size();
        if (Frame >= n) Frame -= n;
        if (n > 0) sprite.setTextureRect(frames[int(Frame)]);
    }

    bool isEnd()
    {
        return Frame + speed >= frames.size();
    }

};

class Entity
{
public:
    float x, y, dx, dy, R, angle;
    float x1, y1;
    bool life, isCollide, isCollideFuture;
    std::string name;
    Animation anim;

    Entity():life (1),R (30), angle (0),dx (0),dy (0),isCollide(false),isCollideFuture(false),x(0),y(0),x1(0),y1(0){}

    void settings(Animation& a, float X, float Y, float Angle = 0, float radius = 30 )
    {
        anim = a;
        x = X; y = Y;
        angle = Angle;
        R = radius;
        isCollide = false;
        
    }

    virtual void update() {};

    virtual void draw(RenderWindow& app)
    {
        anim.sprite.setPosition(x, y);
        anim.sprite.setRotation(angle + 90);
        app.draw(anim.sprite);
    }

    virtual ~Entity() {};
};

class bird : public Entity
{
public:
   
    bird()
    {
        name = "bird";
        x1=rand() % W;
        y1=rand() % H;
   
    }
    void  update()
    {
      if(!isCollide)angle = (atan2(x1 - x, y - y1))*57.2956/6;
      angle += (rand() % 7 - 3)* DEGTORAD; //отклонение траектории
      //   angle = angle * DEGTORAD; //первод в радианы
      if (isCollideFuture) {
          if (x > W) {
              angle = angle + 3.14;
              x = W;
          } 
          if (x < 0) {
              angle = angle + 3.14;
              x = 0;
          }
          if (y > H) {
              angle = angle + 3.14;
              y = H;
          }
          if (y < 0) {
              angle = angle + 3.14;
              y = 0;
          }
          isCollideFuture = false;
      }
      else {
          if (x > W) {
              angle = 3.14 - angle;
              x = W;
          }
          if (x < 0) {
              angle = 3.14 - angle;
              x = 0;
          }
          if (y > H) {
              angle = -angle;
              y = H;
          }
          if (y < 0) {
               angle = -angle;
               y = 0;
          }
      }
        dx = cos(angle)*6 ;
        dy = sin(angle)*6 ;
        x += dx;
        y += dy;
    }

    void draw(RenderWindow& app)
    {
        anim.sprite.setPosition(x, y);
        anim.sprite.setRotation(angle * 57.2956 + 90); // отрисовка спрайта (перевод с радиан в градусы)
        app.draw(anim.sprite);
   /*
   
    CircleShape circle(2);
    circle.setFillColor(Color(255, 0, 0, 170));
    circle.setPosition(x, y);
    circle.setOrigin(R, R);
    app.draw(circle);   
  */       
    }
};


bool isCollideFuture(Entity* a, Entity* b)// пересечение в будующем
{

    return (b->x - a->x) * (b->x - a->x) +
        (b->y - a->y) * (b->y - a->y) <
        ((a->R+6)+ (b->R+6)) * ((a->R+6) + (b->R+6));
        }

bool isCollide(Entity* a, Entity* b)  // пересечение
{
    /*
    return (b->x - a->x) * (b->x - a->x) +
        (b->y - a->y) * (b->y - a->y) <
        (a->R + b->R) * (a->R + b->R) - 1;
      */  
    return abs(a->x - b->x) < a->R && abs(a->y - b->y) < a->R;
}
float isMidlle(Entity* a, Entity* b) {
    return abs(a->x - b->x) + abs(a->y - b->y); //середина между точками
}
float calcCoordinate(float &a, float &b) { // куда лететь
    if (a < b) return a + ((b - a) / 2);
    return b + ((a - b) / 2);
}
int main()
{
    float x1, x2, y1, y2,minDistance;
    
    srand(time(0));

    RenderWindow app(VideoMode(W, H), "boids");
    app.setFramerateLimit(60);

    Texture t1, t2;
    
    t2.loadFromFile("images/background.jpg");
  
    t1.loadFromFile("images/fire_blue.png");

    Sprite background(t2);
   
   Animation sBullet(t1, 0, 0, 32, 64, 16, 0.8);
  
    std::list<Entity*> entities;
    
    Entity* tempBird = new bird();
  
    for (int i = 0; i< 3;i++)
    {
        bird* a = new bird();
        a->settings(sBullet, rand() % W, rand() % H, rand() % 360, 100);
        entities.push_back(a);
    }
   
    /////main loop/////
    while (app.isOpen())
    {
        
        Event event;
        while (app.pollEvent(event))
        {
            if (event.type == Event::Closed)
                app.close();     
        }

        for (auto a : entities) {
            x2 = a->x;
            y2 = a->y;
            minDistance = a-> x + a-> y;
            for (auto b : entities)
            {
                if ((a->x == b->x) || (a->y == b->y))continue;
                if (isMidlle(a, b) < minDistance) {
                    minDistance = isMidlle(a, b);
                    x1 = x2;
                    y1 = y2;
                    x2 = b->x;
                    y2 = b->y;
                    tempBird = b;
   
                }
             
            }
            if ((a->x == tempBird->x) || (a->y == tempBird->y))continue;
            if (isCollide(a, tempBird)) {
                a->isCollide = true;
                if (a->x+a->y > tempBird->x+ tempBird->y) {
                    a->angle = tempBird->angle + (rand() % 1 + 17) / 100; // выбор отклонения
                }
                else {
                    a->angle = tempBird->angle - (rand() % 1 + 17) / 100;
                }

            }  
            if (isCollideFuture(a, tempBird))  a->isCollideFuture = true;
            a->x1 = calcCoordinate(x1, x2);  // передача координат движения
            a->y1 = calcCoordinate(y1, y2);

        }

        for (auto i = entities.begin(); i != entities.end();)
        {
            Entity* e = *i;
            e->update();
            e->anim.update();
            i++;
        }
        //////draw//////
        app.draw(background);
        for (auto i : entities) i->draw(app);
     
        app.display();
    }

    return 0;
}
