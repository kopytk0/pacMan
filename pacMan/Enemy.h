#pragma once
#include <vector>
#include <queue>
#include <thread>
#include <chrono>
#include <Windows.h>
#include "Player.h"
#include <time.h>

using namespace std;
static enum Move
{
	Left,
	Right,
	Up,
	Down,
};
struct coords
{
	int x, y;
	coords()
	{
		
	}
	coords(int xc, int yc)
	{
		x = xc;
		y = yc;
	}
};

static class AI
{
	static Move ReverseMove()
	{
		
	}
	
	static Move BFS(int targetX, int targetY, int x, int y, vector<vector<char>>& map)
	{
		if(y == targetY && x == targetX)
		{
			return Move::Right;
		}
		vector<vector<bool>> visited = vector<vector<bool>>(map.size(), vector<bool>(map[0].size(), false));
		queue<pair<coords, Move>> moveQueue;
		moveQueue.push({ coords(x + 1, y), Move::Right });
		moveQueue.push({ coords(x - 1, y), Move::Left });
		moveQueue.push({ coords(x, y + 1), Move::Down });
		moveQueue.push({ coords(x, y - 1), Move::Up });
		visited[y][x] = true;

		while(!moveQueue.empty())
		{
			auto token = moveQueue.front();
			int newX = token.first.x;
			int newY = token.first.y;
			moveQueue.pop();
			visited[newY][newX] = true;
			if(map[newY][newX] == 'x' || map[newY][newX] == '+'||map[newY][newX] == '-' || map[newY][newX] == '|')
			{
				continue;
			}
			if (map[newY][newX] == 'P')
			{
				newX = !newX ? map[0].size() - 2 : 1;
			}
			if(newX == targetX && newY == targetY)
			{
				return token.second;
			}
			if(!visited[newY][newX+1])
			moveQueue.push({ coords(newX + 1, newY), token.second });
			
			if (!visited[newY][newX - 1])
			moveQueue.push({ coords(newX - 1, newY), token.second });
			
			if (!visited[newY + 1][newX])
			moveQueue.push({ coords(newX, newY + 1), token.second });
			
			if (!visited[newY - 1][newX])
			moveQueue.push({ coords(newX, newY - 1), token.second });
			
		}
		
	}

	struct moveData
	{
		coords cords;
		int cost;
		Move move;
		moveData(int x, int y, int cost, Move move)
		{
			this->cords = coords(x, y);
			this->cost = cost;
			this->move = move;
		}
	};
	
	struct dijsktraComparator
	{
		bool operator()(moveData a, moveData b)
		{
			return a.cost > b.cost;
		}
	};
	static Move Dijkstra(int targetX, int targetY, int x, int y, vector<vector<char>>& map, vector<vector<int>>& costMap)
	{
		if (y == targetY && x == targetX)
		{
			return Move::Right;
		}
		vector<vector<bool>> visited = vector<vector<bool>>(map.size(), vector<bool>(map[0].size(), false));
		priority_queue<moveData, vector<moveData>, dijsktraComparator> moveQueue; // cost, coords, move
		moveQueue.push(moveData(x + 1, y, costMap[y][x + 1], Move::Right));
		moveQueue.push(moveData(x - 1, y, costMap[y][x - 1], Move::Left));
		moveQueue.push(moveData(x, y + 1, costMap[y + 1][x], Move::Down));
		moveQueue.push(moveData(x, y - 1, costMap[y - 1][x], Move::Up));
		visited[y][x] = true;

		while (!moveQueue.empty())
		{
			auto token = moveQueue.top();
			int newX = token.cords.x;
			int newY = token.cords.y;
			moveQueue.pop();
			visited[newY][newX] = true;
			if (map[newY][newX] == 'x' || map[newY][newX] == '+' || map[newY][newX] == '-' || map[newY][newX] == '|')
			{
				continue;
			}
			if (map[newY][newX] == 'P')
			{
				newX = !newX ? map[0].size() - 2 : 1;
			}
			if (newX == targetX && newY == targetY)
			{
				return token.move;
			}
			if (!visited[newY][newX + 1])
				moveQueue.push(moveData(newX + 1, newY, token.cost + costMap[newY][newX + 1], token.move));

			if (!visited[newY][newX - 1])
				moveQueue.push(moveData(newX - 1, newY, token.cost + costMap[newY][newX - 1], token.move));

			if (!visited[newY + 1][newX])
				moveQueue.push(moveData(newX, newY + 1, token.cost + costMap[newY + 1][newX], token.move));

			if (!visited[newY - 1][newX])
				moveQueue.push(moveData(newX, newY - 1, token.cost + costMap[newY - 1][newX], token.move));

		}
	}
	
public:
	static Move ChaseDirect(int enemyX, int enemyY, int playerX, int playerY, vector<vector<char>>& map)
	{
		return BFS(playerX, playerY, enemyX, enemyY, map);
	}
	static Move ChaseAhead(int enemyX, int enemyY, int playerX, int playerY, vector<vector<char>>& map, Player::Move PlayerMove)
	{
		int oldPlayerX = playerX, oldPlayerY = playerY;
		switch(PlayerMove)
		{
		case Player::Move::Left:
			playerX -= 3;
			break;
		case Player::Move::Right:
			playerX += 3;
			break;
		case Player::Move::Up:
			playerY -= 3;
			break;
		case Player::Move::Down:
			playerY += 3;
			break;
		}
		if (playerX < 1) playerX = 1;
		if (playerX > map[0].size()-2) playerX = map[0].size() - 2;
		if (playerY < 1) playerY = 1;
		if (playerY > map.size() - 2) playerY = map.size() - 2;

		if(map[playerY][playerX] == 'x' || map[playerY][playerX] == '+' || map[playerY][playerX] == '-' || map[playerY][playerX] == '|')
		{
			playerX = oldPlayerX;
			playerY = oldPlayerY;
		}
		return BFS(playerX, playerY, enemyX, enemyY, map);
	}
	static Move ChaseDots(int enemyX, int enemyY, int playerX, int playerY, vector<vector<char>>& map, vector<vector<bool>> dotsMap)
	{
		vector<vector<int>> costMap = vector<vector<int>>(dotsMap.size(), vector<int>(dotsMap[0].size(), 5));

		for(int i = 0; i < costMap.size(); i++)
		{
			for(int j = 0; j < costMap[0].size(); j++)
			{
				if(dotsMap[i][j])
				{
					costMap[i][j] = 1;
				}
			}
		}

		return Dijkstra(playerX, playerY, enemyX, enemyY, map, costMap);
		
	}
	static Move ChaseAndAvoidGhosts(vector<coords>& ghostPositions, int enemyX, int enemyY, int playerX, int playerY, vector<vector<char>>& map)
	{
		vector<vector<int>> costMap = vector<vector<int>>(map.size(), vector<int>(map[0].size(), 1));

		for(auto cords : ghostPositions)
		{
			for(int x = cords.x - 4; x < cords.x + 4; x++)
			{
				if (x < 0 || x > map[0].size() - 1) continue;
				for(int y = cords.y - 4; y < cords.y + 4; y++)
				{
					if(y < 0 ||y > map.size() - 1) continue;
					costMap[y][x] = 10;
				}
			}
		}
		return Dijkstra(playerX, playerY, enemyX, enemyY, map, costMap);
	}
	static Move Wander(int x, int y, vector<vector<char>> &map, int ghostNum)
	{
		switch(ghostNum)
		{
		case 1:
			if (x == map[0].size() - 2 && y == 1) return Move::Left;
			return BFS(map[0].size() - 2, 1, x, y, map);
		case 2:
			return BFS(1, 1, x, y, map);
		case 3:
			return BFS(1, map.size() - 2, x, y, map);
		case 4:
			if (x == map[0].size() - 2 && y == map.size() - 2) return Move::Left;
			return BFS(map[0].size() - 2, map.size() - 2, x, y, map);
		}
	}
};

class Enemy
{
	thread stateSwap;
	void StateSwapper()
	{
		while(true)
		{
			Sleep(ChaseTime * 1000);
			state = State::wander;
			Sleep(WanderTime * 1000);
			state = State::chase;
		}
	}
public:
	enum State
	{
		chase,
		run,
		wander
	};
	
	int x, y;
	int WanderTime;
	int ChaseTime;
	int RunTime;
	int Speed;
	State state = State::chase;
	bool IsRunning = false;
	
	Move NextMove;
	void ChasePlayer(int playerX, int playerY, vector<vector<char>>& map)
	{
		if(state == State::wander)
		{
			NextMove = AI::Wander(x, y, map, 1);
			return;
		}
		NextMove = AI::ChaseDirect(x, y, playerX, playerY, map);
	}

	void ChaseAhead(int playerX, int playerY, vector<vector<char>>& map, Player::Move playerMove)
	{
		if (state == State::wander)
		{
			NextMove = AI::Wander(x, y, map, 2);
			return;
		}
		NextMove = AI::ChaseAhead(x, y, playerX, playerY, map, playerMove);
	}

	void ChaseDots(int playerX, int playerY, vector<vector<char>>& map, vector<vector<bool>>& dotsMap)
	{
		if (state == State::wander)
		{
			NextMove = AI::Wander(x, y, map, 3);
			return;
		}
		NextMove = AI::ChaseDots(x, y, playerX, playerY, map, dotsMap);
	}

	void ChaseAndAvoidGhosts(vector<coords> ghostCoords, int playerX, int playerY, vector<vector<char>>& map)
	{
		if (state == State::wander)
		{
			NextMove = AI::Wander(x, y, map, 4);
			return;
		}
		NextMove = AI::ChaseAndAvoidGhosts(ghostCoords, x, y, playerX, playerY, map);
	}

	void MakeMove(vector<vector<char>> &map, vector<vector<bool>> &dotsMap, char enemyChar)
	{
		int newX = x;
		int newY = y;
		switch (NextMove)
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
		if (map[newY][newX] == 'x' || map[newY][newX] == '+' || map[newY][newX] == '-' || map[newY][newX] == '|')
		{
			return;
		}
		if (map[newY][newX] == 'P')
		{
			map[y][x] = dotsMap[y][x] ? '.' : ' ';
			if (!newX)
			{
				x = map[0].size() - 2;
			}
			else {
				x = 1;
			}
			map[y][x] = enemyChar;
			dotsMap[y][x] = false;
			return;

		}
		map[y][x] = dotsMap[y][x] ? '.' : ' ';
		x = newX;
		y = newY;
		map[y][x] = enemyChar;
	}

	Enemy()
	{
		stateSwap = thread(&Enemy::StateSwapper, this);
	}
};

