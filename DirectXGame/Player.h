#pragma once

#include "Input.h"
#include "Model.h"
#include "Sprite.h"
#include "WorldTransform.h"
#include "ViewProjection.h"

enum class LRDirection {
	kRight,
	kLeft,
};

/// <summary>
/// プレイヤー
/// </summary>
class Player {
public:

	//加速率
	static inline const float kAcceleration = 0.01f;
	//加速減衰率
	static inline const float kAttenuation = 0.18f;
	//最大速度制限
	static inline const float kLimitRunSpeed = 0.5f;
	//旋回時間<秒>
	static inline const float kTimeTurn = 0.3f;
	//重力加速度(下方向}
	static inline const float kGravityAcceleration = 0.01f;
	//最大落下速度(下方向)
	static inline const float kLimitFallSpeed = 0.6f;
	//ジャンプ初速(上方向)
	static inline const float kjumpAcceleration = 0.3f;

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
	Vector3 velosity_ = {};
	//加速度
	Vector3 acceleration_ = {};
	//プレイヤーの向き
	LRDirection lrDirection_ = LRDirection::kRight;
	//旋回開始時の角度
	float turnFirstRotationY_ = 0.0f;
	//旋回タイマー
	float turnTimer_ = 0.0f;
	//接地状態フラグ
	bool onGround_ = true;
	//着地フラグ
	bool landing = false;
};

