// 由来时路褒贬不一制作


#include "Gas/Ability/AuraDamageGameplayAbility.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"

void UAuraDamageGameplayAbility::CauseDamage(AActor* TargetActor)
{
	/*创建一个GE*/
	FGameplayEffectSpecHandle DamageSpecHandle = MakeOutgoingGameplayEffectSpec(DamageEffectClass,1.f);
	/*通过调用SetByCall设置属性伤害*/
	for (auto Pair : DamageTypes)
	{
		const float ScaleDamage = Pair.Value.GetValueAtLevel(GetAbilityLevel());
		UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(DamageSpecHandle,Pair.Key,ScaleDamage);
	}
	/*将GE应用给目标*/
	GetAbilitySystemComponentFromActorInfo()->ApplyGameplayEffectSpecToTarget(*DamageSpecHandle.Data.Get(),UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor));
}

FTaggedMontage UAuraDamageGameplayAbility::GetRandomTaggedMontageFromArray(
	const TArray<FTaggedMontage>& TaggedMontageArray) const
{
	if (TaggedMontageArray.Num() > 0)
	{
		const int32 Selection = FMath::RandRange(0, TaggedMontageArray.Num() - 1);
		return TaggedMontageArray[Selection];
	}
	return FTaggedMontage();
}

float UAuraDamageGameplayAbility::GetDamageByDamageType(int32 InLevel, const FGameplayTag& DamageTag)
{
	checkf(DamageTypes.Contains(DamageTag),TEXT("技能[%s]没有包含[%s]类型的伤害"), *GetNameSafe(this), *DamageTag.ToString());
	/*根据技能等级获取伤害*/
	return DamageTypes[DamageTag].GetValueAtLevel(InLevel);
}
