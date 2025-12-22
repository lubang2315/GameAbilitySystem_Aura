// 由来时路褒贬不一制作
//元属性跟普通属性有什么区别吗，其实就是元属性是接收数值然后经过各种自定义计算输出数值，并且计算完输出后不会保存数据，而普通属性就是直接计算属性值并且保存不适合复杂计算。

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

DECLARE_DELEGATE_RetVal(FGameplayAttribute,FAttributeSignature);

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

	/*在升级后重置血量蓝量为最大值，下面这个预制函数在修改GE后自动调用*/
	virtual void PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue) override;
	
    /**开始，夹值*/
	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
	virtual void PostGameplayEffectExecute(const struct FGameplayEffectModCallbackData& Data) override;
     /**结束，夹值*/
	
	static void SetEffectProperties(const FGameplayEffectModCallbackData& Data,FEffectPropreties& Props);
	
	TMap<FGameplayTag, FGameplayAttribute(*)()> TagsToAttributes;

	/*主要属性*/
	UPROPERTY(BlueprintReadOnly,ReplicatedUsing = OnRep_Strength,Category = "Vital Attributes")
    FGameplayAttributeData Strength;
	/*下面这个数属性访问的宏，加了他还有在其他地方直接读取访问和修改*/
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet,Strength);

	UPROPERTY(BlueprintReadOnly,ReplicatedUsing = OnRep_Intelligence,Category = "Vital Attributes")
	FGameplayAttributeData Intelligence;
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet,Intelligence);
	
	UPROPERTY(BlueprintReadOnly,ReplicatedUsing = OnRep_Resilience,Category = "Vital Attributes")
	FGameplayAttributeData Resilience;
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet,Resilience);
	
	UPROPERTY(BlueprintReadOnly,ReplicatedUsing = OnRep_Vigor,Category = "Vital Attributes")
	FGameplayAttributeData Vigor;
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet,Vigor);

	/*元属性*/
	UPROPERTY(BlueprintReadOnly,Category = "Meta Attributes")
	FGameplayAttributeData IncomingDamage;
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet,IncomingDamage);

	UPROPERTY(BlueprintReadOnly,Category = "Meta Attributes")
	FGameplayAttributeData IncomingXP;
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet,IncomingXP);
	
	/*次级属性*/
	UPROPERTY(BlueprintReadOnly,ReplicatedUsing = OnRep_HP/*回调函数*/,Category = "Vital Attributes")
	FGameplayAttributeData HP;
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet,HP);/*在宏中注册属性，这样你就可以直接使用初始化，取值，设置值。同时属性系统可以感知记录*/

	UPROPERTY(BlueprintReadOnly,ReplicatedUsing = OnRep_MaxHP,Category = "Vital Attributes")
	FGameplayAttributeData MaxHp;
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet,MaxHp);

	UPROPERTY(BlueprintReadOnly,ReplicatedUsing = OnRep_Mana,Category = "Vital Attributes")
	FGameplayAttributeData Mana;
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet,Mana);

	UPROPERTY(BlueprintReadOnly,ReplicatedUsing = OnRep_MaxMana,Category = "Vital Attributes")
	FGameplayAttributeData MaxMana;
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet,MaxMana);

	UPROPERTY(BlueprintReadOnly,ReplicatedUsing = OnRep_Armor,Category = "Vital Attributes")
	FGameplayAttributeData Armor;
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet,Armor);

	UPROPERTY(BlueprintReadOnly,ReplicatedUsing = OnRep_ArmorPenetratinon,Category = "Vital Attributes")
	FGameplayAttributeData ArmorPenetratinon;
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet,ArmorPenetratinon);
	
	UPROPERTY(BlueprintReadOnly,ReplicatedUsing = OnRep_blockChance,Category = "Vital Attributes")
	FGameplayAttributeData blockChance;
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet,blockChance);
	
	UPROPERTY(BlueprintReadOnly,ReplicatedUsing = OnRep_CriticalHitChance,Category = "Vital Attributes")
	FGameplayAttributeData CriticalHitChance;
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet,CriticalHitChance);

	UPROPERTY(BlueprintReadOnly,ReplicatedUsing = OnRep_CriticalHitDamage,Category = "Vital Attributes")
	FGameplayAttributeData CriticalHitDamage;
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet,CriticalHitDamage);

	UPROPERTY(BlueprintReadOnly,ReplicatedUsing = OnRep_CriticalHitResistance,Category = "Vital Attributes")
	FGameplayAttributeData CriticalHitResistance;
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet,CriticalHitResistance);

	UPROPERTY(BlueprintReadOnly,ReplicatedUsing = OnRep_HealthRegeration,Category = "Vital Attributes")
	FGameplayAttributeData HealthRegeration;
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet,HealthRegeration);

	UPROPERTY(BlueprintReadOnly,ReplicatedUsing = OnRep_ManaRegeration,Category = "Vital Attributes")
	FGameplayAttributeData ManaRegeration;
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet,ManaRegeration);

	
	/*Resistance Damage Types Attributes*/
	UPROPERTY(BlueprintReadOnly,ReplicatedUsing = OnRep_FireResistance,Category = "Resistance Damage Types Attributes")
	FGameplayAttributeData FireResistance;
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet,FireResistance);

	UPROPERTY(BlueprintReadOnly,ReplicatedUsing = OnRep_LightningResistance,Category = "Resistance Damage Types Attributes")
	FGameplayAttributeData LightningResistance;
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet,LightningResistance);

	UPROPERTY(BlueprintReadOnly,ReplicatedUsing = OnRep_ArcaneResistance,Category = "Resistance Damage Types Attributes")
	FGameplayAttributeData ArcaneResistance;
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet,ArcaneResistance);

	UPROPERTY(BlueprintReadOnly,ReplicatedUsing = OnRep_PhysicalResistance,Category = "Resistance Damage Types Attributes")
	FGameplayAttributeData PhysicalResistance;
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet,PhysicalResistance);
	

	/**当接收到GetLifetimeReplicatedProps里面符合复制要求的打包好的数据，客户端接收数据后会触发回调函数就是以下函数。你可以做任何用途，在这里用作GAS预测系统，记录回滚值，以下用来记录旧值*/
    /*主要属性*/
	UFUNCTION()
	void OnRep_Strength(const FGameplayAttributeData& OldStrength) const;

	UFUNCTION()
	void OnRep_Intelligence(const FGameplayAttributeData& OldIntelligence) const;

	UFUNCTION()
	void OnRep_Resilience(const FGameplayAttributeData& OldResilience) const;

	UFUNCTION()
	void OnRep_Vigor(const FGameplayAttributeData& OldVigor) const;
	 
	/*次级属性*/
	UFUNCTION()
	void OnRep_HP(const FGameplayAttributeData& OldHP) const;

	UFUNCTION()
	void OnRep_MaxHp(const FGameplayAttributeData& OldMaxHp) const;

	UFUNCTION()
	void OnRep_Mana(const FGameplayAttributeData& OldMana) const;

	UFUNCTION()
	void OnRep_MaxMana(const FGameplayAttributeData& OldMaxMana) const;

	UFUNCTION()
	void OnRep_Armor(const FGameplayAttributeData& OldArmor) const;

	UFUNCTION()
	void OnRep_ArmorPenetratinon(const FGameplayAttributeData& OldArmorPenetratinon) const;

	UFUNCTION()
	void OnRep_blockChance(const FGameplayAttributeData& OldblockChance) const;

	UFUNCTION()
	void OnRep_CriticalHitChance(const FGameplayAttributeData& OldCriticalHitChance) const;
 
    UFUNCTION()
	void OnRep_CriticalHitDamage(const FGameplayAttributeData& OldCriticalHitDamage) const;

    UFUNCTION()
	void OnRep_CriticalHitResistance(const FGameplayAttributeData& OldCriticalHitResistance) const;

    UFUNCTION()
	void OnRep_HealthRegeration(const FGameplayAttributeData& OldHealthRegeration) const;

    UFUNCTION()
	void OnRep_ManaRegeration(const FGameplayAttributeData& OldManaRegeration) const;


	/*Resistance Damage Types Attributes*/
	UFUNCTION()
	void OnRep_FireResistance(const FGameplayAttributeData& OldFireResistance) const;

	UFUNCTION()
	void OnRep_LightningResistance(const FGameplayAttributeData& OldLightningResistance) const;

	UFUNCTION()
	void OnRep_ArcaneResistance(const FGameplayAttributeData& OldArcaneResistance) const;

	UFUNCTION()
	void OnRep_PhysicalResistance(const FGameplayAttributeData& OldPhysicalResistance) const;
	
private:
	void ShowFloatingText(const FEffectPropreties& Props,float DamageValue,bool IsBlockHit,bool IsCriticalHit);

	/*在蓝图中通过GA监听，通过下面函数触发，通过这种方式来修改XP*/
	void SendXPEvent(const FEffectPropreties& Props);

	/*用来辅助是否重置血量蓝量*/
	bool bFillHealth = false;
	bool bFillMana = false;

	/*当传入的为伤害属性进行处理的逻辑*/
	void HandelIncomingDamage(const FEffectPropreties& Props);

	/*当传入的为经验属性进行处理的逻辑*/
	void HandelIncomingXP(const FEffectPropreties& Props);

	/*如果伤害触发负面效果进行处理的逻辑*/
	void HandelDebuff(const FEffectPropreties& Props);
	
};


