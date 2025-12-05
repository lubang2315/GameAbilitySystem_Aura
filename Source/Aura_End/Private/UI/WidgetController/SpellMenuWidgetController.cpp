// 由来时路褒贬不一制作


#include "UI/WidgetController/SpellMenuWidgetController.h"

#include "Gas/Player/AbilitySystemComponent/AuraAbilitySystemComponent.h"

void USpellMenuWidgetController::BroadcastInitialValues()
{
	BroadcastAbilityInfo();
	/*广播技能点*/
	SpellPointsChangedDelegate.Broadcast(GetAuraPlayerState()->GetPlayerSpellPoints());
}

void USpellMenuWidgetController::BindCallbacksToDependencies()
{
	GetAuraAbilitySystemComponent()->AbilityStatusChangedDelegate.AddLambda([this](const FGameplayTag& AbilityTag,const FGameplayTag& StatusTag)
	{
		if (AbilityInfo)
		{
			/*获取技能数据，并更新技能解锁状态*/
			FAuraAbilityInfo AuraAbilityInfo = AbilityInfo->FindAbilityInfoForTag(AbilityTag);
			AuraAbilityInfo.StatusTag = StatusTag;
			AbilityInfoDelegate.Broadcast(AuraAbilityInfo);
		}
	});

	/*向UI广播属性点*/
	GetAuraPlayerState()->OnSpellPointsChangedDelegate.AddLambda([this](const int32 Points)
	{
		SpellPointsChangedDelegate.Broadcast(Points);
	});
	
}
