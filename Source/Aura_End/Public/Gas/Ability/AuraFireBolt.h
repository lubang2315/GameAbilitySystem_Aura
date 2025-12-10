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


protected:

	/*在技能面板显示最多能发射火球数量*/
	UPROPERTY(EditDefaultsOnly)
	int32 NumProjectile = 5;	
};
