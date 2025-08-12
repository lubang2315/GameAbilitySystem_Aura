// 由来时路褒贬不一制作


#include "Gas/Player/AbilitySystemComponent/AuraAbilitySystemComponent.h"
#include "Engine/Engine.h"

void UAuraAbilitySystemComponent::AbilityActorInfoSet()
{
	
	OnGameplayEffectAppliedDelegateToSelf.AddUObject(this, &UAuraAbilitySystemComponent::EffectApplied);
}

void UAuraAbilitySystemComponent::EffectApplied(UAbilitySystemComponent* AbilitySystemComponent,
	const FGameplayEffectSpec& SpecApplied, FActiveGameplayEffectHandle ActiveHandle)
{
    FGameplayTagContainer TagContainer;
	SpecApplied.GetAllAssetTags(TagContainer);
    /*广播Tag给widget*/
	EffectAssetTag.Broadcast(TagContainer);
}