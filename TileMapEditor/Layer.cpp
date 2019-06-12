#include "stdafx.h"
#include "Layer.h"



Layer::Layer(string tilePath, int tileSize, int winL, int maxX, int winH, int maxY)
{
	this->tilePath = tilePath;
	this->tileSize = tileSize;

	this->winL = winL;
	this->maxX = maxX;
	this->winH = winH;
	this->maxY = maxY;

	update();

}

void Layer::setTile(string tilePath, int tileSize)
{
	this->tilePath = tilePath;
	this->tileSize = tileSize;

	update();
}

void Layer::update()
{
	image.loadFromFile(tilePath);
	imageSize = image.getSize();

	textureTile.loadFromFile(tilePath);
	spriteTile.setTexture(textureTile);

	spriteTile.setTextureRect(sf::IntRect(0, 0, tileSize, tileSize));

	spriteTile.setScale(sf::Vector2f(winL / maxX / (float)tileSize, winH / maxY / (float)tileSize));

	mapSprite.setTexture(textureTile);
	mapSprite.setScale(sf::Vector2f(winL / maxX / (float)tileSize, winH / maxY / (float)tileSize));
}




Layer::~Layer()
{
}
