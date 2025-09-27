// 由来时路褒贬不一制作


#include "UI/WidgetController/OverlayWidgetController.h"
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
	const UAuraAttributeSet* AuraAttributeSet = CastChecked<UAuraAttributeSet>(AttributeSet);
	
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AuraAttributeSet->GetHPAttribute()).AddLambda([this](const FOnAttributeChangeData& Data){OnHPChangedEvent.Broadcast(Data.NewValue);});
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AuraAttributeSet->GetMaxHpAttribute()).AddLambda([this](const FOnAttributeChangeData& Data){OnMaxHPChangedEvent.Broadcast(Data.NewValue);});
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AuraAttributeSet->GetManaAttribute()).AddLambda([this](const FOnAttributeChangeData& Data){OnManaChangedEvent.Broadcast(Data.NewValue);});
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AuraAttributeSet->GetMaxManaAttribute()).AddLambda([this](const FOnAttributeChangeData& Data){OnMaxManaChangedEvent.Broadcast(Data.NewValue);});

	

	/*Tag广播 */
	Cast<UAuraAbilitySystemComponent>(AbilitySystemComponent)->EffectAssetTag.AddLambda([this](const FGameplayTagContainer& AssertTag)
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

