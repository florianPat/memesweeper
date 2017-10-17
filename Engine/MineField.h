#pragma once

#include "Graphics.h"
#include "Vei2.h"
#include "RectI.h"
#include "SpriteCodex.h"
#include <random>

class MineField
{
	class Tile
	{
		enum class State
		{
			Flagged,
			Revealed,
			Closed
		};
		Vei2 pos;
		State state = State::Closed;
		bool hasBomb = false;
	public:
		Tile() = default;
		Tile(Vei2& pos, bool hasBomb);
		void Draw(Graphics& gfx, int nNeigbourBombs, bool fucked);
		bool isRevealed() const;
		bool hasFlag() const;
		void Reveal();
		void ToggleFlag();
		bool HasBomb() const;
		Vei2& GetPos() const;
	};
	static constexpr int width = 6, height = 4, nBombs = 2,
		paddingX = (Graphics::ScreenWidth / 2) - ((width / 2) * SpriteCodex::tileSize), paddingY = (Graphics::ScreenHeight  / 2) - ((height / 2) * SpriteCodex::tileSize);
	Tile tiles[width * height];
	int nNeigbourBombs[width * height];
	bool fucked = false;
	bool win = false;
	int tileBombs[nBombs];
private:
	void RevealTile(Vei2& gridPos);
	bool hasNoNeighborBombs(Vei2& gridPos) const;
public:
	MineField();
	void OnToggleFlag(const Vei2& mousePos);
	void OnReveal(const Vei2& mousePos);
	void Draw(Graphics& gfx);
	bool CheckWin();
};