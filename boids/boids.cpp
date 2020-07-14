#include <SFML/Graphics.hpp>
#include <time.h>
#include <list>
#include <sstream>
#include<iostream>
#include <cmath>
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

    Animation() {}

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
    bool life, isCollide;
    std::string name;
    Animation anim;

    Entity()
    {
        life = 1;
    }

    void settings(Animation& a, int X, int Y, float Angle = 0, int radius = 30 )
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
        /*
        CircleShape circle(R);
        circle.setFillColor(Color(255, 0, 0, 170));
        circle.setPosition(x, y);
        circle.setOrigin(R, R);
        app.draw(circle);

    */


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
      
      //if(!isCollide)angle = (atan2(x1 - x, y - y1))*57.2956/6;
      angle += (rand() % 7 - 3)* DEGTORAD;  /*try this*/

      
  
      //   angle = angle * DEGTORAD;
     
      /* 
        if (isCollide) { 
           // angle += 3.14;
            angle = -angle;
            isCollide = false; 
            //std::cout << "check" << std::endl;
        }
        */
      
        if (x > W) angle = 3.14 - angle;
        if (x < 0) angle = 3.14 - angle;
        if (y > H) angle = -angle;
        if (y < 0) angle =- angle;
        

      
      
    
        
        dx = cos(angle)*6 ;
        dy = sin(angle)*6 ;
        

        //std::cout << dx << " " << dy << std::endl;

        /*
       if ((x + dx) > 50 && (x + dx) <( W-50)) x += dx;
       if ((y + dy) > 50 && (y + dy) < (H-50)) y += dy;
       */
        x += dx;
        y += dy;
        //if (x > W || x<0 || y>H || y < 0) life = 0;
        /*
         if (x > W) x = 0;  if (x < 0) x = W;
        if (y > H) y = 0;  if (y < 0) y = H;

        */
        
          
          //  angle = angle * 57.2956;
        
        /*
        if (x > W || x<0 || y>H || y < 0) {
            if (angle < 90) angle = 180 - angle;
            else
                if (angle < 180)angle = 270 - (angle - 90);
                    else
                    if (angle < 270)angle = 360 - (angle - 180);
                    else angle = 180 - (360 - angle);
        }
        */
        
    }

    void draw(RenderWindow& app)
    {
       
     
        anim.sprite.setPosition(x, y);
        anim.sprite.setRotation(angle * 57.2956 + 90);
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




bool isCollide(Entity* a, Entity* b)
{
    return (b->x - a->x) * (b->x - a->x) +
        (b->y - a->y) * (b->y - a->y) <
        (a->R + b->R) * (a->R + b->R);
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
    Font font;//шрифт 
    font.loadFromFile("CyrilicOld.TTF");
    Text text("", font, 20);
    text.setFillColor(Color::Red);//покрасили текст в красный. если убрать эту строку, то по умолчанию он белый
   // text.setStyle(Text::Bold);
    text.setPosition(0, 0);
    text.setCharacterSize(24);



    srand(time(0));

    RenderWindow app(VideoMode(W, H), "boids");
    app.setFramerateLimit(60);

    Texture t1, t2, t3, t4, t5, t6, t7;

    t1.loadFromFile("images/spaceship.png");
    t2.loadFromFile("images/background.jpg");
    t3.loadFromFile("images/explosions/type_C.png");
    t4.loadFromFile("images/rock.png");
    t5.loadFromFile("images/fire_blue.png");
    t6.loadFromFile("images/rock_small.png");
    t7.loadFromFile("images/explosions/type_B.png");

    t1.setSmooth(true);
    t2.setSmooth(true);

    Sprite background(t2);

    Animation sExplosion(t3, 0, 0, 256, 256, 48, 0.5);
    Animation sRock(t4, 0, 0, 64, 64, 16, 0.2);
    Animation sRock_small(t6, 0, 0, 64, 64, 16, 0.2);
    Animation sBullet(t5, 0, 0, 32, 64, 16, 0.8);
    Animation sPlayer(t1, 40, 0, 40, 40, 1, 0);
    Animation sPlayer_go(t1, 40, 40, 40, 40, 1, 0);
    Animation sExplosion_ship(t7, 0, 0, 192, 192, 64, 0.5);


    std::list<Entity*> entities;
    
    Entity* tempBird = new bird();
    /*
    bird* a = new bird();
    a->settings(sBullet, rand() % W, rand() % H, rand() % 360, 25);
    entities.push_back(a);
    */
    for (int i = 0; i< 3; i++)
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
            /*
            if (event.type == Event::KeyPressed)
                if (event.key.code == Keyboard::Space)
                {
                    bullet* b = new bullet();
                    b->settings(sBullet, p->x, p->y, p->angle, 10);
                    entities.push_back(b);
                }*/
        }
       /* if (Keyboard::isKeyPressed(Keyboard::Space))
        {
            bullet* b = new bullet();
            b->settings(sBullet, p->x, p->y, p->angle, 10);
            entities.push_back(b);
        }
        */
        /*
        if (Keyboard::isKeyPressed(Keyboard::Right)) p->angle += 3;
        if (Keyboard::isKeyPressed(Keyboard::Left))  p->angle -= 3;
        if (Keyboard::isKeyPressed(Keyboard::Up)) p->thrust = true;
        else p->thrust = false;
        */

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
               
                  if (isCollide(a, b)) {
                    a->angle = b->angle;
                    a->isCollide = true;
                }
                else a->isCollide = false;
              
                
                }
               // if (isCollide(a, b))std::cout << 1 << std::endl;
                
              
               
            }
            if (isCollide(a, tempBird)) {
                a->angle = tempBird->angle;
                a->isCollide = true;
            }
            else a->isCollide = false;

            a->x1 = calcCoordinate(x1, x2);
            a->y1 = calcCoordinate(y1, y2);

        }
        /*
        if (p->thrust)  p->anim = sPlayer_go;
        else   p->anim = sPlayer;
        */
        /*
        for (auto e : entities)
            if (e->name == "explosion")
                if (e->anim.isEnd()) e->life = 0;
     */
        for (auto i = entities.begin(); i != entities.end();)
        {
            Entity* e = *i;

            e->update();
            e->anim.update();
            std::cout <<". "<< e->x1 << " " << e->y1 << "  ";

           // if (e->life == false) { i = entities.erase(i); delete e; }
          //  else
            i++;
        }
        std :: cout << std::endl;
        //////draw//////
        app.draw(background);
        for (auto i : entities) i->draw(app);
       // std::ostringstream ss;
       // ss << (entities.size() - 2);
       //// text.setString(ss.str());
        app.draw(text);
        app.display();
    }

    return 0;
}
