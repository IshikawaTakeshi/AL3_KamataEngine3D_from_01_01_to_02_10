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

//bornの要素数
const uint32_t kNumBorn = 2;


void Skeleton::Initialize(const Vector3& rootPos) {

	input_ = Input::GetInstance();

	//要素数を変更
	bone_.resize(kNumBorn);

	//ボーン1の生成
	bone_[0].tip = { 10,0,rootPos.z };
	bone_[0].root = rootPos;
	bone_[0].rotation = { 0,0,0 };
	bone_[0].length = 10.0f;

	//ボーン2の生成
	bone_[1].tip = { 10,0,rootPos.z };
	bone_[1].root = { 0,0,rootPos.z };
	bone_[1].rotation = { 0,0,0 };
	bone_[1].length = 10.0f;

	//ボーンの最大・最小回転角の初期化
	//maxBornRotation_ = sqrtf(powf(bone_[0].length, 2) + powf(bone_[1].length, 2) - 2 * bone_[0].length * bone_[1].length * cosf(170));
	//minBornRotation_ = sqrtf(powf(bone_[0].length, 2) + powf(bone_[1].length, 2) - 2 * bone_[0].length * bone_[1].length * cosf(30));
}

void Skeleton::UpdateArm(const WorldTransform& target) {
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

	float bone1Numerator = -powf(bone_[1].length, 2) + powf(bone_[0].length, 2)
		+ (powf(target.translation_.x - bone_[0].root.x, 2) + powf(target.translation_.y - bone_[0].root.y, 2));

	float bone1Denominator = 2 * bone_[0].length
		* sqrtf(powf(target.translation_.x - bone_[0].root.x, 2) + powf(target.translation_.y - bone_[0].root.y, 2));

	//atanの範囲が-PI/2 ~ PI/2なので場合分けして計算する
	if (bone1Denominator != 0) {
		float npd1 = bone1Numerator / bone1Denominator;
		npd1 = std::clamp(npd1, -1.0f, 1.0f);
		float acosNpd1 = acosf(npd1);
		
		if (bone_[0].rotation.z > 180.0f && bone_[0].rotation.z < 0) {

			bone_[0].rotation.z = acosNpd1 + atan2f(target.translation_.y - bone_[0].root.y, target.translation_.x - bone_[0].root.x);


		} else { //どちらの値もマイナスの時
			bone_[0].rotation.z = -acosNpd1 + atan2f(target.translation_.y - bone_[0].root.y, target.translation_.x - bone_[0].root.x);
		}
	}

	//================================= ボーン1の根本・先端の更新 =============================//

	bone_[0].tip.x = bone_[0].root.x + bone_[0].length * cosf(bone_[0].rotation.z);
	bone_[0].tip.y = bone_[0].root.y + bone_[0].length * sinf(bone_[0].rotation.z);


	//================================= ボーン2の角度の更新 ==================================//

	//float root2TargetLength = MyMath::Length(targetPos_->translation_ - bone_[0].root);
	float bone2Numerator = -(powf(target.translation_.x - bone_[0].root.x, 2) + powf(target.translation_.y - bone_[0].root.y, 2)) + powf(bone_[1].length, 2) + powf(bone_[0].length, 2);

	float bone2Denominator = 2 * bone_[0].length * bone_[1].length;

	if (bone2Denominator != 0) {
		float npd2 = bone2Numerator / bone2Denominator;
		npd2 = std::clamp(npd2, -1.0f, 1.0f);
		float acosNpd2 = acosf(npd2);
		if (bone_[1].rotation.z > 180 && bone_[1].rotation.z < 0) {
			bone_[1].rotation.z = std::numbers::pi_v<float> +acosNpd2;
		} else {
			bone_[1].rotation.z = -(std::numbers::pi_v<float> +acosNpd2);
		}
	}

	//================================= ボーン2の根本・先端の更新 ==============================//

	bone_[1].root.x = bone_[0].tip.x;
	bone_[1].root.y = bone_[0].tip.y;

	bone_[1].tip.x = bone_[1].root.x + bone_[1].length * cosf(bone_[0].rotation.z + bone_[1].rotation.z);
	bone_[1].tip.y = bone_[1].root.y + bone_[1].length * sinf(bone_[0].rotation.z + bone_[1].rotation.z);

#pragma endregion
}

void Skeleton::UpdateLeg(const WorldTransform& target) {

	//================================= ボーン1の角度の更新 =================================//

	float bone1Numerator = -powf(bone_[1].length, 2) + powf(bone_[0].length, 2)
		+ (powf(target.translation_.x - bone_[0].root.x, 2) + powf(target.translation_.y - bone_[0].root.y, 2));

	float bone1Denominator = 2 * bone_[0].length
		* sqrtf(powf(target.translation_.x - bone_[0].root.x, 2) + powf(target.translation_.y - bone_[0].root.y, 2));

	//atanの範囲が-PI/2 ~ PI/2なので場合分けして計算する
	if (bone1Denominator != 0) {
		float npd1 = bone1Numerator / bone1Denominator;
		npd1 = std::clamp(npd1, -1.0f, 1.0f);
		float acosNpd1 = acosf(npd1);

		if (bone_[0].rotation.z > 180.0f && bone_[0].rotation.z < 0) {

			bone_[0].rotation.z = -acosNpd1 + atan2f(target.translation_.y - bone_[0].root.y, target.translation_.x - bone_[0].root.x);


		} else { //どちらの値もマイナスの時
			bone_[0].rotation.z = acosNpd1 + atan2f(target.translation_.y - bone_[0].root.y, target.translation_.x - bone_[0].root.x);
		}
	}

	//================================= ボーン1の根本・先端の更新 =============================//

	bone_[0].tip.x = bone_[0].root.x + bone_[0].length * cosf(bone_[0].rotation.z);
	bone_[0].tip.y = bone_[0].root.y + bone_[0].length * sinf(bone_[0].rotation.z);


	//================================= ボーン2の角度の更新 ==================================//

	//float root2TargetLength = MyMath::Length(targetPos_->translation_ - bone_[0].root);
	float bone2Numerator = -(powf(target.translation_.x - bone_[0].root.x, 2) + powf(target.translation_.y - bone_[0].root.y, 2)) + powf(bone_[1].length, 2) + powf(bone_[0].length, 2);

	float bone2Denominator = 2 * bone_[0].length * bone_[1].length;

	if (bone2Denominator != 0) {
		float npd2 = bone2Numerator / bone2Denominator;
		npd2 = std::clamp(npd2, -1.0f, 1.0f);
		float acosNpd2 = acosf(npd2);
		if (bone_[1].rotation.z > 180 && bone_[1].rotation.z < 0) {
			bone_[1].rotation.z = -(std::numbers::pi_v<float> +acosNpd2);
		} else {
			bone_[1].rotation.z = std::numbers::pi_v<float> +acosNpd2;
		}
	}

	//================================= ボーン2の根本・先端の更新 ==============================//

	bone_[1].root.x = bone_[0].tip.x;
	bone_[1].root.y = bone_[0].tip.y;

	bone_[1].tip.x = bone_[1].root.x + bone_[1].length * cosf(bone_[0].rotation.z + bone_[1].rotation.z);
	bone_[1].tip.y = bone_[1].root.y + bone_[1].length * sinf(bone_[0].rotation.z + bone_[1].rotation.z);

}

void Skeleton::UpdateImGui(const char* label) {

	//========================== ボーンの操作(imgui) ================================//

	ImGui::Begin(label);
	ImGui::Text("boneDebug");
	ImGui::DragFloat3("bone1TipTranslate", &bone_[0].tip.x, 0.01f);
	ImGui::DragFloat3("bone2rootTranslate", &bone_[1].root.x, 0.01f);
	ImGui::DragFloat3("bone2Translate", &bone_[1].tip.x, 0.01f);
	ImGui::DragFloat3("bone1rotation", &bone_[0].rotation.x, 0.01f);
	ImGui::DragFloat3("bone2rotation", &bone_[1].rotation.x, 0.01f);
	ImGui::SliderFloat3("bone1RootTranslate", &bone_[0].root.x, -20.0f, 20.0f, "%.3f");
	ImGui::End();
}

void Skeleton::Draw(const ViewProjection& viewProjection) {



	//ライン描画
	PrimitiveDrawer::GetInstance()->SetViewProjection(&viewProjection);
	PrimitiveDrawer::GetInstance()->DrawLine3d(bone_[0].root, bone_[0].tip, { 1.0f,0.0f,0.0f,1.0f });
	PrimitiveDrawer::GetInstance()->DrawLine3d(bone_[1].root, bone_[1].tip, { 0.0f,1.0f,0.0f,1.0f });
}
