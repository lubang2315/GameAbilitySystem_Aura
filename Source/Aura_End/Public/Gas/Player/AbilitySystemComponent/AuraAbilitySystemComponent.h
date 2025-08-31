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

	/*添加能力，注意能力在添加后要使用还要激活 *ActivateGA**/
	void AddCharacterAbilities(const TArray<TSubclassOf<UGameplayAbility>>& StartupAbilities);

	/*触发和结束两种状态下，激活GA和移除GA(因为结束输入操作不一定GA结束，GA可能作用更长时间，所以这里只做通知) *ActivateGA**/
	void AbilityInputTagHold(const FGameplayTag GameplayTag);
	void AbilityAssetTagReleased(const FGameplayTag GameplayTag);

protected:
	UFUNCTION(Client, reliable)
    void ClientEffectApplied(UAbilitySystemComponent* AbilitySystemComponent, const FGameplayEffectSpec& SpecApplied, FActiveGameplayEffectHandle ActiveHandle);
};
