#include "Player.h"
#include <cassert>

void Player::Initialize() {
	//worldTransform初期化
	worldTransform_.Initialize();
	//モデル初期化
	model_ = Model::CreateFromOBJ("Player02_03", true);
}



void Player::Update() {
	worldTransform_.translation_ = { 0.0f,2.0f,0.0f };
	worldTransform_.UpdateMatrix();
	//行列を定数バッファに転送
	worldTransform_.TransferMatrix();
}

void Player::Draw(const ViewProjection& viewProjection) {

	//モデル描画
	model_->Draw(worldTransform_, viewProjection);
}
