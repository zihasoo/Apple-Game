#include "GameMain.h"
#include <iostream>
#include <format>

GameMain::GameMain() {
	window = new RenderWindow(VideoMode(1440, 800), "Apple-Game!");
	gen = new std::mt19937(time(0));
	uni = new std::uniform_int_distribution<int>(1, 9);

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
}

void GameMain::showFinalScene() {
	Event event;
	Text finalText(L"게임 종료", font, 120);
	Text infoText(L"다시 시작하려면 R", font, 80);
	finalText.setPosition(500, 200);
	infoText.setPosition(430, 400);
	window->clear();
	window->draw(finalText);
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

DragRect GameMain::calcDragRect() {
	return {
	(std::min(dragStartPos.x, curPos.x) - whiteSpaceX + textSize * 2 / 3) / textSize,
	(std::min(dragStartPos.y, curPos.y) - whiteSpaceY + textSize / 2) / textSize,
	(std::max(dragStartPos.x, curPos.x) - whiteSpaceX - textSize / 4) / textSize,
	(std::max(dragStartPos.y, curPos.y) - whiteSpaceY - textSize / 2) / textSize,
	};
}

bool GameMain::buttonCoolDown() {
	static Clock clock;
	if (clock.getElapsedTime().asSeconds() > 1) {
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

		if (Keyboard::isKeyPressed(Keyboard::M) && buttonCoolDown()) {

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

			DragRect dragRect = calcDragRect();

			int sum = 0;
			if (dragRect.left >= 0 && dragRect.top >= 0 && dragRect.right < boardWidth && dragRect.bottom < boardHeight) {
				for (int i = dragRect.top; i <= dragRect.bottom; i++) {
					for (int j = dragRect.left; j <= dragRect.right; j++) {
						sum += arr[i][j];
					}
				}
			}

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
				DragRect dragRect = calcDragRect();

				if (dragRect.left >= 0 && dragRect.top >= 0 && dragRect.right < boardWidth && dragRect.bottom < boardHeight) {
					for (int i = dragRect.top; i <= dragRect.bottom; i++) {
						for (int j = dragRect.left; j <= dragRect.right; j++) {
							arr[i][j] = 0;
							score++;
						}
					}
					scoreText.setString(L"점수: " + std::to_string(score));
				}
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