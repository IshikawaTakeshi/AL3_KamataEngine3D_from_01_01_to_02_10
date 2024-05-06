#pragma once
#include "Vector3.h"

struct Line final {
	Vector3 origin; //始点
	Vector3 diff; //終点への差分ベクトル
};

struct Ray final {
	Vector3 origin; //始点
	Vector3 diff; //終点への差分ベクトル
};

//線分
struct Segment final {
	Vector3 origin; //始点
	Vector3 diff; //終点への差分ベクトル
};
