#include "graphicsHelper.h"
#include "Player.h"
#include <iostream>
#include <vector>
#include <thread>
#include <fstream>
#include <sstream>
#include <chrono>
#include <String>

#include "Enemy.h"

using namespace std;


vector<vector<char>> LoadMap(string path, int &dotsCount, vector<vector<bool>>& dotsMap)
{
	vector<vector<char>> map;
	WCHAR exePath[MAX_PATH];
	GetModuleFileNameW(NULL, exePath, MAX_PATH);
	wstring ws(exePath);
	string pth(ws.begin(), ws.end());
	path = pth.substr(0, pth.length() - 10) + path;
	
	ifstream file(path);
	string line;
	while (getline(file, line))
	{
		map.push_back(vector<char>());
		map.back().reserve(line.size());
		for(char c : line)
		{
			if (c == '.') dotsCount++;
			map.back().push_back(c);
		}
	}
	dotsMap = vector<vector<bool>>(map.size(), vector<bool>(map[0].size(), false));
	for(int i = 0; i < map.size(); i++)
	{
		for(int j = 0; j < map[i].size(); j++)
		{
			if(map[i][j] == '.')
			{
				dotsMap[i][j] = true;
			} 
		}
	}
	return map;
}

void LoadCoords(vector<vector<char>>& map, Player &player, Enemy& chaser, Enemy& ahead, Enemy& dotEater, Enemy& shyEnemy)
{
	for(int i = 0; i < map.size(); i++)
	{
		for(int j = 0; j < map[i].size(); j++)
		{
			switch(map[i][j])
			{
			case 'O':
				player.x = j;
				player.y = i;
				break;
			case '1':
				chaser.x = j;
				chaser.y = i;
				break;
			case '2':
				ahead.x = j;
				ahead.y = i;
				break;
			case '3':
				dotEater.x = j;
				dotEater.y = i;
				break;
			case '4':
				shyEnemy.x = j;
				shyEnemy.y = i;
				break;
			}
		}
	}
}

void LoadDifficulty(int level, Player &player, Enemy &chaser, Enemy &ahead, Enemy &dotEater, Enemy &shyEnemy)
{
	switch(level)
	{
	case 0:
		player.Speed = 3;
		
		chaser.WanderTime = 7;
		chaser.ChaseTime = 20;
		chaser.Speed = 1 << 31;
		chaser.RunTime = 0;
		
		ahead.WanderTime = 5;
		ahead.ChaseTime = 22;
		ahead.Speed = 1 << 31;
		ahead.RunTime = 0;

		dotEater.WanderTime = 1;
		dotEater.Speed = 1 << 31;
		dotEater.ChaseTime = 1;
		dotEater.RunTime = 0;

		shyEnemy.WanderTime = 1;
		shyEnemy.Speed = 1 << 31;
		shyEnemy.ChaseTime = 1;
		shyEnemy.RunTime = 0;
		break;
	case 1:
		player.Speed = 10;
		
		chaser.WanderTime = 7;
		chaser.ChaseTime = 20;
		chaser.Speed = 13;
		chaser.RunTime = 10;
		
		ahead.WanderTime = 14;
		ahead.ChaseTime = 22;
		ahead.Speed = 14;
		ahead.RunTime = 10;

		dotEater.WanderTime = 15;
		dotEater.Speed = 14;
		dotEater.ChaseTime = 22;
		dotEater.RunTime = 10;

		shyEnemy.WanderTime = 15;
		shyEnemy.Speed = 12;
		shyEnemy.ChaseTime = 20;
		shyEnemy.RunTime = 10;
		break;
	default:
		player.Speed = 10;
		
		chaser.WanderTime = 0;		
		chaser.ChaseTime = 1<<20;
		chaser.Speed = 1;
		chaser.RunTime = 0;
		
		ahead.WanderTime = 0;
		ahead.ChaseTime = 1<<20;
		ahead.Speed = 1;
		ahead.RunTime = 0;

		dotEater.WanderTime = 0;
		dotEater.Speed = 1;
		dotEater.ChaseTime = 1 << 20;
		dotEater.RunTime = 0;

		shyEnemy.WanderTime = 8;
		shyEnemy.Speed = 1;
		shyEnemy.ChaseTime = 1 << 20;
		shyEnemy.RunTime = 0;
		break;
	}
}

bool CheckWin(int dotsCount)
{
	if (dotsCount == 0) return true;
	return false;
}

bool CheckLose(vector<coords> enemyCoords, coords playerCoords)
{
	for(auto pos : enemyCoords)
	{
		if(pos.x == playerCoords.x && pos.y == playerCoords.y)
		{
			return true;
		}
	}
	return false;
}

int main()
{
	std::ios_base::sync_with_stdio(false);
	int level = 1;

	cout << "PAC MAN\nWybierz poziom (Na razie tylko 1):\n";
	cin >> level;
	Graphics::Clear();
	Sleep(500);
	
	vector<vector<bool>> dotsMap;
	int dotsCount = 0;
	auto map = LoadMap("pacMap.txt", dotsCount, dotsMap);
	auto oldMap = map;
	
	Graphics::PrintMap(map);
	Enemy chaser;
	Enemy ahead;
	Enemy dotEater;
	Enemy shyEnemy;
	Player player = Player();
	LoadCoords(map, player, chaser, ahead, dotEater, shyEnemy);
	bool lost = false;
	unsigned long long frameCount = 0;
	LoadDifficulty(level, player , chaser, ahead, dotEater, shyEnemy);
	//player.Speed = 3;
	
	while(true)
	{
		lost = CheckLose({ coords(chaser.x, chaser.y), coords(ahead.x, ahead.y), coords(dotEater.x, dotEater.y),
		coords(shyEnemy.x, shyEnemy.y) }, coords(player.x, player.y));
		chrono::steady_clock::time_point begin = chrono::steady_clock::now();
		if(frameCount % chaser.Speed == 0)
		{
			chaser.ChasePlayer(player.x, player.y, map);
			chaser.MakeMove(map, dotsMap, '1');
		}
		if (frameCount % ahead.Speed == 0)
		{
			ahead.ChaseAhead(player.x, player.y, map, player.LastMove);
			ahead.MakeMove(map, dotsMap, '2');
		}
		if (frameCount % dotEater.Speed == 0)
		{
			dotEater.ChaseDots(player.x, player.y, map, dotsMap);
			dotEater.MakeMove(map, dotsMap, '3');
		}
		if (frameCount % shyEnemy.Speed == 0)
		{
			shyEnemy.ChaseAndAvoidGhosts({coords(chaser.x, chaser.y), coords(ahead.x, ahead.y), coords(dotEater.x, dotEater.y) }, player.x, player.y, map);
			shyEnemy.MakeMove(map, dotsMap, '4');
		}
		chrono::steady_clock::time_point end = chrono::steady_clock::now();

		auto sleepTime = 25 - chrono::duration_cast<chrono::milliseconds>(end - begin).count();
		Sleep(sleepTime > 0 ? sleepTime : 0);
		
		if(CheckLose({ coords(chaser.x, chaser.y), coords(ahead.x, ahead.y), coords(dotEater.x, dotEater.y),
		coords(shyEnemy.x, shyEnemy.y)}, coords(player.x, player.y)) || lost)
		{
			player.PauseInput = true;
			Graphics::Clear();
			cout << "Przegrales. Poziom: " << level << ". Nacisnij dowolna litere\n";
			_getch();
			cout << "Wybierz poziom (Na razie tylko 1):\n";
			cin >> level;
			map = LoadMap("pacMap.txt", dotsCount, dotsMap);
			LoadCoords(map, player, chaser, ahead, dotEater, shyEnemy);
			LoadDifficulty(level, player, chaser, ahead, dotEater, shyEnemy);
			Graphics::Clear();
			Sleep(1000);
			Graphics::PrintMap(map);
			oldMap = map;
			frameCount = 0;
			player.PauseInput = false;
			lost = false;
			continue;
		}
		
		if (frameCount % player.Speed == 0) player.MakeMove(map, dotsMap, dotsCount);
		
		if(CheckWin(dotsCount))
		{
			player.PauseInput = true;
			Graphics::Clear();
			cout << "Wygrales poziom "<<level<<". Wynik (im mniejszy tym lepszy) - "<< frameCount <<". Nacisnij dowolna litere\n";
			_getch();
			cout << "Wybierz poziom (Na razie tylko 1):\n";
			cin >> level;
			map = LoadMap("pacMap.txt", dotsCount, dotsMap);
			LoadCoords(map, player, chaser, ahead, dotEater, shyEnemy);
			LoadDifficulty(level, player, chaser, ahead, dotEater, shyEnemy);
			Graphics::Clear();
			Sleep(1000);
			Graphics::PrintMap(map);
			oldMap = map;
			frameCount = 0;
			player.PauseInput = false;
			lost = false;
			continue;
		}
		Graphics::PrintMap(map, oldMap);
		oldMap = map;
		frameCount++;
	}
}

