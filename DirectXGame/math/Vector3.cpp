#include "Vector3.h"
#include "MyMath/MyMath.h"

Vector3 operator+(const Vector3& v1, const Vector3& v2) {
	return MyMath::Add(v1, v2);
}
