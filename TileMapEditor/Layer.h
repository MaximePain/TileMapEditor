#pragma once
#include <iostream>
#include <SFML/Graphics.hpp>

using namespace std;

class Layer
{
public:
	Layer(string tilePath, int tileSize, int winL, int maxX, int winH, int maxY);
	~Layer();

	string tilePath = "";
	int tileSize = 64;

	sf::Image image;
	sf::Texture textureTile;
	sf::Sprite spriteTile;
	sf::Sprite mapSprite;

	int winL, maxX, winH, maxY;
	sf::Vector2u imageSize;

	void setTile(string tilePath, int tileSize);

private:
	void update();
};

