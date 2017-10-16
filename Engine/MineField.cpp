#include "MineField.h"
#include <minmax.h>

MineField::MineField()
{
	for (int y = 0; y < height; ++y)
	{
		for (int x = 0; x < width; ++x)
		{
			tiles[x + y * width] = Tile(Vei2(x * SpriteCodex::tileSize, y * SpriteCodex::tileSize), false);
		}
	}

	std::random_device rd;
	std::mt19937 rng(rd());
	std::uniform_int_distribution<int> xDist(0, width - 1);
	std::uniform_int_distribution<int> yDist(0, height - 1);

	int x = 0, y = 0;
	for (int i = 0; i < nBombs; ++i)
	{
		do
		{
			x = xDist(rng);
			y = yDist(rng);
		} while (tiles[x + y * width].HasBomb());
		tiles[x + y * width] = Tile(Vei2(x * SpriteCodex::tileSize, y * SpriteCodex::tileSize), true);
	}

	for (int y = 0; y < height; ++y)
	{
		for (int x = 0; x < width; ++x)
		{
			int nBombs = 0;

			int xStart = max(0, x - 1);
			int yStart = max(0, y - 1);

			int xEnd = min(width - 1, x + 1);
			int yEnd = min(height - 1, y + 1);

			for (int y = yStart; y <= yEnd; ++y)
			{
				for (int x = xStart; x <= xEnd; ++x)
				{
					if (tiles[x + y * width].HasBomb())
						++nBombs;
				}
			}

			nNeigbourBombs[x + y * width] = nBombs;
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
			tiles[x + y * width].Draw(gfx, nNeigbourBombs[x + y * width]);
		}
	}
}

MineField::Tile::Tile(Vei2 & pos, bool hasBomb) : pos(pos), hasBomb(hasBomb)
{
}

void MineField::Tile::Draw(Graphics & gfx, int nNeighbourBombs)
{
	gfx.DrawRect(RectI(pos, SpriteCodex::tileSize, SpriteCodex::tileSize), Color(192, 192, 192));
	if (isRevealed())
	{
		if (hasBomb)
			SpriteCodex::DrawTileBomb(pos, gfx);
		else
			SpriteCodex::DrawTileNumber(pos,nNeighbourBombs, gfx);
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

bool MineField::Tile::HasBomb() const
{
	return hasBomb;
}

Vei2 & MineField::Tile::GetPos() const
{
	return (Vei2&)pos;
}
