// 由来时路褒贬不一制作

#pragma once

#include "CoreMinimal.h"
#include "AuraAbilityTypes.h"
#include "Gas/Ability/AuraGameplayAbility.h"
#include "Interface/CombotInterface.h"
#include "AuraDamageGameplayAbility.generated.h"

/**
 * 
 */
UCLASS()
class AURA_END_API UAuraDamageGameplayAbility : public UAuraGameplayAbility
{
	GENERATED_BODY()

public:
	/*创建负面影响相关参数结构体*/
	UFUNCTION(BlueprintPure)
	FDamageEffectPrams MakeDamageEffectPramsFromClassDefault(
		AActor* TargetActor = nullptr,
		FVector InRadialDamageOrigin = FVector::ZeroVector,
		bool bOverrideKnockbackDirection = false,
		FVector KnockbackDirectionOverride = FVector::ZeroVector,
		bool bOverrideDeathImpulse = false,
		FVector DeathImpulseDirectionOverride = FVector::ZeroVector,
		bool bOverridePitch = false,
		float PitchOverride = 0.f) const;
	
	/*获取技能伤害*/
	UFUNCTION(BlueprintPure)
	float GetDamageAtLevel();

protected:
	
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly)
	TSubclassOf<UGameplayEffect> DamageEffectClass ;

	/*伤害类型*/
	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	FGameplayTag DamageType;

	/*伤害数值*/
	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	FScalableFloat Damage;

	/*触发负面影响的几率*/
	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	float DebuffChance = 20.f;
	
	/*负面影响伤害数值*/
	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	float DebuffDamage = 5.f;
	
	/*负面伤害触发间隔时间*/
	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	float DebuffFrequency = 1.f;
	
	/*负面影响持续时间*/
	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	float DebuffDurations = 1.f;
	
	UFUNCTION(BlueprintCallable)
	void CauseDamage(AActor* TargetActor);

	UFUNCTION(BlueprintPure)
	FTaggedMontage GetRandomTaggedMontageFromArray(const TArray<FTaggedMontage>& TaggedMontageArray) const;

	/*被技能击中并造成致命一击被弹飞的强度*/
	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	float DeathImpulseMagnitude = 60.f;

	/*被技能击中并造成击飞的概率*/
	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	float KnockBackChance = 60.f;

	/*被技能击中并造成击飞的幅度*/
	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	float KnockBackMagnitude = 60.f;

	/*设置该技能是否具有范围半径伤害*/
	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	bool IsRadialDamage = false;

	/*设置该技能范围半径伤害的内半径大小*/
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly, Category = "Damage")
	float RadialDamageInnerRadius = 0.f;

	/*设置该技能范围半径伤害的外半径伤害*/
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,  Category = "Damage")
	float RadialDamageOuterRadius = 0.f;

};
