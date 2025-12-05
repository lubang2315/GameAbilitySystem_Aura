// 由来时路褒贬不一制作

#pragma once

#include "CoreMinimal.h"
#include "Gas/DataAsset/AbilityInfo.h"
#include "Gas/Player/AuraPlayerState.h"
#include "AuraWidgetController.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlayerStateChangedSignature,int32,NewValue);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FAbilityInfoSignature,const FAuraAbilityInfo,Info);

class AbilitySystemComponent;
class AttributeSet;
class AAuraPlayerController;
class AuraWidgetController;
class UAuraAttributeSet;
class UAuraAbilitySystemComponent;

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
	UFUNCTION(BlueprintCallable)
	virtual  void BroadcastInitialValues();
    /**当属性变化进行广播*/
	virtual void BindCallbacksToDependencies();

	/*技能UI数据表*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly,Category="Widget Data")
	TObjectPtr<UAbilityInfo> AbilityInfo;

	/*用于广播给Overlay技能栏信息结构体*/
	UPROPERTY(BlueprintAssignable, Category="GAS|Message")
	FAbilityInfoSignature AbilityInfoDelegate;

	/*广播技能信息,AuraGAS那边技能初始化完成后触发此回调*/
	void BroadcastAbilityInfo();
	
protected:
	
	UPROPERTY(BlueprintReadOnly, Category="WidgetController")
	TObjectPtr<APlayerController> PlayerController;

	UPROPERTY(BlueprintReadOnly, Category="WidgetController")
	TObjectPtr<APlayerState> PlayerState;

	UPROPERTY(BlueprintReadOnly, Category="WidgetController")
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY(BlueprintReadOnly, Category="WidgetController")
	TObjectPtr<UAttributeSet> AttributeSet;

	/*因为子类经常要用这里直接在基类创建*/
	UPROPERTY(BlueprintReadOnly, Category="WidgetController")
	TObjectPtr<AAuraPlayerController> AuraPlayerController;

	UPROPERTY(BlueprintReadOnly, Category="WidgetController")
	TObjectPtr<AAuraPlayerState> AuraPlayerState;

	UPROPERTY(BlueprintReadOnly, Category="WidgetController")
	TObjectPtr<UAuraAbilitySystemComponent> AuraAbilitySystemComponent;

	UPROPERTY(BlueprintReadOnly, Category="WidgetController")
	TObjectPtr<UAuraAttributeSet> AuraAttributeSet;

	/*子类可通过下面函数直接获取GAS相关类型*/
	AAuraPlayerController* GetAuraPlayerController();
	AAuraPlayerState* GetAuraPlayerState();
	UAuraAbilitySystemComponent* GetAuraAbilitySystemComponent();
	UAuraAttributeSet* GetAuraAttributeSet();
};
