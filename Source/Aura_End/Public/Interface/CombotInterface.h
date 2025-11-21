// 由来时路褒贬不一制作
//这里战斗接口

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Gas/DataAsset/CharacterClassInfo.h"
#include "UObject/Interface.h"
#include "CombotInterface.generated.h"

class UNiagaraSystem;
class UAnimMontage;

/*创建一个结构体，用来存储蒙太奇和标签和骨骼位置的映射，用于攻击技能获取和设置攻击范围*/
USTRUCT(BlueprintType)
struct  FTaggedMontage
{
	GENERATED_BODY()

	/*使用的蒙太奇*/
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly)
	UAnimMontage* Montage = nullptr;

	/*对应的标签*/
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly)
	FGameplayTag MontageTag;

	/*技能释放位置的标签*/
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly)
	FGameplayTag SocketTag;
	
	/*受击音效*/
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly)
	USoundBase* ImpactSound = nullptr;
};


// This class does not need to be modified.
UINTERFACE(MinimalAPI,BlueprintType)
class UCombotInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class AURA_END_API ICombotInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintNativeEvent)
	int32 GetPlayerLevel();

	UFUNCTION(BlueprintNativeEvent,BlueprintCallable)
	FVector GetCombatSocketLocation(const FGameplayTag& MontageTag)const;

	/*获取敌人位置释放技能时利用运动扭曲组件面向敌人*/
	UFUNCTION(BlueprintCallable,BlueprintImplementableEvent)
	void PassTargetDataVector(const FVector& Vector);

	UFUNCTION(BlueprintNativeEvent,BlueprintCallable)
	UAnimMontage* GetAnimMontage();
	
	virtual void Die() = 0;

	/*传递Character状态*/
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	bool IsDead();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	AActor* GetAvatar();

	/*在本页Top生成结构体，在人物基类创建容器，在这里创建获取函数，并由人物基类覆写*/
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	TArray<FTaggedMontage> GetTaggedMontages();

	/*获取敌人受击特效*/
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	UNiagaraSystem* GetBloodEffect();

	/*通过标签传递对应的结构体*/
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	FTaggedMontage GetTaggedMontageByTag(const FGameplayTag& MontageTag);

	/*传递随从数量*/
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	int32 GetMinionCount();

	/*改变随从数量*/
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void SetMinionCount(int32 Amount);

	/*获取敌人类型*/
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	ECharacterClass GetCharacterClass();
};
