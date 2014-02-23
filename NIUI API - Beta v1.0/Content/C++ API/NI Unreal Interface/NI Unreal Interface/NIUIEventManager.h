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

#ifndef NIUI_EVENT_MANAGER_H
#define NIUI_EVENT_MANAGER_H

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

// Forward Declarations
namespace NIUI
{
}; // End Namespace NIUI

namespace NIUI
{

	enum NIUI_Event
	{
		EVENT_UserLost,
		EVENT_UserDetected,
		EVENT_UserPoseDetected,
		EVENT_UserPoseLost,
		EVENT_CalibrationStart,
		EVENT_CalibrationSuccess,
		EVENT_CalibrationFail,
		EVENT_TrackingStart,

		EVENT_Count,
	};

	struct Event
	{
		NIUI_Event theEvent;
		int user;
	};

	static const char* eventStrings[] = { "UserLost",
								  "NewUser",
								  "PoseDetected",
								  "PoseLost",
								  "CalibrationStart",
								  "CalibrationSuccess",
								  "CalibrationFailed",
								  "TrackingStarted",
									};

	// ------------------------------------------------------------------------
	//								EventManager
	// ------------------------------------------------------------------------

	class EventManager
	{
	private:
		Event* m_events;
		int m_eventCount;
		int m_eventPoolSize;

		bool m_logEvents;
		bool m_recieveEvents;

		void LogEvent(const Event &theEvent);

	protected:
	public:

		EventManager();
		~EventManager();

		void Initialise(int eventPoolSize);
		void Destroy();

		int GetEventCount();

		// Single accessor method, not recomended. Use the methods to fetch entire mem-ptr for the event pool if able.
		bool GetEvent(int id, Event &out_event);

		void ClearEvents();
		void AddEvent(NIUI_Event theEvent, int user);
		void AddEvent(const char* theEvent, int user);

		Event* GetEventPool();

		void EnableEventLogging(bool enable);

		void EnableEventRecieving(bool enable);
	};

	const char* EventToString(NIUI_Event theEvent);
	NIUI_Event StringToEvent(const char* theString);

}; // End Namespace NIUI


#endif // #ifndef NIUI_EVENT_MANAGER_H