// 由来时路褒贬不一制作

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "CombotInterface.generated.h"

class UAnimMontage;

// This class does not need to be modified.
UINTERFACE(MinimalAPI,BlueprintType)
class UCombotInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class AURA_END_API ICombotInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	virtual int32 GetPlayerLevel();

	virtual FVector GetCombatSocketLocation();

	/*获取敌人位置释放技能时利用运动扭曲组件面向敌人*/
	UFUNCTION(BlueprintCallable,BlueprintImplementableEvent)
	void PassTargetDataVector(const FVector& Vector);

	UFUNCTION(BlueprintNativeEvent,BlueprintCallable)
	UAnimMontage* GetAnimMontage();
	
	virtual void Die() = 0;
	
};
