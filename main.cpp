#include <SFML/Graphics.hpp>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <SFML/Audio.hpp>


using namespace std;

struct backGr
{

    sf::Texture bg;
    sf::Sprite background;
    sf::Vector2f position;

    backGr()
    {
        bg.loadFromFile("Ground.png");
        bg.setRepeated(1);  //render sprite contain to fit width of biger size.
        background.setTexture(bg);
        background.setOrigin(0, 0);
        background.scale(1.5, 2);
        background.setTextureRect(sf::IntRect(0, 0, 1600, 13)); // load ảnh trong texture. 2 trọng số đầu là tọa độ, chiều dài, chiều rộng của ảnh
        background.setPosition(0, 500); // set vị trí ảnh background
        position = sf::Vector2f(background.getPosition());
    }

    void draw(sf::RenderWindow& window)
    {
        window.draw(background);
    }

    void update()
    {
        position.x = position.x - 4;  //tạo chuyển động

        if (position.x == -1000){
            position.x = 0;
        }
        background.setPosition(position);
        position = sf::Vector2f(background.getPosition());
    }
};

struct Barrier
{
    float x=866, y =470;
    sf::Texture texture;
    sf::Sprite enemies;
    sf::IntRect rect;
    sf::Vector2f position;
    bool enemy_on_screen;


    Barrier() : rect(0, 0, 50, 50)
    {
        texture.loadFromFile("enemies.png");
        enemies.setTexture(texture);
        enemies.setTextureRect(rect);
        enemies.setPosition(x, y); // set vi tri cua barrier dau tien.
        srand(time(NULL));
        enemy_on_screen = false;
        position = sf::Vector2f(enemies.getPosition());
    }


    void draw(sf::RenderWindow& window)
    {
        window.draw(enemies);
    }

    void update(int frames)
    {
        int k = rand() % 4;
        if (frames % 6 == 0 && !enemy_on_screen)
        {
            switch (k)
            {
            case 0:
                rect = sf::IntRect(0, 0, 50, 50);
                break;

            case 1:
                rect = sf::IntRect(50, 0, 50, 50);
                break;

            case 2:
                rect = sf::IntRect(0, 50, 50, 50);
                break;

            case 3:
                rect = sf::IntRect(50, 50, 50, 50);
                break;
            }
            enemy_on_screen = true;
        }

        enemies.setTextureRect(rect);

        if (enemy_on_screen)
        {
            position.x = position.x - 4; // toc do
            enemies.setPosition(position);
            if (position.x <= 0)
            {
                enemy_on_screen = false;
                enemies.setPosition(x, y);
            }
        }
        position = sf::Vector2f(enemies.getPosition());
    }
};
struct Dino
{
    sf::Vector2f position;
    sf::Vector2f velocity;
    sf::Vector2f gravity;
    sf::Texture texture;
    sf::Sprite dino;
    sf::IntRect rect;
    int t;
    bool jump;
    bool ground;

    Dino() : velocity(0, 0),
             gravity(0, 0.98),
             rect(0, 0, 50, 43)
    {
        jump = false;
        ground = true;
        texture.loadFromFile("dino.png");
        dino.setTexture(texture);
        dino.setTextureRect(rect);
        dino.setPosition(50, 480);
        position = sf::Vector2f(dino.getPosition());
        t = 0;
    }

    void draw(sf::RenderWindow& window)
    {
        window.draw(dino);
    }

    void keyboard_input()
    {
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space) && ground)
        {
            velocity.y = -16; // nhảy lên 16px
            jump = true;
            ground = false;
        }
    }

    void update(int frames , Barrier enemy)
    {
        position += velocity;
        if (!ground)
        {
            velocity += gravity;
            if (position.y >= 480)
            {
                position.y = 480;
                ground = true;
                jump = false;
                velocity.y = 0;
            }
        }

        rect = sf::IntRect(t, 0, 50, 43);

        if (frames % 10 == 0)  //move chan cua dino
            t = (t + 50) % 100;

        dino.setTextureRect(rect);
        dino.setPosition(position);

        if (ground)
            velocity = sf::Vector2f(0, 0);
    }
};

struct GameOverMessage
{
    sf::Font font;
    sf::Text text;
    GameOverMessage() {
        if (!font.loadFromFile("game_over.ttf"))
        {
            cout << "Error loading game_over.ttf" << endl;
        }
        text.setFont(font);
        text.setString("Game Over !!!");
        text.setCharacterSize(300); // in pixels, not points!
        // set the color
        text.setFillColor(sf::Color::White);
        // set the text style
        text.setStyle(sf::Text::Bold | sf::Text::Underlined);
        text.setPosition(sf::Vector2f(120, 0));
    }

    void draw(sf::RenderWindow& window) {
        window.draw(text);
    }
};

struct Status
{
    sf::RenderWindow window;
    Dino dino;
    backGr backGr;
    Barrier enemy;
    int frames;
    sf::Music soundTrack;
    GameOverMessage gameovermsg;
    bool gameOver = false;

    Status() : window(sf::VideoMode(1000, 600), "T-rex running")
    {
        window.setFramerateLimit(60);
        soundTrack.openFromFile("nhac.wav");
        soundTrack.play();
        soundTrack.setLoop(true);
        frames = 0;
        loop();
    }

    void loop()
    {
        while (window.isOpen())
        {
            sf::Event event;
            while (window.pollEvent(event))
            {
                switch(event.type){
                    case sf::Event::Closed:
                        window.close();
                        break;
                }
            }
            update();
            render();
        }
    }

    void update()
    {
        checkLose();
        if (!gameOver) {
            backGr.update();
            enemy.update(frames);
            dino.keyboard_input();
            dino.update(frames , enemy);
            frames = frames + 1;
        }
    }

    void checkLose() {
        if(!dino.jump){
            sf::Vector2f pos = enemy.position;
            if(pos.x >= 25 && pos.x <= 75){
                gameOver = true;
            }
        }
    }

    void render()
    {
        window.clear(sf::Color::Black);
        backGr.draw(window);
        enemy.draw(window);
        dino.draw(window);
        if(gameOver){
            gameovermsg.draw(window);
        }
        window.display();
    }
};

/*
struct Score
{
    sf::Font font;
    sf::Text textScore;
    Score()
    {
        if (!font.loadFromFile("game_over.ttf"))
        {
            cout << "Error loading game_over.ttf" << endl;
        }
        textScore.setFont(font);
        textScore.setString("0");
        textScore.setCharacterSize(100);
        textScore.setPosition(200,0);
        textScore.setFillColor(sf::Color::White);
    }

    void updateScore(int Score){
        textScore.setString(std::string to_string(Score));
    }

    void draw(sf::RenderWindow& window){
        window.draw(textScore);
    }
};

*/
int main(){
    Status status;
    return 0;
}
