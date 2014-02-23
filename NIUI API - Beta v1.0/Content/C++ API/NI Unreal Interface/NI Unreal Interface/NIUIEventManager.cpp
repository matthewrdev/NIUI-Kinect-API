/*****************************************************************************
*                                                                            *
*  Natural Interaction Unreal Interface (NIUI)                               *
*  Copyright (C) 2011 Matthew Robbins                                        *
*                                                                            *
*  This file is part of NIUI.                                                *
*                                                                            *
*  NIUI is free software: you can redistribute it and/or modify              *
*  it under the terms of the GNU Lesser General Public License as published  *
*  by the Free Software Foundation, either version 3 of the License, or      *
*  (at your option) any later version.                                       *
*                                                                            *
*  NIUI is distributed in the hope that it will be useful,                   *
*  but WITHOUT ANY WARRANTY; without even the implied warranty of            *
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the              *
*  GNU Lesser General Public License for more details.                       *
*                                                                            *
*  You should have received a copy of the GNU Lesser General Public License  *
*  along with NIUI. If not, see <http://www.gnu.org/licenses/>.              *
*                                                                            *
*****************************************************************************/

//****************************************************************************
//**
//**    NIUIEventManager.h
//**
//**    Copyright (c) 2011 Matthew Robbins
//**
//**    Author:  Matthew Robbins
//**    Created: 01/2011
//**
//****************************************************************************

#include <Windows.h>
#include <stdio.h>

#include "NIUIEventManager.h"

#include "Logger.h"

namespace NIUI
{
	EventManager::EventManager()
		: m_events(NULL)
		, m_eventCount(0)
		, m_eventPoolSize(0)
		, m_logEvents(false)
		, m_recieveEvents(true)
	{
		// Not used
	}

	EventManager::~EventManager()
	{
		// Not used.
	}

	void EventManager::Initialise(int eventPoolSize)
	{
		LOG("Event manager has been initilised.", LOG_Init);
		m_events = new Event[eventPoolSize];


		m_eventCount = 0;
		m_eventPoolSize = eventPoolSize;
	}

	void EventManager::Destroy()
	{
		// Release the event pool.
		if (m_events != NULL)
		{
			delete [] m_events;
			m_events = NULL;
		}
	}

	int EventManager::GetEventCount()
	{
		return m_eventCount;
	}

	// Single accessor method, not recomended. Use the methods to fetch entire mem-ptr for the event pool if able.
	bool EventManager::GetEvent(int id, Event &out_event)
	{
		if (id >= m_eventPoolSize)
		{
			char buff[64] = {0};
			sprintf(buff, "Cannot fetch event at %i as event pool size is only of size %i", id, m_eventPoolSize);
			LOG(buff, LOG_Warning);
			return false;
		}
		out_event.theEvent = m_events[id].theEvent;
		out_event.user = m_events[id].user;
		return true;
	}

	void EventManager::ClearEvents()
	{
		m_eventCount = 0;
	}

	void EventManager::AddEvent(const char* theEvent, int user)
	{
		if (!m_recieveEvents)
			return;

		m_events[m_eventCount].theEvent = StringToEvent(theEvent);
		m_events[m_eventCount].user = user;

		if (m_logEvents)
		{
			LogEvent(m_events[m_eventCount]);
		}

		m_eventCount++;
	}

	void EventManager::AddEvent(NIUI_Event theEvent, int user)
	{
		if (!m_recieveEvents)
			return;

		m_events[m_eventCount].theEvent = theEvent;
		m_events[m_eventCount].user = user;

		if (m_logEvents)
		{
			LogEvent(m_events[m_eventCount]);
		}

		m_eventCount++;
	}

	Event* EventManager::GetEventPool()
	{
		return m_events;
	}

	
	void EventManager::LogEvent(const Event &theEvent)
	{
		printf("NIUI Event %s was recieved for user %i.\n", EventToString(theEvent.theEvent), theEvent.user);
	}

	void EventManager::EnableEventLogging(bool enable)
	{
		m_logEvents = enable;
		if (enable)
		{
			printf("NIUI Event dumping has been enabled.\n");
		}
		else
		{
			printf("NIUI Event dumping has been disabled.\n");
		}
	}

	void EventManager::EnableEventRecieving(bool enable)
	{
		m_recieveEvents = enable;
	}
	
	const char* EventToString(NIUI_Event theEvent)
	{
		if (theEvent >= EVENT_Count)
			return "Invalid Event";

		return eventStrings[(int)theEvent];
	}

	NIUI_Event StringToEvent(const char* theString)
	{
		for (int i = 0; i < EVENT_Count; i++)
		{
			if (strcmp(theString, eventStrings[i]) == 0)
				return (NIUI_Event)i;
		}

		return NIUI::EVENT_Count;
	}

}; // End Namespace NIUI