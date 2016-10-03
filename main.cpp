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
////////////////////////////////////����� ����� ��������//////////////////////////
class Entity {
public:
	std::vector<Object> obj;
	float dx, dy, x, y, speed, moveTimer;//�������� ���������� ������ ��� ������� �����
	int w, h, health;
	bool life, isMove, onGround, firstdie;
	Texture texture;
	Sprite sprite;
	String name;//����� ����� ���� ������, �� �� ����� ������ ������ ����� ��� �������������� �����.����� ���� �������� ������ �� ����� � ����� ������� ���� �������� � update � ����������� �� �����
	Entity(Image &image, String Name, float X, float Y, int W, int H ){
		x = X; y = Y; w = W; h = H; name = Name; moveTimer = 0;
		speed = 0; health = 100; dx = 0; dy = 0;
		life = true; onGround = false; isMove = false;
		texture.loadFromImage(image);
		sprite.setTexture(texture);
		sprite.setOrigin(w / 2, h / 2);
	}
	FloatRect getRect(){//�-��� ��������� ��������������. ��� �����,������� (���,�����).
		return FloatRect(x, y, w, h);//��� �-��� ����� ��� �������� ������������ 
	}
	virtual void update(float time) = 0;

};
////////////////////////////////////////////////////����� ������////////////////////////
class Player :public Entity {
public:
	enum { left, right, up, down, jump, stay } state;//��������� ��� ������������ - ��������� �������
	int playerScore;//��� ���������� ����� ���� ������ � ������
	bool endlevel;
	Font font;//����� 
	Text text_gameover;

	SoundBuffer bounceBuffer, looseBuffer,scoreBuffer;//������ ����� ��� �����
	float CurrentFrame;
	Music music;
	Sound bounce_sound, loose_sound, score_sound;//������� ���� � ��������� � ���� ���� �� ������
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
		bounceBuffer.loadFromFile("bounce_sound.wav");//��������� � ���� ����
		looseBuffer.loadFromFile("loose.wav");
		scoreBuffer.loadFromFile("score_sound.wav");
		score_sound.setBuffer(scoreBuffer);
		loose_sound.setBuffer(looseBuffer);
		score_sound.setVolume(100);
		loose_sound.setVolume(100);
		bounce_sound.setBuffer(bounceBuffer);//������� ���� � ��������� � ���� ���� �� ������
		if (name == "Player1"){
			sprite.setTextureRect(IntRect(0, 0, w, h));
		}
	}


	void control(float time){
		if (life)
		{
			if (Keyboard::isKeyPressed){//���� ������ �������

				if (Keyboard::isKeyPressed(Keyboard::Left)) {//� ������ �����
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

				if ((Keyboard::isKeyPressed(Keyboard::Up)) && (onGround)) {//���� ������ ������� ����� � �� �� �����, �� ����� �������
					state = jump; dy = -0.36; onGround = false;//�������� ������ ������
					bounce_sound.play();
				}

				if (Keyboard::isKeyPressed(Keyboard::Down)) {
					state = down;
				}
			}
		}
	}

	void checkCollisionWithMap(float Dx, float Dy)//� ��� �������� ������������ � ������
	{
		for (int i = 0; i<obj.size(); i++)//���������� �� ��������
		if (getRect().intersects(obj[i].rect))//��������� ����������� ������ � ��������
		{
			if (obj[i].name == "endlevel")
			{
				endlevel = true;
			}

			if (obj[i].name == "solid")//���� ��������� �����������
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
		control(time);//������� ���������� ����������
		switch (state)//��� �������� ��������� �������� � ����������� �� ���������
		{
		case right: {
						dx = speed;

						break;
					}		
		case left:{
						dx = -speed; 
						break; 
				  }//��������� ���� �����
		case up: break;//����� ��������� �������� ������ (�������� �� ��������)
		case down: dx = 0; break;//����� ��������� �� ����� ������ ��������� (�������� �� ��������)
		case stay: break;//� ����� ����		
		}
		x += dx*time;
		checkCollisionWithMap(dx, 0);//������������ ������������ �� �
		y += dy*time;
		checkCollisionWithMap(0, dy);//������������ ������������ �� Y
		sprite.setPosition(x + w / 2, y + h / 2); //������ ������� ������� � ����� ��� ������
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
				text_gameover.setPosition(view.getCenter().x - 200, view.getCenter().y-100 );//������ ������� ������, ����� ������
			}
		}
		if (!isMove){ speed = 0; }
		if (life) { setPlayerCoordinateForView(x, y); }
		dy = dy + 0.0005*time;//��������� ������������� � �����
	}
};


class Enemy :public Entity{
public:
	Enemy(Image &image, String Name, Level &lvl, float X, float Y, int W, int H) :Entity(image, Name, X, Y, W, H){
		obj = lvl.GetAllObjects();
		if (name == "EasyEnemy"){
			sprite.setTextureRect(IntRect(0, 0, w, h));
			dx = 0.1;//���� ��������.���� ������ ������ ���������
		}

	}

	void checkCollisionWithMap(float Dx, float Dy)//� ��� �������� ������������ � ������
	{
		for (int i = 0; i<obj.size(); i++)
		{

			if (getRect().intersects(obj[i].rect))//��������� ����������� ������ � ��������
			{
				if (obj[i].name == "solid"){//���� ��������� ����������� (������ � ������ solid)
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
		if (name == "EasyEnemy"){//��� ��������� � ����� ������ ������ ����� �����

			//moveTimer += time;if (moveTimer>3000){ dx *= -1; moveTimer = 0; }//������ ����������� �������� ������ 3 ���
			x += dx*time;
			checkCollisionWithMap(dx, 0);
			sprite.setPosition(x + w / 2, y + h / 2); //������ ������� ������� � ����� ��� ������
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

	//////////////////////////////����///////////////////
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
			if (menuNum == 1) isMenu = false;//���� ������ ������ ������, �� ������� �� ���� 
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

	Font font;//����� 
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

	for (int i = 0; i < e.size(); i++)//���������� �� ��������� ����� �������(� ������ �� ������)
		entities.push_back(new Enemy(easyEnemyImage, "EasyEnemy", lvl, e[i].rect.left, e[i].rect.top, 32, 32));
	Object player = lvl.GetObject("player");

	Player p(heroImage, "Player1", lvl, player.rect.left, player.rect.top, 32, 32);//������ ������ ������
	
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

		p.moveTimer += time;//���������� ������
		if (Keyboard::isKeyPressed(Keyboard::R)) { numberLevel=0; return true; }//���� ���, �� ������������� ����
		if (Keyboard::isKeyPressed(Keyboard::Escape)) { return false; }//���� ������, �� ������� �� ����
		if (Keyboard::isKeyPressed(Keyboard::S)) if (p.moveTimer > 1000){ if (sound_switch){ p.music.setVolume(0); sound_switch = false; } else { p.music.setVolume(30); sound_switch = true; } p.moveTimer = 0; }//���� ������, �� ������� �� ����


		p.update(time);// Player update function	
		if (p.endlevel)
		{
			p.endlevel = false;
			return true;
		}
		for (it = entities.begin(); it != entities.end();)//������� ��� ���������� �� ������ �� �����
		{
			Entity *b = *it;//��� ��������, ����� �� ������ (*it)->
			b->update(time);//�������� �-��� update ��� ���� �������� (�� ���� ��� ���, ��� ���)
			if (b->life == false)	{ it = entities.erase(it); delete b; p.playerScore++; p.score_sound.play(); }// ���� ���� ������ �����, �� ������� ���
			else it++;//� ���� �������� (����������) � ���� �������. ��� ������ �� ����� ��������� ������
		}

		for (it = entities.begin(); it != entities.end(); it++)//���������� �� ��-��� ������
		{
			if ((*it)->getRect().intersects(p.getRect()))//���� ������������� ������� ������� ������������ � �������
			{
				if ((*it)->name == "EasyEnemy"){//� ��� ���� ��� ������� EasyEnemy,��..

					if ((p.dy>0) && (p.onGround == false))
					{
						(*it)->dx = 0;
						p.dy = -0.2;
						(*it)->health = 0;
					}//���� �������� �� �����,�� ���� ����� �������� 0,����������� �� ���� ���� �����,���� ��� �������� 0
					else {
						//����� ���� ������� � ��� ����� � ����� ����
					}
				}
			}
		}
		p.moveTimer += time;//���������� ������
		for (it = entities.begin(); it != entities.end(); it++)//���������� �� ��-��� ������
		{
			if ((*it)->getRect().intersects(p.getRect()))//���� ������������� ������� ������� ������������ � �������
			{
				if (p.moveTimer > 1200)
				{
					if ((*it)->name == "EasyEnemy"){//� ��� ���� ��� ������� EasyEnemy,��..
						if ((*it)->dx > 0)//���� ���� ���� ������
						{
							p.health -= 5;
							p.dy = -0.2;
							p.moveTimer = 0;
						}
						if ((*it)->dx < 0)//���� ���� ���� �����
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
			window.draw((*it)->sprite); //������ entities ������� (������ ��� ������ �����)
		}

		window.draw(p.sprite);//������ ������ ������� p ������ player

		std::ostringstream healthString;    // �������� ����������
		healthString << p.health;
		health_text.setString("Health :" + healthString.str());//������ ������ ������
		health_text.setPosition(view.getCenter().x - 300, view.getCenter().y - 230);//������ ������� ������, ����� ������
		window.draw(health_text);//����� ���� �����


		std::ostringstream playerScoreString;    // �������� ����������
		playerScoreString << p.playerScore;
		score_text.setString("Score :" + playerScoreString.str());//������ ������ ������
		score_text.setPosition(view.getCenter().x - 300, view.getCenter().y - 190);//������ ������� ������, ����� ������
		window.draw(score_text);//����� ���� �����

		std::ostringstream levelString;    // �������� ����������
		levelString << numberLevel;
		level_text.setString("Level :" + levelString.str());//������ ������ ������
		level_text.setPosition(view.getCenter().x + 230, view.getCenter().y - 230);//������ ������� ������, ����� ������
		window.draw(level_text);//����� ���� �����


		window.draw(p.text_gameover);
		window.display();
	}
}


void gameRunning(RenderWindow & window, int & numberLevel){//�-��� ������������� ���� , ���� ��� ����������
	if (startGame(window, numberLevel)) 
	{ 
		numberLevel++; 
		gameRunning(window, numberLevel); 
	}//��������� � ������ ������ ������ ����
}

int main()
{
	ShowWindow(GetConsoleWindow(), SW_HIDE);
	RenderWindow window(VideoMode(640, 480), "game");
	menu(window);//����� ����
	int numberLevel = 1;//������� 1-�� �������
	gameRunning(window, numberLevel);
	return 0;
}