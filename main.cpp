#include <SFML/Graphics.hpp>
#include <random>
#include <iostream>
using namespace sf;

int textSize = 50;
int whiteSpaceX = 300;
int whiteSpaceY = 150;

int score = 0;

int arr[10][17];
Font font;
Text texts[10][17];
Text timeText;
Text scoreText;

std::mt19937 gen(1);
std::uniform_int_distribution<int> uni(1,9);

bool isDraging = false;
bool isCorrect = false;
Vector2i dragStartPos;
RectangleShape mouseDragRect;

Clock c;
int main()
{
    RenderWindow &window = *new RenderWindow(VideoMode(1440, 800), "SFML works!");
    window.setFramerateLimit(144);
    font.loadFromFile("fonts/Pretendard-Regular.ttf");
    timeText.setFont(font);
    scoreText.setFont(font);
    timeText.setPosition(680, 0);
    scoreText.setPosition(1300, 0);
    scoreText.setString(L"Á¡¼ö: 0");
    mouseDragRect.setFillColor(Color(255,255,255,100));
    for (int i = 0; i < 10; ++i) {
        for (int j = 0; j < 17; j++) {
            arr[i][j] = uni(gen);
            texts[i][j].setFont(font);
            texts[i][j].setCharacterSize(40);
            texts[i][j].setString(std::to_string(arr[i][j]));
            texts[i][j].setPosition(j * textSize + whiteSpaceX, i * textSize + whiteSpaceY);
        }
    }
    while (window.isOpen())
    {
        Event event;
        while (window.pollEvent(event))
        {
            if (event.type == Event::Closed)
                window.close();
        }

        if(Mouse::isButtonPressed(Mouse::Left))
        {
            Vector2i curPos = Mouse::getPosition(window);

	        if(!isDraging)
	        {
                dragStartPos = curPos;
                mouseDragRect.setPosition(Vector2f(dragStartPos));
                isDraging = true;
	        }
            mouseDragRect.setSize(Vector2f(curPos - dragStartPos));

            int stCol = (std::min(dragStartPos.x, curPos.x) - whiteSpaceX - textSize / 4) / textSize;
            int stRow = (std::min(dragStartPos.y, curPos.y) - whiteSpaceY - textSize / 2) / textSize;

            int edCol = (std::max(dragStartPos.x, curPos.x) - whiteSpaceX - textSize / 4) / textSize;
            int edRow = (std::max(dragStartPos.y, curPos.y) - whiteSpaceY - textSize / 2) / textSize;

            std::cout << stRow << ' ' << stCol << ", " << edRow << ' ' << edCol << '\n';

            int sum = 0;
            if (stCol >= 0 && stRow >= 0 && edCol < 17 && edRow < 17) {
                for (int i = stRow; i <= edRow; i++) {
                    for (int j = stCol; j <= edCol; j++) {
                        sum += arr[i][j];
                    }
                }
            }

            if (sum == 10) {
                isCorrect = true;
                mouseDragRect.setFillColor(Color(255, 180, 180, 100));
            }
            else {
                isCorrect = false;
                mouseDragRect.setFillColor(Color(255, 255, 255, 100));
            }
        }
        else if(isDraging)
        {
            if (isCorrect) {
                Vector2i curPos = Mouse::getPosition(window);

                int stCol = (std::min(dragStartPos.x, curPos.x) - whiteSpaceX - textSize / 4) / textSize;
                int stRow = (std::min(dragStartPos.y, curPos.y) - whiteSpaceY - textSize / 2) / textSize;

                int edCol = (std::max(dragStartPos.x, curPos.x) - whiteSpaceX - textSize / 4) / textSize;
                int edRow = (std::max(dragStartPos.y, curPos.y) - whiteSpaceY - textSize / 2) / textSize;

                if (stCol >= 0 && stRow >= 0 && edCol < 17 && edRow < 17) {
                    for (int i = stRow; i <= edRow; i++) {
                        for (int j = stCol; j <= edCol; j++) {
                            arr[i][j] = 0;
                            score++;
                        }
                    }
                    scoreText.setString(scoreText.getString() + std::to_string(score));
                }
            }
            isDraging = false;
            isCorrect = false;
            mouseDragRect.setSize({ 0,0 });
        }

        timeText.setString(std::to_string(120 - c.getElapsedTime().asSeconds()));

        window.clear();
        for (int i = 0; i < 10; ++i) {
            for (int j = 0; j < 17; j++) {
                if(arr[i][j] != 0)
					window.draw(texts[i][j]);
            }
        }
        window.draw(mouseDragRect);
        window.draw(scoreText);
        window.draw(timeText);
        window.display();
    }
    delete& window;
    return 0;
}