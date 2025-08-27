// 由来时路褒贬不一制作

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/DataAsset.h"
#include "AuraInputConfig.generated.h"

class UInputAction;

USTRUCT(BlueprintType)
struct FInputActionStruct
{
    GENERATED_BODY();
	
	UPROPERTY(EditDefaultsOnly)
	const class UInputAction* InputAction = nullptr; 	

	UPROPERTY(EditDefaultsOnly)
	FGameplayTag InputTag = FGameplayTag();
};

/**
 * 
 */
UCLASS(BlueprintType, Blueprintable)
class AURA_END_API UAuraInputConfig : public UDataAsset
{
	GENERATED_BODY()

	public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<FInputActionStruct> AbilityInputActions;

	const UInputAction* FindAbilityInputActionForTag(const FGameplayTag& InputTag,bool bLogNotFond = false) const;
	
};
