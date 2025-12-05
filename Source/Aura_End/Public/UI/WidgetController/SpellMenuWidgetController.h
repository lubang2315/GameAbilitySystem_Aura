// 由来时路褒贬不一制作

#pragma once

#include "CoreMinimal.h"
#include "UI/WidgetController/AuraWidgetController.h"
#include "SpellMenuWidgetController.generated.h"

/**
 * 
 */
UCLASS(BlueprintType,Blueprintable)
class AURA_END_API USpellMenuWidgetController : public UAuraWidgetController
{
	GENERATED_BODY()

public:
	virtual  void BroadcastInitialValues() override;
	virtual void BindCallbacksToDependencies() override;

	/*向Ui广播技能点*/
	UPROPERTY(BlueprintAssignable, Category="GAS|Spell")
	FOnPlayerStateChangedSignature SpellPointsChangedDelegate;
};
