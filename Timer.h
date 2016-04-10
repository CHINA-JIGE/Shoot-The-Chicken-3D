
/***********************************************************************

                           h：NoiseTimer

************************************************************************/

#pragma once


namespace Noise3D
{
	namespace Ut
	{
		class _declspec(dllexport) ITimer
		{
		public:

			//构造函数
			ITimer(NOISE_TIMER_TIMEUNIT timeUnit);

			void		SelfDestruction();

			//在函数名后加const表示此函数不会修改类成员
			//暂停后不再加
			double GetTotalTime()const;

			//获取间隔时间
			double GetInterval()const;

			//设置时间单位
			void SetTimeUnit(NOISE_TIMER_TIMEUNIT timeUnit = NOISE_TIMER_TIMEUNIT_MILLISECOND);

			//进入下一帧
			void NextTick();

			//....
			UINT	 GetFPS() const;

			//暂停计时
			void Pause();

			void Continue();

			//所有数据重置
			void ResetAll();

			void ResetTotalTime();

		private:
			//更新后的总时间(ms)
			double					mTotalTime;
			//就是INTERVAL 两帧间的时间间隔
			double					mDeltaTime;
			//每一count占了多少毫秒
			double					mMilliSecondsPerCount;
			//how many ticks are in current second
			UINT						mCurrentSecondTickCount;
			//a rounded total time
			UINT						mCurrentSecondInteger;
			//frames per second
			UINT						mFPS;

			//时间制
			NOISE_TIMER_TIMEUNIT	mTimeUnit;
			//
			BOOL					mIsPaused;
			//queryPerformance貌似返回的是绝对count而不是两次query间的count
			INT64					mPrevCount;
			//
			INT64					mCurrentCount;
			//
			INT64					mDeltaCount;
		};
	}
}