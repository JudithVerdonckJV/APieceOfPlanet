#include "TileManager.h"
#include "BaseTile_Interface.h"
#include "ForestTile.h"
#include "DesertTile.h"
#include "TundraTile.h"
#include "Engine/World.h"
#include "Kismet/KismetMathLibrary.h"

#include "Delegates.h"

ATileManager::ATileManager()
	: Radius{ 410.f }
	, Scale{ 7.5f }
	, m_VertexBuffer{}
	, m_IndexBuffer{}
	, m_pTiles{}
	, m_pCampTile{}
{
 	PrimaryActorTick.bCanEverTick = false;

	//Values for "pentakis dodecahedron"
	//http://dmccooey.com/polyhedra/PentakisDodecahedron.html
	//Ty Jorge for pointing this website out

	const float C0 = Radius * 0.927050983124842272306880251548;
	const float C1 = Radius * 1.33058699733550141141687582919;
	const float C2 = Radius * 2.15293498667750705708437914596;
	const float C3 = Radius * 2.427050983124842272306880251548;
	const float C4 = Radius * 1.5;

	m_VertexBuffer =
	{
		{0.0, C0, C3}, {0.0, C0, -C3}, {0.0, -C0, C3}, {0.0, -C0, -C3},
		{C3, 0.0, C0}, {C3, 0.0, -C0}, {-C3, 0.0, C0}, {-C3, 0.0, -C0},
		{C0, C3, 0.0}, {C0, -C3, 0.0}, {-C0, C3, 0.0}, {-C0, -C3, 0.0},
		{C1, 0.0, C2}, {C1, 0.0, -C2}, {-C1, 0.0, C2}, {-C1, 0.0, -C2},
		{C2, C1, 0.0}, {C2, -C1, 0.0}, {-C2, C1, 0.0}, {-C2, -C1, 0.0},
		{0.0, C2, C1}, {0.0, C2, -C1}, {0.0, -C2, C1}, {0.0, -C2, -C1},
		{C4, C4, C4}, {C4, C4, -C4}, {C4, -C4,  C4}, {C4, -C4, -C4},
		{-C4, C4, C4}, {-C4, C4, -C4}, {-C4, -C4, C4}, {-C4, -C4, -C4}
	};

	m_IndexBuffer =
	{
		{ 12,  0,  2 }, { 12,  2, 26 }, { 12, 26,  4 }, { 12,  4, 24 }, { 12, 24,  0 },
		{ 13,  3,  1 }, { 13,  1, 25 }, { 13, 25,  5 }, { 13,  5, 27 }, { 13, 27,  3 },
		{ 14,  2,  0 }, { 14,  0, 28 }, { 14, 28,  6 }, { 14,  6, 30 }, { 14, 30,  2 },
		{ 15,  1,  3 }, { 15,  3, 31 }, { 15, 31,  7 }, { 15,  7, 29 }, { 15, 29,  1 },
		{ 16,  4,  5 }, { 16,  5, 25 }, { 16, 25,  8 }, { 16,  8, 24 }, { 16, 24,  4 },
		{ 17,  5,  4 }, { 17,  4, 26 }, { 17, 26,  9 }, { 17,  9, 27 }, { 17, 27,  5 },
		{ 18,  7,  6 }, { 18,  6, 28 }, { 18, 28, 10 }, { 18, 10, 29 }, { 18, 29,  7 },
		{ 19,  6,  7 }, { 19,  7, 31 }, { 19, 31, 11 }, { 19, 11, 30 }, { 19, 30,  6 },
		{ 20,  8, 10 }, { 20, 10, 28 }, { 20, 28,  0 }, { 20,  0, 24 }, { 20, 24,  8 },
		{ 21, 10,  8 }, { 21,  8, 25 }, { 21, 25,  1 }, { 21,  1, 29 }, { 21, 29, 10 },
		{ 22, 11,  9 }, { 22,  9, 26 }, { 22, 26,  2 }, { 22,  2, 30 }, { 22, 30, 11 },
		{ 23,  9, 11 }, { 23, 11, 31 }, { 23, 31,  3 }, { 23,  3, 27 }, { 23, 27,  9 }
	};
}

void ATileManager::BeginPlay()
{
	Super::BeginPlay();
	
	//GENERATION
	for (int triangle{ 0 }; triangle < m_IndexBuffer.size(); ++triangle)
	{
		FVector middlePoint{ CalculateTriangleMiddlePoint(triangle) };
		FRotator rotation{ CalculateTriangleRotation(triangle, middlePoint) };
		FTransform transform{ rotation, middlePoint, {Scale, Scale, Scale} };
		
		int biomeNr = UKismetMathLibrary::RandomIntegerInRange(0, (int)EBiome::MAX - 1);
		AActor* tile{};
		switch ((EBiome)biomeNr)
		{
		case EBiome::Forest:
			tile = GetWorld()->SpawnActor<AForestTile>(ForestTile, transform);
			break;
		case EBiome::Desert:
			tile = GetWorld()->SpawnActor<ADesertTile>(DesertTile, transform);
			break;
		case EBiome::Tundra:
			tile = GetWorld()->SpawnActor<ATundraTile>(TundraTile, transform);
			break;
		default:
			break;
		}
		
		m_pTiles.Add(tile);
	}
}

// Called every frame
void ATileManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

FVector ATileManager::CalculateTriangleMiddlePoint(int nr)
{
	FVector point{};

	point.X = (m_VertexBuffer[m_IndexBuffer[nr].X].X + m_VertexBuffer[m_IndexBuffer[nr].Y].X + m_VertexBuffer[m_IndexBuffer[nr].Z].X) / 3.f;
	point.Y = (m_VertexBuffer[m_IndexBuffer[nr].X].Y + m_VertexBuffer[m_IndexBuffer[nr].Y].Y + m_VertexBuffer[m_IndexBuffer[nr].Z].Y) / 3.f;
	point.Z = (m_VertexBuffer[m_IndexBuffer[nr].X].Z + m_VertexBuffer[m_IndexBuffer[nr].Y].Z + m_VertexBuffer[m_IndexBuffer[nr].Z].Z) / 3.f;

	return point;
}

FRotator ATileManager::CalculateTriangleRotation(int nr, FVector middlePoint)
{
	FRotator rotation{};

	FVector axis1{ m_VertexBuffer[m_IndexBuffer[nr].X] - middlePoint };
	FVector axis2{ m_VertexBuffer[m_IndexBuffer[nr].Y] - middlePoint };
	rotation = UKismetMathLibrary::MakeRotFromXY(axis1, axis2);

	return rotation;
}

void ATileManager::SpawnEnemies(int nr, float damageMultiplier, float healthMultiplier)
{
	int counter{};

	while (counter < nr)
	{
		if (m_pTiles.Num() <= 0)
		{
			UE_LOG(LogTemp, Error, TEXT("SpawnEnemies in TileManager: no tiles!"));
			return;
		}
		
		int tileNr = UKismetMathLibrary::RandomIntegerInRange(0, m_pTiles.Num() - 1);
		
		AActor* tile = m_pTiles[tileNr];
		
		if (!tile->IsValidLowLevelFast()) continue;
		IBaseTile_Interface* tileInterface = Cast<IBaseTile_Interface>(tile);
		if (tileInterface)
		{
			if (tileInterface->m_IsBroken) continue;
		}


		if (tile == m_pCampTile) continue;
				
		tileInterface->SpawnEnemy(damageMultiplier, healthMultiplier);
		++counter;		
	}
}

FTransform ATileManager::GetNewCampLocation()
{
	while (true)
	{
		if (m_pTiles.Num() <= 0)
		{
			UE_LOG(LogTemp, Error, TEXT("GetNewCampLocation in TileManager: no tiles!"));
			return {};
		}
		
		int tileNr = UKismetMathLibrary::RandomIntegerInRange(0, m_pTiles.Num() - 1);
		AActor* tile = m_pTiles[tileNr];
		
		if (!tile->IsValidLowLevelFast()) continue;
		if (tile == m_pCampTile) continue;
		IBaseTile_Interface* tileInterface = Cast<IBaseTile_Interface>(tile);
		if (!tileInterface) continue;
		if (tileInterface->m_IsBroken) continue;
		
		if (tileInterface->GetBiome() == EBiome::Desert)
		{
			if (Cast<ADesertTile>(tile)->IsOasis()) continue; // do not spawn on an oasis cuz of visual reasons ._.
		}
		
		m_pCampTile = tile;
		return m_pCampTile->GetActorTransform();
	}
}

AActor* ATileManager::GetCampTile()
{
	return m_pCampTile;
}
