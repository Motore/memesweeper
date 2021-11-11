#include "MemeField.h"
#include <assert.h>
#include <random>

void MemeField::Tile::SpawnMeme()
{
	assert(!hasMeme);
	hasMeme = true;
}

bool MemeField::Tile::HasMeme() const
{
	return hasMeme;
}

void MemeField::Tile::Draw(const Vei2& screenPos, Graphics& gfx) const
{
	switch (state) 
	{
	case State::Hidden:
		SpriteCodex::DrawTileButton(screenPos, gfx);
		break;
	case State::Flagged:
		SpriteCodex::DrawTileButton(screenPos, gfx);
		SpriteCodex::DrawTileFlag(screenPos, gfx);
		break;
	case State::Revealed:
		if (!HasMeme())
		{
			SpriteCodex::DrawTileNumber(screenPos, nNeigbourMemes, gfx);
		}
		else
		{
			SpriteCodex::DrawTileBomb(screenPos, gfx);
		}
		break;
	}
}

void MemeField::Tile::Reveal()
{
	assert(state == State::Hidden);
	state = State::Revealed;
}

bool MemeField::Tile::IsRevealed() const
{
	return state == State::Revealed;
}

void MemeField::Tile::ToggleFlag()
{
	assert(!IsRevealed());
	state = State::Flagged;
}

bool MemeField::Tile::Flagged() const
{
	return state == State::Flagged;
}

void MemeField::Tile::SetNeighboursMemeCount(int memeCount)
{
	assert(nNeigbourMemes == -1);
	nNeigbourMemes = memeCount;

}

MemeField::MemeField(int nMemes)
{
	assert(nMemes > 0 && nMemes < (width * height));
	std::random_device rd;
	std::mt19937 rng(rd());
	std::uniform_int_distribution<int> xDist(0, width - 1);
	std::uniform_int_distribution<int> yDist(0, height - 1);

	for (int nSpawned = 0; nSpawned < nMemes; ++nSpawned)
	{
		Vei2 spawnPos;
		do
		{
			spawnPos = { xDist(rng), yDist(rng) };
		} while (TileAt(spawnPos).HasMeme());
		
		TileAt(spawnPos).SpawnMeme();
	}

	for (Vei2 gridPos = { 0, 0 }; gridPos.y < height; ++gridPos.y)
	{
		for (gridPos.x = 0; gridPos.x < width; ++gridPos.x)
		{
			TileAt(gridPos).SetNeighboursMemeCount(CountNeghbourMemes(gridPos));
		}
	}
}

void MemeField::Draw(Graphics& gfx) const
{
	gfx.DrawRect(GetRect(), SpriteCodex::baseColor);
	for (Vei2 gridPos = { 0, 0 }; gridPos.y < height; ++gridPos.y)
	{
		for (gridPos.x = 0; gridPos.x < width; ++gridPos.x)
		{
			TileAt(gridPos).Draw(gridPos * SpriteCodex::tileSize, gfx);
		}
	}
}

RectI MemeField::GetRect() const
{
	return RectI(0, width * SpriteCodex::tileSize, 0, height * SpriteCodex::tileSize);
}

void MemeField::OnRevealClick(const Vei2& screenPos)
{
	const Vei2 gridPos = ScreenToGrid(screenPos);
	assert(gridPos.x > 0 && gridPos.x < width&& gridPos.y > 0 && gridPos.y < height);
	Tile& tile = TileAt(gridPos);
	if (!tile.IsRevealed() && !tile.Flagged())
	{
		tile.Reveal();
	}
}

void MemeField::OnFlagClick(const Vei2& screenPos)
{
	const Vei2 gridPos = ScreenToGrid(screenPos);
	assert(gridPos.x > 0 && gridPos.x < width&& gridPos.y > 0 && gridPos.y < height);
	Tile& tile = TileAt(gridPos);
	if (!tile.Flagged() && !tile.IsRevealed())
	{
		tile.ToggleFlag();
	}
}

MemeField::Tile& MemeField::TileAt(const Vei2 gridPos)
{
	return field[gridPos.y * width + gridPos.x];
}

const MemeField::Tile& MemeField::TileAt(const Vei2 gridPos) const
{
	return field[gridPos.y * width + gridPos.x];
}

Vei2 MemeField::ScreenToGrid(const Vei2& screenPos)
{
	return screenPos / SpriteCodex::tileSize;
}

int MemeField::CountNeghbourMemes(const Vei2& gridPos)
{
	const int startX = std::max(0, gridPos.x - 1);
	const int startY = std::max(0, gridPos.y - 1);
	const int endX = std::min(width - 1, gridPos.x + 1);
	const int endY = std::min(height - 1, gridPos.y + 1);

	int count = 0;
	for (Vei2 gridPos = { startX, startY }; gridPos.y <= endY; gridPos.y++)
	{
		for (gridPos.x = startX; gridPos.x <= endX; gridPos.x++)
		{
			if (TileAt(gridPos).HasMeme())
			{
				count++;
			}
		}
	}
	return count;
}
