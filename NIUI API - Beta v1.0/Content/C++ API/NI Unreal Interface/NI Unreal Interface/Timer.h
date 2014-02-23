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

#ifndef NIUI_TIMER_H
#define NIUI_TIMER_H

//****************************************************************************
//**
//**    Timer.h
//**
//**    Copyright (c) 2009 Matthew Robbins
//**
//**    Author:  Matthew Robbins
//**    Created: 12/2009
//**
//****************************************************************************


namespace NIUI
{
	class Timer
	{
	public:
		Timer();

		float getGameTime() const;	//returns game time in seconds
		float getDeltaTime() const;	//returns the change in time in seconds

		void Tick();

		int GetFPS() const;
		float GetSecsPerFrame() const;

	private:
		double mSecondsPerCount;
		double mDeltaTime;
		double mAvSecsPerFrame;

		int FPS;

		int numFrames;
		float timeElapsed;

		__int64 mBaseTime;
		__int64 mPrevTime;
		__int64 mCurrTime;
	};
	
}; // End namespace NIUI.

#endif // #ifndef NIUI_TIMER_H