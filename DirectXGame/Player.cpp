#include "Player.h"
#include <cassert>

void Player::Initialize(Model* model, uint32_t textureHandle, ViewProjection* viewProjection) {

	//NULLポインタチェック
	assert(model);

	//モデル初期化
	model_ = model;
	//テクスチャハンドル初期化
	TextureHandle_ = textureHandle;
	//worldTransform初期化
	worldTransform_.Initialize();
	//viewProjection初期化
	viewProjection_ = viewProjection;
}



void Player::Update() {

	//行列を定数バッファに転送
	worldTransform_.TransferMatrix();
}

void Player::Draw() {

	//モデル描画
	model_->Draw(worldTransform_, *viewProjection_, TextureHandle_);
}
