#pragma once

#include "Input.h"
#include "Model.h"
#include "Sprite.h"
#include "WorldTransform.h"
#include "ViewProjection.h"

/// <summary>
/// プレイヤー
/// </summary>
class Player {
public:

	//加速率
	static inline const float kAcceleration = 0.005f;
	//加速減衰率
	static inline const float kAttenuation = 0.005f;
	//最大速度制限
	static inline const float kLimitRunSpeed = 3.0f;

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// 更新処理
	/// </summary>
	void Update();

	/// <summary>
	/// 描画処理
	/// </summary>
	void Draw(const ViewProjection& viewProjection);

	void SetPos(Vector3 pos) { worldTransform_.translation_ = pos; }

private:
	//ワールド変換データ
	WorldTransform worldTransform_;
	//ビュープロジェクション
	ViewProjection* viewProjection_ = nullptr;
	//モデル
	Model* model_ = nullptr;
	//速度
	Vector3 velosity_;
	//加速度
	Vector3 acceleration_;
};

