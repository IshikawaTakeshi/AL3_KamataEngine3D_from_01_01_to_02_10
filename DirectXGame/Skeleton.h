#pragma once
#include "Vector3.h"
#include "Matrix4x4.h"
#include "ViewProjection.h"
#include "WorldTransform.h"
#include "Model.h"
#include "Input.h"

struct Bone {
	Vector3 tip; //ボーンの先端
	Vector3 root; //ボーンの根本
	Vector3 rotation; //ボーンの角度
	float xyLength;
	float yzLength;
	float length; //ボーンの長さ
	
};

class Skeleton {
public:

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(const Vector3& rootPos);

	/// <summary>
	/// 毎フレーム処理
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw(const ViewProjection& viewProjection);

private:

	Input* input_ = nullptr;


	//ボーンの最大回転角(度)
	float maxBornRotation_;
	//ボーンの最小回転角(度)
	float minBornRotation_;



	//ボーン
	std::vector<Bone> bone_;
	std::vector<WorldTransform*> joints_; //関節
	Model* modelJoint_ = nullptr; //関節のモデル
	//目標位置
	WorldTransform* targetPos_ = nullptr;
	Model* targetModel_ = nullptr; //目標位置のモデル

};

