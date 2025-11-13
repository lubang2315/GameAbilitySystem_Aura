// 由来时路褒贬不一制作

#pragma once

#include "CoreMinimal.h"
#include "Gas/DataAsset/AbilityInfo.h"
#include "Gas/Player/AbilitySystemComponent/AuraAttributeSet.h"
#include "UI/WidgetController/AuraWidgetController.h"
#include "Windows/AllowWindowsPlatformTypes.h"
#include "OverlayWidgetController.generated.h"


USTRUCT(BlueprintType)
struct FUIWidgetRow : public FTableRowBase
{
	GENERATED_BODY();

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGameplayTag MessageTag = FGameplayTag();

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FText MEssage = FText();

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<class UAuraUserWidget> MessageWidget;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UTexture2D* Image = nullptr;
};

class UAuraUserWidget;
class UAbilityInfo;
class UAuraAbilitySystemComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAttributeChangedSignature,float,NewAttribute);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMessageWidgetRowSignature,FUIWidgetRow,Row);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FAbilityInfoSignature,const FAuraAbilityInfo,Info);

/**
 * 
 */
UCLASS(BlueprintType,Blueprintable)
class AURA_END_API UOverlayWidgetController : public UAuraWidgetController
{
	GENERATED_BODY()
public:
	virtual void BroadcastInitialValues() override;

	virtual void BindCallbacksToDependencies() override;
	
	UPROPERTY(BlueprintAssignable, Category="GAS|AttributSet")
	FOnAttributeChangedSignature OnHPChangedEvent;

	UPROPERTY(BlueprintAssignable, Category="GAS|AttributSet")
	FOnAttributeChangedSignature OnMaxHPChangedEvent;

	UPROPERTY(BlueprintAssignable, Category="GAS|AttributSet")
	FOnAttributeChangedSignature OnManaChangedEvent;

	UPROPERTY(BlueprintAssignable, Category="GAS|AttributSet")
	FOnAttributeChangedSignature OnMaxManaChangedEvent;

	UPROPERTY(BlueprintAssignable, Category="GAS|Message")
	FMessageWidgetRowSignature MessageWidgetDelegate;

	/*用于广播给Overlay技能栏信息结构体*/
	UPROPERTY(BlueprintAssignable, Category="GAS|Message")
	FAbilityInfoSignature AbilityInfoDelegate;

protected:
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly,Category="Widget Data")
	TObjectPtr<UDataTable> MessageWidgetDataTable;

	/*技能UI数据表*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly,Category="Widget Data")
	TObjectPtr<UAbilityInfo> AbilityInfo;

	template<typename T>
	T* GetDataTableRowByTag(UDataTable* DataTable,const FGameplayTag& Tag);

	/*AuraGAS那边技能初始化完成后触发此回调*/
	void OnInitializeStartupAbilities(UAuraAbilitySystemComponent* AuraASC)const;
};

template <typename T>
T* UOverlayWidgetController::GetDataTableRowByTag(UDataTable* DataTable, const FGameplayTag& Tag)
{
	T* Row = DataTable->FindRow<T>(Tag.GetTagName(),TEXT(""));
	return Row;
}
