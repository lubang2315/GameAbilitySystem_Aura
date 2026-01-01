// 由来时路褒贬不一制作


#include "Actor/Auraprojectile.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Aura_End/Aura_End.h"
#include "Components/AudioComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Gas/FunctionLibrary/MyFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AAuraprojectile::AAuraprojectile()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;
    /*设置球体碰撞响应*/
	Sphere = CreateDefaultSubobject<USphereComponent>("Sphere");
	SetRootComponent(Sphere);
	Sphere->SetCollisionObjectType(ECC_Projectile);
	Sphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	Sphere->SetCollisionResponseToChannels(ECR_Ignore);
	Sphere->SetCollisionResponseToChannel(ECC_WorldDynamic,ECR_Overlap);
	Sphere->SetCollisionResponseToChannel(ECC_WorldStatic,ECR_Overlap);
	Sphere->SetCollisionResponseToChannel(ECC_Pawn,ECR_Overlap);

	/*设置发射组件*/
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>("ProjectileMovement");
	ProjectileMovement->InitialSpeed = 550.f;
	ProjectileMovement->MaxSpeed = 550.f;
	ProjectileMovement->ProjectileGravityScale = 0.f;
}


void AAuraprojectile::BeginPlay()
{
	Super::BeginPlay();
	SetLifeSpan(LifeSpan);
	SetReplicateMovement(true);
	Sphere->OnComponentBeginOverlap.AddDynamic(this,&AAuraprojectile::OnSphereOverlap);
	LoopSoundComponent = UGameplayStatics::SpawnSoundAttached(ImpactSound2,GetRootComponent());
}

void AAuraprojectile::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!DamageEffectParams.SourceAbilitySystemComponent) return;
	AActor* SourceActor = DamageEffectParams.SourceAbilitySystemComponent->GetAvatarActor();
	if (!SourceActor) return;
	/*判断火球是不是击中自己*/
	if (SourceActor == OtherActor) return;

	/*判断伤害的是不是友军*/
	if (!UMyFunctionLibrary::ISNotFriend(SourceActor,OtherActor)) return;
	
	if (!bHit)
	{
			PlayImpact();
			bHit = true;
	}

	if(HasAuthority())
	{
		if (UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(OtherActor))
		{
			/*获取致命一击的方向和力度*/
			const FVector Deathimpulse = GetActorForwardVector() * DamageEffectParams.DeathImpulseMagnitude;
			DamageEffectParams.DeathImpulse = Deathimpulse;

			/*获取在成工触发下的击飞的方向和力度*/
			if (const bool bKnockBack = FMath::RandRange(1,100) < DamageEffectParams.KnockBackChance)
			{
				/*将击飞的方向向上偏转，实现向上向前的抛物线击飞效果*/
				const FVector KnockBackDirection = GetActorForwardVector().RotateAngleAxis(-45.f,GetActorRightVector());
				DamageEffectParams.KnockBackForce = KnockBackDirection * DamageEffectParams.KnockBackMagnitude;
			}
			
			DamageEffectParams.TargetAbilitySystemComponent = TargetASC;
			UMyFunctionLibrary::ApplyGameplayEffect(DamageEffectParams);
		}
		Destroy();
	}
	else
	{
		bHit = true;
	}
}

void AAuraprojectile::Destroyed()
{
	if(!bHit && !HasAuthority())
	{
		PlayImpact();
		bHit = true;
	}
	Super::Destroyed();
}

void AAuraprojectile::PlayImpact() const
{
	UGameplayStatics::PlaySoundAtLocation(this,ImpactSound,GetActorLocation(),FRotator::ZeroRotator);
	UNiagaraFunctionLibrary::SpawnSystemAtLocation(this,ImpactEffect,GetActorLocation());
	if (LoopSoundComponent) LoopSoundComponent->Stop();
}





