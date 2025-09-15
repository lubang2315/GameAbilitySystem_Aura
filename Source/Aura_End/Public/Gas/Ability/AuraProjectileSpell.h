// 由来时路褒贬不一制作

#pragma once

#include "CoreMinimal.h"
#include "Gas/Ability/AuraGameplayAbility.h"
#include "AuraProjectileSpell.generated.h"

class AAuraprojectile;
/**
 * 
 */
UCLASS()
class AURA_END_API UAuraProjectileSpell : public UAuraGameplayAbility
{
	GENERATED_BODY()
protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	UFUNCTION(BlueprintCallable, Category = "Projectile")
	void SpawnProjectile(const FVector& ProjectileTargetLocation);
	
	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	TSubclassOf<AAuraprojectile> ProjectileSpellClass;

    UPROPERTY(EditDefaultsOnly,BlueprintReadOnly)
	TSubclassOf<UGameplayEffect> DamageEffectClass ;

	
};
