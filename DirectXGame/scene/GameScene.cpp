#include "GameScene.h"
#include "TextureManager.h"
#include "ImGuiManager.h"
#include "PrimitiveDrawer.h"
#include "AxisIndicator.h"
#include <cassert>

GameScene::GameScene() {}

GameScene::~GameScene() {
	delete sprite_;
	delete model_;
	delete player_;

#ifdef _DEBUG
	delete debugCamera_;
#endif // _DEBUG

}

void GameScene::Initialize() {

	dxCommon_ = DirectXCommon::GetInstance();
	input_ = Input::GetInstance();
	audio_ = Audio::GetInstance();


	//テクスチャ読み込み
	textureHandle_ = TextureManager::Load("pictogram.png");
	//スプライト生成
	sprite_ = Sprite::Create(textureHandle_, { 100,50 });

	//3Dモデル生成
	model_ = Model::Create();
	//ワールドトランスフォーム初期化
	worldTransform_.Initialize();
	//ビュープロジェクション初期化
	viewProjection_.Initialize();

	//ライン描画が参照するビュープロジェクションを指定する（アドレス渡し）
	PrimitiveDrawer::GetInstance()->SetViewProjection(&viewProjection_);

	//サウンドデータの読み込み
	soundDataHandle_ = audio_->LoadWave("mokugyo.wav");
	//音声再生
	audio_->PlayWave(soundDataHandle_);
	voiceHandle_ = audio_->PlayWave(soundDataHandle_, true);

	//デバッグカメラ生成
	debugCamera_ = new DebugCamera(WinApp::kWindowWidth, WinApp::kWindowHeight);

	//軸方向の表示を有効にする
	AxisIndicator::GetInstance()->SetVisible(true);
	//軸方向表示が参照するビュープロジェクションを指定する（アドレス渡し）
	AxisIndicator::GetInstance()->SetTargetViewProjection(&debugCamera_->GetViewProjection());

	//プレイヤー生成
	player_ = new Player();
	player_->Initialize(model_,textureHandle_, &viewProjection_);
}

void GameScene::Update() {

	//プレイヤー更新
	player_->Update();

	//スプライトの今の座標の取得
	Vector2 position = sprite_->GetPosition();
	//座標{2,1}移動
	position.x += 2.0f;
	position.y += 1.0f;
	//移動した座標をスプライトに反映
	sprite_->SetPosition(position);

	//スペースキーを押した瞬間
	if (input_->TriggerKey(DIK_SPACE)) {
		//音声停止
		audio_->StopWave(voiceHandle_);
	}

	//デバッグカメラ更新
	debugCamera_->Update();

#ifdef _DEBUG
	//デバッグテキストの表示
	ImGui::Begin("Debug1");
	//テキスト
	ImGui::Text("Kamata tarou %d.%d.%d", 2050, 12, 31);
	//float3入力ボックス
	ImGui::InputFloat3("inputFloat3", inputFloat3);
	//float3スライダー
	ImGui::SliderFloat3("SliderFloat3", inputFloat3, 0.0f, 1.0f);
	ImGui::End();
	ImGui::ShowDemoWindow();
#endif

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
	player_->Draw();

	//3Dモデル描画
	//model_->Draw(worldTransform_, debugCamera_->GetViewProjection(), textureHandle_);



	// 3Dオブジェクト描画後処理
	Model::PostDraw();

	//ラインの描画
	PrimitiveDrawer::GetInstance()->DrawLine3d({ 0,0,0 }, { 0,10,0 }, { 1.0f,0.0f,0.0f,1.0f });

#pragma endregion

#pragma region 前景スプライト描画
	// 前景スプライト描画前処理
	Sprite::PreDraw(commandList);

	/// <summary>
	/// ここに前景スプライトの描画処理を追加できる
	/// </summary>
	sprite_->Draw();


	// スプライト描画後処理
	Sprite::PostDraw();

#pragma endregion
}
