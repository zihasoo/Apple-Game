#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <random>

using namespace sf;

struct DragRect {
	int left, top, right, bottom;
};

class GameMain {
	RenderWindow* window;
	int textSize = 50;
	int whiteSpaceX = 300;
	int whiteSpaceY = 150;
	int boardHeight = 10;
	int boardWidth = 17;
	int score = 0;

	std::vector<std::vector<int>> arr;
	std::vector<std::vector<Text>> texts;
	Clock gameTimer;
	Font font;
	Text timeText;
	Text scoreText;

	std::mt19937* gen;
	std::uniform_int_distribution<int>* uni;

	bool isDraging = false;
	bool isCorrect = false;
	Vector2i dragStartPos;
	Vector2i curPos;
	RectangleShape dragRectShape;

	DragRect calcDragRect();
	bool buttonCoolDown();
	void showFinalScene();
	void reset();
	
public:
	GameMain();
	~GameMain();
	void runGame();
};
