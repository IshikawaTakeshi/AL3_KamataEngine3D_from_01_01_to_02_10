#include "Skydome.h"
#include "MyMath/MatrixMath.h"

void Skydome::Initialize(Model* model,uint32_t textureHandle,const ViewProjection* viewProjection) {
	model_ = model;
	viewProjection_ = viewProjection;
	textureHandle_ = textureHandle;
	worldTransform_.scale_ = { 1.0f,1.0f,1.0f };
	worldTransform_.rotation_ = { 0.0f,0.0f,0.0f };
	worldTransform_.translation_ = { 0.0f,0.0f,0.0f };

}

void Skydome::Update() {
	worldTransform_.matWorld_ = MatrixMath::MakeAffineMatrix(
		worldTransform_.scale_,
		worldTransform_.rotation_,
		worldTransform_.translation_
	);
	worldTransform_.TransferMatrix();
}

void Skydome::Draw() {

	model_->Draw(worldTransform_,*viewProjection_,textureHandle_);
}

Skydome::~Skydome() {
	
}
