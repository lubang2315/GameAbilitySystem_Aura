// 由来时路褒贬不一制作

#pragma once

#include "CoreMinimal.h"
#include "Gas/Player/AuraPlayerState.h"
#include "AuraWidgetController.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlayerStateChangedSignature,int32,NewValue);

class AbilitySystemComponent;
class AttributeSet;

USTRUCT(BlueprintType)
struct FWidgetControllerParams
{
    GENERATED_BODY()
	/**因为要在蓝图中调用所以不管，要不要赋值都要写下面这句话，要不然蓝图那边会出错*/
	FWidgetControllerParams(){};
	/**新建一个结构体用于初始化和接收参数*/
	FWidgetControllerParams(APlayerController* PC,APlayerState* PS,UAbilitySystemComponent* ASC,UAttributeSet* AS):
	PlayerController(PC),PlayerState(PS),AbilitySystemComponent(ASC),AttributeSet(AS){}

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<APlayerController> PlayerController = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<APlayerState> PlayerState = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UAttributeSet> AttributeSet = nullptr;
};
/**
 * 
 */

UCLASS()
class AURA_END_API UAuraWidgetController : public UObject
{
	GENERATED_BODY()

public:
	/**注意因为是基类所以都是虚函数，在子类中会重写定义*/
	void SetWidgetControllerParams(const FWidgetControllerParams& WCParams);
    /**当widget Controller等相关组件加载成功，紧接着就把属性的初值通过广播形式传给HUD*/
	virtual  void BroadcastInitialValues();
    /**当属性变化进行广播*/
	virtual void BindCallbacksToDependencies();
	
protected:
	
	UPROPERTY(BlueprintReadOnly, Category="WidgetController")
	TObjectPtr<APlayerController> PlayerController;

	UPROPERTY(BlueprintReadOnly, Category="WidgetController")
	TObjectPtr<APlayerState> PlayerState;

	UPROPERTY(BlueprintReadOnly, Category="WidgetController")
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY(BlueprintReadOnly, Category="WidgetController")
	TObjectPtr<UAttributeSet> AttributeSet;
	
};
