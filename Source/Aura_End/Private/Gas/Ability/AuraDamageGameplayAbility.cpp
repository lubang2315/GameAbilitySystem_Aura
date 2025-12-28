// 由来时路褒贬不一制作


#include "Gas/Ability/AuraDamageGameplayAbility.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Gas/FunctionLibrary/MyFunctionLibrary.h"

FDamageEffectPrams UAuraDamageGameplayAbility::MakeDamageEffectPramsFromClassDefault(AActor* TargetActor) const
{
	FDamageEffectPrams Params;
	Params.WorldContextObject = GetAvatarActorFromActorInfo();
	Params.DamageGameplayEffectClass = DamageEffectClass;
	Params.SourceAbilitySystemComponent = GetAbilitySystemComponentFromActorInfo();
	Params.TargetAbilitySystemComponent = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);
	Params.BaseDamage = Damage.GetValueAtLevel(GetAbilityLevel());
	Params.AbilityLevel = GetAbilityLevel();
	Params.DamageType = DamageType;
	Params.DebuffChance = DebuffChance;
	Params.DebuffDamage = DebuffDamage;
	Params.DebuffFrequency = DebuffFrequency;
	Params.DebuffDuration = DebuffDurations;
	Params.DeathImpulseMagnitude = DeathImpulseMagnitude;
	Params.KnockBackChance = KnockBackChance;
	Params.KnockBackMagnitude = KnockBackMagnitude;
	return Params;
}

float UAuraDamageGameplayAbility::GetDamageAtLevel()
{
	return Damage.GetValueAtLevel(GetAbilityLevel());
}

void UAuraDamageGameplayAbility::CauseDamage(AActor* TargetActor)
{
	/*创建一个GE*/
	FGameplayEffectSpecHandle DamageSpecHandle = MakeOutgoingGameplayEffectSpec(DamageEffectClass,1.f);
	/*通过调用SetByCall设置属性伤害*/
	const float ScaleDamage = Damage.GetValueAtLevel(GetAbilityLevel());
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(DamageSpecHandle,DamageType,ScaleDamage);
	/*将GE应用给目标*/
	GetAbilitySystemComponentFromActorInfo()->ApplyGameplayEffectSpecToTarget(*DamageSpecHandle.Data.Get(),UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor));

	/*以下是敌人攻击击飞玩家并应用伤害*/
	//生成配置
	FDamageEffectPrams Params = MakeDamageEffectPramsFromClassDefault(TargetActor);

	//设置死亡冲击和击退
	if(IsValid(TargetActor))
	{
		//获取到攻击对象和目标的朝向，并转换成角度
		FRotator Rotation = (TargetActor->GetActorLocation() - GetAvatarActorFromActorInfo()->GetActorLocation()).Rotation();
		Rotation.Pitch = 45.f; //设置击退角度垂直45度
		const FVector ToTarget = Rotation.Vector();
		Params.DeathImpulse = ToTarget * DeathImpulseMagnitude;
		//判断攻击是否触发击退
		if(FMath::RandRange(1, 100) < Params.KnockBackChance)
		{
			Params.KnockBackForce = ToTarget * KnockBackMagnitude;
		}
	}
		
	//通过配置项应用给目标ASC
	UMyFunctionLibrary::ApplyGameplayEffect(Params);

	/*End*/
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
