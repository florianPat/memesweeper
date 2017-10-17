#include "MineField.h"
#include <minmax.h>

void MineField::RevealTile(Vei2 & gridPos)
{
	Tile& tile = tiles[gridPos.x + gridPos.y * width];
	if (!tile.isRevealed())
	{
		tile.Reveal();

		if (tile.HasBomb())
		{
			fucked = true;
			return;
		}

		if (hasNoNeighborBombs(gridPos))
		{
			int xStart = max(0, gridPos.x - 1);
			int yStart = max(0, gridPos.y - 1);

			int xEnd = min(width - 1, gridPos.x + 1);
			int yEnd = min(height - 1, gridPos.y + 1);

			for (int y = yStart; y <= yEnd; ++y)
			{
				for (int x = xStart; x <= xEnd; ++x)
				{
					RevealTile(Vei2{ x, y });
				}
			}
		}
	}
}

bool MineField::hasNoNeighborBombs(Vei2 & gridPos) const
{
	int xStart = max(0, gridPos.x - 1);
	int yStart = max(0, gridPos.y - 1);

	int xEnd = min(width - 1, gridPos.x + 1);
	int yEnd = min(height - 1, gridPos.y + 1);

	for (int y = yStart; y <= yEnd; ++y)
	{
		for (int x = xStart; x <= xEnd; ++x)
		{
			if (tiles[x + y * width].HasBomb())
				return false;
		}
	}

	return true;
}

MineField::MineField()
{
	for (int y = 0; y < height; ++y)
	{
		for (int x = 0; x < width; ++x)
		{
			tiles[x + y * width] = Tile(Vei2(x * SpriteCodex::tileSize + paddingX, y * SpriteCodex::tileSize + paddingY), false);
		}
	}

	std::random_device rd;
	std::mt19937 rng(rd());
	std::uniform_int_distribution<int> xDist(0, width - 1);
	std::uniform_int_distribution<int> yDist(0, height - 1);

	int x = 0, y = 0;
	for (int i = 0, j = 0; i < nBombs; ++i)
	{
		do
		{
			x = xDist(rng);
			y = yDist(rng);
		} while (tiles[x + y * width].HasBomb());
		tiles[x + y * width] = Tile(Vei2(x * SpriteCodex::tileSize + paddingX, y * SpriteCodex::tileSize + paddingY), true);
		tileBombs[j++] = x + y * width;
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
	if (!fucked && !win)
	{
		if (mousePos.x - paddingX > width * SpriteCodex::tileSize || mousePos.y - paddingY > height * SpriteCodex::tileSize
			|| mousePos.x - paddingX < 0 || mousePos.y - paddingY < 0)
			return;
		Vei2 gridPos((mousePos.x - paddingX) / SpriteCodex::tileSize, (mousePos.y - paddingY) / SpriteCodex::tileSize);

		tiles[gridPos.x + gridPos.y * width].ToggleFlag();
	}
}

void MineField::OnReveal(const Vei2 & mousePos)
{
	if (!fucked && !win)
	{
		if (mousePos.x - paddingX > width * SpriteCodex::tileSize || mousePos.y - paddingY > height * SpriteCodex::tileSize
			|| mousePos.x - paddingX < 0 || mousePos.y - paddingY < 0)
			return;
		Vei2 gridPos((mousePos.x - paddingX) / SpriteCodex::tileSize, (mousePos.y - paddingY) / SpriteCodex::tileSize);

		RevealTile(gridPos);
	}
}

void MineField::Draw(Graphics & gfx)
{
	int x0 = max(0, paddingX - SpriteCodex::tileSize),
		y0 = max(0, paddingY - SpriteCodex::tileSize),
		x1 = min(gfx.ScreenWidth - 1, paddingX + width * SpriteCodex::tileSize + SpriteCodex::tileSize),
		y1 = min(gfx.ScreenHeight - 1, paddingY + height * SpriteCodex::tileSize + SpriteCodex::tileSize);
	gfx.DrawRect(x0, y0, x1, y1, Colors::Green);

	for (int y = 0; y < height; ++y)
	{
		for (int x = 0; x < width; ++x)
		{
			tiles[x + y * width].Draw(gfx, nNeigbourBombs[x + y * width], fucked);
		}
	}
}

bool MineField::CheckWin()
{
	if (!win)
	{
		bool result = false;
		for (int i = 0; i < nBombs; ++i)
		{
			if (tiles[tileBombs[i]].hasFlag())
				result = true;
			else
			{
				result = false;
				break;
			}
		}

		win = result;
	}

	return win;
}

MineField::Tile::Tile(Vei2 & pos, bool hasBomb) : pos(pos), hasBomb(hasBomb)
{
}

void MineField::Tile::Draw(Graphics & gfx, int nNeighbourBombs, bool fucked)
{
	if (!fucked)
	{
		gfx.DrawRect(RectI(pos, SpriteCodex::tileSize, SpriteCodex::tileSize), SpriteCodex::baseColor);
		if (isRevealed())
		{
			if (hasBomb)
				SpriteCodex::DrawTileBomb(pos, gfx);
			else
				SpriteCodex::DrawTileNumber(pos, nNeighbourBombs, gfx);
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
	else
	{
		gfx.DrawRect(RectI(pos, SpriteCodex::tileSize, SpriteCodex::tileSize), Color(192, 192, 192));
		if (isRevealed())
		{
			if (hasBomb)
				SpriteCodex::DrawTileBombRed(pos, gfx);
			else
				SpriteCodex::DrawTileNumber(pos, nNeighbourBombs, gfx);
		}
		else if (hasFlag())
		{
			if (hasBomb)
			{
				SpriteCodex::DrawTileButton(pos, gfx);
				SpriteCodex::DrawTileFlag(pos, gfx);
			}
			else
			{
				SpriteCodex::DrawTileButton(pos, gfx);
				SpriteCodex::DrawTileFlag(pos, gfx);
				SpriteCodex::DrawTileCross(pos, gfx);
			}
		}
		else
		{
			if (hasBomb)
			{
				SpriteCodex::DrawTileBomb(pos, gfx);
			}
			else
				SpriteCodex::DrawTileButton(pos, gfx);
		}
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
