#pragma once
#include "Model.h"
#include "WorldTransform.h"

#include <stdint.h>

class Skydome {
public:
	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="model">モデル</param>
	/// <param name="textureHandle">テクスチャハンドル</param>
	/// <param name="viewProjection">ビュープロジェクション</param>
	void Initialize(uint32_t textureHandle, const ViewProjection* viewProjection);

	/// <summary>
	/// 更新処理
	/// </summary>
	void Update();

	/// <summary>
	/// 描画処理
	/// </summary>
	void Draw();

	Skydome() = default;
	~Skydome();

private:
	//ワールド変換データ
	WorldTransform worldTransform_;
	//ビュープロジェクション
	const ViewProjection* viewProjection_ = nullptr;
	//モデル
	Model* model_ = nullptr;
	//テクスチャハンドル
	uint32_t textureHandle_ = 0u;

};

