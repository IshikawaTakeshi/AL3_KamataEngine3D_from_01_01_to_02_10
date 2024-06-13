#include "Enemy.h"
#include <numbers>

Enemy::~Enemy() {
	delete viewProjection_;
	delete model_;
}

void Enemy::Initialize() {
	//worldTransform初期化
	worldTransform_.Initialize();
	//初期回転
	worldTransform_.rotation_.y = std::numbers::pi_v<float> / 2.0f;
	//モデル初期化
	model_ = model_ = Model::CreateFromOBJ("enemyPieroFace", true);
}

void Enemy::Update() {

#pragma region 行列の更新
	worldTransform_.UpdateMatrix();
	//行列を定数バッファに転送
	worldTransform_.TransferMatrix();
#pragma endregion
}

void Enemy::Draw(const ViewProjection& viewProjection) {

	//モデル描画
	model_->Draw(worldTransform_, viewProjection);
}
