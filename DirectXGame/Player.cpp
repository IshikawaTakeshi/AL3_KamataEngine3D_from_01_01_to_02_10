#define NOMINMAX
#include "Player.h"
#include "Math/MyMath/MyMath.h"
#include "Math/MyMath/Easing.h"
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


	//接地状態
	if (onGround_) {
#pragma region 移動入力
		//======================= 左右移動操作 ========================//
		if (Input::GetInstance()->PushKey(DIK_A) ||
			Input::GetInstance()->PushKey(DIK_D) ||
			Input::GetInstance()->PushKey(DIK_LEFT) ||
			Input::GetInstance()->PushKey(DIK_RIGHT)) {

			if (Input::GetInstance()->PushKey(DIK_D) ||
				Input::GetInstance()->PushKey(DIK_RIGHT)) {

				//左移動中の右入力
				if (velosity_.x < 0.0f) {
					//ブレーキ
					velosity_.x *= (1.0f - kAttenuation);
				}

				//加速
				acceleration_.x += kAcceleration;

				//左右状態切り替え
				if (lrDirection_ != LRDirection::kRight) {
					lrDirection_ = LRDirection::kRight;
					turnFirstRotationY_ = worldTransform_.rotation_.y;
					turnTimer_ = 0.7f;
				}

			} else if (Input::GetInstance()->PushKey(DIK_A) ||
				Input::GetInstance()->PushKey(DIK_LEFT)) {

				//右移動中の左入力
				if (velosity_.x > 0.0f) {
					//ブレーキ
					velosity_.x *= (1.0f - kAttenuation);
				}

				//加速
				acceleration_.x -= kAcceleration;

				//左右状態切り替え
				if (lrDirection_ != LRDirection::kLeft) {
					lrDirection_ = LRDirection::kLeft;
					turnFirstRotationY_ = worldTransform_.rotation_.y;
					turnTimer_ = 0.7f;
				}
			}
			//加速・減速
			velosity_ = MyMath::Add(velosity_, acceleration_);

			//最大速度制限
			velosity_.x = std::clamp(velosity_.x, -kLimitRunSpeed, kLimitRunSpeed);

			if (acceleration_.x >= 0.01f || acceleration_.x <= -0.01f) {
				acceleration_.x = 0;
			}



		} else {
			//非入力時に移動減衰をかける
			velosity_.x *= (1.0f - kAttenuation);
		}

#pragma endregion

#pragma region 衝突判定

		//衝突情報を初期化
		CollisionMapInfo collisionMapInfo;
		//移動量に速度の値をコピー
		collisionMapInfo.move = velosity_;

		//マップ衝突チェック


#pragma endregion

#pragma region ジャンプ入力
		//======================= ジャンプ入力 ====================//
		if (Input::GetInstance()->PushKey(DIK_UP) ||
			Input::GetInstance()->PushKey(DIK_SPACE)) {
			//ジャンプ初速
			velosity_.y += kjumpAcceleration;

		}
#pragma endregion


		//空中にいる場合
#pragma region 空中にいるとき
	} else {
		//======================= 落下速度計算 ====================//

		//落下速度
		velosity_.y += -kGravityAcceleration;
		//落下速度制限
		velosity_.y = std::max(velosity_.y, -kLimitFallSpeed);

		//着地フラグ
		landing = false;
		//地面との当たり判定
		if (velosity_.y < 0) {

			if (worldTransform_.translation_.y <= 2.0f) {
				landing = true;
			}
		}
	}
#pragma endregion

	//移動
	worldTransform_.translation_ = MyMath::Add(worldTransform_.translation_, velosity_);


#pragma region 接地状態切り替え
	//接地判定
	if (onGround_) {
		//ジャンプ開始
		if (velosity_.y > 0.0f) {
			//空中状態に移行
			onGround_ = false;
		}
	} else {
		//着地
		if (landing) {

			//めり込み排斥
			worldTransform_.translation_.y = 2.0f;
			//摩擦で横方向速度が減衰する	
			velosity_.x *= (1.0f - kAttenuation);
			//下方向速度をリセット
			velosity_.y = 0.0f;
			//接地状態に移行
			onGround_ = true;

		}
	}
#pragma endregion

#pragma region 旋回制御
	//======================== 旋回制御 =======================//

	if (turnTimer_ > 0.0f) {

		//タイマーのカウントダウン
		turnTimer_ -= 1.0f / 60.0f;

		//左右の自キャラ角度テーブル
		float destinationRotationYTable[] = {
			std::numbers::pi_v<float> / 2.0f,
			std::numbers::pi_v<float> *3.0f / 2.0f
		};
		//状態に応じた角度を取得する
		float destinationRotationY =
			destinationRotationYTable[static_cast<uint32_t>(lrDirection_)];
		//自キャラの角度を設定する
		worldTransform_.rotation_.y = Easing::Liner(
			destinationRotationY,
			turnFirstRotationY_,
			Easing::EaseInOut(turnTimer_)
		);
	}
#pragma endregion

#pragma region 行列の更新
	worldTransform_.UpdateMatrix();
	//行列を定数バッファに転送
	worldTransform_.TransferMatrix();
#pragma endregion
}

void Player::Draw(const ViewProjection& viewProjection) {

	//モデル描画
	model_->Draw(worldTransform_, viewProjection);
}

void Player::Move() {
#pragma region 移動入力
	//接地状態
	if (onGround_) {

		//======================= 左右移動操作 ========================//
		if (Input::GetInstance()->PushKey(DIK_A) ||
			Input::GetInstance()->PushKey(DIK_D) ||
			Input::GetInstance()->PushKey(DIK_LEFT) ||
			Input::GetInstance()->PushKey(DIK_RIGHT)) {

			if (Input::GetInstance()->PushKey(DIK_D) ||
				Input::GetInstance()->PushKey(DIK_RIGHT)) {

				//左移動中の右入力
				if (velosity_.x < 0.0f) {
					//ブレーキ
					velosity_.x *= (1.0f - kAttenuation);
				}

				//加速
				acceleration_.x += kAcceleration;

				//左右状態切り替え
				if (lrDirection_ != LRDirection::kRight) {
					lrDirection_ = LRDirection::kRight;
					turnFirstRotationY_ = worldTransform_.rotation_.y;
					turnTimer_ = 0.7f;
				}

			} else if (Input::GetInstance()->PushKey(DIK_A) ||
				Input::GetInstance()->PushKey(DIK_LEFT)) {

				//右移動中の左入力
				if (velosity_.x > 0.0f) {
					//ブレーキ
					velosity_.x *= (1.0f - kAttenuation);
				}

				//加速
				acceleration_.x -= kAcceleration;

				//左右状態切り替え
				if (lrDirection_ != LRDirection::kLeft) {
					lrDirection_ = LRDirection::kLeft;
					turnFirstRotationY_ = worldTransform_.rotation_.y;
					turnTimer_ = 0.7f;
				}
			}
			//加速・減速
			velosity_ = MyMath::Add(velosity_, acceleration_);

			//最大速度制限
			velosity_.x = std::clamp(velosity_.x, -kLimitRunSpeed, kLimitRunSpeed);

			if (acceleration_.x >= 0.01f || acceleration_.x <= -0.01f) {
				acceleration_.x = 0;
			}



		} else {
			//非入力時に移動減衰をかける
			velosity_.x *= (1.0f - kAttenuation);
		}

		//======================= ジャンプ入力 ====================//
		if (Input::GetInstance()->PushKey(DIK_UP) ||
			Input::GetInstance()->PushKey(DIK_SPACE)) {
			//ジャンプ初速
			velosity_.y += kjumpAcceleration;

		}

		//空中にいる場合
	} else {
		//======================= 落下速度計算 ====================//

		//落下速度
		velosity_.y += -kGravityAcceleration;
		//落下速度制限
		velosity_.y = std::max(velosity_.y, -kLimitFallSpeed);

		//着地フラグ
		landing = false;
		//地面との当たり判定
		if (velosity_.y < 0) {

			if (worldTransform_.translation_.y <= 2.0f) {
				landing = true;
			}
		}
	}

	//移動
	worldTransform_.translation_ = MyMath::Add(worldTransform_.translation_, velosity_);
#pragma endregion
}

void Player::IsCollitionTop(CollisionMapInfo& info) {
	
	//移動後の4つの角の座標
	std::array<Vector3,4> positionsNew;

	for (uint32_t i = 0; i < positionsNew.size(); i++) {
		positionsNew[i] = CornerPosition(MyMath::Add(worldTransform_.translation_, info.move),
			static_cast<Corner>(i));
	}
}


Vector3 Player::CornerPosition(const Vector3& center, Corner corner) {

	Vector3 offsetTable[kNumCorner] = {
		{ +kWidth / 2.0f, -kHeight / 2.0f, 0 },
		{ -kWidth / 2.0f, -kHeight / 2.0f, 0 },
		{ +kWidth / 2.0f, +kHeight / 2.0f, 0 },
		{ -kWidth / 2.0f, +kHeight / 2.0f, 0 }
	};
	
	return MyMath::Add(center, offsetTable[static_cast<uint32_t>(corner)]);
}
