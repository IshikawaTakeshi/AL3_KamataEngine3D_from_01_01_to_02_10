#include "Player.h"
#include "Math/MyMath/MyMath.h"
#include <algorithm>
#include <numbers>
#include <cassert>

void Player::Initialize() {
	//worldTransform初期化
	worldTransform_.Initialize();
	//初期回転
	worldTransform_.rotation_.y = std::numbers::pi_v<float> / 2.0f;
	//モデル初期化
	model_ = Model::CreateFromOBJ("Player02_03", true);
}



void Player::Update() {

	//移動入力
	//左右移動操作
	if (Input::GetInstance()->PushKey(DIK_A) ||
		Input::GetInstance()->PushKey(DIK_D)) {

		if (Input::GetInstance()->PushKey(DIK_D)) {

			//左移動中の右入力
			if (velosity_.x < 0.0f) {
				//ブレーキ
				velosity_.x *= (1.0f - kAttenuation);
			}
			acceleration_.x += kAcceleration;
		} else if (Input::GetInstance()->PushKey(DIK_A)) {

			//右移動中の左入力
			if (velosity_.x > 0.0f) {
				//ブレーキ
				velosity_.x *= (1.0f - kAttenuation);
			}

			acceleration_.x -= kAcceleration;
		}
		//加速・減速
		velosity_ = MyMath::Add(velosity_, acceleration_);

		//最大速度制限
		velosity_.x = std::clamp(velosity_.x, -kLimitRunSpeed, kLimitRunSpeed);

	} else {
		//非入力時に移動減衰をかける
		velosity_.x *= (1.0f - kAttenuation);
	}

	//移動
	worldTransform_.translation_ = MyMath::Add(worldTransform_.translation_, velosity_);

	worldTransform_.UpdateMatrix();
	//行列を定数バッファに転送
	worldTransform_.TransferMatrix();
}

void Player::Draw(const ViewProjection& viewProjection) {

	//モデル描画
	model_->Draw(worldTransform_, viewProjection);
}
