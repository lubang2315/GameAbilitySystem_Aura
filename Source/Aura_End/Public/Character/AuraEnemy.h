// 由来时路褒贬不一制作

#pragma once

#include "CoreMinimal.h"
#include "Character/AuraCharacterBase.h"
#include "AuraEnemy.generated.h"

/**
 * 
 */
UCLASS()
class AURA_END_API AAuraEnemy : public AAuraCharacterBase
{
	GENERATED_BODY()


public:
	virtual void HighLightActor();
	virtual void UnHighLightActor();
};
