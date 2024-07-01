#include "Skeleton.h"
#include "MyMath/MyMath.h"
#include "MyMath/MatrixMath.h"
#include "PrimitiveDrawer.h"
#include <numbers>
#include <imgui.h>
#include <cstdint>
#include <algorithm>

//関節の要素数
const uint32_t kNumJointHorizonal = 3;
//bornの要素数
const uint32_t kNumBorn = 2;


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
		joints_[row]->translation_.x = kJointHeight * row;
		joints_[row]->translation_.y = kJointWidth;
	}

	modelJoint_ = Model::Create();



	//要素数を変更
	born_.resize(kNumBorn);
	//ボーンの生成
	born_[0] = { {10,0,0}, {0,0,0},{0,0,0},0 };
	born_[0].length = MyMath::Length(born_[0].tip - born_[0].root);
	born_[0].worldMatrix = MatrixMath::MakeIdentity4x4();
	born_[1] = { {20,0,0}, {0,0,0},{0,0,0},0};
	born_[1].length = MyMath::Length(born_[1].tip - born_[1].root);
	born_[1].worldMatrix = MatrixMath::MakeIdentity4x4();
	
	//ボーンの最大・最小回転角の初期化
	maxBornRotation_ = sqrtf(powf(born_[0].length, 2) + powf(born_[1].length, 2) - 2 * born_[0].length * born_[1].length * cosf(170));
	minBornRotation_ = sqrtf(powf(born_[0].length, 2) + powf(born_[1].length, 2) - 2 * born_[0].length * born_[1].length * cosf(30));
}

void Skeleton::Update() {

	//==================================== 1bornIK ==================================//

	born_[0].xyLength = sqrtf(powf(born_[0].tip.x, 2) + powf(born_[0].tip.y, 2));
	born_[0].yzLength = sqrtf(powf(born_[0].tip.y, 2) + powf(born_[0].tip.z, 2));

	//ピッチ回転(X軸)計算
	born_[0].rotation.x = atan2(born_[0].tip.z / born_[0].length, born_[0].xyLength / born_[0].length);

	//ヨー回転(Y軸)計算
	born_[0].rotation.y = atan2(born_[0].tip.y / born_[0].xyLength, born_[0].tip.x / born_[0].xyLength);

	//ロール回転(z軸)計算
	born_[0].rotation.z = atan2(born_[0].tip.y / born_[0].yzLength, born_[0].tip.z / born_[0].yzLength);


	//==================================== 2bornIK ==================================//

	//ボーン全体の長さの初期化
	float bornLength = born_[0].length + born_[1].length;

	//born0の角度の計算
	float born0RotationCos;
	born0RotationCos= cos(powf(bornLength, 2) + powf(born_[0].length, 2) - powf(born_[1].length, 2)) / 2 * bornLength * born_[0].length;
	float born0RotationSin;
	born0RotationSin = sin(sqrtf(1 - powf(born0RotationCos, 2)));
	//born1の角度の計算
	float born1RotationCos;
	born1RotationCos = cos(powf(born_[0].length, 2) + powf(born_[1].length, 2) - powf(bornLength, 2)) / 2 * born_[0].length * born_[1].length;
	float born1RotationSin;
	born1RotationSin = sin(sqrtf(1 - powf(born1RotationCos, 2)));

	//第2関節の回転角が余弦定理で求めた三角形abcの内角Cではなく
	//際には外角のπ-Cとなっている点について考慮しなければならない。

	//ピッチ回転(X軸)計算
	born_[1].rotation.x = atan2(-born0RotationSin, born0RotationCos);

	////ヨー回転(Y軸)計算
	born_[1].rotation.y = atan2(born1RotationSin, -born1RotationCos);

	////ロール回転(z軸)計算
	//born_[0].rotation.z = atan2(born_[0].tip.y / born_[0].yzLength, born_[0].tip.z / born_[0].yzLength);

	born_[1].root = born_[0].tip;

    // 関節の更新
    joints_[0]->translation_ = { born_[0].root.x, born_[0].root.y, born_[0].root.z };
    joints_[1]->translation_ = { born_[1].root.x, born_[1].root.y, born_[1].root.z };
    
    joints_[0]->UpdateMatrix();
    joints_[1]->UpdateMatrix();
   
	born_[0].worldMatrix = MatrixMath::MakeAffineMatrix({ 1.0f,1.0f,1.0f }, born_[0].rotation, born_[0].root);

	//ボーンの操作
	ImGui::Begin("born");
	ImGui::SliderFloat3("born1Translate",&born_[0].tip.x, 0.0f, 90.0f, "%.3f");
	ImGui::SliderFloat3("born2Translate",&born_[1].tip.x, 0.0f, 90.0f, "%.3f");
	ImGui::End();
}

void Skeleton::Draw(const ViewProjection& viewProjection) {

	for (WorldTransform* worldTransformJoint : joints_) {
		modelJoint_->Draw(*worldTransformJoint, viewProjection);
	}

	//ライン描画
	PrimitiveDrawer::GetInstance()->DrawLine3d(born_[0].root,born_[0].tip, {1.0f,0.0f,0.0f,1.0f});
	PrimitiveDrawer::GetInstance()->DrawLine3d(born_[1].root,born_[1].tip, {0.0f,1.0f,0.0f,1.0f});


}
