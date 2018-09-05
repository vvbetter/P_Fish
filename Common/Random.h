#pragma once

extern  UINT RandUInt();
inline INT RandInt()
{
	return (int)RandUInt();
}
//[0 - 1.0f)
inline float RandFloat()
{
	static float UINT_FACTOR = 0.99f / UINT_MAX;
	return RandUInt() * UINT_FACTOR;
}
//[min, max)
inline UINT RandRange(UINT min, UINT max)
{
	return UINT(RandFloat() * (max - min) + min);
}

//���������,trueֻ������ֵ������������
extern void RandomReinit(bool bReset);