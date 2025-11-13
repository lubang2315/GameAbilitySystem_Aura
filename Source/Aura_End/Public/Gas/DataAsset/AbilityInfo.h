// 由来时路褒贬不一制作

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/DataAsset.h"
#include "AbilityInfo.generated.h"

USTRUCT(BlueprintType)
struct FAuraAbilityInfo
{
	GENERATED_BODY()
	/*技能标签*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	FGameplayTag AbilityTag = FGameplayTag();

	/*技能输入映射标签*/
	UPROPERTY(BlueprintReadWrite)
	FGameplayTag InputTag = FGameplayTag();

	/*技能冷却标签*/
	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite)
	FGameplayTag CoolDownTag = FGameplayTag();
	
	/*技能图标*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TObjectPtr<const UTexture2D> Icon = nullptr;

	/*技能背景*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TObjectPtr<UMaterialInterface> BackgroundMaterial = nullptr;

	
};

/**
 * 
 */
UCLASS()
class AURA_END_API UAbilityInfo : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite,Category = "AbilityInformation")
	TArray<FAuraAbilityInfo> Abilities;

	FAuraAbilityInfo FindAbilityInfoForTag(const FGameplayTag& AbilityTag,bool bLogNotFound = false) const;
};
