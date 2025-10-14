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
	AAuraCharacterBase();
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	UAttributeSet* GetAttributeSet() const {return AttributeSet;}

	UFUNCTION(NetMulticast,Reliable)
	virtual void MultCastHandleDeath();

	/*IcombotInterface*/
	
	virtual UAnimMontage* GetAnimMontage_Implementation() override;
	virtual FVector GetCombatSocketLocation_Implementation(const FGameplayTag& MontageTag)const override;
	virtual bool IsDead_Implementation() override;
	virtual AActor* GetAvatar_Implementation() override;
	virtual void Die() override;
	
	/*创建一个容器，方便敌人近战模版的泛用性，因为有拿武器和不拿武器的小兵，并且有左右手切换攻击的类型，保证泛用性，这里创建容器把标签和蒙太奇关联*/
	UPROPERTY(EditAnywhere,Category="Combat")
	TArray<FTaggedMontage> AttackMontage;
	virtual TArray<FTaggedMontage> GetTaggedMontages_Implementation() override;

	/*End*/

	
	
protected:
	virtual void BeginPlay() override;

	bool bDead = false;

	//创建一个可以附着在骨骼网格体上的类
	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="Combat")
	TObjectPtr<USkeletalMeshComponent> Weapon;

	/*武器插槽*/
	UPROPERTY(EditAnywhere,Category="Combat")
	FName WeaponTipSocketName;

	UPROPERTY(EditAnywhere,Category="Combat")
	FName LeftHandSocketName;

	UPROPERTY(EditAnywhere,Category="Combat")
	FName RightHandSocketName;
	
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

	UPROPERTY(EditAnywhere,Category="Combot")
	TObjectPtr<UAnimMontage> HitReactMontage;

	/*溶解效果*/

	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	TObjectPtr<UMaterialInstance> DissolveMaterialInstance;

	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	TObjectPtr<UMaterialInstance> DissolveWeaponMaterialInstance;

	void Dissolve();

	UFUNCTION(BlueprintImplementableEvent)
	void StartDissolveTimeline(const UMaterialInstanceDynamic* DynamicMaterialInstance);

	UFUNCTION(BlueprintImplementableEvent)
	void WeaponStartDissolveTimeline(const UMaterialInstanceDynamic* DynamicMaterialInstance);
	/*End*/

private:
	UPROPERTY(EditAnywhere,Category="Attributes")
	TArray<TSubclassOf<UGameplayAbility>> StartupAbility;
};
