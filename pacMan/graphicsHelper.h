#include <Windows.h>
#pragma once
#include <iostream>
#include <vector>
using namespace std;
static class Graphics
{
public:
    static void Clear() {
        DWORD n;                        
        DWORD size;                   
        COORD coord = { 0 };              
        CONSOLE_SCREEN_BUFFER_INFO csbi;  
        HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
        GetConsoleScreenBufferInfo(h, &csbi);      
        size = csbi.dwSize.X * csbi.dwSize.Y;
       
        FillConsoleOutputCharacter(h, TEXT(' '), size, coord, &n);
        GetConsoleScreenBufferInfo(h, &csbi);
        FillConsoleOutputAttribute(h, csbi.wAttributes, size, coord, &n);       
        SetConsoleCursorPosition(h, coord);
    }
    static void SetCursorPosition(int x, int y)
    {
        static const HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
        std::cout.flush();
        COORD coord = { (SHORT)x, (SHORT)y };
        SetConsoleCursorPosition(hOut, coord);
    }
	static void PrintMap(vector<vector<char>>& map)
    {
        SetCursorPosition(0, 0);
        Clear();
	    for(auto line : map)
	    {
		    for(auto c : line)
		    {
		    	if(c == 'e' || c == 'P')
		    	{
                    cout << ' ';
                    continue;
		    	}
                cout << c;
		    }
            cout << "\n";
	    }
    }
    static void PrintMap(vector<vector<char>>& map, vector<vector<char>>& oldMap)
    {
        for (int i = 0; i < map.size(); i++)
        {
            for (int j = 0; j < map[i].size(); j++)
            {
                if(map[i][j]!=oldMap[i][j])
                {
                    SetCursorPosition(j, i);
                    cout << map[i][j];
                }
            }
            cout << "\n";
        }
        SetCursorPosition(0, 0);
    }
};

