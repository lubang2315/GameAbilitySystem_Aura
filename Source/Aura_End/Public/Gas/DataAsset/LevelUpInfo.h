// 由来时路褒贬不一制作

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "LevelUpInfo.generated.h"

USTRUCT(BlueprintType)
struct FAuraLevelUpInfo
{
	GENERATED_BODY()

	/*升级到此等级所需要经验值*/
	UPROPERTY(EditDefaultsOnly)
	int32 LevelUpRequirement = 0;

	/*奖励的属性点*/
	UPROPERTY(EditDefaultsOnly)
	int32 AttributePointAward = 0;

	/*奖励的法术点*/
	UPROPERTY(EditDefaultsOnly)
	int32 SpellPointAward = 0;
};

/**
 * 
 */
UCLASS()
class AURA_END_API ULevelUpInfo : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly)
	TArray<FAuraLevelUpInfo> LevelUpInfo;

	/*根据传入经验值XP返回等级*/
	int32 FindLevelForXP(int32 XP) const;
};
