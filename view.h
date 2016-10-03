#include <SFML/Graphics.hpp>
using namespace sf;

View view;//объ€вили sfml объект "вид", который и €вл€етс€ камерой

void setPlayerCoordinateForView(float x, float y) { //функци€ дл€ считывани€ координат игрока
	float tempX = x; float tempY = y;

//	if (x < 320) tempX = 320;//убираем из вида левую сторону
	//if (y < 240) tempY = 240;//верхнюю сторону
	if (y > 624) tempY = 624;//нижнюю сторону	
	//if (x > 960) tempX = 960;//убираем из вида левую сторону

	view.setCenter(tempX, tempY); //следим за игроком, передава€ его координаты.

}