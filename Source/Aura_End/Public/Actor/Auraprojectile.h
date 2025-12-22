// 由来时路褒贬不一制作

#pragma once

#include "CoreMinimal.h"
#include "AuraAbilityTypes.h"
#include "GameFramework/Actor.h"
#include "Auraprojectile.generated.h"



class UProjectileMovementComponent;
class USphereComponent;
class UNiagaraSystem;

UCLASS()
class AURA_END_API AAuraprojectile : public AActor
{
	GENERATED_BODY()
	
public:	
	
	AAuraprojectile();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UProjectileMovementComponent> ProjectileMovement;

	UPROPERTY(BlueprintReadWrite,meta=(ExposeOnSpawn = true))
	FDamageEffectPrams DamageEffectParams;
	
protected:
	virtual void BeginPlay() override;



	UFUNCTION()
	void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,UPrimitiveComponent* OtherComp,int32 OtherBodyIndex,bool bFromSweep,const FHitResult& SweepResult);
private:

	bool bHit = false;

	UPROPERTY(EditDefaultsOnly)
	float LifeSpan = 8.f;
	
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USphereComponent> Sphere;
    /*播放FireBolt特效*/
	UPROPERTY(EditAnywhere)
	TObjectPtr<UNiagaraSystem> ImpactEffect;

	UPROPERTY(EditAnywhere)
	TObjectPtr<USoundBase> ImpactSound;

	UPROPERTY(EditAnywhere)
	TObjectPtr<USoundBase> ImpactSound2;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UAudioComponent> LoopSoundComponent;
	
	virtual void Destroyed() override;

	void PlayImpact() const;
    /*End*/
};
