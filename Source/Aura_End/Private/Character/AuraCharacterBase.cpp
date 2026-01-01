// 由来时路褒贬不一制作


#include "Character/AuraCharacterBase.h"

#include "AbilitySystemComponent.h"
#include "Aura_End/Aura_End.h"
#include "Components/CapsuleComponent.h"
#include "Gas/Debuff/DebuffNiagaraComponent.h"
#include "Gas/Player/AbilitySystemComponent/AuraAbilitySystemComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Tags/AuraGameplayTags.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Net/UnrealNetwork.h"


void AAuraCharacterBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AAuraCharacterBase,bIsStunned);
	DOREPLIFETIME(AAuraCharacterBase,bIsBurn);
	DOREPLIFETIME(AAuraCharacterBase,bIsBeingShocked);
}

// Sets default values
AAuraCharacterBase::AAuraCharacterBase()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	/*设置燃烧Debuff的Niagara附着在跟骨骼以及标签*/
	BurnDebuffComponent = CreateDefaultSubobject<UDebuffNiagaraComponent>("DebuffNiagara");
	BurnDebuffComponent->SetupAttachment(GetRootComponent());
	BurnDebuffComponent->DebuffTag = FMyGameplayTags::Get().Debuff_Burn;

	/*设置眩晕Debuff的Niagara附着在跟骨骼以及标签*/
	StunDebuffComponent = CreateDefaultSubobject<UDebuffNiagaraComponent>("StunDebuffComponent");
	//StunDebuffComponent->SetupAttachment(GetMesh(),FName("Hand_02"));
	StunDebuffComponent->SetupAttachment(GetRootComponent());
	StunDebuffComponent->DebuffTag = FMyGameplayTags::Get().Debuff_Stun;

	//1.创建一个骨骼网格体实例
	//2.实例的插槽名称为WeaponHandSocket通过GetMesh（）附着在人物网格体下面
	//3.设置碰撞为没有碰撞
	Weapon = CreateDefaultSubobject<USkeletalMeshComponent>("Weapon");
	Weapon->SetupAttachment(GetMesh(),FName("WeaponHandSocket"));
	Weapon->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Camera,ECR_Ignore);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Projectile,ECR_Overlap);
	GetCapsuleComponent()->SetGenerateOverlapEvents(true);
	//GetMesh()->SetCollisionResponseToChannel(ECC_Camera,ECR_Ignore);
	//GetMesh()->SetCollisionResponseToChannel(ECC_Projectile,ECR_Overlap);
	//GetMesh()->SetGenerateOverlapEvents(true);
}

UAbilitySystemComponent* AAuraCharacterBase::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

UAnimMontage* AAuraCharacterBase::GetAnimMontage_Implementation()
{
	return HitReactMontage;
}

void AAuraCharacterBase::Die(const FVector& DeathImpulse)
{
	/*敌人死亡设置武器脱手*/
	Weapon->DetachFromComponent(FDetachmentTransformRules(EDetachmentRule::KeepWorld, true));
	MultCastHandleDeath(DeathImpulse);
}

USkeletalMeshComponent* AAuraCharacterBase::GetWeapon_Implementation()
{
	return Weapon;
}

bool AAuraCharacterBase::GetIsBeingShocked_Implementation() const
{
	return bIsBeingShocked;
}

void AAuraCharacterBase::SetIsBeingShocked_Implementation(bool isBeingShocked)
{
	bIsBeingShocked = isBeingShocked;
}

void AAuraCharacterBase::MultCastHandleDeath_Implementation(const FVector& DeathImpulse)
{
	/*play Death Sound*/
	UGameplayStatics::SpawnSoundAtLocation(this,DeathSound,GetActorLocation(),GetActorRotation());
	
	/*开启武器物理效果*/
	Weapon->SetSimulatePhysics(true);
	Weapon->SetEnableGravity(true);
	Weapon->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
	Weapon->AddImpulse(DeathImpulse * 0.1f,NAME_None,true);

	/*开启角色物理效果*/
	GetMesh()->SetSimulatePhysics(true);
	GetMesh()->SetEnableGravity(true);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
	GetMesh()->SetCollisionResponseToChannel(ECC_WorldStatic,ECR_Block);
	GetMesh()->AddImpulse(DeathImpulse,NAME_None,true);

	/*关闭角色碰撞体碰撞通道，避免死亡尸体影响*/
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	/*调用死亡溶解*/
	Dissolve();
	
	bDead = true;

	/*玩家死亡停止燃烧和眩晕Debuff*/
	BurnDebuffComponent->Deactivate();
	StunDebuffComponent->Deactivate();

	/*广播死亡事件*/
	OnDeath.Broadcast(this);
}

TArray<FTaggedMontage> AAuraCharacterBase::GetTaggedMontages_Implementation()
{
	return AttackMontage;
}

UNiagaraSystem* AAuraCharacterBase::GetBloodEffect_Implementation()
{
	return BloodSystem;
}

FTaggedMontage AAuraCharacterBase::GetTaggedMontageByTag_Implementation(const FGameplayTag& MontageTag)
{
	for (FTaggedMontage TagMontage : AttackMontage)
	{
		if (TagMontage.MontageTag.MatchesTagExact(MontageTag))
		{
			return TagMontage;
		}
	}
	return FTaggedMontage();
}

int32 AAuraCharacterBase::GetMinionCount_Implementation()
{
	return MinionCount;
}

void AAuraCharacterBase::SetMinionCount_Implementation(int32 Amount)
{
	MinionCount += Amount;
}

FOnASCRegistered& AAuraCharacterBase::GetASCRegistered()
{
	return OnASCRegistered;
}

FOnDeath& AAuraCharacterBase::GetDeath()
{
	return OnDeath;
}

ECharacterClass AAuraCharacterBase::GetCharacterClass_Implementation()
{
	return CharacterClass;
}

// Called when the game starts or when spawned
void AAuraCharacterBase::BeginPlay()
{
	Super::BeginPlay();
	
}

void AAuraCharacterBase::OnRep_Stunned()
{
}

void AAuraCharacterBase::OnRep_Burn()
{
}

void AAuraCharacterBase::StunTagChange(const FGameplayTag CallBackTag, int32 NewCount)
{
	bIsStunned = NewCount > 0;
	GetCharacterMovement()->MaxWalkSpeed = bIsStunned ? 0 : BaseWalkSpeed;
}

FVector AAuraCharacterBase::GetCombatSocketLocation_Implementation(const FGameplayTag& MontageTag) const
{
	const FMyGameplayTags& GameplayTags = FMyGameplayTags::Get();
	if (MontageTag.MatchesTagExact(GameplayTags.CombatSocket_Weapon) && IsValid(Weapon))
	{
		return Weapon->GetSocketLocation(WeaponTipSocketName);
	}
	if (MontageTag.MatchesTagExact(GameplayTags.CombatSocket_LeftHand))
	{
		return GetMesh()->GetSocketLocation(LeftHandSocketName);
	}
	if (MontageTag.MatchesTagExact(GameplayTags.CombatSocket_RightHand))
	{
		return GetMesh()->GetSocketLocation(RightHandSocketName);
	}
	if (MontageTag.MatchesTagExact(GameplayTags.CombatSocket_Tail))
	{
		return GetMesh()->GetSocketLocation(TailSocketName);
	}
	return FVector();
} 

bool AAuraCharacterBase::IsDead_Implementation()
{
	return bDead;
}

AActor* AAuraCharacterBase::GetAvatar_Implementation()
{
	return this;
}

void AAuraCharacterBase::InitAbilityActorInfo()
{
}

void AAuraCharacterBase::InitializePrimaryAttributes() const
{
	ApplyEffectToTager(PrimaryAttribute,1.f);
	ApplyEffectToTager(SecondaryAttribute,1.f);
	ApplyEffectToTager(DefaultVitalAttribute,1.f);
}

void AAuraCharacterBase::ApplyEffectToTager(TSubclassOf<UGameplayEffect> GameplayEffectClass,float Lever) const
{
	/*人物主要属性的值，采用GE模式赋值，以下是初始化主要属性值*/
 	check(AbilitySystemComponent);
 	check(PrimaryAttribute);
 	/*给人物应用GE*/
 	FGameplayEffectContextHandle ContextHandle = GetAbilitySystemComponent()->MakeEffectContext();
	ContextHandle.AddSourceObject(this);/*设置源对象是自己，以便在后续使用GE时可以直接从GE中获取作用目标*/
 	FGameplayEffectSpecHandle EffectSpec = GetAbilitySystemComponent()->MakeOutgoingSpec(GameplayEffectClass,Lever,ContextHandle);
 	GetAbilitySystemComponent()->ApplyGameplayEffectSpecToTarget(*EffectSpec.Data.Get(),GetAbilitySystemComponent());
}

void AAuraCharacterBase::AddCharacterAbilities() const
{
	UAuraAbilitySystemComponent* AuraASC = CastChecked<UAuraAbilitySystemComponent>(GetAbilitySystemComponent());

	if (!HasAuthority()) return;

	AuraASC->AddCharacterAbilities(StartupAbility);
	AuraASC->AddCharacterPassiveAbilities(StartupPassiveAbility);
}

void AAuraCharacterBase::Dissolve()
{
	if (IsValid(DissolveMaterialInstance))
	{
		UMaterialInstanceDynamic* DynamicMaterial = UMaterialInstanceDynamic::Create(DissolveMaterialInstance,this);
		GetMesh()->SetMaterial(0, DynamicMaterial);
	    StartDissolveTimeline(DynamicMaterial);
	}
	if (IsValid(DissolveWeaponMaterialInstance))
	{
		UMaterialInstanceDynamic* DynamicWeaponMaterial = UMaterialInstanceDynamic::Create(DissolveWeaponMaterialInstance,this);
		Weapon->SetMaterial(0,DynamicWeaponMaterial);
		WeaponStartDissolveTimeline(DynamicWeaponMaterial);
	}
	
}





