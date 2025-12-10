// 由来时路褒贬不一制作

#pragma once

#include "CoreMinimal.h"
#include "Tags/AuraGameplayTags.h"
#include "UI/WidgetController/AuraWidgetController.h"
#include "SpellMenuWidgetController.generated.h"

struct FMyGameplayTags;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FSpellGlobeSelectionSignature, bool, bSpendPointEnabled, bool,bEquipEnable, FString,Description,FString,NextLevelDescription);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FWaitForEquipSelectionSignature,const FGameplayTag&, GameplayTag);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSpellGlobeReesignedSignature,const FGameplayTag&, GameplayTag);

struct FSelectedAbility
{
	FGameplayTag AbilityTag = FGameplayTag();
	FGameplayTag StateTag = FGameplayTag();
};

/**
 * 
 */
UCLASS(BlueprintType,Blueprintable)
class AURA_END_API USpellMenuWidgetController : public UAuraWidgetController
{
	GENERATED_BODY()

public:
	virtual  void BroadcastInitialValues() override;
	virtual void BindCallbacksToDependencies() override;

	/*向Ui广播技能点*/
	UPROPERTY(BlueprintAssignable, Category="GAS|Spell")
	FOnPlayerStateChangedSignature SpellPointsChangedDelegate;

	/*选中技能球后，利用技能点升级按钮和技能装备按钮的变动回调*/
	UPROPERTY(BlueprintAssignable, Category="GAS|Spell")
	FSpellGlobeSelectionSignature SpellGlobeSelectionSignature;

	/*选择技能球后调用此函数，用来根据传入技能球的技能标签判断技能解锁状态和获取技能点*/
	UFUNCTION(BlueprintCallable)
	void SpellGlobeSelected(const FGameplayTag& AbilityTag);

	/*花费技能点增加技能等级*/
	UFUNCTION(BlueprintCallable)
	void SpendSpellPointsOnButtonPressed();

	/*取消技能球选择并清空信息栏信息显示*/
	UFUNCTION(BlueprintCallable)
	void GlobeDeselected();

	/*向Equip按钮广播光标选择的技能球的技能类型*/
	UPROPERTY(BlueprintAssignable, Category="GAS|Equip")
	FWaitForEquipSelectionSignature WaitForEquipSelectionDelegate;

	/*向Equip按钮广播取消选择的技能球的技能类型*/
	UPROPERTY(BlueprintAssignable, Category="GAS|Equip")
	FWaitForEquipSelectionSignature DeselectionForEquipSelectionDelegate;

	/*已经装备技能后取消播放技能树的选择动画和播放选择声音*/
	UPROPERTY(BlueprintAssignable)
	FSpellGlobeReesignedSignature SpellGlobeReesignedDelegate;
	
	/*技能确认装备技能栏按钮按下*/
	UFUNCTION(BlueprintCallable)
	void EquipButtonPressed();

	UFUNCTION(BlueprintCallable)
	void SpellRowGlobePressed(const FGameplayTag& SlotTag,const FGameplayTag& AbilityTypeTag);

	/*监听技能装备后的处理*/
	void OnAbilityEquipped(const FGameplayTag& AbilityTag,const FGameplayTag& StatusTag,const FGameplayTag& SlotTag,const FGameplayTag& PreviousTag);
	
private:
	/*通过技能状态标签和技能点数剩余，来判断按钮状态，注意这里是bool&说明他是直接更改的原bool这里效果就跟有返回值函数一样了，也算学到了*/
	static void ShouldEnableButtons(const FGameplayTag& StatusTag,bool bHasSpellPoints,bool& bShpuldEnableSpellPoints,bool& bShouldEnableEquip);

	/*这里初始化一下结构体里面的公开变量，这里主要是为了解决可能技能点和解锁状态不是同步更新问题*/
	FSelectedAbility SelectedAbility = {FMyGameplayTags::Get().Abilities_None,FMyGameplayTags::Get().Abilities_Status_Locked};
	int32 CurrentSpellPoints = 0;

	/*复制判断装备按钮是否已经选择*/
	bool bWaitForEquipSelection = false;

	/*装备技能栏装备技能在哪个技能球通过标签选择*/
	FGameplayTag SelectionSlot;
};
