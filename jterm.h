/*
 * jterm library
 * by Josh Bedwell
 * version 0.0
 *
 * This library is my wrapper for ncurses.
 *
 * In CMake:
 * find_package(Curses REQUIRED)
 * include_directories(${CURSES_INCLUDE_DIR})
 * ...
 * target_link_libraries(your_executable_name ${CURSES_LIBRARIES})
 */

#ifndef JTERM_H
#define JTERM_H

#include <iostream>
#include <mutex>
#include <string>
#include <vector>

#include <ncurses.h>

namespace jterm
{

class Term
{
public:
	static Term& getInstance()
	{
		static std::once_flag flag;
		std::call_once(flag, [] {instance();});
		return instance();
	}
	static void exit()
	{
		endwin();
	}

	void addLine()
	{
		line++;
	}
	void addLine(const std::string& msg)
	{
		for (const auto& msgLine : breakMessage(msg))
		{
			addSingleLine(msgLine);
		}
	}
	void addLines(const std::vector<std::string>& msgs)
	{
		for (const auto& msg : msgs)
		{
			addLine(msg);
		}
	}
	void replaceLine(const std::string& msg)
	{
		clearLine(line);
		if (line > 0)
		{
			line--;
			clearLine(line);
		}
		addLine(msg);
	}

	void clearScreen()
	{
		clear();
		line = 0;
	}
	static void clearLine(int lineNum)
	{
		int y, x;
		getyx(stdscr, y, x);
		move(lineNum, 0);
		clrtoeol();
		move(y, x);
	}

	static void pauseTerm()
	{
		endwin();
		for (int i = 0; i < numClearLines; i++)
		{
			std::cout << std::endl;
		}
	}
	static void resumeTerm()
	{
		initscr();
		cbreak();
		noecho();
		clear();
	}

private:
	Term()
	{
		initscr();
		cbreak();
		noecho();
		clear();
	}
	static Term& instance()
	{
		static Term instance;
		return instance;
	}

	static const int numClearLines = 100;

	void addSingleLine(const std::string& msg)
	{
		mvaddstr(line, 0, msg.c_str());
		line++;
	}

	static std::vector<std::string> breakMessage(std::string msg)
	{
		int screenWidth, screenHeight;
		getmaxyx(stdscr, screenHeight, screenWidth);

		std::vector<std::string> lines;

		int i = 0;
		while (!msg.empty())
		{
			if (i == msg.size() - 1)
			{
				lines.push_back(msg);
				return lines;
			}
			if (msg.at(i) == '\n')
			{
				lines.push_back(msg.substr(0, i));
				msg = msg.substr(i + 1);
				i = 0;
			}
			else if (i == screenWidth)
			{
				lines.push_back(msg.substr(0, i));
				msg = msg.substr(i);
				i = 0;
			}
			else
			{
				i++;
			}
		}

		return lines;
	}

	size_t line = 0;
};

}

#endif //JTERM_H
