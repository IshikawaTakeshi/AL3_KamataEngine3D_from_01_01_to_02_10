#pragma once

#include "Input.h"
#include "Model.h"
#include "Sprite.h"
#include "WorldTransform.h"
#include "ViewProjection.h"

class Player;
class CameraController {
public:

	//矩形構造体
	struct Rect {
		float left = 0.0f;
		float riight = 1.0f;
		float bottom = 0.0f;
		float top = 1.0f;
	};

	CameraController() = default;
	~CameraController() = default;

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// 更新処理
	/// </summary>
	void Update();

	/// <summary>
	/// リセット処理
	/// </summary>
	void Reset();

	/// <summary>
	/// targetのsetter
	/// </summary>
	void SetTarget(Player* target) { target_ = target; }

	/// <summary>
	/// ViewProjectionの取得
	/// </summary>
	const ViewProjection& GetViewProjection() { return *viewProjection_; }



private:

	WorldTransform worldTransform_;
	ViewProjection* viewProjection_ = nullptr;
	Player* target_ = nullptr;
	//追従対象とカメラの座標の差(オフセット)
	Vector3 targetOffset_ = { 0, 0, -45.0f };
};

