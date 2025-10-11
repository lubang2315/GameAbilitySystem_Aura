// 由来时路褒贬不一制作

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectTypes.h"
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
	static void GiveStartupAbilities(const UObject* WordContextObject,UAbilitySystemComponent* EnemyASC,ECharacterClass CharacterClass);

	UFUNCTION(BlueprintCallable,Category = "MyAbilitySystemLibrary|CharaterClassDefaults")
	static UCharacterClassInfo* GetCharacterClassInfo(const UObject* WorldContextObject);

	/*通过自定义的FAuraGameplayEffectContext（注文件夹名AuraAbilityTypes）传递是否暴击和格挡*/
	UFUNCTION(BlueprintPure,Category = "MyAbilitySystemLibrary|GameplayEffect")
	static bool IsBlockHit(const FGameplayEffectContextHandle& EffectContextHandle);

	UFUNCTION(BlueprintPure,Category = "MyAbilitySystemLibrary|GameplayEffect")
	static bool IsCriticalHit(const FGameplayEffectContextHandle& EffectContextHandle);

	UFUNCTION(BlueprintCallable,Category = "MyAbilitySystemLibrary|GameplayEffect")
	static void SetIsBlockHit(UPARAM(ref) FGameplayEffectContextHandle& EffectContextHandle,bool IsBlockHit);

	UFUNCTION(BlueprintCallable,Category = "MyAbilitySystemLibrary|GameplayEffect")
	static void SetIsCriticalHit(UPARAM(ref) FGameplayEffectContextHandle& EffectContextHandle,bool IsCriticalHitHit);
	/*End*/
	
	/*获取半径范围内的Actor*/
	UFUNCTION(BlueprintCallable,Category = "MyAbilitySystemLibrary|GameplayMEchanics")
	static void GetLivePlayersWithinRadius(const UObject* WorldContextObject,float Radius,TArray<AActor*>& OutOverlappingActors,TArray<AActor*>& ActorsToIgnore,const FVector& SphereOrigin);

	/*在敌人使用技能攻击玩家时，通过标签判断是不是友军从而避免误伤*/
	UFUNCTION(BlueprintPure,Category = "MyAbilitySystemLibrary|GameplayMEchanics")
	static bool ISNotFriend(AActor* FirstActor,AActor* SecondActor);
	
	
};




