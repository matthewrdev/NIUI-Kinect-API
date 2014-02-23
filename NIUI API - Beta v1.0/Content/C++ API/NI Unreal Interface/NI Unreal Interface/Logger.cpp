//****************************************************************************
//**
//**    Logger.cpp
//**
//**    Copyright (c) 2010 Matthew Robbins
//**
//**    Author:  Matthew Robbins
//**    Created: 05/2010
//**
//****************************************************************************

#include <iostream>
#include <fstream>
#include <Windows.h>

#include "Logger.h"

namespace NIUI
{		
	Logger::Logger()
		: m_consoleDump(true)
		, m_fileDump(false)
		, m_level(LogAll)
	{
		m_log.clear();
	}

	Logger::~Logger()
	{
	}

	Logger& Logger::Instance()
	{
		static Logger instance;
		return instance;
	}

	void Logger::EnableConsoleDumping(bool enable)
	{
		m_consoleDump = enable;
	}

	void Logger::SetLogLevel(LogLevel level)
	{
		m_level = level;
		switch (level)
		{
		case LogAll:
			puts("NIUI has been set to NIUI_LogAll. All debug information will be outputed.\n");
			break;
		case WarningAndAbove:
			puts("NIUI has been set to NIUI_WarningAndAbove. Only warnings and critical errors will be outputed.\n");
			break;
		case ErrorOnly:
			puts("NIUI has been set to NIUI_ErrorOnly. Only critical errors will be outputed.\n");
			break;
		case LogNone:
			puts("NIUI has been set to NIUI_LogNone. No debug information will be outputed.\n");
			break;
		}
	}

	void Logger::EnableFileDumping(bool enable)
	{
		m_fileDump = enable;
	}

	void Logger::AddToLogDump(LogItem item)
	{
		if (!ShouldLog(item.type))
			return;

		m_log.push_back(item);
		static char buffer[1024] = {0};
		memset(buffer, 0, 1024);

		if (m_consoleDump)
		{
			sprintf(buffer,"%s %s\n", ToLogType(item.type), item.info.c_str());
			puts(buffer);
		}
	}

	void Logger::DumpLog()
	{
		//if (m_fileDump)
		{
			std::ofstream file;
			file.open("NIUIDebugLog.txt");

			// Dump the loggers contents to the file.
			for (unsigned int i = 0; i < m_log.size(); i++)
			{
				file << ToLogType(m_log[i].type) << m_log[i].info << std::endl;
			}

			file.close();

			printf("NIUI Debuf File dumped.");
			Sleep(1000);
		}
	}

	void Logger::EmptyLog()
	{
		m_log.clear();
	}

	char* Logger::ToLogType(LogType type)
	{
		if (type == LOG_Standard)	
		{
			return "NIUI__Log     : ";
		}
		if (type == LOG_Warning)
		{
			return "NIUI__Warning : ";
		}
		if (type == LOG_Error)	
		{
			return "NIUI__Error   : ";
		}
		if (type == LOG_Init)	
		{
			return "NIUI__Init    : ";
		}
		if (type == LOG_Shutdown)	
		{
			return "NIUI__Shutdown: ";
		}

		return ""; // Shouldnt reach this.
	}

	bool Logger::ShouldLog(LogType type)
	{
		return ((int)type >= (int)m_level);
	}
	
}; // End namespace NIUI