// 由来时路褒贬不一制作
//HUD负责决定谁显示在桌面上，而Controller是负责逻辑和广播给UI数据，而Widget是负责渲染（比如说UI按钮以及按钮动画）
//这里HUD作用主要是把Widget的控制器设置为我们创建的，并把想要显示的Widget设置为显示到桌面，这里同时也具备管理UI生命周期能力

#include "UI/HUD/AuraHUDBase.h"

#include "UI/WidgetController/SpellMenuWidgetController.h"
#include "UI/WidgetController/AttributeMenuWidgetController.h"
#include "UI/WidgetController/OverlayWidgetController.h"

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
	if (AttributeMenuWidgetController == nullptr)
	{
		AttributeMenuWidgetController = NewObject<UAttributeMenuWidgetController>(this,AttributeMenuWidgetControllerClass);
		AttributeMenuWidgetController->SetWidgetControllerParams(WCParams);
		AttributeMenuWidgetController->BindCallbacksToDependencies();

		return AttributeMenuWidgetController;
	}
	return AttributeMenuWidgetController;
}

USpellMenuWidgetController* AAuraHUDBase::GetSpellMenuWidgetController(const FWidgetControllerParams& WCParams)
{
	if (SpellMenuWidgetController == nullptr)
	{
		SpellMenuWidgetController = NewObject<USpellMenuWidgetController>(this,SpellMenuWidgetControllerClass);
		SpellMenuWidgetController->SetWidgetControllerParams(WCParams);
		SpellMenuWidgetController->BindCallbacksToDependencies();

		return SpellMenuWidgetController;
	}
	return SpellMenuWidgetController;
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


