#pragma once

#ifndef NIUI_LOGGER_H
#define NIUI_LOGGER_H

//****************************************************************************
//**
//**    Logger.h
//**
//**    Copyright (c) 2010 Matthew Robbins
//**
//**    Author:  Matthew Robbins
//**    Created: 05/2010
//**
//****************************************************************************

#include <vector>
#include <string>

namespace NIUI
{

	enum LogType
	{						// What will be printed out...
		LOG_Standard,		// Log      :
		LOG_Init,           // Init     :
		LOG_Shutdown,		// Shutdown :
		LOG_Warning,		// Warning  : 
		LOG_Error,			// Error    :
	};

	enum LogLevel
	{
		LogAll = LOG_Standard,
		WarningAndAbove = LOG_Warning,
		ErrorOnly = LOG_Error,
		LogNone = ErrorOnly + 1,
	};

	struct LogItem;
	
	// ------------------------------------------------------------------------
	//								Logger
	// ------------------------------------------------------------------------
	// Desc:
	// The logger is designed to log events as they happen at certain stages of
	// the applications lifetime to help provide an insight into how the 
	// application ran and its possible problems.
	// ------------------------------------------------------------------------
	class Logger
	{
	public:
		static Logger& Instance();
		~Logger();

		// When a log item is submitted this will also dump an item that is inputed to the logger to the console.
		void EnableConsoleDumping(bool enable);
		void EnableFileDumping(bool enable);

		void AddToLogDump(LogItem item);
		void DumpLog();
		void EmptyLog();

		void SetLogLevel(LogLevel level);

	protected:
	private:
		Logger();
		bool m_consoleDump;
		bool m_fileDump;
		LogLevel m_level;

		std::vector<LogItem> m_log;

		char* ToLogType(LogType type);

		// Compares to the current log level.
		bool ShouldLog(LogType type);
	};

	struct LogItem
	{
		std::string info;
		LogType type;
		LogItem(){}
		LogItem(std::string infoIn, LogType typeIn)
		{
			info = infoIn;
			type = typeIn;
		}
	};
	
}; // End namespace NIUI

// Macros to help logging
#define gLogger NIUI::Logger::Instance()

#ifndef IGNORE_LOG
#define LOG(text, type)  NIUI::Logger::Instance().AddToLogDump(NIUI::LogItem(std::string(text), type))
#else
#define LOG(text, type) 
#endif // #ifndef IGNORE_LOG

#endif // #ifndef NIUI_LOGGER_H