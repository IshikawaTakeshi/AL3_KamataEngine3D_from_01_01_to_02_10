#pragma once
#include "Model.h"
#include "WorldTransform.h"
#include "ViewProjection.h"

class MapChipField;
class Enemy {
public:

	Enemy() = default;
	~Enemy();

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// 更新処理
	/// </summary>
	void Update();


	/// <summary>
	/// 描画処理
	/// </summary>
	void Draw(const ViewProjection& viewProjection);


	void SetPos(Vector3 pos) { worldTransform_.translation_ = pos; }

	void SetMapChipField(MapChipField* mapChipField) { mapChipField_ = mapChipField; }

private:

	//歩行の速さ
	static inline const float kWalkSpeed = 0.1f;
	//最初の角度[度]
	static inline const float kWalkMotionAngleStart = 0.0f;
	//最後の角度[度]
	static inline const float kWalkMotionAngleEnd = 3.0f;
	//アニメーションの周期となる時間[秒]
	static inline const float kWalkMotionTime = 1.0f;

	//速度
	Vector3 velocity_;
	//マップチップフィールド
	MapChipField* mapChipField_ = nullptr;
	//ワールド変換データ
	WorldTransform worldTransform_;
	//ビュープロジェクション
	ViewProjection* viewProjection_ = nullptr;
	//モデル
	Model* model_ = nullptr;
	//経過時間
	float walkTimer_;

};

