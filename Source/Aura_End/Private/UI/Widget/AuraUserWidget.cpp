// 由来时路褒贬不一制作


#include "UI/Widget/AuraUserWidget.h"





void UAuraUserWidget::SetWidgetController(UObject* InWidgetController)
{
	WidgetController = InWidgetController;
	WidgetControllerSet();
}
