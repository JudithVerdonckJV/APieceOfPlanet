#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "BaseTile_Interface.generated.h"


enum class EBiome
{
	Forest = 0,
	Desert = 1,
	Tundra = 2,
	MAX = 3,
};

UINTERFACE(MinimalAPI, BlueprintType, Blueprintable)
class UBaseTile_Interface : public UInterface
{
	GENERATED_BODY()
};


class APIECEOFPLANET_API IBaseTile_Interface
{
	GENERATED_BODY()

public:
	virtual void SpawnEnemy(float damageMultiplier, float healthMultiplier) = 0;
	virtual EBiome GetBiome() const
	{
		return m_Biome;
	};

	virtual void PlayerLeaveTile() = 0;
	virtual void PlayerEnterTile() = 0;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
		void DestroyTile();
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
		bool IsBroken() const;

	bool m_IsBroken;

protected:
	int m_Health;
	EBiome m_Biome;
};
