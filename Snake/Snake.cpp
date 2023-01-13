#include <iostream>
#include <vector>
#include <algorithm>
#include <numeric>
#include <conio.h>
#include <windows.h>
#include <ctime> 

using namespace std;

namespace keysCodes
{
    int const LEFT = 'a',
        UP = 'w',
        RIGHT = 'd',
        DOWN = 's',
        ESC = 27;

    vector<int> arrowsCodes = {
        LEFT,
        UP,
        DOWN,
        RIGHT,
    };
}

namespace game
{
    int const NUMBER_OF_COLUMNS = 30, NUMBER_OF_ROWS = 20, DELAY = 300;
    char const EMPTY_SYMBOL = ' ', BORDER_SYMBOL = '#', SNAKE_SYMBOL = '@', APPLE_SYMBOL = '*';
    bool isAppleEaten = false, isEnd = false, isHit = false, isVictory = false;

    vector<char> border(NUMBER_OF_COLUMNS + 2, BORDER_SYMBOL);
}

struct coordinates
{
    int x, y;
    coordinates(int x, int y) : x(x), y(y) {}
};

void printVector(vector<char>& printedVector)
{
    for_each(printedVector.begin(), printedVector.end(), [](char elm) { cout << elm; });
}

void initField(vector<vector<char>>& field, int numberOfRows, int numberOfColumns, char emptySymbol)
{
    field.clear();
    for (int i = 0; i < numberOfRows; i++)
    {
        field.push_back(vector<char>(numberOfColumns, emptySymbol));
    }
}

void setUpDirection(coordinates& newCoors, int& direction)
{
    if (--newCoors.x < 0)
    {
        game::isHit = true;
        ++newCoors.x;

        if (newCoors.y + 1 < game::NUMBER_OF_COLUMNS)
        {
            direction = keysCodes::RIGHT;
            newCoors.y++;
        }
        else
        {
            direction = keysCodes::LEFT;
            newCoors.y--;
        }
    }   
}

void setDownDirection(coordinates& newCoors, int& direction)
{
   if (++newCoors.x >= game::NUMBER_OF_ROWS)
    {
        game::isHit = true;
        --newCoors.x;

        if (newCoors.y + 1 < game::NUMBER_OF_COLUMNS)
        {
            direction = keysCodes::RIGHT;
            newCoors.y++;
        }
        else
        {
            direction = keysCodes::LEFT;
            newCoors.y--;
        }
    }
}

void setLeftDirection(coordinates& newCoors, int& direction)
{
    if (--newCoors.y < 0)
    {
        game::isHit = true;

        ++newCoors.y;
        if (newCoors.x + 1 < game::NUMBER_OF_ROWS)
        {
            direction = keysCodes::DOWN;
            newCoors.x++;
        }
        else
        {
            direction = keysCodes::UP;
            newCoors.x--;
        }
    }
}

void setRightDirection(coordinates& newCoors, int& direction)
{
    if (++newCoors.y >= game::NUMBER_OF_COLUMNS)
    {
        game::isHit = true;

        --newCoors.y;
        if (newCoors.x + 1 < game::NUMBER_OF_ROWS)
        {
            direction = keysCodes::DOWN;
            newCoors.x++;
        }
        else
        {
            direction = keysCodes::UP;
            newCoors.x--;
        }
    }
}

void updateSnakeCoordinates(vector<vector<char>>& field, vector<coordinates>& snake, int& direction)
{
    coordinates newCoors(snake[0].x, snake[0].y);

    if (!game::isAppleEaten)
    {
        snake.pop_back();
    }
    game::isAppleEaten = false;

    switch (direction)
    {
    case keysCodes::UP: //BBерх
        setUpDirection(newCoors, direction);
        break;

    case keysCodes::DOWN: //вниз
        setDownDirection(newCoors, direction);
        break;

    case keysCodes::LEFT: //влево
        setLeftDirection(newCoors, direction);
        break;

    case keysCodes::RIGHT: //вправо
        setRightDirection(newCoors, direction);
        break;
    };

    bool isEnd = game::isEnd;
    for_each(snake.begin() + 1, snake.end(), [&isEnd, &newCoors](coordinates cors) mutable {
        isEnd = newCoors.x == cors.x && newCoors.y == cors.y
            ? true
            : isEnd;
    });
    game::isEnd = isEnd;

    if (game::isHit)
    {
        snake.pop_back();
    }
    game::isHit = false;

    snake.insert(snake.begin(), newCoors);
}

void updateSnake(vector<vector<char>>& field, vector<coordinates>& snake)
{
    for_each(snake.begin(), snake.end(), [&field](coordinates cors) mutable {field[cors.x][cors.y] = game::SNAKE_SYMBOL; });
}

void updateAppleCoordinates(vector<coordinates>& snake, coordinates& apple, bool& isAppleEaten)
{
    isAppleEaten = snake[0].x == apple.x && snake[0].y == apple.y
        ? true
        : isAppleEaten;

    if (isAppleEaten)
    {
        bool isFreeCoors = true;
        int x, y;
        do
        {
            x = rand() % (game::NUMBER_OF_ROWS - 1) + 1;
            y = rand() % (game::NUMBER_OF_COLUMNS - 1) + 1;

            for_each(snake.begin(), snake.end(), [x, y, &isFreeCoors](coordinates cors) mutable {
                isFreeCoors = cors.x == x && cors.y == y
                    ? false
                    : isFreeCoors;
            });
        } while (!isFreeCoors);

        apple.x = x;
        apple.y = y;
    }
}

void updateApple(vector<vector<char>>& field, coordinates& apple)
{
    field[apple.x][apple.y] = game::APPLE_SYMBOL;
}

void drawField(vector<vector<char>>& field)
{
    printVector(game::border);
    cout << endl;

    for_each(field.begin(), field.end(), [](vector<char> elm) {
        cout << game::BORDER_SYMBOL;
        printVector(elm);
        cout << game::BORDER_SYMBOL << endl;
    });

    printVector(game::border);
}

void setCur(int x, int y)
{
    COORD coord;
    coord.X = x;
    coord.Y = y;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

void draw(vector<vector<char>>& field)
{
    drawField(field);
}

int getKeyIfPressed()
{
    if (_kbhit())
    {
        return _getch();
    }

    return -1;
}

void printResultMessage()
{
    if (game::isVictory)
    {
        cout << "victory";
    }
    else
    {
        cout << "failure";
    }
}

int main()
{
    vector<vector<char>> field;
    vector<coordinates> snake = { {2, 2}, {2, 3} };
    coordinates apple(3, 3);
    int i_input, symbolNotDefined = -1, direction = keysCodes::LEFT;

    srand((unsigned)time(0));

    do
    {
        initField(field, game::NUMBER_OF_ROWS, game::NUMBER_OF_COLUMNS, game::EMPTY_SYMBOL);
        updateSnake(field, snake);
        updateApple(field, apple);

        drawField(field);

        updateAppleCoordinates(snake, apple, game::isAppleEaten);
        updateSnakeCoordinates(field, snake, direction);
        i_input = getKeyIfPressed();

        if (i_input != symbolNotDefined
            && find(keysCodes::arrowsCodes.begin(), keysCodes::arrowsCodes.end(), i_input) != keysCodes::arrowsCodes.end())
        {
            direction = i_input;
        }
        Sleep(game::DELAY);
        setCur(0, 0);

        if (snake.size() < 2)
        {
            game::isEnd = true;
        }

        if (snake.size() == game::NUMBER_OF_COLUMNS * game::NUMBER_OF_ROWS)
        {
            game::isEnd = true;
            game::isVictory = true;
        }
    } while (i_input != keysCodes::ESC && !game::isEnd);

    printResultMessage();
}
