#pragma once

class Timer {
public:
	Timer();

public:
	void Tick(float fLockFPS = 0.0f);
	void Reset();

	float GetTimeElapsed();

private:
	double							m_fTimeScale;
	float							m_fTimeElapsed;

	__int64							m_nCurrentPerformanceCounter;
	__int64							m_nLastPerformanceCounter;
	float							m_fFrameTime[MAX_SAMPLE_COUNT];
	ULONG							m_nSampleCount;
	unsigned long					m_nFramesPerSecond;
	float							m_fFPSTimeElapsed;
	unsigned long					m_nCurrentFrameRate;
	__int64							m_nPerformanceFrequencyPerSec;

};