// 由来时路褒贬不一制作

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "AuraAbilitySystemComponent.generated.h"

/**
 * 
 */

DECLARE_MULTICAST_DELEGATE_OneParam(EffectAssetTag,const FGameplayTagContainer& /*AssetTag*/)
/*下面这个委托，技能初始化后调用的委托，目前只是通知技能UI，技能已经初始化完成可以获取技能信息，并加载到OverlayUI里面的技能栏*/
DECLARE_MULTICAST_DELEGATE(FAbilityGiven)
/*遍历已经激活的技能*/
DECLARE_DELEGATE_OneParam(FForEachAbility,const FGameplayAbilitySpec&)
/*广播技能解锁状态*/
DECLARE_MULTICAST_DELEGATE_ThreeParams(FAbilityStatusChanged,const FGameplayTag& /*AbilityTag*/ ,const FGameplayTag&  /*StatusTag*/,int32 /*level*/)
/*技能装配委托*/
DECLARE_MULTICAST_DELEGATE_FourParams(FAbilityEquip,const FGameplayTag& /*技能标签*/,const FGameplayTag& /*技能状态标签*/,const FGameplayTag& /*技能输入插槽Slot标签*/,const FGameplayTag& /*上一个输入插槽标签*/)

UCLASS()
class AURA_END_API UAuraAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()

public:
	/**GE作用到Actor上时创建回调函数，用于广播*/
	void AbilityActorInfoSet();

	EffectAssetTag EffectAssetTag;

	/*创建回调函数，技能初始化完成广播,在这里因为不确定OverlayWidgetController那边绑定回调时这里的广播是否创建所以这里加一个bool用来辅助判断*/
	FAbilityGiven AbilityGivenDelegate;
	bool bStartupAbilityGiven = false;

	/*技能解锁状态更新委托*/
	FAbilityStatusChanged AbilityStatusChangedDelegate;

	/*技能装备到技能栏委托*/
	FAbilityEquip AbilityEquipDelegate;
	
	/*添加能力，注意能力在添加后要使用还要激活 *ActivateGA**/
	void AddCharacterAbilities(const TArray<TSubclassOf<UGameplayAbility>>& StartupAbilities);

	/*添加被动能力*/
	void AddCharacterPassiveAbilities(const TArray<TSubclassOf<UGameplayAbility>>& StartupPassiveAbilities);

	/*获取可激活技能自动在客户端调用下面函数，在这里用来绑定回调通知客户端触发技能信息UI*/
	virtual void OnRep_ActivateAbilities() override;

	/*触发和结束两种状态下，激活GA和移除GA(因为结束输入操作不一定GA结束，GA可能作用更长时间，所以这里只做通知) *ActivateGA**/
	void AbilityInputTagHold(const FGameplayTag GameplayTag);
	void AbilityAssetTagReleased(const FGameplayTag GameplayTag);

	/*遍历激活技能，并广播*/
	void FForEachAbility(const FForEachAbility& Delegate);

	/*帮助WidgetController获取技能标签和Input标签*/
	static FGameplayTag GetAbilityTagFromSpec(const FGameplayAbilitySpec& AbilitySpec);
	static FGameplayTag GetInputTagFromSpec(const FGameplayAbilitySpec& AbilitySpec);
	static FGameplayTag GetStatusTagFromSpec(const FGameplayAbilitySpec& AbilitySpec);

	FGameplayTag GetStatusFromAbilityTag(const FGameplayTag& AbilityTag);
	FGameplayTag GetInputTagFromAbilityTag(const FGameplayTag& AbilityTag);
	
	/*通过技能标签获取已经创建的技能*/
	FGameplayAbilitySpec* GetSpecFromAbilityTag(const FGameplayTag& GameplayTag);

	/*根据角色等级更新技能在技能Menu中状态*/
	void UpdateAbilityStatuses(int32 Level);
	
	/*消耗属性点升级属性*/
	UFUNCTION(BlueprintCallable, Category="GAS|Attributes")
	void UpGradeAttribute(const FGameplayTag& AttributeTag);

	/*服务器运行，消耗属性点升级属性*/
	UFUNCTION(Server,Reliable)
	void ServerUpGradeAttribute(const FGameplayTag& AttributeTag);

	/*服务器运行花费技能点升级技能等级*/
	UFUNCTION(Server,Reliable)
	void ServerSpendSpellPoints(const FGameplayTag& AttributeTag);

	/*通过技能标签获取描述*/
	bool GetDescrptionByAbilityTag(const FGameplayTag& AbilityTag,FString& OutLevelDescription,FString& OutNextLevelDescription);

	/*在服务器处理技能装配*/
	UFUNCTION(Server,Reliable)
	void ServerEquipAbility(const FGameplayTag& SlotTag,const FGameplayTag& AbilityTag);

	/*在客户端处理技能装配*/
	UFUNCTION(Client,Reliable)
	void ClientEquipAbility(const FGameplayTag& SlotTag,const FGameplayTag& AbilityTag,const FGameplayTag& StatusTag,const FGameplayTag& PreviousTag);

	/*清除技能插槽标签，也可以说清除技能装备栏技能球装备的技能*/
	void ClealSlot(FGameplayAbilitySpec* Spec);

	/*如果技能已经装备，并且需要在装备栏移动位置，那就需要先清除原先插槽技能，所以根据插槽标签清除技能插槽技能*/
	void ClealAbilityOfSlot(const FGameplayTag& SlotTag);

	/*用来判断技能属主动还是被动技能类型*/
	static bool AbilityHasSlot(FGameplayAbilitySpec* Spec,const FGameplayTag& Slot);
	
protected:
	UFUNCTION(Client, reliable)
    void ClientEffectApplied(UAbilitySystemComponent* AbilitySystemComponent, const FGameplayEffectSpec& SpecApplied, FActiveGameplayEffectHandle ActiveHandle);

	UFUNCTION(Client, reliable)
	void ClientUpdateAbilityStatus(const FGameplayTag& AbilityTag,const FGameplayTag& StatusTag,int32 NewLevel);
	
};







