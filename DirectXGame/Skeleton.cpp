#include "Skeleton.h"
#include "MyMath/MyMath.h"
#include "PrimitiveDrawer.h"
#include <numbers>
#include <imgui.h>
#include <cstdint>


//関節の要素数
const uint32_t kNumJointHorizonal = 3;



void Skeleton::Initialize() {

	
	//関節1個分の横幅
	const float kJointWidth = 2.0f;
	const float kJointHeight = 2.0f;
	//要素数を変更する
	joints_.resize(kNumJointHorizonal);

	//関節の生成
	for (uint32_t row = 0; row < kNumJointHorizonal; row++) {
		joints_[row] = new WorldTransform();
		joints_[row]->Initialize();
		joints_[row]->translation_.x = kJointHeight * row * 6.0f;
		joints_[row]->translation_.y = kJointWidth;
	}

	//bornの要素数
	const uint32_t kNumBorn = 2;
	//要素数を変更
	born_.resize(kNumBorn);
	//ボーンの生成
	born_[0] = { {10,0,0}, {0,0,0},{0,0,0},0 };
	born_[0].length = MyMath::Distance(born_[0].tip, born_[0].root);
	born_[1] = { {20,0,0}, born_[0].tip,{0,0,0},0 };
	born_[1].length = MyMath::Distance(born_[1].tip, born_[1].root);

	modelJoint_ = Model::Create();
}

void Skeleton::Update() {

	//ボーン1の角度の更新(angle.x)
	float born1NumeratorX = powf(born_[1].tip.x, 2) - powf(born_[1].tip.y, 2)
		+ powf(born_[0].length, 2) - powf(born_[1].length, 2);
	float born1DenominatorX = 2 * born_[0].length
		* sqrtf(powf(born_[1].tip.x, 2) + powf(born_[1].tip.y, 2));
	

	//atanの範囲が-PI/2 ~ PI/2なので場合分けして計算する
	if (born_[1].tip.x >= 0) {
		born_[0].angle.x = acos(born1NumeratorX / born1DenominatorX) + atan(born_[1].tip.y / born_[1].tip.x);
	} 
	else { //xの値がマイナスの時
		born_[0].angle.x = acos(born1NumeratorX / born1DenominatorX) + atan(born_[1].tip.y / born_[1].tip.x) + std::numbers::pi_v<float>;
	}

	//ボーン2の角度の更新(angle.y)
	float born1NumeratorY = powf(born_[1].tip.x, 2) - powf(born_[1].tip.z, 2) + powf(born_[0].length, 2) - powf(born_[1].length, 2);
	float born1DenominatorY = 2 * born_[0].length * sqrtf(powf(born_[1].tip.x, 2) + powf(born_[1].tip.z, 2));

	if (born_[1].tip.x >= 0) {
		born_[0].angle.y = acos(born1NumeratorY / born1DenominatorY) + atan(born_[1].tip.z / born_[1].tip.x);
	} else { // xの値がマイナスの時
		born_[0].angle.y = acos(born1NumeratorY / born1DenominatorY) + atan(born_[1].tip.z / born_[1].tip.x) + std::numbers::pi_v<float>;
	}

	//ボーン1の根本・先端の更新
	born_[0].root.x = 0;
	born_[0].root.y = 0;
	born_[0].tip.x = born_[0].length * cos(born_[0].angle.x);
	born_[0].tip.y = born_[0].length * sin(born_[0].angle.x);

	//ボーン2の角度の更新
	float born2Numerator = powf(born_[0].length, 2) + powf(born_[1].length, 2)
		- (powf(born_[1].tip.x, 2) + powf(born_[1].tip.y, 2));
	float born2Denominator = 2 * born_[0].length * born_[1].length;
	born_[1].angle.x = std::numbers::pi_v<float> + acos(born2Numerator / born2Denominator);

	//ボーン2の根本・先端の更新
	born_[1].root.x = born_[0].length * cos(born_[0].angle.x);
	born_[1].root.y = born_[0].length * sin(born_[0].angle.x);
	born_[1].tip.x = born_[1].root.x + born_[1].length * cos(born_[0].angle.x + born_[1].angle.x);
	born_[1].tip.y = born_[1].root.y + born_[1].length * sin(born_[0].angle.x + born_[1].angle.x);

	//関節の更新
	joints_[0]->translation_ = { born_[0].root.x,born_[0].root.y,born_[0].root.z };
	joints_[1]->translation_ = { born_[1].root.x,born_[1].root.y,born_[1].root.z };
	joints_[2]->translation_ = { born_[1].tip.x,born_[1].tip.y,born_[1].tip.z };

	joints_[0]->UpdateMatrix();
	joints_[1]->UpdateMatrix();
	joints_[2]->UpdateMatrix();

	//ボーンの操作
	ImGui::Begin("born_2");

	ImGui::SliderFloat(
		"tipTranslateX",
		&born_[1].tip.x,
		-1.0f * abs(born_[0].length + born_[1].length),
		abs(born_[0].length + born_[1].length),
		"%.3f"
	);
	ImGui::SliderFloat(
		"tipTranslateY",
		&born_[1].tip.y,
		-1.0f * abs(born_[0].length + born_[1].length + 1.0f),
		abs(born_[0].length + born_[1].length - 1.0f),
		"%.3f"
	);
	ImGui::End();
}

void Skeleton::Draw(const ViewProjection& viewProjection) {

	for (WorldTransform* worldTransformJoint : joints_) {
		modelJoint_->Draw(*worldTransformJoint, viewProjection);
	}

	//ライン描画
	PrimitiveDrawer::GetInstance()->DrawLine3d(born_[0].tip, born_[0].root, { 1.0f,0.0f,0.0f,1.0f });
	PrimitiveDrawer::GetInstance()->DrawLine3d(born_[1].tip, born_[1].root, { 0.0f,1.0f,0.0f,1.0f });


}
