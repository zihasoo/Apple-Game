#include <iostream>
#include <format>
#include "GameMain.h"

GameMain::GameMain() {
	window = new RenderWindow(VideoMode(1440, 800), "Apple-Game!");
	gen = new std::mt19937(time(0));
	uni = new std::uniform_int_distribution<int>(1, 9);
	hack_coro = new Task(hack());

	arr.assign(boardHeight, std::vector<int>(boardWidth));
	texts.assign(boardHeight, std::vector<Text>(boardWidth,Text("0",font,40)));

	std::cout << "R: 게임 재시작\n";
	std::cout << "M: 메크로 작동\n";
	window->setFramerateLimit(144);
	font.loadFromFile("fonts/Pretendard-Regular.ttf");
	timeText.setFont(font);
	scoreText.setFont(font);
	timeText.setPosition(680, 0);
	scoreText.setPosition(1250, 0);
	for (int i = 0; i < boardHeight; ++i)
		for (int j = 0; j < boardWidth; j++) {
			texts[i][j].setPosition(j * textSize + whiteSpaceX, i * textSize + whiteSpaceY);
		}
	reset();
}
GameMain::~GameMain() {
	delete window;
	delete gen;
	delete uni;
	delete hack_coro;
}

DragRect GameMain::calcDragRect() {
	return {
		(std::min(dragStartPos.x, curPos.x) - whiteSpaceX + textSize * 2 / 3) / textSize,
		(std::min(dragStartPos.y, curPos.y) - whiteSpaceY + textSize / 2) / textSize,
		(std::max(dragStartPos.x, curPos.x) - whiteSpaceX - textSize / 4) / textSize,
		(std::max(dragStartPos.y, curPos.y) - whiteSpaceY - textSize / 2) / textSize,
	};
}

int GameMain::calcRectSum(DragRect range) {
	if (range.left < 0 || range.top < 0 || range.right >= boardWidth || range.bottom >= boardHeight)
		return 0;

	int sum = 0;
	for (int i = range.top; i <= range.bottom; i++) {
		for (int j = range.left; j <= range.right; j++) {
			sum += arr[i][j];
		}
	}
	return sum;
}

void GameMain::calcScore(DragRect range) {
	if (range.left < 0 || range.top < 0 || range.right >= boardWidth || range.bottom >= boardHeight)
		return ;

	for (int i = range.top; i <= range.bottom; i++) {
		for (int j = range.left; j <= range.right; j++) {
			if (arr[i][j] == 0) continue;
			score++;
			arr[i][j] = 0;
		}
	}
	scoreText.setString(L"점수: " + std::to_string(score));
}

void GameMain::highlightRect(DragRect range) {
	if (range.left < 0 || range.top < 0 || range.right >= boardWidth || range.bottom >= boardHeight)
		return;

	for (int i = range.top; i <= range.bottom; i++) {
		for (int j = range.left; j <= range.right; j++) {
			texts[i][j].setFillColor(Color(255, 150, 150));
		}
	}
}

bool GameMain::buttonCoolDown() {
	static Clock clock;
	if (clock.getElapsedTime().asSeconds() > 1) {
		clock.restart();
		return true;
	}
	return false;
}

bool GameMain::hackCoolDown() {
	static Clock clock;
	if (clock.getElapsedTime().asSeconds() > 0.8) {
		clock.restart();
		return true;
	}
	return false;
}

void GameMain::reset() {
	score = 0;
	isDraging = false;
	isCorrect = false;
	for (int i = 0; i < boardHeight; ++i) {
		for (int j = 0; j < boardWidth; j++) {
			arr[i][j] = (*uni)(*gen);
			texts[i][j].setString(std::to_string(arr[i][j]));
		}
	}
	scoreText.setString(L"점수: 0");
	gameTimer.restart();
	dragRectShape.setSize({ 0,0 });
	delete hack_coro;
	hack_coro = new Task(hack());
}

void GameMain::showFinalScene() {
	Event event;
	Text scoreText(L"점수: " + std::to_string(score), font, 120);
	Text infoText(L"다시 시작하려면 R", font, 80);
	scoreText.setPosition(530, 200);
	infoText.setPosition(430, 500);
	window->clear();
	window->draw(scoreText);
	window->draw(infoText);
	window->display();
	while (true)
	{
		while (window->pollEvent(event))
		{
			if (event.type == Event::Closed)
				window->close();
		}
		if (Keyboard::isKeyPressed(Keyboard::R)) {
			reset();
			break;
		}
	}
}

void GameMain::runGame(){
	Event event;
	while (window->isOpen())
	{
		while (window->pollEvent(event))
		{
			if (event.type == Event::Closed)
				window->close();
		}

		if (Keyboard::isKeyPressed(Keyboard::Tilde) && hackCoolDown()) {
			if (!hack_coro->co_handler.done()) 
				hack_coro->co_handler.resume();
		}

		if (Keyboard::isKeyPressed(Keyboard::R) && buttonCoolDown())
			reset();

		if (Mouse::isButtonPressed(Mouse::Left))
		{
			curPos = Mouse::getPosition(*window);

			if (!isDraging)
			{
				dragStartPos = curPos;
				dragRectShape.setPosition(Vector2f(dragStartPos));
				isDraging = true;
			}
			dragRectShape.setSize(Vector2f(curPos - dragStartPos));

			int sum = calcRectSum(calcDragRect());

			if (sum == 10) {
				isCorrect = true;
				dragRectShape.setFillColor(Color(255, 180, 180, 100));
			}
			else {
				isCorrect = false;
				dragRectShape.setFillColor(Color(255, 255, 255, 100));
			}
		}
		else if (isDraging)
		{
			if (isCorrect) {
				calcScore(calcDragRect());
			}
			isDraging = false;
			isCorrect = false;
			dragRectShape.setSize({ 0,0 });
		}

		float leftTime = 120 - gameTimer.getElapsedTime().asSeconds();
		timeText.setString(std::format("{:.1f}",leftTime));

		if (leftTime <= 0)
			showFinalScene();

		window->clear();
		for (int i = 0; i < boardHeight; ++i)
			for (int j = 0; j < boardWidth; j++)
				if (arr[i][j] != 0) window->draw(texts[i][j]);

		window->draw(dragRectShape);
		window->draw(scoreText);
		window->draw(timeText);
		window->display();
	}
}

Task GameMain::hack() {
	for (int x = 0; x < 3; x++)
	{
		for (int i = 0; i < boardHeight; ++i) {
			for (int j = 0; j < boardWidth; j++) {
				Task innerTask = isPossible(i, j);
				while (true) {
					innerTask.co_handler.resume();
					if (innerTask.co_handler.done()) break;
					co_await std::suspend_always{};
				}
			}
		}
	}
}

Task GameMain::isPossible(int r, int c) {
	for (int i = r - boardHeight; i <= r + boardHeight; i++)
	{
		for (int j = c - boardWidth; j <= c + boardWidth; j++)
		{
			if (r == i && c == j) continue;
			auto rect = makeDragRect(i, r, j, c);
			int sum = calcRectSum(rect);
			if (sum == 10) {
				//highlightRect(rect);
				calcScore(rect);
				co_await std::suspend_always{};
			}
		}
	}
}

DragRect GameMain::makeDragRect(int y1, int y2, int x1, int x2) {
	return {
		std::min(x1,x2),
		std::min(y1,y2),
		std::max(x1,x2),
		std::max(y1,y2),
	};
}
