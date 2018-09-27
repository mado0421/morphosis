#pragma once
#include <Windows.h>
#include <chrono>
#include <random>

/// <summary>
/// ���� ������ ���ν����� ����� Ŭ���� �ν��Ͻ��� �����ɴϴ�.
/// </summary>
///	<param name = "hWnd"> ������ �ڵ��Դϴ�. �ش� ������ �ڵ�� ����� Ŭ���� �ν��Ͻ��� ��ȯ�մϴ�. </param>
/// <returns> ����� Ŭ���� �ν��Ͻ� �����͸� ��ȯ�մϴ�. </returns>
template <typename FrameworkPtr>
FrameworkPtr GetUserDataPtr(HWND hWnd) noexcept
{
	return reinterpret_cast<FrameworkPtr>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
}

/// <summary>
/// ������ ���ν����� Ŭ���� �ν��Ͻ��� �����մϴ�.
/// </summary>
///	<param name = "hWnd"> ������ �ڵ��Դϴ�. �� �ڵ� ������ �����͸� ã�� �� �ְ� �˴ϴ�. </param>
///	<param name = "ptr"> ������ �ڵ�� ����� �������Դϴ�. ��ü�� Ŭ���� �ν��Ͻ��� �����͸� ����մϴ�. </param>
/// <returns> LONG_PTR �������� ������� ��ȯ�մϴ�. </returns>
inline auto SetUserDataPtr(HWND hWnd, LPVOID ptr) noexcept
{
	return ::SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(ptr));
}

/// <summary>
/// HRESULT�� S_FALSE �� ��� �Էµ� �޽����� �Է��� �޽��� �ڽ��� ����ϸ� true�� ��ȯ�մϴ�.
/// </summary>
///	<param name = "hr"> ���� HRSULT ��ü�Դϴ�. S_FALSE�̸� �޽��� �ڽ��� ����մϴ�. </param>
///	<param name = "msg"> �޽����Դϴ�. ���� �����մϴ�. </param>
///	<param name = "captionMsg"> ĸ�� �����Դϴ�. ���� �����մϴ�. </param>
/// <returns> HRESULT ���� S_FALSE�� ��� true�� ��ȯ�մϴ�. </returns>
inline bool FailureMessage(HRESULT hr, LPCTSTR msg = TEXT(""), LPCTSTR captionMsg = TEXT("Error!"))
{
	if (FAILED(hr))
	{
		MessageBox(NULL, msg, captionMsg, MB_OK);
		return true;
	}
	return false;
}

/// <summary> 
/// ���� �ð�(ms)�� ��ȯ�մϴ�.
/// </summary>
inline long long GetCurrTime()
{
	static constexpr long long den{ 1000 };
	static const long long _Freq{ _Query_perf_frequency() };
	static long long _Ctr, _Whole, _Part;
	_Ctr = _Query_perf_counter();
	_Whole = (_Ctr / _Freq) * den;
	_Part = (_Ctr % _Freq) *  den / _Freq;
	return (_Whole + _Part);
}

static long long  gStopwatch = GetCurrTime();
/// <summary> 
/// ���ο� �� ���⸦ �����մϴ�. 
/// </summary>
inline void STOPWATCH_COUNT_START()
{
	gStopwatch = GetCurrTime();
}
/// <summary> 
/// �� ���⸦ ������ ���� �Ǵ� ���� �Լ�ȣ�� �������� ������� �帥 �ð��� Ȯ���մϴ�.
/// </summary>
inline long long STOPWATCH_COUNT_CHECK()
{
	const long long retval{ GetCurrTime() - gStopwatch };
	gStopwatch = GetCurrTime();
	return retval;
}

static default_random_engine				gRE;
static uniform_int_distribution<int>		giDist;
static uniform_real_distribution<float>		gfDist;
/// <summary> 
/// ������ ���� ������ ��ȯ�մϴ�. 
/// </summary>
inline int rand_int() { return giDist(gRE); }
/// <summary> 
/// ������ ���� �Ǽ��� ��ȯ�մϴ�. 
/// </summary>
inline float rand_float() { return gfDist(gRE); }

/// <summary>
/// ���ڷ� ���� �� ���� t ����ŭ ���� �����մϴ�.
/// </summary>
///	<param name = "start"> ���� ���Դϴ�. </param>
///	<param name = "end"> �� ���Դϴ�. </param>
///	<param name = "t"> ���� ���Դϴ�. ��ü�� 0���� 1 ������ �����Դϴ�. </param>
template<typename Ty>
constexpr Ty Interpolation(Ty start, Ty end, float t)
{
	return static_cast<Ty>(start * (1.f - t) + end * t);
}

/// <summary>
/// ���ڷ� ���� ���� x ���� n���� �մϴ�.
/// </summary>
///	<param name = "x"> �ŵ����������� �� �Դϴ�. </param>
///	<param name = "n"> �ŵ����������� ���� �Դϴ�. </param>
constexpr UINT iPow(UINT x, UINT n)
{
	UINT retval = 1;
	if (n < 0) return 0;
	for (UINT i = 0; i < n; ++i) retval *= x;
	return retval;
}


