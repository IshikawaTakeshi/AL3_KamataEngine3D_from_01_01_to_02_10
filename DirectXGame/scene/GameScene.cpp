#include "GameScene.h"
#include "TextureManager.h"
#include "AxisIndicator.h"
#include "Math/MyMath/MatrixMath.h"
#include <cassert>

GameScene::GameScene() {}

GameScene::~GameScene() {

	delete skydome_;
	delete modelBlock_;
	delete player_;

	for (std::vector<WorldTransform*>& worldTransformBlockLine : worldTransformBlocks_) {
		for (WorldTransform* worldTransformBlock : worldTransformBlockLine) {
			delete worldTransformBlock;
		}
	}
	worldTransformBlocks_.clear();
	delete debugCamera_;
}


void GameScene::Initialize() {

	dxCommon_ = DirectXCommon::GetInstance();
	input_ = Input::GetInstance();
	audio_ = Audio::GetInstance();

	//カメラの生成
	debugCamera_ = new DebugCamera(WinApp::kWindowWidth, WinApp::kWindowHeight);
	cameraMarix_ = MatrixMath::MakeAffineMatrix(
		{ 0.0f,0.0f,0.0f },
		debugCamera_->GetViewProjection().rotation_,
		debugCamera_->GetViewProjection().translation_
	);

	//軸方向表示の生成	
	AxisIndicator::GetInstance()->SetVisible(true);
	AxisIndicator::GetInstance()->SetTargetViewProjection(&debugCamera_->GetViewProjection());

	//プレイヤー生成
	player_ = new Player();
	player_->Initialize();

	//要素数
	const uint32_t kNumblockVirtical = 10;
	const uint32_t kNumBlockHorizonal = 20;
	//ブロック1個分の横幅
	const float kBlockWidth = 2.0f;
	const float kBlockHeight = 2.0f;
	//要素数を変更する
	worldTransformBlocks_.resize(kNumblockVirtical);
	for (uint32_t i = 0; i < kNumblockVirtical; i++) {
		worldTransformBlocks_[i].resize(kNumBlockHorizonal);
	}

	//	ブロックの生成
	for (uint32_t row = 0; row < kNumblockVirtical; row++) {
		for (uint32_t column = 0; column < kNumBlockHorizonal; column++) {

			worldTransformBlocks_[row][column] = new WorldTransform();
			worldTransformBlocks_[row][column]->Initialize();
			if ((row + column) % 2 == 0) {
				worldTransformBlocks_[row][column]->translation_.x = kBlockWidth * column;
				worldTransformBlocks_[row][column]->translation_.y = kBlockHeight * row;
			}
		}
	}

	//天球の生成
	skydome_ = new Skydome();
	skydome_->Initialize();

	viewProjection_.Initialize();
	modelBlock_ = Model::Create();
}

void GameScene::Update() {

	//プレイヤーの更新処理
	player_->Update();

	//ブロックの更新
	for (std::vector<WorldTransform*>& worldTransformBlockLine : worldTransformBlocks_) {
		for (WorldTransform* worldTransformBlock : worldTransformBlockLine) {
			if (!worldTransformBlock) {
				continue;
			}
			worldTransformBlock->UpdateMatrix();
		}
	}
	skydome_->Update();

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

	//プレイヤー描画
	player_->Draw(viewProjection_);

	//ブロック描画
	for (std::vector<WorldTransform*>& worldTransformBlockLine : worldTransformBlocks_) {
		for (WorldTransform* worldTransformBlock : worldTransformBlockLine) {
			modelBlock_->Draw(*worldTransformBlock, viewProjection_);
		}
	}

	//天球描画
	skydome_->Draw(viewProjection_);

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
