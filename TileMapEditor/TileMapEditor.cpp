// TileMapEditor.cpp : définit le point d'entrée pour l'application console.
//

#include "stdafx.h"
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <conio.h>
#include <cmath>
#include <SFML/Graphics.hpp>
#include <rapidjson/filereadstream.h>
#include <rapidjson/filewritestream.h>
#include <rapidjson/document.h>
#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>
#include <fstream>
#include "Layer.h"


using namespace std;

int main()
{
	bool focus = true;

	int choix;
	string pathTileSet;
	string path;
	FILE *jsonFile = NULL;

	ofstream jsonFileOf;

	int tileSize;
	string command;

	float winL;
	float winH;

	float maxX;
	float maxY;
	int nbLayer;

	int nbXY = 1;
	int fillnb = 0;
	int currentLayer = 0;

	int spriteTx = 0;
	int spriteTy = 0;

	int tabX;

	int tabY;

	//exception
	map<string, vector<int>> exception;
	typedef map<string, vector<int>>::iterator mapItEx;
	//map<string, vector<int>>::iterator ITexception;

	vector<int> yMap; //Y
	vector<vector<int>> xMap; //X
	vector<vector<vector<int>>> map;

	vector<Layer*> layers;

	cout << "HELLO WORLD!" << endl;
	cout << "create 1    open 2" << endl;
	cin >> choix;

	cout << "(1280)winL: ";
	cin >> winL;
	cout << "(720)winH: ";
	cin >> winH;

	if (choix == 1) {



		cout << "output path?" << endl;
		cin >> path;

		cout << "maxX" << endl;
		cin >> maxX;
		cout << "maxY" << endl;
		cin >> maxY;

		cout << "nbLayer: ";
		cin >> nbLayer;

		for (unsigned int i = 0; i < maxY; i++)
			yMap.push_back(-1);
		for (unsigned int i = 0; i < maxX; i++)
			xMap.push_back(yMap);

		for (int i = 0; i < nbLayer; i++) {
			map.push_back(xMap);
			cout << "path tileset: ";
			cin >> pathTileSet;
			cout << "taille Tiles: " << endl;
			cin >> tileSize;
			layers.push_back(new Layer(pathTileSet, tileSize, winL, maxX, winH, maxY));
		}

	}
	else if (choix == 2)
	{
		cout << "path?: ";
		cin >> path;

		jsonFile = fopen(path.c_str(), "rb");

		char readBuffer[65536];
		rapidjson::FileReadStream jsonInput(jsonFile, readBuffer, sizeof(readBuffer));

		rapidjson::Document json;
		json.ParseStream(jsonInput);

		assert(json["nbLayer"].IsInt());
		nbLayer = json["nbLayer"].GetInt();
		

		rapidjson::Value &terrain = json["map"];
		assert(json["map"].IsArray());
		maxX = (float)terrain[0].Size();
		maxY = (float)terrain[0][0].Size();

		assert(json["layers"].IsArray());
		rapidjson::Value &listLayers = json["layers"];
		for (rapidjson::Value::ConstValueIterator itr = listLayers.Begin(); itr != listLayers.End(); itr++)
		{
			tileSize = itr->GetObject().FindMember("tileSize")->value.GetInt();
			pathTileSet = itr->GetObject().FindMember("tilePath")->value.GetString();
			layers.push_back(new Layer(pathTileSet, tileSize, winL, maxX, winH, maxY));
		}


		for (unsigned int i = 0; i < maxY; i++)
			yMap.push_back(-1);
		for (unsigned int i = 0; i < maxX; i++)
			xMap.push_back(yMap);
		for (unsigned int i = 0; i < terrain.Size(); i++)
			map.push_back(xMap);

		for (unsigned int l = 0; l < terrain.Size(); l++)
			for (unsigned int x = 0; x < terrain[l].Size(); x++)
				for (unsigned int y = 0; y < terrain[l][x].Size(); y++)
					map[l][x][y] = terrain[l][x][y].GetInt();


		assert(json["exception"].IsObject());
		for (rapidjson::Value::ConstMemberIterator itr = json["exception"].MemberBegin(); itr != json["exception"].MemberEnd(); itr++)
		{
			for (unsigned int i = 0; i < itr->value.GetArray().Size(); i++)
			{
				exception[itr->name.GetString()].push_back(itr->value.GetArray()[i].GetInt());
			}
		}

		fclose(jsonFile);

	}


#define MapSprite layers[currentLayer]->mapSprite
#define SpriteTile layers[currentLayer]->spriteTile
#define layer layers[currentLayer]

	sf::RenderWindow window(sf::VideoMode((unsigned int)winL, (unsigned int)winH), "TileMapEditor Beta 1.1");

	//sf::RectangleShape ligne(sf::Vector2f(largeur, hauteur));

	sf::RectangleShape ligneHex(sf::Vector2f(winL, 1.f));
	ligneHex.setFillColor(sf::Color(0, 0, 0, 255));


	sf::RectangleShape ligneVex(sf::Vector2f(1.f, winH));
	ligneVex.setFillColor(sf::Color(0, 0, 0, 255));




	cout << "Commands: " << endl;

	window.setVerticalSyncEnabled(true);
	while (window.isOpen())
	{
		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();
			if (event.type == sf::Event::Resized)
			{
				winL = (float)event.size.width;
				winH = (float)event.size.height;

				for (int i = 0; i < nbLayer; i++)
				{
					layers[i]->spriteTile.setScale(sf::Vector2f(winL / maxX / (float)layers[i]->tileSize, winH / maxY / (float)layers[i]->tileSize)); //reset scale
					layers[i]->mapSprite.setScale(sf::Vector2f(winL / maxX / (float)layers[i]->tileSize, winH / maxY / (float)layers[i]->tileSize));
				}

				ligneHex.setSize(sf::Vector2f(winL, 1.f));
				ligneVex.setSize(sf::Vector2f(1.f, winH));

				sf::FloatRect visibleArea(0.f, 0.f, (float)event.size.width, (float)event.size.height);
				window.setView(sf::View(visibleArea));

			}
			if (event.type == sf::Event::MouseMoved && focus)
			{
				SpriteTile.setPosition(sf::Vector2f(floor(event.mouseMove.x / (winL / maxX)) * winL / maxX, floor(event.mouseMove.y / (winH / maxY)) * winH / maxY));
				//cout << "x: " << event.mouseMove.x << endl << "ceil: " << ceil(event.mouseMove.x / (winL / maxX)) << endl;
			}
			if (event.type == sf::Event::LostFocus)
			{
				focus = false;
			}
			if (event.type == sf::Event::GainedFocus)
			{
				focus = true;
			}
			if (event.type == sf::Event::MouseWheelScrolled)
			{
				if (event.mouseWheelScroll.wheel == sf::Mouse::VerticalWheel)
				{
					nbXY -= (int)event.mouseWheelScroll.delta;
					if (nbXY < 0)
						nbXY += (int)event.mouseWheelScroll.delta;
					if ((unsigned int)ceil((float)nbXY / ((float)layer->imageSize.x / (float)layer->tileSize)) > layer->imageSize.y / tileSize)
						nbXY += (int)event.mouseWheelScroll.delta;
					spriteTx = nbXY % ((int)layer->imageSize.x / layer->tileSize);
					if (spriteTx == 0)
						spriteTx = layer->imageSize.x / layer->tileSize;
					spriteTy = (int)ceil((float)nbXY / ((float)layer->imageSize.x / (float)layer->tileSize));

					spriteTx = (spriteTx - 1) * layer->tileSize;
					spriteTy = (spriteTy - 1) * layer->tileSize;

					//cout << "spriteTx : " << spriteTx << "  spriteTy : " << spriteTy << endl;
					SpriteTile.setTextureRect(sf::IntRect(spriteTx, spriteTy, layer->tileSize, layer->tileSize));
				}
			}
		}

		if (sf::Mouse::isButtonPressed(sf::Mouse::Left) && focus)
		{
			if (sf::Mouse::getPosition(window).x > 0 && sf::Mouse::getPosition(window).y > 0 && sf::Mouse::getPosition(window).x < winL && sf::Mouse::getPosition(window).y < winH) {
				tabX = (int)floor(sf::Mouse::getPosition(window).x / (winL / maxX));
				tabY = (int)floor(sf::Mouse::getPosition(window).y / (winH / maxY));
				map[currentLayer][tabX][tabY] = nbXY;
			}

		}
		else if (sf::Mouse::isButtonPressed(sf::Mouse::Right) && focus)
		{
			tabX = (int)floor(sf::Mouse::getPosition(window).x / (winL / maxX));
			tabY = (int)floor(sf::Mouse::getPosition(window).y / (winH / maxY));
			map[currentLayer][tabX][tabY] = -1;
		}



		window.clear(sf::Color::White);
		//draw

		for (float i = 0; i < maxY; i++)
		{
			ligneHex.setPosition(sf::Vector2f(0.f, i * winH / maxY));
			window.draw(ligneHex);
		}
		for (float i = 0; i < maxX; i++)
		{
			ligneVex.setPosition(sf::Vector2f(i * winL / maxX, 0.f));
			window.draw(ligneVex);
		}

		for (unsigned int l = 0; l < nbLayer; l++) //draw mapTile!
			for (unsigned int i = 0; i < map[l].size(); i++)
			{
				for (unsigned int y = 0; y < map[l][i].size(); y++)
				{
					if (map[l][i][y] != -1)
					{
						spriteTx = map[l][i][y] % ((int)layers[l]->imageSize.x / layers[l]->tileSize);
						if (spriteTx == 0)
							spriteTx = layers[l]->imageSize.x / layers[l]->tileSize;
						spriteTy = (int)ceil((float)map[l][i][y] / ((float)layers[l]->imageSize.x / (float)layers[l]->tileSize));

						spriteTx = (spriteTx - 1) * layers[l]->tileSize;
						spriteTy = (spriteTy - 1) * layers[l]->tileSize;

						layers[l]->mapSprite.setTextureRect(sf::IntRect(spriteTx, spriteTy, layers[l]->tileSize, layers[l]->tileSize));

						layers[l]->mapSprite.setPosition(sf::Vector2f((float)i * (winL / maxX), (float)y * (winH / maxY)));

						window.draw(layers[l]->mapSprite);
					}
				}
			}


		window.draw(SpriteTile);
		window.display();





		if (_kbhit()) {
			cin >> command;
			if (command == "help")
			{
				cout << "addXY | remXY | addLayer | remLayer | setLayer | nb (debug) | fill | fillPos | ls | save | addEx" << endl;
			}
			if (command == "addXY")
			{
				int addX;
				cout << "combien X?: ";
				cin >> addX;
				int addY;
				cout << "combien Y?: ";
				cin >> addY;

				if (addX > 0 && addY > 0)
				{
					maxX += addX;
					maxY += addY;

					for (int i = 0; i < addY; i++)
						yMap.push_back(-1);
					for (int i = 0; i < addX; i++)
						xMap.push_back(yMap);

					for (unsigned int l = 0; l < map.size(); l++)
					{
						for (unsigned int x = 0; x < map[l].size(); x++)
							for (int i = 0; i < addY; i++)
								map[l][x].push_back(-1);

						for (int i = 0; i < addX; i++)
							map[l].push_back(yMap);
					}

					SpriteTile.setScale(sf::Vector2f(winL / maxX / (float)tileSize, winH / maxY / (float)tileSize)); //reset scale
					MapSprite.setScale(sf::Vector2f(winL / maxX / (float)tileSize, winH / maxY / (float)tileSize));
				}
				else
					cout << "pas de nb -" << endl;
			}
			if (command == "remXY")
			{
				int remX;
				cout << "combien X?: ";
				cin >> remX;
				int remY;
				cout << "combien Y?: ";
				cin >> remY;

				if (remX > 0 && remY > 0)
				{
					maxX -= remX;
					maxY -= remY;

					for (int i = 0; i < remY; i++)
						yMap.pop_back();
					for (int i = 0; i < remX; i++)
						xMap.pop_back();

					for (unsigned int l = 0; l < map.size(); l++)
					{
						for (unsigned int x = 0; x < map[l].size(); x++)
							for (int i = 0; i < remY; i++)
								map[l][x].pop_back();

						for (int i = 0; i < remX; i++)
							map[l].pop_back();
					}

					SpriteTile.setScale(sf::Vector2f(winL / maxX / (float)tileSize, winH / maxY / (float)tileSize)); //reset scale
					MapSprite.setScale(sf::Vector2f(winL / maxX / (float)tileSize, winH / maxY / (float)tileSize));
				}
				else
					cout << "pas de nb -" << endl;

			}
			if (command == "addLayer")
			{
				int addL;
				cout << "combien?: ";
				cin >> addL;

				if (addL > 0)
				{
					nbLayer += addL;

					for (int i = 0; i < addL; i++) {
						map.push_back(xMap);
						string pathTemp;
						cout << "pathTileSet: ";
						cin >> pathTemp;
						int tileSizeTemp;
						cout << "tileSize: ";
						cin >> tileSizeTemp;
						layers.push_back(new Layer(pathTemp, tileSizeTemp, winL, maxX, winH, maxY));
					}
				}
				else
					cout << "pas de nb -" << endl;
			}
			if (command == "remLayer")
			{
				int remL;
				cout << "combien?: ";
				cin >> remL;

				if (remL > 0)
				{
					if (nbLayer - remL > 1)
					{
						nbLayer -= remL;

						for (int i = 0; i < remL; i++) {
							map.pop_back();
							delete layers[layers.size() - 1];
							layers.pop_back();
						}
					}
					else
						cout << "nb indique trop grand" << endl;
				}
				else
					cout << "pas de nb -" << endl;
			}
			if (command == "setLayer")
			{
				int nb;
				cout << "nb: ";
				cin >> nb;
				if (nb < nbLayer)
					currentLayer = nb;
			}
			if (command == "nb")
			{
				cout << "nbXY: ";
				cin >> nbXY;

				spriteTx = nbXY % ((int)layer->imageSize.x / layer->tileSize);
				if (spriteTx == 0)
					spriteTx = layer->imageSize.x / layer->tileSize;
				spriteTy = (int)ceil((float)nbXY / ((float)layer->imageSize.x / (float)layer->tileSize));

				spriteTx = (spriteTx - 1) * layer->tileSize;
				spriteTy = (spriteTy - 1) * layer->tileSize;

				cout << "spriteTx : " << spriteTx << "  spriteTy : " << spriteTy << endl;
				SpriteTile.setTextureRect(sf::IntRect(spriteTx, spriteTy, layer->tileSize, layer->tileSize));
			}
			if (command == "fill")
			{
				cout << "id: ";
				cin >> fillnb;
				for (unsigned int i = 0; i < map[currentLayer].size(); i++)
					for (unsigned int y = 0; y < map[currentLayer][i].size(); y++)
						map[currentLayer][i][y] = fillnb;
			}
			if (command == "fillPos")
			{

			}
			if (command == "ls")
			{
				cout << "nbX: " << maxX << endl;
				cout << "nbY: " << maxY << endl;
				cout << "nbLayer" << map.size() << " || " << nbLayer << endl;
				cout << "currentLayer" << currentLayer << endl;
			}
			if (command == "save")
			{
				//jsonFile = fopen(path.c_str(), "wb+");
				jsonFileOf.open(path, ios::out);

				rapidjson::StringBuffer out;

				rapidjson::Writer<rapidjson::StringBuffer> writer(out);


				writer.StartObject();
				writer.Key("nbLayer");
				writer.Int(nbLayer);
				writer.Key("layers");

				writer.StartArray();
				for (int i = 0; i < nbLayer; i++) {
					writer.StartObject();

					writer.Key("tileSize");
					writer.Int(layers[i]->tileSize);
					writer.Key("tilePath");
					writer.String(layers[i]->tilePath.c_str());

					writer.EndObject();
				}
				writer.EndArray();


				writer.Key("map");
				writer.StartArray();
				for (unsigned int l = 0; l < map.size(); l++)
				{
					writer.StartArray();
					for (unsigned int x = 0; x < map[l].size(); x++)
					{
						writer.StartArray();
						for (unsigned int y = 0; y < map[l][x].size(); y++)
						{
							writer.Int(map[l][x][y]);
						}
						writer.EndArray();
					}
					writer.EndArray();
				}
				writer.EndArray();

				writer.Key("exception");
				writer.StartObject();
				for (mapItEx ITexception = exception.begin(); ITexception != exception.end(); ITexception++)
				{
					writer.Key(ITexception->first.c_str());
					writer.StartArray();
					for (unsigned int i = 0; i < ITexception->second.size(); i++)
						writer.Int(ITexception->second[i]);
					writer.EndArray();
				}
				writer.EndObject();

				writer.EndObject();

				cout << "DONE!" << endl;

				jsonFileOf << out.GetString();
				jsonFileOf.close();
			}
			if (command == "addEx")
			{
				string nameEx; //Ex = exception
				int nbEx = -2;
				bool existEx = false;
				cout << "nom: ";
				cin >> nameEx;
				cout << "num a ajouter:   (marque -1 pour arreter)" << endl;
				cin >> nbEx;
				if (nbEx != -1)
					do {
						for (unsigned int i = 0; i < exception[nameEx].size(); i++)
							if (exception[nameEx][i] == nbEx)
								existEx = true;
						if (!existEx)
							exception[nameEx].push_back(nbEx);

						existEx = false;
						cin >> nbEx;
					} while (nbEx != -1);
			}
		}

	}
	for (int i = 0; i < layers.size(); i++)
	{
		delete layers[i];
	}

	return 0;
}

