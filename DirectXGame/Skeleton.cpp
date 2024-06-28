#include "Skeleton.h"
#include "MyMath/MyMath.h"
#include "MyMath/MatrixMath.h"
#include "PrimitiveDrawer.h"
#include <numbers>
#include <imgui.h>
#include <cstdint>
#include <algorithm>

//関節の要素数
const uint32_t kNumJointHorizonal = 2;
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
		joints_[row]->translation_.x = kJointHeight * row * 6.0f;
		joints_[row]->translation_.y = kJointWidth;
	}


	//要素数を変更
	born_.resize(kNumBorn);
	//ボーンの生成
	born_[0] = { {10,0,0}, {0,0,0},{0,0,0},0 };
	born_[0].worldMatrix = MatrixMath::MakeIdentity4x4();
	modelJoint_ = Model::Create();
}

void Skeleton::Update() {

	born_[0].xyLength = sqrtf(powf(born_[0].tip.x, 2) + powf(born_[0].tip.y, 2));
	born_[0].yzLength = sqrtf(powf(born_[0].tip.y, 2) + powf(born_[0].tip.z, 2));
	born_[0].length = MyMath::Length(born_[0].tip);
	

	//ピッチ回転(X軸)計算
	born_[0].rotation.x = atan2(born_[0].tip.z / born_[0].length, born_[0].xyLength / born_[0].length);

	//ヨー回転(Y軸)計算
	born_[0].rotation.y = atan2(born_[0].tip.y / born_[0].xyLength, born_[0].tip.x / born_[0].xyLength);

	//ロール回転(z軸)計算
	born_[0].rotation.z = atan2(born_[0].tip.y / born_[0].yzLength, born_[0].tip.z / born_[0].yzLength);

    // ボーン1の根本・先端の更新
    born_[0].root.x = 0;
    born_[0].root.y = 0;
    born_[0].tip.x = born_[0].length * cos(born_[0].rotation.x);
    born_[0].tip.y = born_[0].length * sin(born_[0].rotation.x);

    // ボーン2の根本・先端の更新
    born_[1].root.x = born_[0].length * cos(born_[0].rotation.x);
    born_[1].root.y = born_[0].length * sin(born_[0].rotation.x);
    born_[1].tip.x = born_[1].root.x + born_[1].length * cos(born_[0].rotation.x + born_[1].rotation.x);
    born_[1].tip.y = born_[1].root.y + born_[1].length * sin(born_[0].rotation.x + born_[1].rotation.x);

    // 関節の更新
    joints_[0]->translation_ = { born_[0].root.x, born_[0].root.y, born_[0].root.z };
    joints_[1]->translation_ = { born_[1].root.x, born_[1].root.y, born_[1].root.z };
    
    joints_[0]->UpdateMatrix();
    joints_[1]->UpdateMatrix();
   
	born_[0].worldMatrix = MatrixMath::MakeAffineMatrix({ 1.0f,1.0f,1.0f }, born_[0].rotation, born_[0].root);

	//ボーンの操作
	ImGui::Begin("born_2");

	ImGui::SliderFloat("tipTranslateX",&born_[0].tip.x, 0.1f, 20.0f, "%.3f");
	ImGui::SliderFloat("tipTranslateY",&born_[0].tip.y,-20.0f,20.0f,"%.3f");
	ImGui::SliderFloat("tipTranslateZ",&born_[0].tip.z,-20.0f,20.0f,"%.3f");
	ImGui::End();
}

void Skeleton::Draw(const ViewProjection& viewProjection) {

	for (WorldTransform* worldTransformJoint : joints_) {
		modelJoint_->Draw(*worldTransformJoint, viewProjection);
	}

	//ライン描画
	PrimitiveDrawer::GetInstance()->DrawLine3d(born_[0].root,born_[0].tip, {1.0f,0.0f,0.0f,1.0f});
	//PrimitiveDrawer::GetInstance()->DrawLine3d(born_[1].root, std::clamp(born_[1].tip.x,0.1f,10.0f), { 0.0f,1.0f,0.0f,1.0f });


}
