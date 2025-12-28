// 由来时路褒贬不一制作


#include "Gas/Player/AbilitySystemComponent/AuraAbilitySystemComponent.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "Aura_End/AuraAbilityInfoLogChannels.h"
#include "Engine/Engine.h"
#include "Gas/Ability/AuraGameplayAbility.h"
#include "Gas/DataAsset/AbilityInfo.h"
#include "Gas/FunctionLibrary/MyFunctionLibrary.h"
#include "Interface/PlayerInterface.h"
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
			AbilitySpec.DynamicAbilityTags.AddTag(FMyGameplayTags::Get().Abilities_Status_Equipped);
			GiveAbility(AbilitySpec);
		}
		AbilityGivenDelegate.Broadcast();
		bStartupAbilityGiven = true;
	}
}

void UAuraAbilitySystemComponent::AddCharacterPassiveAbilities(const TArray<TSubclassOf<UGameplayAbility>>& StartupPassiveAbilities)
{
	for (TSubclassOf<UGameplayAbility> AbilityClass : StartupPassiveAbilities)
	{
		FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(AbilityClass,1);
		GiveAbilityAndActivateOnce(AbilitySpec);
	}
	
}

void UAuraAbilitySystemComponent::OnRep_ActivateAbilities()
{
	Super::OnRep_ActivateAbilities();
	if (!bStartupAbilityGiven)
	{
		bStartupAbilityGiven = true;
		AbilityGivenDelegate.Broadcast();
	}
}

void UAuraAbilitySystemComponent::AbilityInputTagPressed(FGameplayTag InputTag)
{
	if (!InputTag.IsValid()) return;

	for (auto ActivateGA : GetActivatableAbilities())
	{
		if (ActivateGA.DynamicAbilityTags.HasTagExact(InputTag))
		{
			/*此函数含义复杂，但很重要！！！，告知技能规格技能被触发，技能规格内部是有好几个状态布尔的，你通知他就会改变状态，
			 *就会影响BindAbilityAction(自定义的模版函数)函数里面的回调函数，详见AuraPlayerController，AuraInputComponent*/
			AbilitySpecInputPressed(ActivateGA);
			if (ActivateGA.IsActive())
			{
				/*通知雷电技能GA内部的回调，声明按键已经按下*/
				InvokeReplicatedEvent(EAbilityGenericReplicatedEvent::InputPressed/*告诉服务器的该技能玩家按下按键了*/,ActivateGA.Handle/*要告诉的技能*/,ActivateGA.ActivationInfo.GetActivationPredictionKey());
		
			}
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
		if (ActivateGA.DynamicAbilityTags.HasTagExact(GameplayTag) && ActivateGA.IsActive())
		{
			/*当GA触发完毕，按键已经释放，技能也不一定结束，所以只需要告诉GA已经结束技能停止接收输入操作*/
			AbilitySpecInputReleased(ActivateGA);
			/*通知雷电技能GA内部的回调，声明按键已经释放*/
			InvokeReplicatedEvent(EAbilityGenericReplicatedEvent::InputReleased,ActivateGA.Handle,ActivateGA.ActivationInfo.GetActivationPredictionKey());
		}
	}
}

void UAuraAbilitySystemComponent::FForEachAbility(const ::FForEachAbility& Delegate)
{
	/*域锁*/
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

FGameplayTag UAuraAbilitySystemComponent::GetStatusTagFromSpec(const FGameplayAbilitySpec& AbilitySpec)
{
	for (FGameplayTag Tag : AbilitySpec.DynamicAbilityTags)
	{
		if (Tag.MatchesTag(FGameplayTag::RequestGameplayTag(FName("Abilities.Status"))))
		{
			return Tag;
		}
	}
	return FGameplayTag();
}

FGameplayTag UAuraAbilitySystemComponent::GetStatusFromAbilityTag(const FGameplayTag& AbilityTag)
{
	if (FGameplayAbilitySpec* AbilitySpec = GetSpecFromAbilityTag(AbilityTag))
	{
		return  GetStatusTagFromSpec(*AbilitySpec);
	}
	return FGameplayTag();
}

FGameplayTag UAuraAbilitySystemComponent::GetInputTagFromAbilityTag(const FGameplayTag& AbilityTag)
{
	if (FGameplayAbilitySpec* AbilitySpec = GetSpecFromAbilityTag(AbilityTag))
	{
		return GetInputTagFromSpec(*AbilitySpec);
	}
	return FGameplayTag();
}


FGameplayAbilitySpec* UAuraAbilitySystemComponent::GetSpecFromAbilityTag(const FGameplayTag& GameplayTag)
{
	/*域锁*/
	FScopedAbilityListLock ActiveScopeLoc(*this);
	for (FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
	{
		for (FGameplayTag Tag : AbilitySpec.Ability.Get()->AbilityTags)
		{
			if (Tag.MatchesTag(GameplayTag))
			{
				return &AbilitySpec;
			}
		}
	}
	return nullptr;
}

void UAuraAbilitySystemComponent::UpdateAbilityStatuses(int32 Level)
{
	/*从GameMode获取技能配置数据*/
	UAbilityInfo* AbilityInfo = UMyFunctionLibrary::GetAbilityInfo(GetAvatarActor());
	for (FAuraAbilityInfo AuraAbilityInfo : AbilityInfo->Abilities)
	{
		if (!AuraAbilityInfo.AbilityTag.IsValid()) continue;
		if (Level < AuraAbilityInfo.LeverRequirement) continue;
		/*根据当前技能标签判断这个技能是否加载，没有加载就创建技能Spec加载一下，当然是在满足等级要求下*/
		if (GetSpecFromAbilityTag(AuraAbilityInfo.AbilityTag) == nullptr)
		{
			FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(AuraAbilityInfo.Ability,1);
			/*把技能解锁状态标签打上*/
			AbilitySpec.DynamicAbilityTags.AddTag(FMyGameplayTags::Get().Abilities_Status_Eligible);
			GiveAbility(AbilitySpec);
			MarkAbilitySpecDirty(AbilitySpec);/*设置当前技能立刻复制到每个客户端*/
			/*这里不直接广播，主要原因是想通过客户端执行直接广播*/
			ClientUpdateAbilityStatus_Implementation(AuraAbilityInfo.AbilityTag,FMyGameplayTags::Get().Abilities_Status_Eligible,1);
		}
		
		
	}
}

void UAuraAbilitySystemComponent::UpGradeAttribute(const FGameplayTag& AttributeTag)
{
	if (GetAvatarActor()->Implements<UPlayerInterface>())
	{
		if (IPlayerInterface::Execute_GetAttributePoints(GetAvatarActor()) > 0)
		{
			ServerUpGradeAttribute(AttributeTag);
			///////////////////////////////////////////////////////////////////////////////////////////////
			/*注意这里没做技能点分配*/
		}
	}
}

bool UAuraAbilitySystemComponent::GetDescrptionByAbilityTag(const FGameplayTag& AbilityTag,FString& OutLevelDescription, FString& OutNextLevelDescription)
{
	/*分两种情况一种是技能已经解锁，另一种是没有解锁只要显示未解锁信息和获取需要解锁等级就行*/
	if (FGameplayAbilitySpec* AbilitySpec = GetSpecFromAbilityTag(AbilityTag))
	{
		/*以下是解锁状态*/
		if (UAuraGameplayAbility* AuraGA = Cast<UAuraGameplayAbility>(AbilitySpec->Ability))
		{
			OutLevelDescription = AuraGA->GetDescription(AbilitySpec->Level);
			OutNextLevelDescription = AuraGA->GetNextLevelDescription(AbilitySpec->Level+1);
			return true;
		}
		
	}
	/*以下是未解锁状态*/
	UAbilityInfo* AbilityInfo = UMyFunctionLibrary::GetAbilityInfo(GetAvatarActor());
	if (!AbilityTag.IsValid() || AbilityTag.MatchesTagExact((FMyGameplayTags::Get().Abilities_None)))
	{
		OutLevelDescription = FString();
	}
	else
	{
		OutLevelDescription = UAuraGameplayAbility::GetLockedAbilityDescription(AbilityInfo->FindAbilityInfoForTag(AbilityTag).LeverRequirement);
	}
	OutNextLevelDescription = FString();
	return false;
}

void UAuraAbilitySystemComponent::ServerEquipAbility_Implementation(const FGameplayTag& SlotTag,const FGameplayTag& AbilityTag)
{
	if (FGameplayAbilitySpec* Spec = GetSpecFromAbilityTag(AbilityTag))
	{
		FGameplayTag PreviousSlot = GetInputTagFromSpec(*Spec);
		FGameplayTag StatusTag = GetStatusTagFromSpec(*Spec);
		
		const FMyGameplayTags& GameplayTags = FMyGameplayTags::Get();
		
	    if (StatusTag == GameplayTags.Abilities_Status_Equipped || StatusTag == GameplayTags.Abilities_Status_Unlocked)
	    {
		    /*清除掉目标技能球中已经装载的技能*/
	    	ClealAbilityOfSlot(SlotTag);
	    	/*清除掉以前的技能在技能栏的装载*/
	    	ClealSlot(Spec);
	    	Spec->DynamicAbilityTags.AddTag(SlotTag);

	    	/*更新技能解锁状态*/
		    if (StatusTag.MatchesTagExact(GameplayTags.Abilities_Status_Unlocked))
		    {
			    Spec->DynamicAbilityTags.RemoveTag(GameplayTags.Abilities_Status_Unlocked);
		    	Spec->DynamicAbilityTags.AddTag(GameplayTags.Abilities_Status_Equipped);
		    }
	    	/*立刻复制到每个客户端*/
	    	MarkAbilitySpecDirty(*Spec);
	    	ClientEquipAbility(SlotTag,AbilityTag,StatusTag,PreviousSlot);
	    }
	}
}

void UAuraAbilitySystemComponent::ClientEquipAbility_Implementation(const FGameplayTag& SlotTag,const FGameplayTag& AbilityTag,const FGameplayTag& StatusTag,const FGameplayTag& PreviousTag)
{
	/*在客户端广播技能装备标签*/
	AbilityEquipDelegate.Broadcast(AbilityTag,StatusTag,SlotTag,PreviousTag);
}

void UAuraAbilitySystemComponent::ClealSlot(FGameplayAbilitySpec* Spec)
{
	/*2**/
	const FGameplayTag Slot = GetInputTagFromSpec(*Spec);
	Spec->DynamicAbilityTags.RemoveTag(Slot);
	MarkAbilitySpecDirty(*Spec);
	
}

void UAuraAbilitySystemComponent::ClealAbilityOfSlot(const FGameplayTag& SlotTag)
{
	/*3*上下写这么多函数主要是配合该函数能清除要替换位置的技能球已经装备的技能*/
	FScopedAbilityListLock ActiveScopeLock(*this);
	for (auto& Spec : GetActivatableAbilities())
	{
		if (AbilityHasSlot(&Spec, SlotTag))
		{
			ClealSlot(&Spec);
		}
	}
}

bool UAuraAbilitySystemComponent::AbilityHasSlot(FGameplayAbilitySpec* Spec,const FGameplayTag& AbilityType)
{
	/*1**/
	for (FGameplayTag Tag : Spec->DynamicAbilityTags)
	{
		if (Tag.MatchesTagExact(AbilityType))
		{
			return true;
		}
	}
	return false;
}

void UAuraAbilitySystemComponent::ServerSpendSpellPoints_Implementation(const FGameplayTag& AttributeTag)
{
	if (FGameplayAbilitySpec* AbilitySpec = GetSpecFromAbilityTag(AttributeTag))
	{
		/*减少可分配技能点*/
		if (GetAvatarActor()->Implements<UPlayerInterface>())
		{
			IPlayerInterface::Execute_AddToSpellPoints(GetAvatarActor(),-1);
		}
		/*获取标签库*/
		FMyGameplayTags GameplayTag = FMyGameplayTags::Get();
		FGameplayTag StatusTag = GetStatusTagFromSpec(*AbilitySpec);
		/*根据技能解锁状态判断是解锁还是加点*/
		if (StatusTag.MatchesTagExact(GameplayTag.Abilities_Status_Eligible))
		{
			AbilitySpec->DynamicAbilityTags.RemoveTag(GameplayTag.Abilities_Status_Eligible);
			AbilitySpec->DynamicAbilityTags.AddTag(GameplayTag.Abilities_Status_Unlocked);
			StatusTag = GameplayTag.Abilities_Status_Unlocked;
			
		}
		else if (StatusTag.MatchesTagExact(GameplayTag.Abilities_Status_Unlocked) || StatusTag.MatchesTagExact(GameplayTag.Abilities_Status_Equipped))
		{
			/*提升技能等级*/
			AbilitySpec->Level += 1;
		}
		/*广播一下通知Menu状态更改*/
		ClientUpdateAbilityStatus(AttributeTag,StatusTag,AbilitySpec->Level);
		/*立即更新技能规格而不是等待下一帧*/
		MarkAbilitySpecDirty(*AbilitySpec);
	}
}

void UAuraAbilitySystemComponent::ServerUpGradeAttribute_Implementation(const FGameplayTag& AttributeTag)
{
	FGameplayEventData payload;
	payload.EventTag = AttributeTag;
	payload.EventMagnitude = 1.f;

	/*发送标签，通过标签激活被动技能里面事件然后修改值*/
	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(GetAvatarActor(),AttributeTag,payload);

	/*修改权威可分配属性点减一*/
	if (GetAvatarActor()->Implements<UPlayerInterface>())
	{
		IPlayerInterface::Execute_AddToAttributePoints(GetAvatarActor(),-1);
	}
}

void UAuraAbilitySystemComponent::ClientEffectApplied_Implementation(UAbilitySystemComponent* AbilitySystemComponent,const FGameplayEffectSpec& SpecApplied, FActiveGameplayEffectHandle ActiveHandle)
{
    FGameplayTagContainer TagContainer;
	SpecApplied.GetAllAssetTags(TagContainer);
    /*广播Tag给widget*/
	EffectAssetTag.Broadcast(TagContainer);
}

void UAuraAbilitySystemComponent::ClientUpdateAbilityStatus_Implementation(const FGameplayTag& AbilityTag,const FGameplayTag& StatusTag,int32 NewLevel)
{
	AbilityStatusChangedDelegate.Broadcast(AbilityTag,StatusTag,NewLevel);
}