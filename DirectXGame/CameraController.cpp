#include "CameraController.h"
#include "Player.h"
#include "MyMath/MyMath.h"



void CameraController::Initialize() {
	viewProjection_ = new ViewProjection();
	viewProjection_->Initialize();
}

void CameraController::Update() {

	//追従対象のワールドトランスフォームを参照
	const WorldTransform& targetWorldTransform = target_->GetWorldTransform();
	//追従対象とオフセットからカメラの座標を計算
	viewProjection_->translation_ = MyMath::Add(targetWorldTransform.translation_, targetOffset_);

	//行列の更新
	viewProjection_->UpdateMatrix();
}

void CameraController::Reset() {

	//追従対象のワールドトランスフォームを参照
	const WorldTransform& targetWorldTransform = target_->GetWorldTransform();
	//追従対象とオフセットからカメラの座標を計算
	viewProjection_->translation_ = MyMath::Add(targetWorldTransform.translation_, targetOffset_);

}
