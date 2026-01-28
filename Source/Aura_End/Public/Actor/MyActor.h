// 由来时路褒贬不一制作

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameplayEffect.h"
#include "GameFramework/Actor.h"
#include "Interface/CombotInterface.h"
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

	virtual void Tick(float DeltaTime) override;
	
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
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="ActorLever")
	float ActorLever = 1.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Apply Effects")
	bool bApplyEffectToEnemies = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Apply Effects")
	bool bDestoryOnEffectApplication = false;

	/*实现击杀敌人掉落物品的动态效果*/
	// 计算后的Actor所在的位置
	UPROPERTY(BlueprintReadWrite)
	FVector CalculatedLocation;

	// 计算后的Actor的旋转
	UPROPERTY(BlueprintReadWrite)
	FRotator CalculatedRotation;

	// Actor是否帧更新旋转
	UPROPERTY(BlueprintReadWrite, Category="Pickup Movement")
	bool bRotates = false;

	// Actor每秒旋转的角度
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Pickup Movement")
	float RotationRate = 45.f;

	// Actor是否更新位置
	UPROPERTY(BlueprintReadWrite, Category="Pickup Movement")
	bool bSinusoidalMovement = false;

	// 正弦值-1到1，此值为调整更新移动范围
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Pickup Movement")
	float SineAmplitude = 1.f;

	// 此值参与正弦运算，默认值为1秒一个循环（2PI走完一个正弦的循环，乘以时间，就是一秒一个循环，可用于调整位置移动速度）
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Pickup Movement")
	float SinePeriod = 1.f; //2 * PI

	//调用此函数，Actor开始自动更新上下位置
	UFUNCTION(BlueprintCallable)
	void StartSinusoidalMovement();

	//调用此函数，Actor开始自动旋转
	UFUNCTION(BlueprintCallable)
	void StartRotation();
private:

	//当前掉落物的存在时间，可以通过此时间实现动态效果
	float RunningTime = 0.f;

	// Actor生成的默认初始位置，在Actor动态浮动时，需要默认位置作为基础位置
	FVector InitialLocation;

	// 每一帧更新Actor的位置和转向
	void ItemMovement(float DeltaSeconds);
	
};

