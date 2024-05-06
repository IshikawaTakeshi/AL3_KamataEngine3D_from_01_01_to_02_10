#include "MyMath.h"
#include <cmath>

/////////////////////////////////////////////////////////////////
///		Vector3
/////////////////////////////////////////////////////////////////

//加算
Vector3 MyMath::Add(const Vector3& v1, const Vector3& v2) {
	Vector3 result;
	result.x = v1.x + v2.x;
	result.y = v1.y + v2.y;
	result.z = v1.z + v2.z;
	return result;
}

//減算
Vector3 MyMath::Subtract(const Vector3& v1, const Vector3& v2) {
	Vector3 result;
	result.x = v1.x - v2.x;
	result.y = v1.y - v2.y;
	result.z = v1.z - v2.z;
	return result;
}

//乗算
Vector3 MyMath::Multiply(float scalar, const Vector3& v) {
	Vector3 result;
	result.x = scalar * v.x;
	result.y = scalar * v.y;
	result.z = scalar * v.z;
	return result;
}

Vector3 MyMath::Multiply(const Vector3& v1, const Vector3& v2) {
	Vector3 result;
	result.x = v1.x * v2.x;
	result.y = v1.y * v2.y;
	result.z = v1.z * v2.z;
	return result;
}

Vector3 MyMath::Devide(float scalar, const Vector3& v) {
	Vector3 result;
	result.x = scalar / v.x;
	result.y = scalar / v.y;
	result.z = scalar / v.z;
	return result;
}

//内積
float MyMath::Dot(const Vector3& v1, const Vector3& v2) {
	float result;
	result = (v1.x * v2.x) + (v1.y * v2.y) + (v1.z * v2.z);
	return result;
}

//クロス積
Vector3 MyMath::Cross(const Vector3& v1, const Vector3& v2) {
	Vector3 result = {
	(v1.y * v2.z) - (v1.z * v2.y),
	(v1.z * v2.x) - (v1.x * v2.z),
	(v1.x * v2.y) - (v1.y * v2.x)
	};
	return result;
}


//長さ(ノルム)
float MyMath::Length(const Vector3& v) {
	float result;
	result = sqrtf((v.x * v.x) + (v.y * v.y) + (v.z * v.z));
	return result;
}

//距離
float MyMath::Distance(const Vector3& v1, const Vector3& v2) {
	return sqrtf(powf(v2.x - v1.x, 2) + powf(v2.y - v1.y, 2) + powf(v2.z - v1.z, 2));
}

//正規化
Vector3 MyMath::Normalize(const Vector3& v) {
	Vector3 result;
	float length = Length(v);
	result.x = v.x / length;
	result.y = v.y / length;
	result.z = v.z / length;
	return result;
}

//数値表示
//void MyMath::VectorScreenPrintf(int x, int y, const Vector3& vector, const char* label) {
//	const static int kColumnWidth = 60;
//	Novice::ScreenPrintf(x, y, "%.02f", vector.x);
//	Novice::ScreenPrintf(x + kColumnWidth, y, "%.02f", vector.y);
//	Novice::ScreenPrintf(x + kColumnWidth * 2, y, "%.02f", vector.z);
//	Novice::ScreenPrintf(x + kColumnWidth * 3, y, "%s", label);
//}


//ベクトル射影
Vector3 MyMath::Project(const Vector3& v1, const Vector3& v2) {
	Vector3 result;
	result = Multiply(Dot(v1, Normalize(v2)), Normalize(v2));
	return result;
}

//最近接点
Vector3 MyMath::ClosestPoint(const Vector3& point, const Segment& segment) {
	Vector3 result;
	result = Add(segment.origin, point);
	return result;
}

float MyMath::Clamp(float value, float min, float max) {
	if (value <= min) {
		return min;
	} else if (value >= max) {
		return max;
	} else {
		return value;
	}
}
