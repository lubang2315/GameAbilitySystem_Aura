// 由来时路褒贬不一制作


#include "Gas/Player/AbilitySystemComponent/AuraAbilitySystemComponent.h"
#include "Engine/Engine.h"
#include "Gas/Ability/AuraGameplayAbility.h"
#include "Tags/AuraGameplayTags.h"


void UAuraAbilitySystemComponent::AbilityActorInfoSet()
{
	
	OnGameplayEffectAppliedDelegateToSelf.AddUObject(this, &UAuraAbilitySystemComponent::ClientEffectApplied);
    /*测试代码已废弃*/
	/*const FMyGameplayTags& MyGameplayTags = FMyGameplayTags::Get();
	GEngine->AddOnScreenDebugMessage(
			-1,
			10.f,
			FColor::Blue,
			FString::Printf(TEXT("Tag: %s"), *MyGameplayTags.Attributes_Secondery_Armor.ToString())
			);*/
}

void UAuraAbilitySystemComponent::AddCharacterAbilities(const TArray<TSubclassOf<UGameplayAbility>>& StartupAbilities)
{
	for (TSubclassOf<UGameplayAbility> AbilityClass : StartupAbilities)
	{
		FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(AbilityClass,1);
		if (const UAuraGameplayAbility* AuraGA = Cast<UAuraGameplayAbility>(AbilitySpec.Ability))
		{
			AbilitySpec.DynamicAbilityTags.AddTag(AuraGA->StartUpInputTag);
			GiveAbility(AbilitySpec);
		}
	}
}

void UAuraAbilitySystemComponent::AbilityInputTagHold(const FGameplayTag GameplayTag)
{
	if (!GameplayTag.IsValid()) return;

	for (auto ActivateGA : GetActivatableAbilities())
	{
		if (ActivateGA.DynamicAbilityTags.HasTagExact(GameplayTag))
		{
			/*此函数含义复杂，但很重要！！！，告知技能规格技能被触发，技能规格内部是有好几个状态布尔的，你通知他就会改变状态，
			 *就会影响BindAbilityAction(自定义的模版函数)函数里面的回调函数，详见AuraPlayerController，AuraInputComponent*/
			AbilitySpecInputPressed(ActivateGA);
			if (!ActivateGA.IsActive())
			{
				TryActivateAbility(ActivateGA.Handle);
			}
		}
		
	}
}

void UAuraAbilitySystemComponent::AbilityAssetTagReleased(const FGameplayTag GameplayTag)
{
	if (!GameplayTag.IsValid()) return;

	for (auto ActivateGA : GetActivatableAbilities())
	{
		if (ActivateGA.DynamicAbilityTags.HasTagExact(GameplayTag))
		{
			/*当GA触发完毕，按键已经释放，技能也不一定结束，所以只需要告诉GA已经结束技能停止接收输入操作*/
			AbilitySpecInputReleased(ActivateGA);
		}
	}
}

void UAuraAbilitySystemComponent::ClientEffectApplied_Implementation(UAbilitySystemComponent* AbilitySystemComponent,
                                                const FGameplayEffectSpec& SpecApplied, FActiveGameplayEffectHandle ActiveHandle)
{
    FGameplayTagContainer TagContainer;
	SpecApplied.GetAllAssetTags(TagContainer);
    /*广播Tag给widget*/
	EffectAssetTag.Broadcast(TagContainer);
}
