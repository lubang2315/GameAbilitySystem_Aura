// 由来时路褒贬不一制作

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "NiagaraComponent.h"
#include "DebuffNiagaraComponent.generated.h"


/**
 * 
 */
UCLASS()
class AURA_END_API UDebuffNiagaraComponent : public UNiagaraComponent
{
	GENERATED_BODY()
public:
	UDebuffNiagaraComponent();

	/*标识粒子类型的标签*/
	UPROPERTY(VisibleAnywhere)
	FGameplayTag DebuffTag;
protected:
	virtual void BeginPlay() override;

	/*负面标签变动的回调*/
	void DebuffTagChanged(const FGameplayTag CallbackTag,int32 Newcount);

	/*死亡销毁*/
	UFUNCTION()
	void OnOwnerDeath(AActor* DeathActor);
};
