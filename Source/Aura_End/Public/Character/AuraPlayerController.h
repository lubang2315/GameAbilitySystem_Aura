// 由来时路褒贬不一制作

#pragma once

#include "CoreMinimal.h"
#include "InputAction.h"
#include "GameFramework/PlayerController.h"
#include "AuraPlayerController.generated.h"

/**
 * 
 */
class UInputMappingContext;

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
	
private:

	UPROPERTY(EditAnywhere,Category="Input")
	TObjectPtr<UInputMappingContext> AuraContext;
	
	UPROPERTY(EditAnywhere,Category="Input")
    TObjectPtr<UInputAction> MoveAction;

	void Move(const struct FInputActionValue& InputActionValue);
	
};
