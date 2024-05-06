#pragma once

#include "Audio.h"
#include "DirectXCommon.h"
#include "Input.h"
#include "Model.h"
#include "Sprite.h"
#include "DebugCamera.h"
#include "ViewProjection.h"
#include "WorldTransform.h"

#include <numbers>


/// <summary>
/// ゲームシーン
/// </summary>
class GameScene {

public: // メンバ関数
	/// <summary>
	/// コンストクラタ
	/// </summary>
	GameScene();

	/// <summary>
	/// デストラクタ
	/// </summary>
	~GameScene();

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
	void Draw();

private: // メンバ変数
	DirectXCommon* dxCommon_ = nullptr;
	Input* input_ = nullptr;
	Audio* audio_ = nullptr;

	/// <summary>
	/// ゲームシーン用
	/// </summary>

	struct Born {
		Vector3 tip; //ボーンの先端
		Vector3 root; //ボーンの根本
		Vector3 angle; //ボーンの角度
		float length; //ボーンの長さ
	};

	//ボーン
	std::vector<Born> born_;

	Model* modelJoint_ = nullptr; //関節のモデル
	bool isDebugCameraActive_ = false;
	DebugCamera* debugCamera_ = nullptr;
	Matrix4x4 cameraMarix_;
	WorldTransform worldTrasform_;
	std::vector<WorldTransform*> joints_; //関節
	ViewProjection viewProjection_;
};
