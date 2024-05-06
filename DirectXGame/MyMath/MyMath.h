#pragma once
#include <Vector3.h>
#include "Lines.h"

class MyMath {
public:
	//加算
	static Vector3 Add(const Vector3& v1, const Vector3& v2);

	//減算
	static Vector3 Subtract(const Vector3& v1, const Vector3& v2);

	//乗算
	static Vector3 Multiply(float scalar, const Vector3& v);

	//乗算
	static Vector3 Multiply(const Vector3& v1, const Vector3& v2);

	//商算
	static Vector3 Devide(float scalar, const Vector3& v);

	//内積
	static float Dot(const Vector3& v1, const Vector3& v2);

	//クロス積
	static Vector3 Cross(const Vector3& v1, const Vector3& v2);

	//長さ(ノルム)
	static float Length(const Vector3& v);

	//距離
	static float Distance(const Vector3& v1, const Vector3& v2);

	//正規化
	static Vector3 Normalize(const Vector3& v);

	//数値表示
	//static void VectorScreenPrintf(int x, int y, const Vector3& vector, const char* label);

public:

	//ベクトル射影
	static Vector3 Project(const Vector3& v1, const Vector3& v2);

	//最近接点
	static Vector3 ClosestPoint(const Vector3& point, const Segment& segment);

public:
	static float Clamp(float value, float min, float max);
	
};
