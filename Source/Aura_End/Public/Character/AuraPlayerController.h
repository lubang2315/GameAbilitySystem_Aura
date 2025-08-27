// 由来时路褒贬不一制作

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "GameFramework/PlayerController.h"
#include "Input/AuraInputConfig.h"
#include "AuraPlayerController.generated.h"


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
	
protected:
	
    virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;
	virtual  void PlayerTick(float DeltaTime) override;
	
private:

	UPROPERTY(EditAnywhere,Category="Input")
	TObjectPtr<UInputMappingContext> AuraContext;
	
	UPROPERTY(EditAnywhere,Category="Input")
    TObjectPtr<UInputAction> MoveAction;

	void Move(const struct FInputActionValue& InputActionValue);

	void CursorTrace();
	TScriptInterface<IEnemyInterface> LastActor;
	TScriptInterface<IEnemyInterface> ThisActor;

	/**回调函数，根据状态触发GA *ActivateGA*/
	void AbilityInputTagPressed(FGameplayTag InputTag);
	void AbilityInputTagReleased(FGameplayTag InputTag);
	void AbilityInputTagHold(FGameplayTag InputTag);

	UPROPERTY(EditAnywhere, Category="Input")
	TObjectPtr<UAuraInputConfig> AuraInputConfig;

	/*获取ASC，然后用来调用里面的激活GA函数*/
	UPROPERTY()
	TObjectPtr<UAuraAbilitySystemComponent> AuraASC;

	UAuraAbilitySystemComponent* GetASC();
	
};
