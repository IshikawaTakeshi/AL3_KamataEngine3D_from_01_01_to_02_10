#include "WorldTransform.h"
#include "math/MyMath/MatrixMath.h"

void WorldTransform::UpdateMatrix() {

	matWorld_ = MatrixMath::MakeAffineMatrix(scale_, rotation_, translation_);
	TransferMatrix();
}
