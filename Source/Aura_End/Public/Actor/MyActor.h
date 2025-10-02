// 由来时路褒贬不一制作

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffect.h"
#include "GameFramework/Actor.h"
#include "MyActor.generated.h"

struct FActiveGameplayEffectHandle;
class UAbilitySystemComponent;
class UGameplayEffect;

UENUM(BlueprintType)
enum class EEffectApplicationPolicy : uint8
{
	ApplyOnOverlap,
	ApplyOnEndOverlap,
	DoNotApplyOnOverlap,
	
};

UENUM()
enum class EEffectRemovalPolicy
{
	RemoveOnEndOverlap,
	DoNotRemove
};

UCLASS()
class AURA_END_API AMyActor : public AActor
{
	GENERATED_BODY()
	
public:	
	
	AMyActor();

protected:
	
	virtual void BeginPlay() override;
	
	//给与目标添加GameplayEffect效果
	UFUNCTION(BlueprintCallable) 
	void ApplyEffectToTarget(AActor* TargetActor, TSubclassOf<UGameplayEffect> GameplayEffectClass);

	//在重叠开始时处理效果的添加删除逻辑
	UFUNCTION(BlueprintCallable) 
	void OnOverlap(AActor* TargetActor);

	//在重叠结束时处理效果的添加删除逻辑
	UFUNCTION(BlueprintCallable) 
	void OnEndOverlap(AActor* TargetActor);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Apply Effects")
	TSubclassOf<UGameplayEffect> InstantGameplayEffectClass; //生成GameplayEffect的类

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Apply Effects")
	EEffectApplicationPolicy InstantEffectApplicationPolicy = EEffectApplicationPolicy::DoNotApplyOnOverlap;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Apply Effects")
	TSubclassOf<UGameplayEffect> DurationGameplayEffectClass; //生成具有一定持续时间的GameplayEffect的类

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Apply Effects")
	EEffectApplicationPolicy DurationEffectApplicationPolicy = EEffectApplicationPolicy::DoNotApplyOnOverlap;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Apply Effects")
	TSubclassOf<UGameplayEffect> InfinityGameplayEffectClass; //生成具有一定持续时间的GameplayEffect的类

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Apply Effects")
	EEffectApplicationPolicy InfinityEffectApplicationPolicy = EEffectApplicationPolicy::DoNotApplyOnOverlap;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Apply Effects")
	EEffectRemovalPolicy InfinityEffectRemovalPolicy = EEffectRemovalPolicy::RemoveOnEndOverlap;

	//用于存储当前已经激活的GameplayEffect的句柄的map
	TMap<FActiveGameplayEffectHandle, UAbilitySystemComponent*> ActiveEffectHandles;
    /**曲线技能等级，按等级应用伤害*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="ActorLever")
	float ActorLever = 1.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Apply Effects")
	bool bApplyEffectToEnemies = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Apply Effects")
	bool bDestoryOnEffectApplication = false;
};

