#include "WorldTransform.h"
#include "../MyMath/MatrixMath.h"
#include <string>

#ifdef _DEBUG
#include <imgui.h>
#endif // _DEBUG

void WorldTransform::UpdateMatrix() {
	matWorld_ = MatrixMath::MakeAffineMatrix(scale_, rotation_, translation_);
	TransferMatrix();
//#ifdef _DEBUG
//	std::string label = "Window::Joint";
//	label += "##" + std::to_string(id);
//	ImGui::Begin(label.c_str());
//	//ImGui::DragFloat3("Translate", &translation_.x, 0.01f);
//
//	ImGui::End();
//#endif // _DEBUG
}
