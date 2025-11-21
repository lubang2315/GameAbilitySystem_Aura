// 由来时路褒贬不一制作

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "AttributeSet.h"
#include "GameFramework/PlayerState.h"
#include "AuraPlayerState.generated.h"


DECLARE_MULTICAST_DELEGATE_OneParam(FOnPlayerStateChanged, int32)

class ULevelUpInfo;
/**
 * 
 */
UCLASS()
class AURA_END_API AAuraPlayerState : public APlayerState,public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	AAuraPlayerState();
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	UAttributeSet* GetAttributeSet() const {return AttributeSet;}

	/*添加等级信息*/
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<ULevelUpInfo> LevelUpInfo;
	
	/*等级，经验值变化委托*/
	FOnPlayerStateChanged OnLevelChangedDelegate;
	FOnPlayerStateChanged OnXPChangedDelegate;
	
	/*获取玩家等级,这里前缀FORCEINLINE是强制内联的意思*/
	FORCEINLINE int32 GetPlayerLevel() const {return Level;}
	void AddToLevel(int32 InLevel);
	void SetLevel(int32 InLevel);

	/*获取玩家经验值，增加经验值，设置当前经验值*/
	FORCEINLINE int32 GetPlayerXP() const {return XP;}
	void AddToXP(int32 InXP);
	void SetXP(int32 InXP);
	
protected:
	UPROPERTY()
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY()
	TObjectPtr<UAttributeSet> AttributeSet;

	/*玩家属性等级，在反射宏特别标注网络复制，同时在服务器端监测值发生变化并复制到客户端然后调用OnRepLevel，注意这里提供回调函是提供制作者自定义的*/
	UPROPERTY(VisibleAnywhere,ReplicatedUsing = OnRepLevel);
	int32 Level = 1;

	/*玩家经验值*/
	UPROPERTY(VisibleAnywhere,ReplicatedUsing = OnRepXP);
	int32 XP = 0;

	/*在服务器数值变化并复制到客户端后自动调用该函数*/
	UFUNCTION()
	void OnRepLevel(int32 OldLevel);

	UFUNCTION()
	void OnRepXP(int32 OldXP);

	void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	
};
