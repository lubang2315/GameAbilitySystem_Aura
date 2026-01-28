// 由来时路褒贬不一制作

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "EnemyInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UEnemyInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class AURA_END_API IEnemyInterface
{
	GENERATED_BODY()

	
public:


	/*传递Enemy攻击目标*/

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void SetCombotTarget(AActor* CombotTarget);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	AActor* GetCombotTarget();


};
