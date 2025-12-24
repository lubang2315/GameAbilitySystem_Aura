// 由来时路褒贬不一制作

#pragma once

#include "CoreMinimal.h"
#include "Gas/Ability/AuraProjectileSpell.h"
#include "AuraFireBolt.generated.h"

/**
 * 
 */
UCLASS()
class AURA_END_API UAuraFireBolt : public UAuraProjectileSpell
{
	GENERATED_BODY()
public:
	/*关于火球技能在技能面板的描述*/
	virtual FString GetDescription(int32 Level) override;
	virtual FString GetNextLevelDescription(int32 Level) override;

	/*多段火球技能，可以根据等级生成不同数量火球并以特定角度*/
	UFUNCTION(BlueprintCallable, Category = "Projectile")
	void AuraSpawnProjectile(const FVector& ProjectileTargetLocation,const FGameplayTag& SocketTag,const bool bOverriderPitch,const float PitchOverrider,AActor* HomingTag = nullptr);

protected:
	/*生成火球的区域范围*/
	UPROPERTY(EditDefaultsOnly, Category = "FireBolt")
	float ProjectileSpread = 90.f;
	
	/*在技能面板显示最多能发射火球数量*/
	UPROPERTY(EditDefaultsOnly, Category = "FireBolt")
	int32 MaxProjectile = 5;

	/*移动朝向目标最小加速度*/
	UPROPERTY(EditDefaultsOnly, Category = "FireBolt")
	float HomingAccelerationMin = 1600.f;

	/*移动朝向目标最大加速度*/
	UPROPERTY(EditDefaultsOnly, Category = "FireBolt")
	float HomingAccelectionMax = 3200.f;

	/*是否需要朝向目标飞行*/
	UPROPERTY(EditDefaultsOnly, Category = "FireBolt")
	bool bLunchHomingProjectiles = true;
};
