// 由来时路褒贬不一制作

#pragma once

#include "CoreMinimal.h"
#include "Gas/DataAsset/CharacterClassInfo.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "MyFunctionLibrary.generated.h"

class UAttributeMenuWidgetController;
class UOverlayWidgetController;
/**
 * 
 */
UCLASS()
class AURA_END_API UMyFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintPure, Category = "MyAbilitySystemFunctionLibrary|WidgetController")
	static UOverlayWidgetController* GetOverlayWidgetController(const UObject* WorldContextObject);

	UFUNCTION(BlueprintPure, Category = "MyAbilitySystemFunctionLibrary|WidgetController")
	static UAttributeMenuWidgetController* GetAttributeMenuWidgetController(const UObject* WorldContextObject);

	UFUNCTION(BlueprintCallable,Category = "FunctionLibrary|CharacterClassInfo")
	static void InitializeDefaultAttribute(float Lever,ECharacterClass CharacterClass,UAbilitySystemComponent* EnemyASC,const UObject* WordContextObject);

	UFUNCTION(BlueprintCallable,Category = "MyAbilitySystemLibrary|CharaterClassDefaults")
	static void GiveStartupAbilities(const UObject* WordContextObject,UAbilitySystemComponent* EnemyASC);
};
