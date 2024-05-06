#include "GameScene.h"
#include "TextureManager.h"
#include "PrimitiveDrawer.h"
#include "AxisIndicator.h"

#include "MyMath/MatrixMath.h"
#include "MyMath/MyMath.h"

#include <cassert>

#ifdef _DEBUG
#include <imgui.h>
#endif // _DEBUG

GameScene::GameScene() {}

GameScene::~GameScene() {

	delete debugCamera_;
	delete modelJoint_;
}

void GameScene::Initialize() {

	dxCommon_ = DirectXCommon::GetInstance();
	input_ = Input::GetInstance();

	//カメラの生成
	debugCamera_ = new DebugCamera(WinApp::kWindowWidth, WinApp::kWindowHeight);
	cameraMarix_ = MatrixMath::MakeAffineMatrix(
		{ 0.0f,0.0f,0.0f },
		debugCamera_->GetViewProjection().rotation_,
		debugCamera_->GetViewProjection().translation_
	);
	AxisIndicator::GetInstance()->SetVisible(true);
	AxisIndicator::GetInstance()->SetTargetViewProjection(&debugCamera_->GetViewProjection());

	//要素数
	const uint32_t kNumJointHorizonal = 3;

	//ブロック1個分の横幅
	const float kBlockWidth = 2.0f;
	const float kBlockHeight = 2.0f;
	//要素数を変更する
	joints_.resize(kNumJointHorizonal);

	//	ブロックの生成
	for (uint32_t row = 0; row < kNumJointHorizonal; row++) {
		joints_[row] = new WorldTransform();
		joints_[row]->Initialize();
		joints_[row]->translation_.x = kBlockHeight * row * 6.0f;
		joints_[row]->translation_.y = kBlockWidth;
	}

	//bornの要素数
	const uint32_t kNumBorn = 2;
	//要素数を変更
	born_.resize(kNumBorn);
	//ボーンの生成
	born_[0] = { {10,0,0}, {0,0,0},{0,0,0},0 };
	born_[0].length = MyMath::Distance(born_[0].tip, born_[0].root);
	born_[1] = { {20,0,0}, born_[0].tip,{0,0,0},0};
	born_[1].length = MyMath::Distance(born_[1].tip, born_[1].root);

	modelJoint_ = Model::Create();
	worldTrasform_.Initialize();
	viewProjection_.Initialize();

	PrimitiveDrawer::GetInstance()->SetViewProjection(&debugCamera_->GetViewProjection());
}

void GameScene::Update() {

	float born1Numerator = powf(born_[1].tip.x, 2) - powf(born_[1].tip.y, 2)
		+ powf(born_[0].length, 2) - powf(born_[1].length, 2);
	float born1Denominator = 2 * born_[0].length
		* sqrtf(powf(born_[1].tip.x, 2) + powf(born_[1].tip.y, 2));

	//ボーン1の角度の更新
	//atanの範囲が-PI/2 ~ PI/2なので場合分けして計算する
	if (born_[1].tip.x >= 0) {
		
		born_[0].angle.x = acos(born1Numerator / born1Denominator)
			+ atan(born_[1].tip.y / born_[1].tip.x);

	} else { //xの値がマイナスの時
		born_[0].angle.x = acos(born1Numerator / born1Denominator) 
			+ atan(born_[1].tip.y / born_[1].tip.x) + std::numbers::pi_v<float>;
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
	born_[1].angle.x = std::numbers::pi_v<float> +acos(born2Numerator / born2Denominator);
	
	//ボーン2の根本・先端の更新
	born_[1].root.x = born_[0].length * cos(born_[0].angle.x);
	born_[1].root.y = born_[0].length * sin(born_[0].angle.x);
	born_[1].tip.x = born_[1].root.x + born_[1].length * cos(born_[0].angle.x + born_[1].angle.x);
	born_[1].tip.y = born_[1].root.y + born_[1].length * sin(born_[0].angle.x + born_[1].angle.x);

	//関節の更新
	joints_[0]->translation_ = { born_[0].root.x,born_[0].root.y,born_[0].root.z };
	joints_[1]->translation_ = { born_[1].root.x,born_[1].root.y,born_[1].root.z };
	joints_[2]->translation_ = { born_[1].tip.x,born_[1].tip.y,born_[1].tip.z };

	//上限値から出ないようにする
	MyMath::Clamp(born_[1].tip.x, -1.0f * abs(born_[0].length + born_[1].length), abs(born_[0].length + born_[1].length));
	MyMath::Clamp(born_[1].tip.y, -1.0f * abs(born_[0].length + born_[1].length), abs(born_[0].length + born_[1].length));

	joints_[0]->UpdateMatrix();
	joints_[1]->UpdateMatrix();
	joints_[2]->UpdateMatrix();



#ifdef _DEBUG

	if (input_->TriggerKey(DIK_SPACE)) {
		isDebugCameraActive_ = true;
	}

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
		-1.0f * abs(born_[0].length + born_[1].length),
		abs(born_[0].length + born_[1].length),
		"%.3f"
	);
	ImGui::End();

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

	for (WorldTransform* worldTransformJoint : joints_) {
		modelJoint_->Draw(*worldTransformJoint, viewProjection_);
	}

	// 3Dオブジェクト描画後処理
	Model::PostDraw();

	//ライン描画
	PrimitiveDrawer::GetInstance()->DrawLine3d(born_[0].tip, born_[0].root, { 1.0f,0.0f,0.0f,1.0f });
	PrimitiveDrawer::GetInstance()->DrawLine3d(born_[1].tip, born_[1].root, { 0.0f,1.0f,0.0f,1.0f });
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
