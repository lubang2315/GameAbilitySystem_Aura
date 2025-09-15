// 由来时路褒贬不一制作

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "AttributeSet.h"
#include "GameplayEffect.h"
#include "GameFramework/Character.h"
#include "Interface/CombotInterface.h"
#include "AuraCharacterBase.generated.h"

class AbilitySystemComponent;
class AttributeSet;

UCLASS(Abstract)
class AURA_END_API AAuraCharacterBase : public ACharacter,public IAbilitySystemInterface,public ICombotInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AAuraCharacterBase();
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	UAttributeSet* GetAttributeSet() const {return AttributeSet;}

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	//创建一个可以附着在骨骼网格体上的类
	UPROPERTY(EditAnywhere,Category="Combat")
	TObjectPtr<USkeletalMeshComponent> Weapon;

	/*法杖火球插槽*/
	UPROPERTY(EditAnywhere,Category="Combat")
	FName WeaponTipSocketName;

	virtual FVector GetCombatSocketLocation() override;
    /*End*/
	
	UPROPERTY()
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY()
	TObjectPtr<UAttributeSet> AttributeSet;

	virtual void InitAbilityActorInfo() ;

	UPROPERTY(BlueprintReadWrite,EditAnywhere,Category="Attributes")
	TSubclassOf<UGameplayEffect> PrimaryAttribute;

	UPROPERTY(BlueprintReadWrite,EditAnywhere,Category="Attributes")
	TSubclassOf<UGameplayEffect> SecondaryAttribute;

	UPROPERTY(BlueprintReadWrite,EditAnywhere,Category="Attributes")
	TSubclassOf<UGameplayEffect> DefaultVitalAttribute;

	virtual void InitializePrimaryAttributes() const;/*利用GE初始化主要属性值*/

	void ApplyEffectToTager(TSubclassOf<UGameplayEffect> GameplayEffectClass,float Lever) const;/*利用GE初始化主要属性值*/

	void  AddCharacterAbilities() const;



	

private:
	UPROPERTY(EditAnywhere,Category="Attributes")
	TArray<TSubclassOf<UGameplayAbility>> StartupAbility;
};
