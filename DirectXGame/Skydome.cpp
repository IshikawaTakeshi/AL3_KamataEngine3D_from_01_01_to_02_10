#include "Skydome.h"
#include "MyMath/MatrixMath.h"

void Skydome::Initialize(uint32_t textureHandle,const ViewProjection* viewProjection) {
	model_ = Model::Create();
	viewProjection_ = viewProjection;
	textureHandle_ = textureHandle;
}

void Skydome::Update() {
	worldTransform_->matWorld_ = MatrixMath::MakeAffineMatrix(
		worldTransform_->scale_,
		worldTransform_->rotation_,
		worldTransform_->translation_
	);
	worldTransform_->TransferMatrix();
}

void Skydome::Draw() {

	model_->Draw(*worldTransform_,*viewProjection_,textureHandle_);
}

Skydome::~Skydome() {
	
}
