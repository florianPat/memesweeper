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
		void Draw(Graphics& gfx);
		bool isRevealed() const;
		bool hasFlag() const;
		void Reveal();
		void ToggleFlag();
		bool HasBomb() const;
	};
	static constexpr int width = 20, height = 18, nBombs = 10;
	Tile tiles[width * height];
public:
	MineField();
	void OnToggleFlag(const Vei2& mousePos);
	void OnReveal(const Vei2& mousePos);
	void Draw(Graphics& gfx);
};