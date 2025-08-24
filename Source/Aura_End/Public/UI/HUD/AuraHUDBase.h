// 由来时路褒贬不一制作

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "GameFramework/HUD.h"
#include "UI/Widget/AuraUserWidget.h"
#include "UI/WidgetController/AttributeMenuWidgetController.h"
#include "UI/WidgetController/OverlayWidgetController.h"
#include "AuraHUDBase.generated.h"

class UOverlayWidgetController;
class UAuraUserWidget;
struct FWidgetControllerParams;

/**
 * 
 */
UCLASS()
class AURA_END_API AAuraHUDBase : public AHUD
{
	GENERATED_BODY()

public:
	UPROPERTY()
	TObjectPtr<UAuraUserWidget> OverlayWidget;

	/**在HUD侧获取AurawidgetController*/
	UOverlayWidgetController* GetOverlayWidgetController(const FWidgetControllerParams& WCParams);

	UAttributeMenuWidgetController* GetAttributeMenuWidgetController(const FWidgetControllerParams& WCParams);

	/**初始化覆盖，把AuraWidgetController传进来的值传给AuraUserWidget,并把HUD添加到主屏幕*/
	void InitOverlay(APlayerController* PC,APlayerState* PS,UAbilitySystemComponent* ASC,UAttributeSet* AS);

protected:

private:
	UPROPERTY(EditAnywhere)
	TSubclassOf<UAuraUserWidget> OverlayWidgetClass;

	UPROPERTY()
	TObjectPtr<UOverlayWidgetController> OverlayWidgetController;
	
	UPROPERTY(EditAnywhere)
	TSubclassOf<UOverlayWidgetController> OverlayWidgetControllerClass;

	UPROPERTY()
	TObjectPtr<UAttributeMenuWidgetController> MenuWidgetController;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UAttributeMenuWidgetController> MenuWidgetControllerClass;
};
