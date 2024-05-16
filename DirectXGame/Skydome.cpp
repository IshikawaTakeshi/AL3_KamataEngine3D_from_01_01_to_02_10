#include "Skydome.h"
#include "MyMath/MatrixMath.h"
#include "TextureManager.h"

void Skydome::Initialize() {
	worldTransform_.Initialize();
	model_ = Model::CreateFromOBJ("Skydome", true);
	////テクスチャハンドル
	//textureHandle_ = TextureManager::Load("skydome/Skydome.jpg");

}

void Skydome::Update() {

}

void Skydome::Draw(const ViewProjection& viewProjection) {

	model_->Draw(worldTransform_,viewProjection);
}

Skydome::~Skydome() {
	
}
