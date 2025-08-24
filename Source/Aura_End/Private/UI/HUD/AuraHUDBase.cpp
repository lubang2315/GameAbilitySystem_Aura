// 由来时路褒贬不一制作


#include "UI/HUD/AuraHUDBase.h"

UOverlayWidgetController* AAuraHUDBase::GetOverlayWidgetController(const FWidgetControllerParams& WCParams)
{
	if (OverlayWidgetController == nullptr)
	{
		OverlayWidgetController = NewObject<UOverlayWidgetController>(this,OverlayWidgetControllerClass);
		OverlayWidgetController->SetWidgetControllerParams(WCParams);
		/**当属性变化时把值广播给HUD*/
		OverlayWidgetController->BindCallbacksToDependencies();
		
		return OverlayWidgetController;
	}
	return OverlayWidgetController;
}

UAttributeMenuWidgetController* AAuraHUDBase::GetAttributeMenuWidgetController(const FWidgetControllerParams& WCParams)
{
	if (MenuWidgetController == nullptr)
	{
		MenuWidgetController = NewObject<UAttributeMenuWidgetController>(this,MenuWidgetControllerClass);
		MenuWidgetController->SetWidgetControllerParams(WCParams);
		MenuWidgetController->BindCallbacksToDependencies();

		return MenuWidgetController;
	}
	return MenuWidgetController;
}

/**注意此函数是初始化函数，在调用前需要确保各个值已经加载，要确保已经加载需要在人物那里进行激活*/
void AAuraHUDBase::InitOverlay(APlayerController* PC, APlayerState* PS, UAbilitySystemComponent* ASC, UAttributeSet* AS)
{
	checkf(OverlayWidgetClass,TEXT("OverlayWidgetClass 未设置请在HUD上设置"));
	checkf(OverlayWidgetControllerClass,TEXT("Overlay Widget class未设置请在HUD上设置"));

	UAuraUserWidget* Widget = CreateWidget<UAuraUserWidget>(GetWorld(),OverlayWidgetClass);
	OverlayWidget = Cast<UAuraUserWidget>(Widget);

	const FWidgetControllerParams WidgetControllerParams(PC,PS,ASC,AS);
	OverlayWidgetController = GetOverlayWidgetController(WidgetControllerParams);
    /**把值传给AuraUserWidget*/
	OverlayWidget->SetWidgetController(OverlayWidgetController);
	/**广播初始值给HUD*/
	OverlayWidgetController->BroadcastInitialValues();


	Widget->AddToViewport();
}


