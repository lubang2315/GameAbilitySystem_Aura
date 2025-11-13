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
DECLARE_MULTICAST_DELEGATE_OneParam(FAbilityGiven,UAuraAbilitySystemComponent*)
/*遍历已经激活的技能*/
DECLARE_DELEGATE_OneParam(FForEachAbility,const FGameplayAbilitySpec&)

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
	

	/*添加能力，注意能力在添加后要使用还要激活 *ActivateGA**/
	void AddCharacterAbilities(const TArray<TSubclassOf<UGameplayAbility>>& StartupAbilities);

	/*获取可激活技能自动在客户端调用下面函数，在这里用来绑定回调通知客户端触发技能信息UI*/
	virtual void OnRep_ActivateAbilities() override;

	/*触发和结束两种状态下，激活GA和移除GA(因为结束输入操作不一定GA结束，GA可能作用更长时间，所以这里只做通知) *ActivateGA**/
	void AbilityInputTagHold(const FGameplayTag GameplayTag);
	void AbilityAssetTagReleased(const FGameplayTag GameplayTag);

	/*遍历激活技能，并广播*/
	void FForEachAbility(const FForEachAbility& Delegate);

	/*帮助OverlayWidgetController获取技能标签和Input标签*/
	static FGameplayTag GetAbilityTagFromSpec(const FGameplayAbilitySpec& AbilitySpec);
	static FGameplayTag GetInputTagFromSpec(const FGameplayAbilitySpec& AbilitySpec);
protected:
	UFUNCTION(Client, reliable)
    void ClientEffectApplied(UAbilitySystemComponent* AbilitySystemComponent, const FGameplayEffectSpec& SpecApplied, FActiveGameplayEffectHandle ActiveHandle);
};
