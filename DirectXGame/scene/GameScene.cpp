#include "GameScene.h"
#include "TextureManager.h"

#include "AxisIndicator.h"
#include "PrimitiveDrawer.h"
#include "MyMath/MatrixMath.h"
#include "MyMath/MyMath.h"

#include <algorithm>
#include <cassert>

#ifdef _DEBUG
#include <imgui.h>
#endif // _DEBUG

GameScene::GameScene() {}

GameScene::~GameScene() {

	delete leftLeg_;
	delete rightLeg_;
	delete leftArm_;
	delete rightArm_;

	delete debugCamera_;
}

void GameScene::Initialize() {

	dxCommon_ = DirectXCommon::GetInstance();
	input_ = Input::GetInstance();

	//カメラの生成
	debugCamera_ = new DebugCamera(WinApp::kWindowWidth, WinApp::kWindowHeight);
	cameraMatrix_ = MatrixMath::MakeAffineMatrix(
		{ 0.0f,0.0f,0.0f },
		debugCamera_->GetViewProjection().rotation_,
		debugCamera_->GetViewProjection().translation_
	);
	AxisIndicator::GetInstance()->SetVisible(true);
	AxisIndicator::GetInstance()->SetTargetViewProjection(&debugCamera_->GetViewProjection());

	//目標位置の初期化
	
	targetPos_[0].Initialize();
	targetPos_[0].translation_ = { 10.0f,0,0 };
	targetPos_[0].scale_ = { 1.5f,1.5f,1.5f };

	targetPos_[1].Initialize();
	targetPos_[1].translation_ = { 10.0f,0,10.0f };
	targetPos_[1].scale_ = { 1.5f,1.5f,1.5f };

	targetPos_[2].Initialize();
	targetPos_[2].translation_ = { 10.0f,-25.0f,3.0f };
	targetPos_[2].scale_ = { 1.5f,1.5f,1.5f };

	targetPos_[3].Initialize();
	targetPos_[3].translation_ = { 10.0f,-25.0f,7.0f };
	targetPos_[3].scale_ = { 1.5f,1.5f,1.5f };

	for (size_t i = 0; i < 4; i++) {

		modelTarget_[i] = new Model();
		modelTarget_[i] = Model::Create();
	}
	//スケルトン初期化
	rightArm_ = new Skeleton();
	rightArm_->Initialize({ 0,0,0 });

	leftArm_ = new Skeleton();
	leftArm_->Initialize({ 0,0,10.0f });

	rightLeg_ = new Skeleton();
	rightLeg_->Initialize({ 0,-20.0f,3.0f });

	leftLeg_ = new Skeleton();
	leftLeg_->Initialize({ 0,-20.0f,7.0f });

	//胴体の座標の初期化
	bodyPosRoot_.Initialize();
	bodyPosRoot_.translation_ = { 0,-20.0f,5.0f };
	bodyPosTip_.Initialize();
	bodyPosTip_.translation_ = { 0,5.0f,5.0f };

	//頭部の座標の初期化
	headPos_.Initialize();
	headPos_.translation_ = { 0,5.0f,5.0f };
	modelHead_ = new Model();
	modelHead_ = Model::CreateSphere();

	viewProjection_.Initialize();

	PrimitiveDrawer::GetInstance()->SetViewProjection(&debugCamera_->GetViewProjection());
}

void GameScene::Update() {

	//スケルトンの更新処理
	rightArm_->UpdateArm(targetPos_[0]);
	leftArm_->UpdateArm(targetPos_[1]);
	rightLeg_->UpdateLeg(targetPos_[2]);
	leftLeg_->UpdateLeg(targetPos_[3]);

	//================================= updateMatrix =====================================//

	//目標位置の更新
	//rightArm
	targetPos_[0].translation_.x = std::clamp(targetPos_[0].translation_.x, rightArm_->GetRootPos(0).x - 20.0f, rightArm_->GetRootPos(0).x + 20.0f);
	targetPos_[0].translation_.y = std::clamp(targetPos_[0].translation_.y, rightArm_->GetRootPos(0).y - 20.0f, rightArm_->GetRootPos(0).y + 20.0f);
	targetPos_[0].UpdateMatrix();
	//leftArm
	targetPos_[1].translation_.x = std::clamp(targetPos_[1].translation_.x, leftArm_->GetRootPos(0).x - 20.0f, leftArm_->GetRootPos(0).x + 20.0f);
	targetPos_[1].translation_.y = std::clamp(targetPos_[1].translation_.y, leftArm_->GetRootPos(0).y - 20.0f, leftArm_->GetRootPos(0).y + 20.0f);
	targetPos_[1].UpdateMatrix();
	//rightLeg
	targetPos_[2].translation_.x = std::clamp(targetPos_[2].translation_.x, rightLeg_->GetRootPos(0).x - 20.0f, rightLeg_->GetRootPos(0).x + 20.0f);
	targetPos_[2].translation_.y = std::clamp(targetPos_[2].translation_.y, rightLeg_->GetRootPos(0).y - 20.0f, rightLeg_->GetRootPos(0).y + 20.0f);
	targetPos_[2].UpdateMatrix();
	//leftLeg
	targetPos_[3].translation_.x = std::clamp(targetPos_[3].translation_.x, leftLeg_->GetRootPos(0).x - 20.0f, leftLeg_->GetRootPos(0).x + 20.0f);
	targetPos_[3].translation_.y = std::clamp(targetPos_[3].translation_.y, leftLeg_->GetRootPos(0).y - 20.0f, leftLeg_->GetRootPos(0).y + 20.0f);
	targetPos_[3].UpdateMatrix();

	headPos_.UpdateMatrix();

#ifdef _DEBUG

	//if (input_->TriggerKey(DIK_SPACE)) {
	//	isDebugCameraActive_ = true;
	//}

#endif // _DEBUG

	//カメラの処理
	if (isDebugCameraActive_ == true) {
		debugCamera_->Update();
		viewProjection_.matView = debugCamera_->GetViewProjection().matView;
		viewProjection_.matProjection = debugCamera_->GetViewProjection().matProjection;
		viewProjection_.TransferMatrix();
	} else {
		//ビュープロジェクション行列の更新と転送
		viewProjection_.UpdateMatrix();
	}

	//ImGuiの更新
	ImGui::Begin("GameScene");
	ImGui::Checkbox("isDebugCameraActive", &isDebugCameraActive_);
	ImGui::Text("targetPos : RightArm");
	ImGui::SliderFloat("targetPosTransformX", &targetPos_[0].translation_.x, rightArm_->GetRootPos(0).x - 20.0f, rightArm_->GetRootPos(0).x + 20.0f, "%.3f");
	ImGui::SliderFloat("targetPosTransformY", &targetPos_[0].translation_.y, rightArm_->GetRootPos(0).y - 20.0f, rightArm_->GetRootPos(0).y + 20.0f, "%.3f");
	ImGui::Text("targetPos : LeftArm");
	ImGui::SliderFloat("targetPosTransformX##1", &targetPos_[1].translation_.x, leftArm_->GetRootPos(0).x - 20.0f, leftArm_->GetRootPos(0).x + 20.0f, "%.3f");
	ImGui::SliderFloat("targetPosTransformY##1", &targetPos_[1].translation_.y, leftArm_->GetRootPos(0).y - 20.0f, leftArm_->GetRootPos(0).y + 20.0f, "%.3f");
	ImGui::Text("targetPos : RightLeg");
	ImGui::SliderFloat("targetPosTransformX##2", &targetPos_[2].translation_.x, rightLeg_->GetRootPos(0).x - 20.0f, rightLeg_->GetRootPos(0).x + 20.0f, "%.3f");
	ImGui::SliderFloat("targetPosTransformY##2", &targetPos_[2].translation_.y, rightLeg_->GetRootPos(0).y - 20.0f, rightLeg_->GetRootPos(0).y + 20.0f, "%.3f");
	ImGui::Text("targetPos : LeftLeg");
	ImGui::SliderFloat("targetPosTransformX##3", &targetPos_[3].translation_.x, leftLeg_->GetRootPos(0).x - 20.0f, leftLeg_->GetRootPos(0).x + 20.0f, "%.3f");
	ImGui::SliderFloat("targetPosTransformY##3", &targetPos_[3].translation_.y, leftLeg_->GetRootPos(0).y - 20.0f, leftLeg_->GetRootPos(0).y + 20.0f, "%.3f");
	ImGui::Text("headPos");
	ImGui::DragFloat3("headPosTranslate", &headPos_.translation_.x, 0.01f);
	
	ImGui::End();

	rightArm_->UpdateImGui("rightArm");
	leftArm_->UpdateImGui("leftArm");
	rightLeg_->UpdateImGui("rightLeg");
	leftLeg_->UpdateImGui("leftLeg");
}

void GameScene::Draw() {

	// コマンドリストの取得
	ID3D12GraphicsCommandList* commandList = dxCommon_->GetCommandList();

#pragma region 背景スプライト描画
	// 背景スプライト描画前処理
	Sprite::PreDraw(commandList);

	/// <summary>
	/// ここに背景スプライトの描画処理を追加できる
	/// </summary>

	// スプライト描画後処理
	Sprite::PostDraw();
	// 深度バッファクリア
	dxCommon_->ClearDepthBuffer();
#pragma endregion

#pragma region 3Dオブジェクト描画
	// 3Dオブジェクト描画前処理
	Model::PreDraw(commandList);

	/// <summary>
	/// ここに3Dオブジェクトの描画処理を追加できる
	/// </summary>

	modelTarget_[0]->Draw(targetPos_[0], debugCamera_->GetViewProjection());
	modelTarget_[1]->Draw(targetPos_[1], debugCamera_->GetViewProjection());
	modelTarget_[2]->Draw(targetPos_[2], debugCamera_->GetViewProjection());
	modelTarget_[3]->Draw(targetPos_[3], debugCamera_->GetViewProjection());

	//頭部の描画処理
	modelHead_->Draw(headPos_, debugCamera_->GetViewProjection());

	//スケルトンの描画処理
	rightArm_->Draw(debugCamera_->GetViewProjection());

	leftArm_->Draw(debugCamera_->GetViewProjection());

	rightLeg_->Draw(debugCamera_->GetViewProjection());

	leftLeg_->Draw(debugCamera_->GetViewProjection());

	//胴体の描画処理
	PrimitiveDrawer::GetInstance()->DrawLine3d(bodyPosRoot_.translation_, bodyPosTip_.translation_, { 1.0f,0.0f,0.0f,1.0f });

	// 3Dオブジェクト描画後処理
	Model::PostDraw();


#pragma endregion

#pragma region 前景スプライト描画
	// 前景スプライト描画前処理
	Sprite::PreDraw(commandList);

	/// <summary>
	/// ここに前景スプライトの描画処理を追加できる
	/// </summary>

	// スプライト描画後処理
	Sprite::PostDraw();

#pragma endregion
}
