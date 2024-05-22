#pragma once

#include "Input.h"
#include "Model.h"
#include "Sprite.h"
#include "WorldTransform.h"
#include "ViewProjection.h"

enum class MapChipType {
	kBlank, //空白
	kBlock, //ブロック
};

struct MapChipData {
	std::vector<std::vector<MapChipType>> data;
};

/// <summary>
/// マップチップフィールド
/// </summary>
class MapChipField {
public:

	//1ブロックのサイズ
	static inline const float kBlockWidth = 2.0f;
	static inline const float kBlockHeight = 2.0f;

	//ブロックの個数
	static inline const uint32_t kNumBlockVirtical = 20;
	static inline const uint32_t kNumBlockHorizontal = 20;

	/// <summary>
	/// 読み込み済みをリセットする関数
	/// </summary>
	void ResetMapChipData();

	/// <summary>
	/// csvファイル読み込み
	/// </summary>
	void LoadMapChipCsv(const std::string& filePath);

	/// <summary>
	/// マップチップ種別の取得
	/// </summary>
	MapChipType GetMapChipTypeByIndex(uint32_t xIndex, uint32_t yIndex);

	/// <summary>
	/// マップチップ座標の取得
	/// </summary>
	Vector3 GetMapChipPositionByIndex(uint32_t xIndex, uint32_t yIndex);

	uint32_t GetNumBlockVirtical() { return kNumBlockVirtical; }
	uint32_t GetNumBlockHorizontal() { return kNumBlockHorizontal; }

private:

	MapChipData mapChipData_;
};

