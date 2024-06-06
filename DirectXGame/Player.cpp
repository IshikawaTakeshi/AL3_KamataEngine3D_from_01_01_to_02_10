#define NOMINMAX
#include "Player.h"
#include "Math/MyMath/MyMath.h"
#include "Math/MyMath/Easing.h"
#include "MapChipField.h"
#include "DebugText.h"
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

	Move();

#pragma region 2.衝突判定

	//移動量に速度の値をコピー
	collisionMapInfo.move = velosity_;

	//マップ衝突チェック
	IsCollitionTop(collisionMapInfo);

#pragma endregion

#pragma region 3.判定結果を反映した移動

	//3.判定結果を反映した移動
	MovementByCollision(collisionMapInfo);

	//4.天井に接触している場合の処理
	ProcessWhenTouchCeiling(collisionMapInfo);


#pragma endregion

#pragma region 6.接地状態切り替え
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

#pragma region 7.旋回制御
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

#pragma region 8.行列の更新
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
			velosity_.y += kJumpAcceleration;

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
#pragma endregion
}

void Player::IsCollitionTop(CollisionMapInfo& info) {

	//上昇しているかどうか
	if (info.move.y <= 0) {
		return;
	}

	//移動後の4つの角の座標
	std::array<Vector3, kNumCorner> positionsNew;

	for (uint32_t i = 0; i < positionsNew.size(); i++) {
		positionsNew[i] = CornerPosition(
			MyMath::Add(worldTransform_.translation_, info.move),
			static_cast<Corner>(i));
	}

	MapChipType mapchipType;
	//真上の当たり判定を行う
	bool hit = false;
	//左上の角の判定
	MapChipField::IndexSet indexSet;
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kLeftTop]);
	mapchipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	if (mapchipType == MapChipType::kBlock) {
		hit = true;
	}

	//右上の角の当たり判定
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kRightTop]);
	mapchipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	if (mapchipType == MapChipType::kBlock) {
		hit = true;
	}

	//ブロックに衝突した場合
	if (hit == true) {

		// 上端中央の座標を計算
		Vector3 topCenterPosition;
		topCenterPosition.x = (positionsNew[kLeftTop].x + positionsNew[kRightTop].x) / 2;
		topCenterPosition.y = positionsNew[kLeftTop].y; // 左右で高さは同じ
		topCenterPosition.z = (positionsNew[kLeftTop].z + positionsNew[kRightTop].z) / 2;

		//移動量を調整してめり込みを防止
		indexSet = mapChipField_->GetMapChipIndexSetByPosition(topCenterPosition);

		//めり込み先ブロックの範囲矩形
		MapChipField::Rect rect = mapChipField_->GetRectByIndex(indexSet.xIndex, indexSet.yIndex);
		info.move.y = std::max(0.0f, rect.bottom - worldTransform_.translation_.y - (kHeight / 2 + kBlank));
		//天井に当たったことを記録する
		info.touchCeiling = true;
	}
}


void Player::MovementByCollision(const CollisionMapInfo& info) {
	worldTransform_.translation_ = MyMath::Add(worldTransform_.translation_, info.move);
}

void Player::ProcessWhenTouchCeiling(CollisionMapInfo& info) {

	//天井に接触している場合
	if (info.touchCeiling == true) {
		DebugText::GetInstance()->ConsolePrintf("hit ceiling\n");
		velosity_.y = 0;
		info.touchCeiling = false;
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
