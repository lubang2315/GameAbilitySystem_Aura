// 由来时路褒贬不一制作

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "AttributeSet.h"
#include "GameplayEffect.h"
#include "GameFramework/Character.h"
#include "Gas/Passive/PassiveNiagaraComponent.h"
#include "Interface/CombotInterface.h"
#include "AuraCharacterBase.generated.h"

class UDebuffNiagaraComponent;
class UNiagaraSystem;
class AbilitySystemComponent;
class AttributeSet;

UCLASS(Abstract)
class AURA_END_API AAuraCharacterBase : public ACharacter,public IAbilitySystemInterface,public ICombotInterface
{
	GENERATED_BODY()

public:
	virtual void Tick(float DeltaSeconds) override;

	/**设置服务器属性值复制到客户端，用于预测系统，我们已经把过去值和限值提交给能力系统管理，当服务器收到改变值会验证有效性并同步其他客户端*/
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	AAuraCharacterBase();
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	UAttributeSet* GetAttributeSet() const {return AttributeSet;}

	UFUNCTION(NetMulticast,Reliable)
	virtual void MultCastHandleDeath(const FVector& DeathImpulse);

	/*IcombotInterface*/
	
	virtual UAnimMontage* GetAnimMontage_Implementation() override;
	virtual FVector GetCombatSocketLocation_Implementation(const FGameplayTag& MontageTag)const override;
	virtual bool IsDead_Implementation() override;
	virtual AActor* GetAvatar_Implementation() override;
	virtual void Die(const FVector& DeathImpulse) override;
	virtual USkeletalMeshComponent* GetWeapon_Implementation() override;
	virtual bool GetIsBeingShocked_Implementation() const override;
	virtual void SetIsBeingShocked_Implementation(bool isBeingShocked) override;
	
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

	/*ASC注册成功委托*/
	virtual FOnASCRegistered& GetASCRegistered() override;

	/*Actor死亡委托*/
	virtual FOnDeath& GetDeath() override;
	
	/*End*/

	/*ASC注册成功委托*/
	FOnASCRegistered OnASCRegistered;
	/*Actor死亡委托*/
	FOnDeath OnDeath;
	
	/*设置人物类型*/
	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="Character Class Default");
	ECharacterClass CharacterClass = ECharacterClass::Warrior;

	/*获取人物类型*/
	virtual ECharacterClass GetCharacterClass_Implementation() override;
	
protected:
	virtual void BeginPlay() override;

	bool bDead = false;

	/*是否处于眩晕Debuff效果*/
	UPROPERTY(ReplicatedUsing=OnRep_Stunned,BlueprintReadOnly)
	bool bIsStunned = false;

	/*是否处于燃烧Debuff效果*/
	UPROPERTY(ReplicatedUsing=OnRep_Burn,BlueprintReadOnly)
	bool bIsBurn = false;

	/*敌人受到眩晕Debuff用来同步客户端阻止移动标签*/
	UFUNCTION()
	virtual void OnRep_Stunned();

	/*敌人受到燃烧Debuff用来同步客户端Niagara*/
	UFUNCTION()
	virtual void OnRep_Burn();

	/*是否处于持续受击状态*/
	UPROPERTY(Replicated,BlueprintReadOnly)
	bool bIsBeingShocked = false;
	
	/*眩晕标签变化后的回调*/
	virtual void StunTagChange(const FGameplayTag CallBackTag,int32 NewCount);

	/*角色最大移动速度*/
	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="Combat")
	float BaseWalkSpeed = 600.f;

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

	/*设置燃烧Debuff的Niagara特效*/
	UPROPERTY(visibleAnywhere)
	TObjectPtr<UDebuffNiagaraComponent> BurnDebuffComponent;

	/*设置眩晕Debuff的Niagara特效*/
	UPROPERTY(visibleAnywhere)
	TObjectPtr<UDebuffNiagaraComponent> StunDebuffComponent;

	/*被动技能特效相关*/
	/*光环守护*/
	UPROPERTY(visibleAnywhere)
	TObjectPtr<UPassiveNiagaraComponent> HaloOfProtection;

	/*生命恢复*/
	UPROPERTY(visibleAnywhere)
	TObjectPtr<UPassiveNiagaraComponent> Lifesiphon;

	/*蓝量恢复*/
	UPROPERTY(visibleAnywhere)
	TObjectPtr<UPassiveNiagaraComponent> ManaSiphon;

	/*场景组件，这里主要是让被动技能特效附着在场景组件，而场景组件可以在附着在根骨骼情况下不随之发生旋转*/
	UPROPERTY(visibleAnywhere)
	TObjectPtr<USceneComponent> EffectAttachComponent;
	/*End*/

private:
	/*AuraGA*/
	UPROPERTY(EditAnywhere,Category="Attributes")
	TArray<TSubclassOf<UGameplayAbility>> StartupAbility;

	/*被动技能：XP*/
	UPROPERTY(EditAnywhere,Category="Attributes")
	TArray<TSubclassOf<UGameplayAbility>> StartupPassiveAbility;
	
};
