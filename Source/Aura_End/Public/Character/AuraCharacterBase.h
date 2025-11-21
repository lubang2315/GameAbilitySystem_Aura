// 由来时路褒贬不一制作

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "AttributeSet.h"
#include "GameplayEffect.h"
#include "GameFramework/Character.h"
#include "Interface/CombotInterface.h"
#include "AuraCharacterBase.generated.h"

class UNiagaraSystem;
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

	/*返回受击特效*/
	virtual UNiagaraSystem* GetBloodEffect_Implementation() override;

	/*在Cue中运用，主要用来通过标签获取对应结构体*/
	virtual FTaggedMontage GetTaggedMontageByTag_Implementation(const FGameplayTag& MontageTag) override;

	/*返回随从数量*/
	virtual int32 GetMinionCount_Implementation() override;

	/*设置随从数量*/
	virtual void SetMinionCount_Implementation(int32 Amount) override;
	
	/*End*/

	/*设置人物类型*/
	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="Character Class Default");
	ECharacterClass CharacterClass = ECharacterClass::Warrior;

	/*获取人物类型*/
	virtual ECharacterClass GetCharacterClass_Implementation() override;
	
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

	UPROPERTY(EditAnywhere,Category="Combat")
	FName TailSocketName;
	
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

	/*受击特效*/
	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	UNiagaraSystem* BloodSystem;

	/*Death sound*/
	UPROPERTY(BlueprintReadWrite,EditAnywhere,Category="Combat")
	USoundBase* DeathSound;

	/*召唤随从数量*/
	int32 MinionCount = 0;
	
private:
	/*AuraGA*/
	UPROPERTY(EditAnywhere,Category="Attributes")
	TArray<TSubclassOf<UGameplayAbility>> StartupAbility;

	/*被动技能：XP*/
	UPROPERTY(EditAnywhere,Category="Attributes")
	TArray<TSubclassOf<UGameplayAbility>> StartupPassiveAbility;
	
};
