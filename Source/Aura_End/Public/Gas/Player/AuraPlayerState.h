// 由来时路褒贬不一制作

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "AttributeSet.h"
#include "GameFramework/PlayerState.h"
#include "AuraPlayerState.generated.h"


DECLARE_MULTICAST_DELEGATE_OneParam(FOnPlayerStateChanged, int32)
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnLeverChanged, int32,bool);

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
	
	/*等级，经验值,属性点，技能点变化委托*/
	FOnLeverChanged OnLevelChangedDelegate;
	FOnPlayerStateChanged OnXPChangedDelegate;
	FOnPlayerStateChanged OnAttributePointsChangedDelegate;
	FOnPlayerStateChanged OnSpellPointsChangedDelegate;
	
	/*获取玩家等级,这里前缀FORCEINLINE是强制内联的意思*/
	FORCEINLINE int32 GetPlayerLevel() const {return Level;}
	void AddToLevel(int32 InLevel);
	void SetLevel(int32 InLevel);

	/*获取玩家经验值，增加经验值，设置当前经验值*/
	FORCEINLINE int32 GetPlayerXP() const {return XP;}
	void AddToXP(int32 InXP);
	void SetXP(int32 InXP);
	
	/*获取玩家属性值，增加属性值，设置当前属性值*/
	FORCEINLINE int32 GetPlayerAttributePoints() const {return AttributePoints;}
	void AddToAttributePoints(int32 InAttributePoints);
	void SetAttributePoints(int32 InAttributePoints);

	/*获取玩家技能值，增加技能值，设置当前技能值*/
	FORCEINLINE int32 GetPlayerSpellPoints() const {return SpellPoints;}
	void AddToSpellPoints(int32 InSpellPoints);
	void SetSpellPoints(int32 InSpellPoints);


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

	/*属性点*/
	UPROPERTY(VisibleAnywhere,ReplicatedUsing = OnRepAttributePoints);
	int32 AttributePoints = 0;

	/*技能点*/
	UPROPERTY(VisibleAnywhere,ReplicatedUsing = OnRepSpellPoints);
	int32 SpellPoints = 0;

	/*在服务器数值变化并复制到客户端后自动调用该函数*/
	UFUNCTION()
	void OnRepLevel(int32 OldLevel);

	UFUNCTION()
	void OnRepXP(int32 OldXP);

	UFUNCTION()
	void OnRepAttributePoints(int32 OldAttributePoints);

	UFUNCTION()
	void OnRepSpellPoints(int32 OldSpellPoints);

	void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	
};
