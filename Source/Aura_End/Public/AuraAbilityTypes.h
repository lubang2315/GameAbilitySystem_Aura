#pragma once

#include "GameplayEffectTypes.h"
#include "AuraAbilityTypes.generated.h"

/*这是游戏效果上下文，他的主要功能就是你能自定义一个信息结构体并由你决定网络同步传递哪些数据就跟一个信封，你可以决定信封里面传递什么*/

class UGameplayEffect;

USTRUCT(BlueprintType)
struct FDamageEffectPrams
{
	GENERATED_BODY()

	/*默认构造函数，增加代码可读性*/
	FDamageEffectPrams(){}

	/*当前上下文对象*/
	UPROPERTY()
	TObjectPtr<UObject> WorldContextObject = nullptr;

	/*需要应用的GE类*/
	UPROPERTY()
	TSubclassOf<UGameplayEffect> DamageGameplayEffectClass = nullptr;
	
	/*源ASC*/
	UPROPERTY()
	TObjectPtr<UAbilitySystemComponent> SourceAbilitySystemComponent = nullptr;
	
	/*目标ASC*/
	UPROPERTY()
	TObjectPtr<UAbilitySystemComponent> TargetAbilitySystemComponent = nullptr;
	
	/*基础伤害*/
	UPROPERTY()
	float BaseDamage = 0.0f;
	
	/*技能等级*/
	UPROPERTY()
	float AbilityLevel = 1.f;

	/*伤害类型*/
	UPROPERTY()
	FGameplayTag DamageType = FGameplayTag();
	
	/*触发负面影响的几率*/
	UPROPERTY()
	float DebuffChance = 20.f;
	
	/*负面影响伤害数值*/
	UPROPERTY()
	float DebuffDamage = 5.f;
	
	/*负面伤害触发间隔时间*/
	UPROPERTY()
	float DebuffFrequency = 1.f;
	
	/*负面影响持续时间*/
	UPROPERTY()
	float DebuffDuration = 0.f;
	
	/*被技能击中并造成致命一击被弹飞的强度*/
	UPROPERTY()
	float DeathImpulseMagnitude = 0.f;

	/*死亡时受到攻击朝向*/
	UPROPERTY()
	FVector DeathImpulse = FVector::ZeroVector;

	/*被技能击中并造成击飞的概率*/
	UPROPERTY()
	float KnockBackChance = 0.f;

	/*被技能击中并造成击飞的概率*/
	UPROPERTY()
	float KnockBackMagnitude = 0.f;

	/*被击飞时受到攻击朝向*/
	UPROPERTY()
	FVector KnockBackForce = FVector::ZeroVector;
};


USTRUCT(BlueprintType)
struct FAuraGameplayEffectContext : public FGameplayEffectContext
{
	/*宏，自动生成构造函数，析构函数，拷贝函数等*/
	GENERATED_BODY()

public:
	/*返回用于序列化的实际结构体，子类必须覆盖他，所以这里覆盖*/
	virtual UScriptStruct* GetScriptStruct() const
	{
		return FAuraGameplayEffectContext::StaticStruct();
	}

	/*自定义序列化，同样子类必须override他*/
	virtual bool NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess);

	/** Creates a copy of this context, used to duplicate for later modifications */
	virtual FAuraGameplayEffectContext* Duplicate() const
	{
		FAuraGameplayEffectContext* NewContext = new FAuraGameplayEffectContext();
		*NewContext = *this;
		if (GetHitResult())
		{
			// Does a deep copy of the hit result
			NewContext->AddHitResult(*GetHitResult(), true);
		}
		return NewContext;
	}
	
	/*在其他地方可以获取格挡和暴击的bool*/
	bool IsblockedHit() const { return bIsBlockedHit; }
	bool IsCriticalHit() const { return bIsCriticalHit; }

	/*获取负面影响的成功状态，伤害，持续时间，频率，以及负面影响的类型,和致命一击时攻击方向，被击飞时攻击方向*/
	bool GetIsSuccessfulDebuff() const { return bIsSuccessfulDebuff; }
	float GetDebuffDamage() const{return DebuffDamage;}
	float GetDebuffFrequency() const{return DebuffFrequency;}
	float GetDebuffDuration() const{return DebuffDuration;}
	TSharedPtr<FGameplayTag> GetDamageType() const{return DamageType;}
	FVector GetDeathImpulse() const{return DeathImpulse;}
	FVector GetKnockBackForce() const{return KnockBackForce;}

	/*在其他地方可以设置格挡暴击的bool*/
	void SetBlockedHit(bool BlockedHit) { bIsBlockedHit = BlockedHit; }
	void SetCriticalHit(bool CriticalHit) { bIsCriticalHit = CriticalHit; }

	/*设置负面影响的成功状态，伤害，持续时间，频率，以及负面影响的类型,和致命一击时攻击方向，被击飞时攻击方向*/
	void SetIsSuccessfulDebuff(const bool IsSuccessfulDebuff) {bIsSuccessfulDebuff = IsSuccessfulDebuff;}
	void SetDebuffDamage(const float InDebuffDamage) { DebuffDamage = InDebuffDamage; }
	void SetDebuffFrequency(const float InDebuffFrequency) { DebuffFrequency = InDebuffFrequency; }
	void SetDebuffDuration(const float InDebuffDuration) { DebuffDuration = InDebuffDuration; }
	void SetDamageType(const TSharedPtr<FGameplayTag> InDamageType) {DamageType = InDamageType; }
	void SetDeathImpulse(const FVector InDeathImpulse) { DeathImpulse = InDeathImpulse; }
	void SetKonckBackForce(const FVector InKnockBackForce) {KnockBackForce = InKnockBackForce; }
	
protected:

	UPROPERTY()/*格挡*/
	bool bIsBlockedHit = false;

	UPROPERTY()/*暴击*/
	bool bIsCriticalHit = false;

	UPROPERTY()/*是否成功应用负面影响*/
	bool bIsSuccessfulDebuff = false;

	UPROPERTY()/*负面影响伤害*/
	float DebuffDamage = 0.f;
	
	UPROPERTY()/*负面影响频率*/
	float DebuffFrequency = 0.f;

	UPROPERTY()/*负面影响持续时间*/
	float DebuffDuration = 0.f;
	
	/*负面影响伤害类型*/
	TSharedPtr<FGameplayTag> DamageType;

	/*受到攻击时攻击方向*/
	UPROPERTY()
	FVector DeathImpulse = FVector::ZeroVector;

	/*受到攻击时攻击方向*/
	UPROPERTY()
	FVector KnockBackForce = FVector::ZeroVector;

	
};

template<>
struct TStructOpsTypeTraits<FAuraGameplayEffectContext> : public TStructOpsTypeTraitsBase2<FAuraGameplayEffectContext>
{
	enum
	{
		WithCopy = true,		
		WithNetSerializer = true,
	};
};
