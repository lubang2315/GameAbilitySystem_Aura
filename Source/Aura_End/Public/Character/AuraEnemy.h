// 由来时路褒贬不一制作

#pragma once

#include "CoreMinimal.h"
#include "AI/AuraAIController.h"
#include "Character/AuraCharacterBase.h"
#include "Interface/EnemyInterface.h"
#include "Interface/HighLightInterface.h"
#include "UI/WidgetController/OverlayWidgetController.h"
#include "AuraEnemy.generated.h"

class UWidgetComponent;
/**
 * 
 */
UCLASS()
class AURA_END_API AAuraEnemy : public AAuraCharacterBase,public IEnemyInterface,public IHighLightInterface
{
	GENERATED_BODY()


public:
	//人物描边需要设置视觉碰撞阻挡，所以通过代码把碰撞设为阻挡
	AAuraEnemy();

	virtual void BeginPlay() override;
	virtual void PossessedBy(AController* NewController) override;

	/*HighLightInterface*/
	/**敌人描边显示接口，来自于玩家控制器的光标事件*/
	virtual void HighLightActor_Implementation() override;
	virtual void UnHighLightActor_Implementation() override;
	virtual void SetMoveToLocation_Implementation(FVector& OutDestination) override;
	/*End HighLightInterface*/
	virtual void InitAbilityActorInfo() override;

	virtual void InitializePrimaryAttributes() const override;

	virtual void Die(const FVector& DeathImpulse) override;

	/*敌人死亡后触发此函数，在蓝图测执行掉落战利品*/
	UFUNCTION(BlueprintImplementableEvent)
	void SpawnLoot();

	/*传递Enemy攻击目标*/
	UPROPERTY(BlueprintReadWrite,Category="Combat")
	TObjectPtr<AActor> CombotActor;
	
	virtual void SetCombotTarget_Implementation(AActor* CombotTarget) override;
	virtual AActor* GetCombotTarget_Implementation() override;

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Combat")
	float LifeSpan = 5.f;

	UPROPERTY(BlueprintReadOnly,Category = "Combat")
	bool bHitReacting = false;
	
    void HitReactTagChanged(const FGameplayTag CallBackTag,int32 NewCount);

	/*设置玩家等级*/
	void SetEnemyLevel(int32 NewLevel) {Level = NewLevel;};
	
	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="Character Class Default");
	int32 Level = 1;
    /*设置敌人属性*/
	virtual int32 GetPlayerLevel_Implementation() override;

	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="Widget")
	TObjectPtr<UWidgetComponent> HPWidget;

	UPROPERTY(BlueprintAssignable,Category="GAS|AttributSet")
	FOnAttributeChangedSignature OnEnemyHPChangedEvent;

	UPROPERTY(BlueprintAssignable,Category="GAS|AttributSet")
	FOnAttributeChangedSignature OnEnemyMaxHPChangedEvent;

protected:
	UPROPERTY(EditAnywhere,Category="AI")
	TObjectPtr<UBehaviorTree> BehaviorTree;

	UPROPERTY()
	TObjectPtr<AAuraAIController> AuraAIController;

	/*眩晕回调*/
	virtual void StunTagChange(const FGameplayTag CallBackTag, int32 NewCount) override;
};
