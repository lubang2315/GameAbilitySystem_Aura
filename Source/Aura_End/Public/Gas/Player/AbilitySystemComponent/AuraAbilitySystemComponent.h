// 由来时路褒贬不一制作

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "AuraAbilitySystemComponent.generated.h"

/**
 * 
 */

DECLARE_MULTICAST_DELEGATE_OneParam(EffectAssetTag,const FGameplayTagContainer& /*AssetTag*/)

UCLASS()
class AURA_END_API UAuraAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()

public:
	/**GE作用到Actor上时创建回调函数，用于广播*/
	void AbilityActorInfoSet();

	EffectAssetTag EffectAssetTag;

protected:
    void EffectApplied(UAbilitySystemComponent* AbilitySystemComponent, const FGameplayEffectSpec& SpecApplied, FActiveGameplayEffectHandle ActiveHandle);
};
