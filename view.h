#include <SFML/Graphics.hpp>
using namespace sf;

View view;//�������� sfml ������ "���", ������� � �������� �������

void setPlayerCoordinateForView(float x, float y) { //������� ��� ���������� ��������� ������
	float tempX = x; float tempY = y;

//	if (x < 320) tempX = 320;//������� �� ���� ����� �������
	//if (y < 240) tempY = 240;//������� �������
	if (y > 624) tempY = 624;//������ �������	
	//if (x > 960) tempX = 960;//������� �� ���� ����� �������

	view.setCenter(tempX, tempY); //������ �� �������, ��������� ��� ����������.

}