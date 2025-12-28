// 由来时路褒贬不一制作
//此类是蓝图函数库，只能写静态函数，像一些经常使用工具函数可以写在这里，并且可以在不同蓝图中调用。

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectTypes.h"
#include "Gas/DataAsset/CharacterClassInfo.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "MyFunctionLibrary.generated.h"

struct FDamageEffectPrams;
class USpellMenuWidgetController;
class UAttributeMenuWidgetController;
class UOverlayWidgetController;
struct FWidgetControllerParams;
class AAuraHUDBase;
/**
 * 
 */
UCLASS()
class AURA_END_API UMyFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintPure, Category = "MyAbilitySystemFunctionLibrary|WidgetController",meta=(DefaultToSelf = "WorldContextObject"))
	static bool MakeWidgetControllerParams(const UObject* WorldContextObject,FWidgetControllerParams& OutWCParams,AAuraHUDBase*& OutHUD);
	
	UFUNCTION(BlueprintPure, Category = "MyAbilitySystemFunctionLibrary|WidgetController",meta=(DefaultToSelf = "WorldContextObject"))
	static UOverlayWidgetController* GetOverlayWidgetController(const UObject* WorldContextObject);

	UFUNCTION(BlueprintPure, Category = "MyAbilitySystemFunctionLibrary|WidgetController",meta=(DefaultToSelf = "WorldContextObject"))
	static UAttributeMenuWidgetController* GetAttributeMenuWidgetController(const UObject* WorldContextObject);

	UFUNCTION(BlueprintPure, Category = "MyAbilitySystemFunctionLibrary|WidgetController",meta=(DefaultToSelf = "WorldContextObject"))
	static USpellMenuWidgetController* GetSpellMenuWidgetController(const UObject* WorldContextObject);

	UFUNCTION(BlueprintCallable,Category = "FunctionLibrary|CharacterClassInfo")
	static void InitializeDefaultAttribute(float Lever,ECharacterClass CharacterClass,UAbilitySystemComponent* EnemyASC,const UObject* WordContextObject);

	UFUNCTION(BlueprintCallable,Category = "MyAbilitySystemLibrary|CharaterClassDefaults")
	static void GiveStartupAbilities(const UObject* WordContextObject,UAbilitySystemComponent* EnemyASC,ECharacterClass CharacterClass);
	/*获取玩家属性技能相关信息*/
	UFUNCTION(BlueprintCallable,Category = "MyAbilitySystemLibrary|CharaterClassDefaults")
	static UCharacterClassInfo* GetCharacterClassInfo(const UObject* WorldContextObject);

	/*获取玩家技能配置相关信息*/
	UFUNCTION(BlueprintCallable,Category = "MyAbilitySystemLibrary|AbilityInfo")
	static UAbilityInfo* GetAbilityInfo(const UObject* WorldContextObject);

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

	/*根据数量要求获取距离最近的几个目标Actor*/
	UFUNCTION(BlueprintCallable,Category = "MyAbilitySystemLibrary|GameplayMEchanics")
	static void GetClosestTargets(int32 MaxTarget,const FVector& Origin,const TArray<AActor*>& Actors,TArray<AActor*>& OutTargets);
	
	/*在敌人使用技能攻击玩家时，通过标签判断是不是友军从而避免误伤*/
	UFUNCTION(BlueprintPure,Category = "MyAbilitySystemLibrary|GameplayMEchanics")
	static bool ISNotFriend(AActor* FirstActor,AActor* SecondActor);

	/*根据传入的负面影响参数生产GE并应用*/
	static FGameplayEffectContextHandle ApplyGameplayEffect(const FDamageEffectPrams& DamageEffectPrams);

	/*根据敌人类型和等级返回相应的XP值*/
	UFUNCTION(BlueprintCallable,Category = "MyAbilitySystemLibrary|CharacterClassDefaults")
	static int32 GetXPRewardForClassAndLevel(const UObject* WorldContextObject,int32 Level,ECharacterClass CharacterClass);

	/*Debuff*/
	/*获取是否成功应用负面影响*/
	UFUNCTION(BlueprintPure,Category = "MyAbilitySystemLibrary|GameplayEffect")
	static bool GetIsSuccessfulDebuff(const FGameplayEffectContextHandle& EffectContextHandle);

	/*获取负面影响值*/
	UFUNCTION(BlueprintPure,Category = "MyAbilitySystemLibrary|GameplayEffect")
	static float GetDebuffDamage(const FGameplayEffectContextHandle& EffectContextHandle);

	/*获取负面影响频率*/
	UFUNCTION(BlueprintPure,Category = "MyAbilitySystemLibrary|GameplayEffect")
	static float GetDebuffFrequency(const FGameplayEffectContextHandle& EffectContextHandle);

	/*获取负面影响持续时间*/
	UFUNCTION(BlueprintPure,Category = "MyAbilitySystemLibrary|GameplayEffect")
	static float GetDebuffDuration(const FGameplayEffectContextHandle& EffectContextHandle);

	/*获取负面影响类型标签*/
	UFUNCTION(BlueprintPure,Category = "MyAbilitySystemLibrary|GameplayEffect")
	static FGameplayTag GetDebuffType(const FGameplayEffectContextHandle& EffectContextHandle);

	/*设置是否成功应用负面影响*/
	UFUNCTION(BlueprintCallable,Category = "MyAbilitySystemLibrary|GameplayEffect")
	static void SetIsSuccessfulDebuff(UPARAM(ref) FGameplayEffectContextHandle& EffectContextHandle,bool IsSuccessfulDebuff);

	/*设置负面影响值*/
	UFUNCTION(BlueprintCallable,Category = "MyAbilitySystemLibrary|GameplayEffect")
	static void SetDebuffDamage(UPARAM(ref) FGameplayEffectContextHandle& EffectContextHandle,float DebuffDamage);

	/*设置负面影响频率*/
	UFUNCTION(BlueprintCallable,Category = "MyAbilitySystemLibrary|GameplayEffect")
	static void SetDebuffFrequency(UPARAM(ref) FGameplayEffectContextHandle& EffectContextHandle,float DebuffFrequency);

	/*设置负面影响持续时间*/
	UFUNCTION(BlueprintCallable,Category = "MyAbilitySystemLibrary|GameplayEffect")
	static void SetDebuffDuration(UPARAM(ref) FGameplayEffectContextHandle& EffectContextHandle,float DebuffDuration);

	/*设置负面影响持续时间*/
	UFUNCTION(BlueprintCallable,Category = "MyAbilitySystemLibrary|GameplayEffect")
	static void SetDebuffType(UPARAM(ref) FGameplayEffectContextHandle& EffectContextHandle,FGameplayTag& DamagetDebuffType);

	/*获取致命一击一击时攻击的方向和力度*/
	UFUNCTION(BlueprintPure,Category = "MyAbilitySystemLibrary|GameplayEffect")
	static FVector GetDeathImpulse(const FGameplayEffectContextHandle& EffectContextHandle);

	/*设置致命一击的攻击方向和力度*/
	UFUNCTION(BlueprintCallable,Category = "MyAbilitySystemLibrary|GameplayEffect")
	static void SetDeathImpulse(UPARAM(ref) FGameplayEffectContextHandle& EffectContextHandle,const FVector& DeathImpluse);

	/*获取击飞时攻击的方向和力度*/
	UFUNCTION(BlueprintPure,Category = "MyAbilitySystemLibrary|GameplayEffect")
	static FVector GetKnockBackForce(const FGameplayEffectContextHandle& EffectContextHandle);

	/*设置击飞的攻击方向和力度*/
	UFUNCTION(BlueprintCallable,Category = "MyAbilitySystemLibrary|GameplayEffect")
	static void SetKnockBackForce(UPARAM(ref) FGameplayEffectContextHandle& EffectContextHandle,const FVector& KnockBackForce);
	/*End*/

	/*在一定角度范围内，通过传入均分份数均分角度并输出*/
	UFUNCTION(BlueprintCallable,Category = "MyAbilitySystemLibrary|GameplayMechanics")
	static TArray<FRotator> EvenlySpacedRotators(const FVector& ForWard/*正前方向*/,const FVector& Axis/*基于旋转的轴*/,float Spread /*角度范围*/,int32 NumRotators/*分段数*/); 

	/*在一定角度范围内，通过传入均分份数均分角度并通过向量形式返回*/
	UFUNCTION(BlueprintCallable,Category = "MyAbilitySystemLibrary|GameplayMechanics")
	static TArray<FVector> EvenlySpacedVectors(const FVector& ForWard/*正前方向*/,const FVector& Axis/*基于旋转的轴*/,float Spread /*角度范围*/,int32 NumRotators/*分段数*/); 
	
};






