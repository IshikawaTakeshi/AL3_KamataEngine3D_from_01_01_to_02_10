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
	float length; //ボーンの長さ
};

class Skeleton {
public:

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(const Vector3& rootPos);

	/// <summary>
	/// 腕の更新処理
	/// </summary>
	void UpdateArm(const WorldTransform& target);

	/// <summary>
	/// 脚の更新処理
	/// </summary>
	void UpdateLeg(const WorldTransform& target);

	/// <summary>
	/// ImGuiの更新
	/// </summary>
	/// <param name="label"></param>
	void UpdateImGui(const char* label);

	/// <summary>
	/// 描画
	/// </summary>
	void Draw(const ViewProjection& viewProjection);

	/// <summary>
	/// ボーンの根元の座標の取得
	/// </summary>
	Vector3 GetRootPos(int num) {
		return bone_[num].root;
	}

	/// <summary>
	/// ボーンの根元の座標の取得
	/// </summary>
	Vector3 GetTipPos(int num) {
		return bone_[num].tip;
	}



private:

	Input* input_ = nullptr;


	//ボーンの最大回転角(度)
	//float maxBornRotation_;
	//ボーンの最小回転角(度)
	//float minBornRotation_;

	//ボーン
	std::vector<Bone> bone_;
};

