// 由来时路褒贬不一制作

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Components/SplineComponent.h"
#include "GameFramework/PlayerController.h"
#include "Input/AuraInputConfig.h"
#include "AuraPlayerController.generated.h"


class AMagicCircle;
class UNiagaraSystem;
class UDamageTextComponent;
class UAuraAbilitySystemComponent;
class UInputAction;
class UInputMappingContext;
class IEnemyInterface;
struct FInputActionValue;
/**
 * 
 */


UCLASS()
class AURA_END_API AAuraPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	//设置人物控制复制到服务器
	AAuraPlayerController();

	/*在控制器端显示损伤敌人数值*/
	UFUNCTION(Client, Reliable)
	void ShowDamageNumber(float DamageAmount,ACharacter* TargerCharacter,bool IsBlockHit,bool IsCriticalHit);

	/*显示魔法圈*/
	UFUNCTION(BlueprintCallable)
	void ShowMagicCircle(UMaterialInterface* DeaclMaterial);

	/*隐藏魔法圈*/
	UFUNCTION(BlueprintCallable)
	void HideMagicCircle() const;
	
protected:
	
    virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;
	virtual  void PlayerTick(float DeltaTime) override;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UDamageTextComponent> DamageTextComponentClass;
	
private:

	UPROPERTY(EditAnywhere,Category="Input")
	TObjectPtr<UInputMappingContext> AuraContext;
	
	UPROPERTY(EditAnywhere,Category="Input")
    TObjectPtr<UInputAction> MoveAction;
    /*Shift Activate FireBolt*/
	UPROPERTY(EditAnywhere,Category="Input")
	TObjectPtr<UInputAction> ShiftAction;

	void ShiftPressed() {bShiftKeyDown = true;};
	void ShiftReleased() {bShiftKeyDown = false;};
	bool bShiftKeyDown = false;

	void Move(const struct FInputActionValue& InputActionValue);

	void CursorTrace();
	TScriptInterface<IEnemyInterface> LastActor;
	TScriptInterface<IEnemyInterface> ThisActor;
	
	/**回调函数，根据状态触发GA *ActivateGA**MouseMove*/
	void AbilityInputTagPressed(FGameplayTag InputTag);
	void AbilityInputTagReleased(FGameplayTag InputTag);
	void AbilityInputTagHold(FGameplayTag InputTag);

	UPROPERTY(EditAnywhere, Category="Input")
	TObjectPtr<UAuraInputConfig> AuraInputConfig;

	/*获取ASC，然后用来调用里面的激活GA函数*/
	UPROPERTY()
	TObjectPtr<UAuraAbilitySystemComponent> AuraASC;

	UAuraAbilitySystemComponent* GetASC();
	

	/**鼠标点击移动 *MouseMove*/
	FVector CachedDestination = FVector::ZeroVector;
	float FollowTime = 0.f;
	bool bAutoRunning = false;
	bool bTargeting = false;

	UPROPERTY(EditDefaultsOnly)
	float shortpressThreshold = 0.3f;

	UPROPERTY(EditDefaultsOnly)
	float AutoRunAcceptanceRadius = 50.f;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<USplineComponent> Spline;

	void AutoMove();

	FHitResult HitResult;
	/*End*/

	/*鼠标点击移动特效*/
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UNiagaraSystem> ClickNiagaraComponent;

	/*设置魔法圈的类型*/
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AMagicCircle> MagicCircleClass;

	/*存储魔法圈实例*/
	UPROPERTY()
	TObjectPtr<AMagicCircle> MagicCircle;

	/*每帧更新魔法圈位置*/
	void UpdataMagicCircleLocation() const;
	
};
