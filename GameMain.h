#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <random>

#include "models.h"

using namespace sf;


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

	Task* hack_coro;

	bool isDraging = false;
	bool isCorrect = false;
	Vector2i dragStartPos;
	Vector2i curPos;
	RectangleShape dragRectShape;

	DragRect calcDragRect();
	bool buttonCoolDown();
	bool hackCoolDown();
	void showFinalScene();
	void reset();

	int calcRectSum(DragRect range);
	void calcScore(DragRect range);
	void highlightRect(DragRect range);

	Task hack();
	Task isPossible(int r, int c);
	DragRect makeDragRect(int y1, int y2, int x1, int x2);
	
public:
	GameMain();
	~GameMain();
	void runGame();
};
