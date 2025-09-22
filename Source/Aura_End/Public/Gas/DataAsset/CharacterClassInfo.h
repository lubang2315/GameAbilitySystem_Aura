// 由来时路褒贬不一制作

#pragma once

#include "CoreMinimal.h"
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

	UPROPERTY(EditDefaultsOnly,Category = "Commit Class Defaults")
	TSubclassOf<UGameplayEffect> VitalAttributes;

	UPROPERTY(EditDefaultsOnly,Category = "Commit Class Defaults")
	TSubclassOf<UGameplayEffect> SecondaryAttributes;

	FCharacterClassDefaultInfo GetClassDefaultInfo(ECharacterClass CharacterClass);

	UPROPERTY(EditDefaultsOnly,Category = "Commit Class Defaults")
	TArray<TSubclassOf<UGameplayAbility>> CommitAbilities;

	UPROPERTY(EditDefaultsOnly,Category = "Commit Class Defaults|Damage")
	TObjectPtr<UCurveTable> DamageCulationCoefficicients;
};
