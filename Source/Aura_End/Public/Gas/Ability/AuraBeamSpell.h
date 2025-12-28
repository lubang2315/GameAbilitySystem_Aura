// 由来时路褒贬不一制作

#pragma once

#include "CoreMinimal.h"
#include "Gas/Ability/AuraDamageGameplayAbility.h"
#include "AuraBeamSpell.generated.h"

/**
 * 
 */
UCLASS()
class AURA_END_API UAuraBeamSpell : public UAuraDamageGameplayAbility
{
	GENERATED_BODY()

public:

	/*将鼠标拾取命中信息存储 @Params HitResult 在技能中通过TargetDataUnderMouse的task获取到结果*/
	UFUNCTION(BlueprintCallable)
	void StoreMouseDataInfo(const FHitResult& hitResult);

	/*存储所有者变量，从当前技能的执行信息中提取并存储关键的所有者引用*/
	UFUNCTION(BlueprintCallable)
	void StoreOwnerVariables();

	/*雷电光束最先集中的目标*/
	UFUNCTION(BlueprintCallable)
	void TraceFirstTarget(const FVector& BeamTargetLocation);

	/*获取雷电光束击中的敌人的周围的敌人*/
	UFUNCTION(BlueprintCallable)
	void StoreAdditionalTarget(TArray<AActor*>& OutAdditionalTarget);

	/*光束击中主要敌人死亡通知*/
	UFUNCTION(blueprintImplementableEvent)
	void PrimaryTargetDied(AActor* DeadActor);

	/*光束击中次要人物死亡通知*/
	UFUNCTION(blueprintImplementableEvent)
	void AdditionalTargetDied(AActor* DeadActor);

	/*技能结束时取消死亡委托的绑定*/
	UFUNCTION(BlueprintCallable)
	void OnEndAbility(TArray<AActor*> AdditionalTarget);

protected:

	/*鼠标拾取的位置*/
	UPROPERTY(BlueprintReadWrite,Category="Beam")
	FVector MouseHitLocation;

	/*鼠标拾取的对象*/
	UPROPERTY(BlueprintReadWrite,Category="Beam")
	TObjectPtr<AActor> MouseHitActor;

	/*拥有当前技能的玩家控制器*/
	UPROPERTY(BlueprintReadWrite,Category="Beam")
	TObjectPtr<APlayerController> OwnerPlayerController;

	/*获取拥有者，执行技能的玩家角色*/
	UPROPERTY(BlueprintReadWrite,Category="Beam")
	TObjectPtr<ACharacter> OwnerCharacter;

	/*获取光束击中的敌人周围最大数量*/
	UPROPERTY(EditDefaultsOnly,category="Beam")
	int32 MaxNumShockTargets;
private:
};


