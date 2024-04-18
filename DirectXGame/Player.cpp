#include "Player.h"
#include <cassert>

void Player::Initialize(Model* model, uint32_t textureHandle) {

	//NULLポインタチェック
	assert(model);

	//モデル初期化
	model_ = model;
	//テクスチャハンドル初期化
	TextureHandle_ = textureHandle;
	//worldTransform初期化
	worldTransform_.Initialize();
}

void Player::Update() {

}

void Player::Draw() {}
