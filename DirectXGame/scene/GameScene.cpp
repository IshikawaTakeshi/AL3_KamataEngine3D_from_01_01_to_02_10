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
	delete mapChipField_;
}

//ブロックの生成
void GameScene::GenerateBlocks() {
	//要素数
	const uint32_t kNumblockVirtical = mapChipField_->GetNumBlockVirtical();
	const uint32_t kNumBlockHorizonal = mapChipField_->GetNumBlockHorizontal();

	//要素数を変更する
	worldTransformBlocks_.resize(kNumblockVirtical);
	for (uint32_t i = 0; i < kNumblockVirtical; i++) {
		worldTransformBlocks_[i].resize(kNumBlockHorizonal);
	}

	//	ブロックの生成
	for (uint32_t row = 0; row < kNumblockVirtical; row++) {
		for (uint32_t column = 0; column < kNumBlockHorizonal; column++) {
			if (mapChipField_->GetMapChipTypeByIndex(column,row) == MapChipType::kBlock) {
				WorldTransform* worldTransform = new WorldTransform();
				worldTransform->Initialize();
				worldTransformBlocks_[row][column] = worldTransform;
				worldTransformBlocks_[row][column]->translation_ = mapChipField_->GetMapChipPositionByIndex(column,row);
			}
		}
	}
}



void GameScene::Initialize() {

	dxCommon_ = DirectXCommon::GetInstance();
	input_ = Input::GetInstance();
	audio_ = Audio::GetInstance();

	//マップチップフィールド生成
	mapChipField_ = new MapChipField();
	mapChipField_->LoadMapChipCsv("Resources/AL3_MapChip.csv");

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

	//ブロックの生成
	GenerateBlocks();

	//天球の生成
	skydome_ = new Skydome();
	skydome_->Initialize();

	viewProjection_.Initialize();
	modelBlock_ = Model::Create();
}

void GameScene::Update() {

	//天球の更新処理
	skydome_->Update();
	
	//ブロックの更新
	for (std::vector<WorldTransform*>& worldTransformBlockLine : worldTransformBlocks_) {
		for (WorldTransform* worldTransformBlock : worldTransformBlockLine) {
			if (!worldTransformBlock) {
				continue;
			}
			worldTransformBlock->UpdateMatrix();
		}
	}

	//プレイヤーの更新処理
	player_->Update();

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

	//天球描画
	skydome_->Draw(viewProjection_);

	//ブロック描画
	for (std::vector<WorldTransform*>& worldTransformBlockLine : worldTransformBlocks_) {
		for (WorldTransform* worldTransformBlock : worldTransformBlockLine) {
			if (!worldTransformBlock)continue;
			modelBlock_->Draw(*worldTransformBlock, viewProjection_);
		}
	}

	//プレイヤー描画
	player_->Draw(viewProjection_);


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
