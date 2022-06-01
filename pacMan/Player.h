#pragma once
#include<conio.h>
#include<vector>
#include<thread>
#include <Windows.h>
using namespace std;
class Player
{
	thread moveHandler;
	void inputListener()
	{
		while (true) {
			if(PauseInput)
			{
				Sleep(25);
				continue;
			}
			char c = _getch();
			switch (c)
			{
			case 'w':
			case 'W':
				LastMove = Move::Up;
				break;
			case 'a':
			case 'A':
				LastMove = Move::Left;
				break;
			case 's':
			case 'S':
				LastMove = Move::Down;
				break;
			case 'd':
			case 'D':
				LastMove = Move::Right;
				break;
			}
		}
	}
public:
	static enum Move
	{
		Left,
		Right,
		Up,
		Down
	};
	bool IaBerserk = false;
	int Speed;
	int x, y;
	bool PauseInput = false;
	Move LastMove = Move::Right;
	void MakeMove(vector<vector<char>> &map, vector<vector<bool>>& dotsMap, int &dotsCount)
	{
		int newX = x;
		int newY = y;
		switch (LastMove)
		{
		case Move::Right:
			newX++;
			break;
		case Move::Left:
			newX--;
			break;
		case Move::Up:
			newY--;
			break;
		case Move::Down:
			newY++;
			break;
		}
		if(map[newY][newX] == 'x' || map[newY][newX] == 'e'||map[newY][newX] == 'x' || map[newY][newX] == '+' || map[newY][newX] == '-' || map[newY][newX] == '|')
		{
			return;
		}
		if (map[newY][newX] == 'P')
		{
			map[y][x] = ' ';
			if(!newX)
			{
				x = map[0].size() - 2;
			}
			else {
				x = 1;
			}
			map[y][x] = 'O';
			if (dotsMap[y][x]) dotsCount--;
			dotsMap[y][x] = false;
			return;
			
		}
		map[y][x] = ' ';
		x = newX;
		y = newY;
		if (dotsMap[y][x]) dotsCount--;
		dotsMap[y][x] = false;
		map[y][x] = 'O';
	}

	Player()
	{
		moveHandler = thread(&Player::inputListener, this);
	}
};

