#include "SFML/Graphics.hpp"
#include "SFML/Audio.hpp"
#include "view.h"
#include <Windows.h>
#include <iostream>
#include <sstream>
#include "level.h"
#include <vector>
#include <list>
#include "mission.h"
#include "TinyXML/tinyxml.h"

using namespace sf;
////////////////////////////////////Общий класс родитель//////////////////////////
class Entity {
public:
	std::vector<Object> obj;
	float dx, dy, x, y, speed, moveTimer;//добавили переменную таймер для будущих целей
	int w, h, health;
	bool life, isMove, onGround, firstdie;
	Texture texture;
	Sprite sprite;
	String name;//враги могут быть разные, мы не будем делать другой класс для различающегося врага.всего лишь различим врагов по имени и дадим каждому свое действие в update в зависимости от имени
	Entity(Image &image, String Name, float X, float Y, int W, int H ){
		x = X; y = Y; w = W; h = H; name = Name; moveTimer = 0;
		speed = 0; health = 100; dx = 0; dy = 0;
		life = true; onGround = false; isMove = false;
		texture.loadFromImage(image);
		sprite.setTexture(texture);
		sprite.setOrigin(w / 2, h / 2);
	}
	FloatRect getRect(){//ф-ция получения прямоугольника. его коорд,размеры (шир,высот).
		return FloatRect(x, y, w, h);//эта ф-ция нужна для проверки столкновений 
	}
	virtual void update(float time) = 0;

};
////////////////////////////////////////////////////КЛАСС ИГРОКА////////////////////////
class Player :public Entity {
public:
	enum { left, right, up, down, jump, stay } state;//добавляем тип перечисления - состояние объекта
	int playerScore;//эта переменная может быть только у игрока
	bool endlevel;
	Font font;//шрифт 
	Text text_gameover;

	SoundBuffer bounceBuffer, looseBuffer,scoreBuffer;//создаём буфер для звука
	float CurrentFrame;
	Music music;
	Sound bounce_sound, loose_sound, score_sound;//создаем звук и загружаем в него звук из буфера
	Player(Image &image, String Name, Level &lvl, float X, float Y, int W, int H) :Entity(image, Name, X, Y, W, H){

		endlevel = false;

		music.openFromFile("fon.wav");
		music.setVolume(30);
		music.play();
		music.setLoop(true);

		font.loadFromFile("QueenEmpress.ttf");
		text_gameover.setFont(font);
		text_gameover.setString("Game Over");
		text_gameover.setCharacterSize(80);
		text_gameover.setColor(Color::Red);

		playerScore = 0; 
		state = stay; 
		obj = lvl.GetAllObjects();
		CurrentFrame = 0;

		firstdie = true;
		health = 100;
		bounceBuffer.loadFromFile("bounce_sound.wav");//загружаем в него звук
		looseBuffer.loadFromFile("loose.wav");
		scoreBuffer.loadFromFile("score_sound.wav");
		score_sound.setBuffer(scoreBuffer);
		loose_sound.setBuffer(looseBuffer);
		score_sound.setVolume(100);
		loose_sound.setVolume(100);
		bounce_sound.setBuffer(bounceBuffer);//создаем звук и загружаем в него звук из буфера
		if (name == "Player1"){
			sprite.setTextureRect(IntRect(0, 0, w, h));
		}
	}


	void control(float time){
		if (life)
		{
			if (Keyboard::isKeyPressed){//если нажата клавиша

				if (Keyboard::isKeyPressed(Keyboard::Left)) {//а именно левая
					state = left; speed = 0.1;
					CurrentFrame += 0.005*time;
					if (CurrentFrame > 8) CurrentFrame -= 8;
					sprite.setTextureRect(IntRect(224 - 32 * int(CurrentFrame), 0, 32, 32));
					
				}
				if (Keyboard::isKeyPressed(Keyboard::Right)) {
					state = right; speed = 0.1;
					CurrentFrame += 0.005*time;
					if (CurrentFrame > 8) CurrentFrame -= 8;
					sprite.setTextureRect(IntRect(32 * int(CurrentFrame), 0, 32, 32));
				}

				if ((Keyboard::isKeyPressed(Keyboard::Up)) && (onGround)) {//если нажата клавиша вверх и мы на земле, то можем прыгать
					state = jump; dy = -0.36; onGround = false;//увеличил высоту прыжка
					bounce_sound.play();
				}

				if (Keyboard::isKeyPressed(Keyboard::Down)) {
					state = down;
				}
			}
		}
	}

	void checkCollisionWithMap(float Dx, float Dy)//ф ция проверки столкновений с картой
	{
		for (int i = 0; i<obj.size(); i++)//проходимся по объектам
		if (getRect().intersects(obj[i].rect))//проверяем пересечение игрока с объектом
		{
			if (obj[i].name == "endlevel")
			{
				endlevel = true;
			}

			if (obj[i].name == "solid")//если встретили препятствие
			{
				if (Dy>0)	{ y = obj[i].rect.top - h;  dy = 0; onGround = true; }
				if (Dy<0)	{ y = obj[i].rect.top + obj[i].rect.height;   dy = 0; }
				if (Dx>0)	{ x = obj[i].rect.left - w; }
				if (Dx<0)	{ x = obj[i].rect.left + obj[i].rect.width; }
			}
			if (obj[i].name == "SlopeLeft")
			{
				
				FloatRect r = obj[i].rect;
				int y0 = (x + w / 2 - r.left) * r.height / r.width + r.top - h;
				if (y>y0)
				if (x + w / 2>r.left)
				{
					onGround = true;
					y = y0; dy = 0;
				}
			}

			if (obj[i].name == "SlopeRight")
			{
				
				FloatRect r = obj[i].rect;
				int y0 = -(x + w / 2 - r.left) * r.height / r.width + r.top + r.height - h;
				if (y > y0)
				if (x + w / 2<r.left + r.width)
				{
					onGround = true;
					y = y0; dy = 0;
				}
			}


		}
	}

	void update(float time)
	{
		control(time);//функция управления персонажем
		switch (state)//тут делаются различные действия в зависимости от состояния
		{
		case right: {
						dx = speed;

						break;
					}		
		case left:{
						dx = -speed; 
						break; 
				  }//состояние идти влево
		case up: break;//будет состояние поднятия наверх (например по лестнице)
		case down: dx = 0; break;//будет состояние во время спуска персонажа (например по лестнице)
		case stay: break;//и здесь тоже		
		}
		x += dx*time;
		checkCollisionWithMap(dx, 0);//обрабатываем столкновение по Х
		y += dy*time;
		checkCollisionWithMap(0, dy);//обрабатываем столкновение по Y
		sprite.setPosition(x + w / 2, y + h / 2); //задаем позицию спрайта в место его центра
		if (health <= 0)
		{ 
			health = 0; 
			life = false;
			if (firstdie)
			{
				music.setVolume(0);
				firstdie = false;
				loose_sound.play();
				text_gameover.setString("Game Over");
				text_gameover.setPosition(view.getCenter().x - 200, view.getCenter().y-100 );//задаем позицию текста, центр камеры
			}
		}
		if (!isMove){ speed = 0; }
		if (life) { setPlayerCoordinateForView(x, y); }
		dy = dy + 0.0005*time;//постоянно притягиваемся к земле
	}
};


class Enemy :public Entity{
public:
	Enemy(Image &image, String Name, Level &lvl, float X, float Y, int W, int H) :Entity(image, Name, X, Y, W, H){
		obj = lvl.GetAllObjects();
		if (name == "EasyEnemy"){
			sprite.setTextureRect(IntRect(0, 0, w, h));
			dx = 0.1;//даем скорость.этот объект всегда двигается
		}

	}

	void checkCollisionWithMap(float Dx, float Dy)//ф ция проверки столкновений с картой
	{
		for (int i = 0; i<obj.size(); i++)
		{

			if (getRect().intersects(obj[i].rect))//проверяем пересечение игрока с объектом
			{
				if (obj[i].name == "solid"){//если встретили препятствие (объект с именем solid)
					if (Dy>0)	{ y = obj[i].rect.top - h;  dy = 0; onGround = true; }
					if (Dy<0)	{ y = obj[i].rect.top + obj[i].rect.height;   dy = 0; }
					if (Dx>0)	{ x = obj[i].rect.left - w;  dx = -0.1; sprite.scale(-1, 1); }
					if (Dx<0)	{ x = obj[i].rect.left + obj[i].rect.width; dx = 0.1; sprite.scale(-1, 1); }
				}
			}
		}


	}

	void update(float time)
	{
		if (name == "EasyEnemy"){//для персонажа с таким именем логика будет такой

			//moveTimer += time;if (moveTimer>3000){ dx *= -1; moveTimer = 0; }//меняет направление примерно каждые 3 сек
			x += dx*time;
			checkCollisionWithMap(dx, 0);
			sprite.setPosition(x + w / 2, y + h / 2); //задаем позицию спрайта в место его центра
			if (health <= 0){ life = false; }
		}
	}
};

void menu(RenderWindow & window) {
	Texture menuTexture1, menuTexture2, menuTexture3, aboutTexture, menuBackground;
	menuTexture1.loadFromFile("images/new_game.png");
	menuTexture2.loadFromFile("images/about_game.png");
	menuTexture3.loadFromFile("images/exit.png");
	aboutTexture.loadFromFile("images/about.png");
	menuBackground.loadFromFile("images/Bounce.png");
	Sprite menu1(menuTexture1), menu2(menuTexture2), menu3(menuTexture3), about(aboutTexture), menuBg(menuBackground);
	bool isMenu = 1;
	int menuNum = 0;
	menu1.setPosition(50, 30);
	menu2.setPosition(50, 90);
	menu3.setPosition(50, 150);
	menuBg.setPosition(345, 0);

	//////////////////////////////МЕНЮ///////////////////
	while (isMenu)
	{
		menu1.setColor(Color::White);
		menu2.setColor(Color::White);
		menu3.setColor(Color::White);
		menuNum = 0;
		window.clear(Color(255, 255, 225));

		if (IntRect(50, 30, 300, 50).contains(Mouse::getPosition(window))) { menu1.setColor(Color::Blue); menuNum = 1; }
		if (IntRect(50, 90, 300, 50).contains(Mouse::getPosition(window))) { menu2.setColor(Color::Blue); menuNum = 2; }
		if (IntRect(50, 150, 300, 50).contains(Mouse::getPosition(window))) { menu3.setColor(Color::Blue); menuNum = 3; }

		if (Mouse::isButtonPressed(Mouse::Left))
		{
			if (menuNum == 1) isMenu = false;//если нажали первую кнопку, то выходим из меню 
			if (menuNum == 2) { window.draw(about); window.display(); while (!Keyboard::isKeyPressed(Keyboard::Escape)); }
			if (menuNum == 3)  { window.close(); isMenu = false; exit(EXIT_SUCCESS); }

		}

		window.draw(menuBg);
		window.draw(menu1);
		window.draw(menu2);
		window.draw(menu3);

		window.display();
	}
	return;
}

void changeLevel(Level &lvl, int &numberLevel) {
	switch (numberLevel)
	{

		case 1:  lvl.LoadFromFile("map1.tmx"); 
				 break; 

		case 2:  lvl.LoadFromFile("map2.tmx"); 
				 break;

		case 3:  lvl.LoadFromFile("map3.tmx"); 
				 break;

		default: lvl.LoadFromFile("map1.tmx");
				 break;
	}
}

bool startGame(RenderWindow &window, int &numberLevel){

	view.reset(FloatRect(0, 0, 640, 480));
	//RenderWindow window(VideoMode(640, 480), "game");

	bool sound_switch = true;

	Font font;//шрифт 
	Text health_text,score_text, level_text;

	font.loadFromFile("QueenEmpress.ttf");
	health_text.setFont(font);
	health_text.setString("");
	health_text.setCharacterSize(20);
	health_text.setColor(Color::Black);

	score_text.setFont(font);
	score_text.setString("");
	score_text.setCharacterSize(20);
	score_text.setColor(Color::Black);

	font.loadFromFile("QueenEmpress.ttf");
	level_text.setFont(font);
	level_text.setString("");
	level_text.setCharacterSize(20);
	level_text.setColor(Color::Black);

	Level lvl;
	changeLevel(lvl, numberLevel);
	Image heroImage;
	heroImage.loadFromFile("images/balls.png");

	Image easyEnemyImage;
	easyEnemyImage.loadFromFile("images/EnemySpriteSheet1.png");

	std::list<Entity*> entities;
	std::list<Entity*>::iterator it;

	std::vector<Object> e = lvl.GetObjects("EasyEnemy");

	for (int i = 0; i < e.size(); i++)//проходимся по элементам этого вектора(а именно по врагам)
		entities.push_back(new Enemy(easyEnemyImage, "EasyEnemy", lvl, e[i].rect.left, e[i].rect.top, 32, 32));
	Object player = lvl.GetObject("player");

	Player p(heroImage, "Player1", lvl, player.rect.left, player.rect.top, 32, 32);//объект класса игрока
	
	Clock clock;
	while (window.isOpen())
	{


		float time = clock.getElapsedTime().asMicroseconds();
		clock.restart();
		time = time / 600;

		Event event;


		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();
		}

		p.moveTimer += time;//наращиваем таймер
		if (Keyboard::isKeyPressed(Keyboard::R)) { numberLevel=0; return true; }//если таб, то перезагружаем игру
		if (Keyboard::isKeyPressed(Keyboard::Escape)) { return false; }//если эскейп, то выходим из игры
		if (Keyboard::isKeyPressed(Keyboard::S)) if (p.moveTimer > 1000){ if (sound_switch){ p.music.setVolume(0); sound_switch = false; } else { p.music.setVolume(30); sound_switch = true; } p.moveTimer = 0; }//если эскейп, то выходим из игры


		p.update(time);// Player update function	
		if (p.endlevel)
		{
			p.endlevel = false;
			return true;
		}
		for (it = entities.begin(); it != entities.end();)//говорим что проходимся от начала до конца
		{
			Entity *b = *it;//для удобства, чтобы не писать (*it)->
			b->update(time);//вызываем ф-цию update для всех объектов (по сути для тех, кто жив)
			if (b->life == false)	{ it = entities.erase(it); delete b; p.playerScore++; p.score_sound.play(); }// если этот объект мертв, то удаляем его
			else it++;//и идем курсором (итератором) к след объекту. так делаем со всеми объектами списка
		}

		for (it = entities.begin(); it != entities.end(); it++)//проходимся по эл-там списка
		{
			if ((*it)->getRect().intersects(p.getRect()))//если прямоугольник спрайта объекта пересекается с игроком
			{
				if ((*it)->name == "EasyEnemy"){//и при этом имя объекта EasyEnemy,то..

					if ((p.dy>0) && (p.onGround == false))
					{
						(*it)->dx = 0;
						p.dy = -0.2;
						(*it)->health = 0;
					}//если прыгнули на врага,то даем врагу скорость 0,отпрыгиваем от него чуть вверх,даем ему здоровье 0
					else {
						//иначе враг подошел к нам сбоку и нанес урон
					}
				}
			}
		}
		p.moveTimer += time;//наращиваем таймер
		for (it = entities.begin(); it != entities.end(); it++)//проходимся по эл-там списка
		{
			if ((*it)->getRect().intersects(p.getRect()))//если прямоугольник спрайта объекта пересекается с игроком
			{
				if (p.moveTimer > 1200)
				{
					if ((*it)->name == "EasyEnemy"){//и при этом имя объекта EasyEnemy,то..
						if ((*it)->dx > 0)//если враг идет вправо
						{
							p.health -= 5;
							p.dy = -0.2;
							p.moveTimer = 0;
						}
						if ((*it)->dx < 0)//если враг идет влево
						{
							p.health -= 5;
							p.dy = -0.2;
							p.moveTimer = 0;
						}
					
					}
				}
			}
		}
		//easyEnemy.update(time);//easyEnemy update function
		window.setView(view);
		window.clear(sf::Color::Color(0, 128, 255));
		lvl.Draw(window);

		for (it = entities.begin(); it != entities.end(); it++){
			window.draw((*it)->sprite); //рисуем entities объекты (сейчас это только враги)
		}

		window.draw(p.sprite);//рисуем спрайт объекта p класса player

		std::ostringstream healthString;    // объявили переменную
		healthString << p.health;
		health_text.setString("Health :" + healthString.str());//задает строку тексту
		health_text.setPosition(view.getCenter().x - 300, view.getCenter().y - 230);//задаем позицию текста, центр камеры
		window.draw(health_text);//рисую этот текст


		std::ostringstream playerScoreString;    // объявили переменную
		playerScoreString << p.playerScore;
		score_text.setString("Score :" + playerScoreString.str());//задает строку тексту
		score_text.setPosition(view.getCenter().x - 300, view.getCenter().y - 190);//задаем позицию текста, центр камеры
		window.draw(score_text);//рисую этот текст

		std::ostringstream levelString;    // объявили переменную
		levelString << numberLevel;
		level_text.setString("Level :" + levelString.str());//задает строку тексту
		level_text.setPosition(view.getCenter().x + 230, view.getCenter().y - 230);//задаем позицию текста, центр камеры
		window.draw(level_text);//рисую этот текст


		window.draw(p.text_gameover);
		window.display();
	}
}


void gameRunning(RenderWindow & window, int & numberLevel){//ф-ция перезагружает игру , если это необходимо
	if (startGame(window, numberLevel)) 
	{ 
		numberLevel++; 
		gameRunning(window, numberLevel); 
	}//принимает с какого уровня начать игру
}

int main()
{
	ShowWindow(GetConsoleWindow(), SW_HIDE);
	RenderWindow window(VideoMode(640, 480), "game");
	menu(window);//вызов меню
	int numberLevel = 1;//сначала 1-ый уровень
	gameRunning(window, numberLevel);
	return 0;
}