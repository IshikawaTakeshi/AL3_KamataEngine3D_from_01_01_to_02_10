#pragma once
#include "Vector3.h"
#include "ViewProjection.h"
#include "WorldTransform.h"
#include "Model.h"

struct Born {
	Vector3 tip; //ボーンの先端
	Vector3 root; //ボーンの根本
	Vector3 angle; //ボーンの角度
	float length; //ボーンの長さ
};

class Skeleton {
public:

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// 毎フレーム処理
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw(const ViewProjection& viewProjection);

private:

	//ボーン
	std::vector<Born> born_;
	std::vector<WorldTransform*> joints_; //関節
	Model* modelJoint_ = nullptr; //関節のモデル

};

