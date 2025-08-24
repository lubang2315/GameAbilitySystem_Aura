// 由来时路褒贬不一制作


#include "Gas/Player/AbilitySystemComponent/AuraAbilitySystemComponent.h"
#include "Engine/Engine.h"
#include "Tags/AuraGameplayTags.h"


void UAuraAbilitySystemComponent::AbilityActorInfoSet()
{
	
	OnGameplayEffectAppliedDelegateToSelf.AddUObject(this, &UAuraAbilitySystemComponent::EffectApplied);

	/*const FMyGameplayTags& MyGameplayTags = FMyGameplayTags::Get();
	GEngine->AddOnScreenDebugMessage(
			-1,
			10.f,
			FColor::Blue,
			FString::Printf(TEXT("Tag: %s"), *MyGameplayTags.Attributes_Secondery_Armor.ToString())
			);*/
}

void UAuraAbilitySystemComponent::EffectApplied(UAbilitySystemComponent* AbilitySystemComponent,
	const FGameplayEffectSpec& SpecApplied, FActiveGameplayEffectHandle ActiveHandle)
{
    FGameplayTagContainer TagContainer;
	SpecApplied.GetAllAssetTags(TagContainer);
    /*广播Tag给widget*/
	EffectAssetTag.Broadcast(TagContainer);
}