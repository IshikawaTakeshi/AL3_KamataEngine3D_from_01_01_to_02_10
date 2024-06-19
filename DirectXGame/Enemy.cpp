#include "Enemy.h"
#include "Math/MyMath/Easing.h"
#include <numbers>

Enemy::~Enemy() {
	delete viewProjection_;
	delete model_;
}

void Enemy::Initialize() {

	velocity_ = { -kWalkSpeed, 0, 0 };
	//worldTransform初期化
	worldTransform_.Initialize();
	//初期回転
	worldTransform_.rotation_.y = std::numbers::pi_v<float> / 2.0f;
	//モデル初期化
	model_ = Model::CreateFromOBJ("enemyPieroFace", true);
	//アニメーション経過時間の初期化
	walkTimer_ = 0.0f;
}

void Enemy::Update() {

	//タイマーの加算
	walkTimer_ += 1.0f / 60.0f;

	//回転アニメーション
	float param = std::sin(2.0f * std::numbers::pi_v<float> * walkTimer_ / kWalkMotionTime);
	float radian = kWalkMotionAngleStart + kWalkMotionAngleEnd * (param + 1.0f) / 2.0f;
	//状態に応じた角度を取得する
	worldTransform_.rotation_.z = Easing::Liner(
		radian,
		kWalkMotionAngleEnd,
		Easing::EaseOut(walkTimer_)
	);

	worldTransform_.translation_ += velocity_;

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
