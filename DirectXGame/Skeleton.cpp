#include "Skeleton.h"
#include "MyMath/MyMath.h"
#include "MyMath/MatrixMath.h"
#include "PrimitiveDrawer.h"
#include "input/Input.h"
#include <numbers>
#include <imgui.h>
#include <cstdint>
#include <algorithm>
#include <cassert>

//関節の要素数
const uint32_t kNumJointHorizonal = 3;
//bornの要素数
const uint32_t kNumBorn = 2;


void Skeleton::Initialize() {

	input_ = Input::GetInstance();


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
	bone_.resize(kNumBorn);

	//ボーン1の生成
	bone_[0].tip = { 10,0,0 };
	bone_[0].root = { 0,0,0 };
	bone_[0].rotation = { 0,0,0 };
	bone_[0].length = MyMath::Length(bone_[0].tip - bone_[0].root);

	//ボーン2の生成
	bone_[1].tip = { 10,0,0 };
	bone_[1].root = { 0,0,0 };
	bone_[1].rotation = { 0,0,0 };
	bone_[1].length = MyMath::Length(bone_[1].tip - bone_[1].root);
	
	//ボーンの最大・最小回転角の初期化
	maxBornRotation_ = sqrtf(powf(bone_[0].length, 2) + powf(bone_[1].length, 2) - 2 * bone_[0].length * bone_[1].length * cosf(170));
	minBornRotation_ = sqrtf(powf(bone_[0].length, 2) + powf(bone_[1].length, 2) - 2 * bone_[0].length * bone_[1].length * cosf(30));

	//目標位置の生成
	targetPos_ = new WorldTransform();
	targetPos_->Initialize();
	targetPos_->translation_ = { 10,0,0 };
	targetPos_->scale_ = { 1.5f,1.5f,1.5f };
	targetModel_ = Model::CreateSphere();
}

void Skeleton::Update() {
#pragma region 1bornIK
	//==================================== 1bornIK ==================================//

	//born_[0].xyLength = sqrtf(powf(born_[0].tip.x, 2) + powf(born_[0].tip.y, 2));
	//born_[0].yzLength = sqrtf(powf(born_[0].tip.y, 2) + powf(born_[0].tip.z, 2));

	////ピッチ回転(X軸)計算
	//born_[0].rotation.x = atan2(born_[0].tip.z / born_[0].length, born_[0].xyLength / born_[0].length);

	////ヨー回転(Y軸)計算
	//born_[0].rotation.y = atan2(born_[0].tip.y / born_[0].xyLength, born_[0].tip.x / born_[0].xyLength);

	////ロール回転(z軸)計算
	//born_[0].rotation.z = atan2(born_[0].tip.y / born_[0].yzLength, born_[0].tip.z / born_[0].yzLength);


	//ボーン全体の長さの初期化
	//float bornLength = born_[0].length + born_[1].length;

	////born0の角度の計算
	//float born0RotationCos;
	//born0RotationCos= cos(powf(bornLength, 2) + powf(born_[0].length, 2) - powf(born_[1].length, 2)) / 2 * bornLength * born_[0].length;
	//float born0RotationSin;
	//born0RotationSin = sin(sqrtf(1 - powf(born0RotationCos, 2)));
	////born1の角度の計算
	//float born1RotationCos;
	//born1RotationCos = cos(powf(born_[0].length, 2) + powf(born_[1].length, 2) - powf(bornLength, 2)) / 2 * born_[0].length * born_[1].length;
	//float born1RotationSin;
	//born1RotationSin = sin(sqrtf(1 - powf(born1RotationCos, 2)));

	////第2関節の回転角が余弦定理で求めた三角形abcの内角Cではなく
	////際には外角のπ-Cとなっている点について考慮しなければならない。

	////ピッチ回転(X軸)計算
	//born_[1].rotation.x = atan2(-born0RotationSin, born0RotationCos);

	//////ヨー回転(Y軸)計算
	//born_[1].rotation.y = atan2(born1RotationSin, -born1RotationCos);

	//////ロール回転(z軸)計算
	////born_[0].rotation.z = atan2(born_[0].tip.y / born_[0].yzLength, born_[0].tip.z / born_[0].yzLength);

	//born_[1].root = born_[0].tip;

 //   // 関節の更新
 //   joints_[0]->translation_ = { born_[0].root.x, born_[0].root.y, born_[0].root.z };
 //   joints_[1]->translation_ = { born_[1].root.x, born_[1].root.y, born_[1].root.z };
 //   
 //   joints_[0]->UpdateMatrix();
 //   joints_[1]->UpdateMatrix();
 //  
	//born_[0].worldMatrix = MatrixMath::MakeAffineMatrix({ 1.0f,1.0f,1.0f }, born_[0].rotation, born_[0].root);
	//born_[1].worldMatrix = MatrixMath::MakeAffineMatrix({ 1.0f,1.0f,1.0f }, born_[1].rotation, born_[1].root);

#pragma endregion

#pragma region 2bornIK
	///////////////////////////////////////////////////////////////////////////////////
	//									 2boneIK 
	///////////////////////////////////////////////////////////////////////////////////
	


	//================================= ボーン1の角度の更新 =================================//

	float bone1Numerator = powf(targetPos_->translation_.x, 2) - powf(targetPos_->translation_.y, 2)
		+ powf(bone_[0].length, 2) - powf(bone_[1].length, 2);
	float bone1Denominator = 2 * bone_[0].length
		* sqrtf(powf(targetPos_->translation_.x, 2) + powf(targetPos_->translation_.y, 2));

	//atanの範囲が-PI/2 ~ PI/2なので場合分けして計算する
	if (bone1Denominator != 0) {
		float npd1 = bone1Numerator / bone1Denominator;
		npd1 = std::clamp(npd1, -1.0f, 1.0f);
		float acosNpd1 = acosf(npd1);
		//float asinNpd1 = asinf(npd1);
		if (targetPos_->translation_.x >= bone_[0].root.x) {

			bone_[0].rotation.z = acosNpd1 + atan2f(targetPos_->translation_.y,targetPos_->translation_.x + std::numbers::pi_v<float>);

			
		} else{ //どちらの値もマイナスの時
			bone_[0].rotation.z = acosNpd1 + atan2f(targetPos_->translation_.y, targetPos_->translation_.x + std::numbers::pi_v<float>);
		}
	}

	//================================= ボーン1の根本・先端の更新 =============================//

	bone_[0].tip.x = bone_[0].length * cosf(bone_[0].rotation.z);
	bone_[0].tip.y = bone_[0].length * sinf(bone_[0].rotation.z);

	//================================= ボーン2の角度の更新 ==================================//

	float bone2Numerator = powf(bone_[0].length, 2) + powf(bone_[1].length, 2) - (powf(MyMath::Length(bone_[0].root + targetPos_->translation_),2));
	float bone2Denominator = 2 * bone_[0].length * bone_[1].length;

	if (bone2Denominator != 0) {
		float npd2 = bone2Numerator / bone2Denominator;
		npd2 = std::clamp(npd2, -1.0f, 1.0f);
		float acosNpd2 = acosf(npd2);

		bone_[1].rotation.z = std::numbers::pi_v<float> + acosNpd2;

	}

	//================================= ボーン2の根本・先端の更新 ==============================//

	bone_[1].root.x = bone_[0].tip.x;
	bone_[1].root.y = bone_[0].tip.y;
	bone_[1].tip.x = targetPos_->translation_.x;
	bone_[1].tip.y = targetPos_->translation_.y;

	//================================= 関節の更新 =========================================//

	joints_[0]->translation_ = { bone_[0].root.x,bone_[0].root.y,bone_[0].root.z };
	joints_[1]->translation_ = { bone_[1].root.x,bone_[1].root.y,bone_[1].root.z };
	joints_[2]->translation_ = { bone_[1].tip.x,bone_[1].tip.y,bone_[1].tip.z };

	//================================= updateMatrix =====================================//

	//born_[0].worldTransform.UpdateMatrix();
	//born_[1].worldTransform.UpdateMatrix();

	joints_[0]->UpdateMatrix();
	joints_[1]->UpdateMatrix();
	joints_[2]->UpdateMatrix();
	targetPos_->UpdateMatrix();
	//========================== ボーンの操作(imgui) ================================//

	ImGui::Begin("debug");
	ImGui::Text("boneDebug");
	ImGui::DragFloat3("bone1TipTranslate", &bone_[0].tip.x, 0.01f);
	ImGui::DragFloat3("bone2rootTranslate", &bone_[1].root.x, 0.01f);
	ImGui::DragFloat3("bone2Translate", &bone_[1].tip.x, 0.01f);
	ImGui::DragFloat3("bone1rotation", &bone_[0].rotation.x, 0.01f);
	ImGui::DragFloat3("bone2rotation", &bone_[1].rotation.x, 0.01f);
	ImGui::Text("boneTransform");
	ImGui::SliderFloat3("bone1RootTranslate", &bone_[0].root.x, -20.0f, 20.0f, "%.3f");
	ImGui::SliderFloat3("targetPosTransform", &targetPos_->translation_.x,-20.0f,20.0f,"%.3f");
	ImGui::Text("joint");
	ImGui::DragFloat3("joint.shoulder", &joints_[0]->translation_.x, 0.01f);
	ImGui::DragFloat3("joint.elbow", &joints_[1]->translation_.x, 0.01f);
	ImGui::DragFloat3("joint.wrist", &joints_[2]->translation_.x, 0.01f);
	ImGui::End();

#pragma endregion
}

void Skeleton::Draw(const ViewProjection& viewProjection) {

	//目標位置描画
	targetModel_->Draw(*targetPos_, viewProjection);


	for (WorldTransform* worldTransformJoint : joints_) {
		modelJoint_->Draw(*worldTransformJoint, viewProjection);
	}
	

	//ライン描画
	PrimitiveDrawer::GetInstance()->DrawLine3d(bone_[0].root, bone_[0].tip, { 1.0f,0.0f,0.0f,1.0f });
	PrimitiveDrawer::GetInstance()->DrawLine3d(bone_[1].root, bone_[1].tip, { 0.0f,1.0f,0.0f,1.0f });

	

}
