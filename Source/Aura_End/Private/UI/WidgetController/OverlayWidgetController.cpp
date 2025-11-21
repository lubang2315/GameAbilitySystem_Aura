// 由来时路褒贬不一制作


#include "UI/WidgetController/OverlayWidgetController.h"

#include "Gas/DataAsset/LevelUpInfo.h"
#include "Gas/Player/AbilitySystemComponent/AuraAbilitySystemComponent.h"
#include "Gas/Player/AbilitySystemComponent/AuraAttributeSet.h"

void UOverlayWidgetController::BroadcastInitialValues()
{
	const UAuraAttributeSet* AuraAttributeSet = CastChecked<UAuraAttributeSet>(AttributeSet);
	OnHPChangedEvent.Broadcast(AuraAttributeSet->GetHP());
	OnMaxHPChangedEvent.Broadcast(AuraAttributeSet->GetMaxHp());
	OnManaChangedEvent.Broadcast(AuraAttributeSet->GetMana());
	OnMaxManaChangedEvent.Broadcast(AuraAttributeSet->GetMaxMana());
	
}


void UOverlayWidgetController::BindCallbacksToDependencies()
{
	/*绑定回调关于XP*/
	AAuraPlayerState* AuraPlayerState = CastChecked<AAuraPlayerState>(PlayerState);
	AuraPlayerState->OnXPChangedDelegate.AddUObject(this,&UOverlayWidgetController::OnXPChanged);

	/*绑定回调并直接广播玩家等级*/
	AuraPlayerState->OnLevelChangedDelegate.AddLambda([this](int32 NewValue){OnPlayerLevelChangedDelegate.Broadcast(NewValue);});
	
	const UAuraAttributeSet* AuraAttributeSet = CastChecked<UAuraAttributeSet>(AttributeSet);
	
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AuraAttributeSet->GetHPAttribute()).AddLambda([this](const FOnAttributeChangeData& Data){OnHPChangedEvent.Broadcast(Data.NewValue);});
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AuraAttributeSet->GetMaxHpAttribute()).AddLambda([this](const FOnAttributeChangeData& Data){OnMaxHPChangedEvent.Broadcast(Data.NewValue);});
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AuraAttributeSet->GetManaAttribute()).AddLambda([this](const FOnAttributeChangeData& Data){OnManaChangedEvent.Broadcast(Data.NewValue);});
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AuraAttributeSet->GetMaxManaAttribute()).AddLambda([this](const FOnAttributeChangeData& Data){OnMaxManaChangedEvent.Broadcast(Data.NewValue);});


	if (UAuraAbilitySystemComponent* AuraASC = Cast<UAuraAbilitySystemComponent>(AbilitySystemComponent))
	{
		if (AuraASC->bStartupAbilityGiven)
		{
			OnInitializeStartupAbilities(AuraASC);
		}
		else
		{
			AuraASC->AbilityGivenDelegate.AddUObject(this, &UOverlayWidgetController::OnInitializeStartupAbilities);
		}
		/*Tag广播 */
		AuraASC->EffectAssetTag.AddLambda([this](const FGameplayTagContainer& AssertTag)
			{
				for (const FGameplayTag& Tag : AssertTag)/*AssertTag是容器而Tag是里面的元素*/
				{
					
					FGameplayTag MessageTag = FGameplayTag::RequestGameplayTag(FName("Message"));
					if (Tag.MatchesTag(MessageTag))/*MatchTag匹配标签*/
					{
					    FUIWidgetRow* Row = GetDataTableRowByTag<FUIWidgetRow>(MessageWidgetDataTable,Tag);
						MessageWidgetDelegate.Broadcast(*Row);
					}
				    
				}
			});
	}
	
}

void UOverlayWidgetController::OnInitializeStartupAbilities(UAuraAbilitySystemComponent* AuraASC) const
{
	if (!AuraASC->bStartupAbilityGiven) return;
	FForEachAbility BroadCastDelegate;
	BroadCastDelegate.BindLambda([this](const FGameplayAbilitySpec& AbilitySpec)
	{
		FAuraAbilityInfo Info = AbilityInfo->FindAbilityInfoForTag(UAuraAbilitySystemComponent::GetAbilityTagFromSpec(AbilitySpec));
		Info.InputTag = UAuraAbilitySystemComponent::GetInputTagFromSpec(AbilitySpec);
		AbilityInfoDelegate.Broadcast(Info);
	});
	AuraASC->FForEachAbility(BroadCastDelegate);
}

void UOverlayWidgetController::OnXPChanged(int32 NewXP) const
{
	/*从玩家状态中获取玩家等级信息，因为玩家状态不会随着Pawn消失而重置*/
	const AAuraPlayerState* AuraPlayerState = CastChecked<AAuraPlayerState>(PlayerState);
	const TObjectPtr<ULevelUpInfo> LevelUpInfo = AuraPlayerState->LevelUpInfo;
	checkf(LevelUpInfo,TEXT("没有在AuraPlayerState中查询到LevelUpInfo，请检查！"));

	/*把获得的XP值转换成按等级的百分比以适应Ui百分比式进度条*/
	/*获取当前等级和最大等级*/
	const int32 Level = LevelUpInfo->FindLevelForXP(NewXP);
	const int32 MaxLevel = LevelUpInfo->LevelUpInfo.Num();

	if (Level <= MaxLevel && Level >0 )
	{
		/*获取当前等级需要经验值和上一级最大经验值*/
		const int32 MaxRequirementXP = LevelUpInfo->LevelUpInfo[Level].LevelUpRequirement;
    	const int32 MinRequirementXP = LevelUpInfo->LevelUpInfo[Level - 1].LevelUpRequirement;
		const float ProgressBarXP = static_cast<float>(NewXP - MinRequirementXP)/static_cast<float>(MaxRequirementXP - MinRequirementXP);
		/*广播经验条比例*/
		OnXPChangedDelegate.Broadcast(ProgressBarXP);
	}
	
}

