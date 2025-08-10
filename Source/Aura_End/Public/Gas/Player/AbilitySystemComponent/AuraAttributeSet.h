// 由来时路褒贬不一制作

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "AuraAttributeSet.generated.h"

#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
    GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)
USTRUCT()
struct FEffectPropreties
{
	/**技能属性集合*/
	GENERATED_BODY()
	FEffectPropreties(){};
	FGameplayEffectContextHandle EffectContextHandle;
	/**源信息*/
    UPROPERTY()
	UAbilitySystemComponent* SourceASC = nullptr;
	UPROPERTY()
	AActor* SourceActor = nullptr;
	UPROPERTY()
	AController* SourceController = nullptr;
	UPROPERTY()
	ACharacter* SourceCharacter = nullptr;
    /**目标信息*/
	UPROPERTY()
	UAbilitySystemComponent* TargetASC = nullptr;
	UPROPERTY()
	AActor* TargetActor = nullptr;
	UPROPERTY()
	AController* TargetController = nullptr;
	UPROPERTY()
	ACharacter* TargetCharacter = nullptr;
};

/**
 * 
 */
UCLASS()
class AURA_END_API UAuraAttributeSet : public UAttributeSet
{
	GENERATED_BODY()

public:
	
	/**set Initial Value*/
	UAuraAttributeSet();
	/**设置服务器属性值复制到客户端，用于预测系统，我们已经把过去值和限值提交给能力系统管理，当服务器收到改变值会验证有效性并同步其他客户端*/
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;

	virtual void PostGameplayEffectExecute(const struct FGameplayEffectModCallbackData& Data) override;

	static void SetEffectProperties(const FGameplayEffectModCallbackData& Data,FEffectPropreties& Props);
	
	/**创建HP，和Mana两个属性值*/
	UPROPERTY(BlueprintReadOnly,ReplicatedUsing = OnRep_HP,Category = "Vital Attributes")
	FGameplayAttributeData HP;
	/**属性访问器，使用这个继承的宏可以快速设置初值和在蓝图端访问属性*/
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet,HP);

	UPROPERTY(BlueprintReadOnly,ReplicatedUsing = OnRep_MaxHP,Category = "Vital Attributes")
	FGameplayAttributeData MaxHp;
	/**属性访问器，使用这个继承的宏可以快速设置初值和在蓝图端访问属性*/
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet,MaxHp);

	UPROPERTY(BlueprintReadOnly,ReplicatedUsing = OnRep_Mana,Category = "Vital Attributes")
	FGameplayAttributeData Mana;
	/**属性访问器，使用这个继承的宏可以快速设置初值和在蓝图端访问属性*/
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet,Mana);

	UPROPERTY(BlueprintReadOnly,ReplicatedUsing = OnRep_MaxMana,Category = "Vital Attributes")
	FGameplayAttributeData MaxMana;
	/**属性访问器，使用这个继承的宏可以快速设置初值和在蓝图端访问属性*/
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet,MaxMana);

	/**GAS预测系统，需要记录回滚值，以下用来记录旧值*/
	UFUNCTION()
	void OnRep_HP(const FGameplayAttributeData& OldHP) const;

	UFUNCTION()
	void OnRep_MaxHp(const FGameplayAttributeData& OldMaxHp) const;

	UFUNCTION()
	void OnRep_Mana(const FGameplayAttributeData& OldMana) const;

	UFUNCTION()
	void OnRep_MaxMana(const FGameplayAttributeData& OldMaxMana) const;
};
