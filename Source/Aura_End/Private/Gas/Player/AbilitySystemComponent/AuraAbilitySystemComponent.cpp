// 由来时路褒贬不一制作


#include "Gas/Player/AbilitySystemComponent/AuraAbilitySystemComponent.h"

#include "Aura_End/AuraAbilityInfoLogChannels.h"
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
		AbilityGivenDelegate.Broadcast(this);
		bStartupAbilityGiven = true;
	}
}

void UAuraAbilitySystemComponent::OnRep_ActivateAbilities()
{
	Super::OnRep_ActivateAbilities();
	if (!bStartupAbilityGiven)
	{
		bStartupAbilityGiven = true;
		AbilityGivenDelegate.Broadcast(this);
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

void UAuraAbilitySystemComponent::FForEachAbility(const ::FForEachAbility& Delegate)
{
	FScopedAbilityListLock ActiveScopedLock(*this);
	for (const FGameplayAbilitySpec& ActivateGA : GetActivatableAbilities())
	{
		if (!Delegate.ExecuteIfBound(ActivateGA))
			/*上面这个ExecuteIfBound作用就是当ActivateGA绑定到Delegate的时候会自动调用委托*/
		{
			UE_LOG(LogAura,Error,TEXT("在函数[%hs]运行委托失败"),__FUNCTION__);
		}
	}
}

FGameplayTag UAuraAbilitySystemComponent::GetAbilityTagFromSpec(const FGameplayAbilitySpec& AbilitySpec)
{
	if (AbilitySpec.Ability)
	{
		for (FGameplayTag Tag: AbilitySpec.Ability.Get()->AbilityTags)
		{
			if (Tag.MatchesTag(FGameplayTag::RequestGameplayTag(FName("Abilities"))))
			{
				return Tag;
			}
		}
	}
	return FGameplayTag();
}

FGameplayTag UAuraAbilitySystemComponent::GetInputTagFromSpec(const FGameplayAbilitySpec& AbilitySpec)
{
	if (AbilitySpec.Ability)
	{
		for (FGameplayTag Tag : AbilitySpec.DynamicAbilityTags)
		{
			if (Tag.MatchesTag(FGameplayTag::RequestGameplayTag(FName("InputTag"))))
			{
				return Tag;
			}
		}
	}
	return FGameplayTag();
}

void UAuraAbilitySystemComponent::ClientEffectApplied_Implementation(UAbilitySystemComponent* AbilitySystemComponent,
                                                                     const FGameplayEffectSpec& SpecApplied, FActiveGameplayEffectHandle ActiveHandle)
{
    FGameplayTagContainer TagContainer;
	SpecApplied.GetAllAssetTags(TagContainer);
    /*广播Tag给widget*/
	EffectAssetTag.Broadcast(TagContainer);
}
