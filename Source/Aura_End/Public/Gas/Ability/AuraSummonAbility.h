// 由来时路褒贬不一制作
// 召唤敌人技能

#pragma once

#include "CoreMinimal.h"
#include "Gas/Ability/AuraGameplayAbility.h"
#include "AuraSummonAbility.generated.h"

/**
 * 
 */
UCLASS()
class AURA_END_API UAuraSummonAbility : public UAuraGameplayAbility
{
	GENERATED_BODY()
public:
	/*获取召唤敌人生成位置*/
	UFUNCTION(BlueprintCallable)
	TArray<FVector> GetSpawnLocations();

	/*召唤的敌人数量*/
	UPROPERTY(EditDefaultsOnly,Category="Summoning")
	int32 NumMinions = 5;

	/*召唤的敌人类型*/
	UPROPERTY(EditDefaultsOnly,Category="Summoning")
	TArray<TSubclassOf<APawn>> MinionsClasses;

	/*召唤最近距离*/
	UPROPERTY(EditDefaultsOnly,Category="Summoning")
	float MinSpawnDistance = 50.f;

	/*召唤最远距离*/
	UPROPERTY(EditDefaultsOnly,Category="Summoning")
	float MaxSpawnDistance = 200.f;

	/*召唤敌人扇形区域角度*/
	UPROPERTY(EditDefaultsOnly,Category="Summoning")
	float SpawnSpread = 90.f;

	UFUNCTION(BlueprintPure, Category="Summoning")
	TSubclassOf<APawn> GetRandomMinionsClass();
};
