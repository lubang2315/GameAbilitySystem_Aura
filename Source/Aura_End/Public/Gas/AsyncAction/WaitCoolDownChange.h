// 由来时路褒贬不一制作

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "WaitCoolDownChange.generated.h"

struct FGameplayTag;
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FCoolDownChangeSignature, float, TimeRemaining);
/**
 * 
 */
UCLASS(BlueprintType, meta = (ExposedAsyncProxy="AsyncTask"))
class AURA_END_API UWaitCoolDownChange : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()
public:
	/*冷却触发引脚*/
	UPROPERTY(BlueprintAssignable)
	FCoolDownChangeSignature CoolDownStart;

	/*冷却完成引脚*/
	UPROPERTY(BlueprintAssignable)
	FCoolDownChangeSignature CoolDownEnd;

	/*创建实例，同时绑定回调*/
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true"))
	static UWaitCoolDownChange* CreateWaitCoolDownChange(UAbilitySystemComponent* AbilitySystemComponent,const FGameplayTag& CoolDownTag);

	/*结束任务防止内存泄漏*/
	UFUNCTION(BlueprintCallable)
	void EndTask();
	
protected:

	/*存储ASC和冷却标签*/
	UPROPERTY()
	TObjectPtr<UAbilitySystemComponent> ASC;
	FGameplayTag CoolDownTag;

	/*监听冷却标签回调函数*/
	void CoolDownTagChanged(const FGameplayTag InCoolDownTag,int32 NewCount) const;

	/*监听ASC激活GE的回调函数*/
	void OnActiveEffectAdded(UAbilitySystemComponent* TargetASC,const FGameplayEffectSpec& Spec,FActiveGameplayEffectHandle ActiveEffectHandel) const; 
	
};
