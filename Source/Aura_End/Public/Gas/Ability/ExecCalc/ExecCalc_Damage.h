// 由来时路褒贬不一制作
//这个类是和GE搭配使用在应用GE后，可以自定义对需要修改的参数进行复杂计算

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectExecutionCalculation.h"
#include "ExecCalc_Damage.generated.h"

/**
 * 
 */
UCLASS()
class AURA_END_API UExecCalc_Damage : public UGameplayEffectExecutionCalculation
{
	GENERATED_BODY()

public:
	UExecCalc_Damage();
	void DetermineDebuff(const FGameplayEffectCustomExecutionParameters& ExecutionParams,const FGameplayEffectSpec& Spec,FAggregatorEvaluateParameters EvaluateParam,const TMap<FGameplayTag,FGameplayEffectAttributeCaptureDefinition>& TagstoCaptureDefs) const;

	/*GE应用后会自动调用此函数，此函数主要用来执行自定义的属性计算逻辑*/
	virtual void Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const override;
	
	
};
