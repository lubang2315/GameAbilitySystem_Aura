// 由来时路褒贬不一制作

#pragma once

#include "CoreMinimal.h"
#include "ScalableFloat.h"
#include "Engine/DataAsset.h"
#include "CharacterClassInfo.generated.h"

class UGameplayEffect;
class UGameplayAbility;

UENUM()
enum class ECharacterClass :uint8
{
	Elementalist,
	Warrior,
	Ranger
};

USTRUCT()
struct FCharacterClassDefaultInfo
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly,Category = "Class Defaults")
	TSubclassOf<UGameplayEffect> PrimaryAttributes;

	/*XP奖励*/
	UPROPERTY(EditDefaultsOnly,Category = "Class Defaults")
	FScalableFloat XPReward = FScalableFloat();
	
	UPROPERTY(EditDefaultsOnly,Category = "Class Defaults")
	TArray<TSubclassOf<UGameplayAbility>> StartupAbilities;

	
};
/**
 * 
 */
UCLASS()
class AURA_END_API UCharacterClassInfo : public UDataAsset
{
	GENERATED_BODY()

public:

	UPROPERTY(EditDefaultsOnly,Category = "Class Defaults")
	TMap<ECharacterClass,FCharacterClassDefaultInfo> CharacterClassInformation;

	/*主要用于加载存档的属性*/
	UPROPERTY(EditDefaultsOnly,Category = "Commit Class Defaults")
	TSubclassOf<UGameplayEffect> PrimaryAttributes_SetByCaller;

	UPROPERTY(EditDefaultsOnly,Category = "Commit Class Defaults")
	TSubclassOf<UGameplayEffect> VitalAttributes;

	UPROPERTY(EditDefaultsOnly,Category = "Commit Class Defaults")
	TSubclassOf<UGameplayEffect> SecondaryAttributes;

	UPROPERTY(EditDefaultsOnly,Category = "Commit Class Defaults")
	TSubclassOf<UGameplayEffect> SecondaryAttributes_Infinite;

	FCharacterClassDefaultInfo GetClassDefaultInfo(ECharacterClass CharacterClass);

	UPROPERTY(EditDefaultsOnly,Category = "Commit Class Defaults")
	TArray<TSubclassOf<UGameplayAbility>> CommitAbilities;

	UPROPERTY(EditDefaultsOnly,Category = "Commit Class Defaults|Damage")
	TObjectPtr<UCurveTable> DamageCulationCoefficicients;
};
