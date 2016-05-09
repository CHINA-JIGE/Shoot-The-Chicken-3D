
/***********************************************************************

							类：TIMER

	简述：高精度计时器，主要用WINAPI的queryPerformanceCount来实现    

***********************************************************************/

#include "MyConsoleEngine.h"

using namespace Math;

ITimer::ITimer(TIMER_TIMEUINT timeUnit)
{
	//默认用毫秒制
	mTimeUnit				= timeUnit;
	mMilliSecondsPerCount	= 0.0;
	mDeltaTime				= 0.0;
	mTotalTime				= 0.0;
	mIsPaused				= FALSE;

	//每秒可以数多少次
	INT64 countsPerSecond;
	//获取这个计数计时器的频率
	QueryPerformanceFrequency((LARGE_INTEGER*)&countsPerSecond);
	mMilliSecondsPerCount = (1000.0) /(double)countsPerSecond;//每一count多少毫秒
	QueryPerformanceCounter((LARGE_INTEGER*)&mCurrentCount);

}

void ITimer::SelfDestruction()
{
};

void ITimer::NextTick()
{
	if(mIsPaused)
	{
		mDeltaTime = 0.0;
	}
	//如果没有暂停
	else
	{
		//更新count
		mPrevCount = mCurrentCount;
		QueryPerformanceCounter((LARGE_INTEGER*)&mCurrentCount);

		//如果在省电模式下，若切换处理器可能会导致counts也是负的
		mDeltaCount = mCurrentCount - mPrevCount;
		BOOL isDeltaTimePositive = ((mDeltaCount) > 0);
		if(isDeltaTimePositive)
		{
			mDeltaTime =(double)(mDeltaCount * mMilliSecondsPerCount);
		}
		else
		{
			mDeltaTime = 0.0;
		};

		//没暂停就更新总时间 单位：ms
		mTotalTime += mDeltaTime;

		//...to compute FPS
		++mCurrentSecondTickCount;

		//compute FPS(check if we had fallen into next second)

		if (mCurrentSecondInteger != UINT(mTotalTime/1000.0))
		{
			mFPS = mCurrentSecondTickCount;
			mCurrentSecondTickCount = 0;//reset
			mCurrentSecondInteger = UINT(mTotalTime / 1000.0);
		};
		
	};
}

UINT ITimer::GetFPS() const
{
	return mFPS;
};


double ITimer::GetTotalTime()const
{
	switch(mTimeUnit)
	{
	case TIMER_TIMEUNIT_MILLISECOND:
		return mTotalTime; 
		break;
	case TIMER_TIMEUNIT_SECOND:
		return (mTotalTime/1000.0); 
		break;
	};
	return 0;
};

double ITimer::GetInterval()const
{
	switch(mTimeUnit)
	{
	case TIMER_TIMEUNIT_MILLISECOND:
		return mDeltaTime; 
		break;
	case TIMER_TIMEUNIT_SECOND:
		return (mDeltaTime/1000.0); 
		break;
	};
	return 0;
};

void ITimer::SetTimeUnit(TIMER_TIMEUINT timeUnit)
{
	if (timeUnit ==TIMER_TIMEUNIT_SECOND||timeUnit==TIMER_TIMEUNIT_MILLISECOND)
	{mTimeUnit = timeUnit;};
};

void ITimer::Pause()
{
	mIsPaused = TRUE;
};

void ITimer::Continue()
{
	mIsPaused = FALSE;
};

void ITimer::ResetAll()
{
	mTotalTime	= 0.0;
	mDeltaTime	= 0.0;
	mIsPaused	= FALSE;
};

void ITimer::ResetTotalTime()
{
	mTotalTime = 0;
};
