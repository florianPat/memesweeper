#include "MineField.h"

MineField::MineField()
{
	for (int y = 0; y < height; ++y)
	{
		for (int x = 0; x < width; ++x)
		{
			tiles[x + y * width] = Tile(Vei2(x * SpriteCodex::tileSize, y * SpriteCodex::tileSize), false);
		}
	}
}

void MineField::OnToggleFlag(const Vei2 & mousePos)
{
	if (mousePos.x > width * SpriteCodex::tileSize || mousePos.y > height * SpriteCodex::tileSize)
		return;
	Vei2 gridPos(mousePos.x / SpriteCodex::tileSize, mousePos.y / SpriteCodex::tileSize);

	tiles[gridPos.x + gridPos.y * width].ToggleFlag();
}

void MineField::OnReveal(const Vei2 & mousePos)
{
	if (mousePos.x > width * SpriteCodex::tileSize || mousePos.y > height * SpriteCodex::tileSize)
		return;
	Vei2 gridPos(mousePos.x / SpriteCodex::tileSize, mousePos.y / SpriteCodex::tileSize);

	Tile& tile = tiles[gridPos.x + gridPos.y * width];
	if (!tile.isRevealed())
	{
		tile.Reveal();
	}
}

void MineField::Draw(Graphics & gfx)
{
	for (int y = 0; y < height; ++y)
	{
		for (int x = 0; x < width; ++x)
		{
			tiles[x + y * width].Draw(gfx);
		}
	}
}

MineField::Tile::Tile(Vei2 & pos, bool hasBomb) : pos(pos), hasBomb(hasBomb)
{
}

void MineField::Tile::Draw(Graphics & gfx)
{
	gfx.DrawRect(RectI(pos, SpriteCodex::tileSize, SpriteCodex::tileSize), Color(192, 192, 192));
	if (isRevealed())
	{
		//Replace this with DrawTile0 etc..
		SpriteCodex::DrawTile0(pos, gfx);
		if (hasBomb)
			SpriteCodex::DrawTileBomb(pos, gfx);
	}
	else if (hasFlag())
	{
		SpriteCodex::DrawTileButton(pos, gfx);
		SpriteCodex::DrawTileFlag(pos, gfx);
	}
	else
	{
		SpriteCodex::DrawTileButton(pos, gfx);
	}
}

bool MineField::Tile::isRevealed() const
{
	return state == State::Revealed;
}

bool MineField::Tile::hasFlag() const
{
	return state == State::Flagged;
}

void MineField::Tile::Reveal()
{
	if (!hasFlag() && !isRevealed())
		state = State::Revealed;
}

void MineField::Tile::ToggleFlag()
{
	if (!isRevealed())
	{
		if (hasFlag())
			state = State::Closed;
		else
			state = State::Flagged;
	}
}
