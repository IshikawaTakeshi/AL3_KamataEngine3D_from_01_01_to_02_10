#include "GameScene.h"
#include "TextureManager.h"

#include "AxisIndicator.h"
#include "PrimitiveDrawer.h"
#include "MyMath/MatrixMath.h"
#include "MyMath/MyMath.h"

#include <cassert>

#ifdef _DEBUG
#include <imgui.h>
#endif // _DEBUG

GameScene::GameScene() {}

GameScene::~GameScene() {

	//delete leftLeg_;
	//delete rightLeg_;
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

	//スケルトン初期化
	rightArm_ = new Skeleton();
	rightArm_->Initialize({0,0,0});

	leftArm_ = new Skeleton();
	leftArm_->Initialize({ 0,0,3.0f });

	//rightLeg_ = new Skeleton();
	//rightLeg_->Initialize({ 0,-1.0f,1.0f });

	//leftLeg_ = new Skeleton();
	//leftLeg_->Initialize({ 0,-1.0f,2.0f });

	viewProjection_.Initialize();

	PrimitiveDrawer::GetInstance()->SetViewProjection(&debugCamera_->GetViewProjection());
}

void GameScene::Update() {

	//スケルトンの更新処理
	rightArm_->Update();
	leftArm_->Update();
	//rightLeg_->Update();
	//leftLeg_->Update();

	
#ifdef _DEBUG

	if (input_->TriggerKey(DIK_SPACE)) {
		isDebugCameraActive_ = true;
	}

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

	//スケルトンの描画処理
	rightArm_->Draw(debugCamera_->GetViewProjection());

	leftArm_->Draw(debugCamera_->GetViewProjection());

	//rightLeg_->Draw(debugCamera_->GetViewProjection());

	//leftLeg_->Draw(debugCamera_->GetViewProjection());

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
