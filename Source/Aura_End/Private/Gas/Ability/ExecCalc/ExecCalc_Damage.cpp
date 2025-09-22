// 由来时路褒贬不一制作
/*这里是使用了执行计算，下面我进行备注执行序号使用捕获属性和计算属性和输出属性
 * 根据教程后面又该代码了，所以占时废弃Armor属性修改，但是我不打算删除他，现在把他注释掉，以备参考。
 */


#include "Gas/Ability/ExecCalc/ExecCalc_Damage.h"
#include "AbilitySystemComponent.h"
#include "Gas/FunctionLibrary/MyFunctionLibrary.h"
#include "Gas/Player/AbilitySystemComponent/AuraAttributeSet.h"
#include "Interface/CombotInterface.h"
#include "Tags/AuraGameplayTags.h"

struct SDamageStruct
{
	/*1*声明一个捕获，利用宏自动创建一个Armor的属性*/
	DECLARE_ATTRIBUTE_CAPTUREDEF(Armor);
	DECLARE_ATTRIBUTE_CAPTUREDEF(ArmorPenetratinon);
	DECLARE_ATTRIBUTE_CAPTUREDEF(blockChance);
	DECLARE_ATTRIBUTE_CAPTUREDEF(CriticalHitChance);
	DECLARE_ATTRIBUTE_CAPTUREDEF(CriticalHitDamage);
	DECLARE_ATTRIBUTE_CAPTUREDEF(CriticalHitResistance);
	
	SDamageStruct()
	{
		/*2*Define Parameter,定义参数。设置从Aura那里或去Armor的属性，同时设置通目标身上获取，在创建时不进行属性快照，激活时才进行快照*/
		/*四个参数：获取属性的地方，获取的属性，获取的是谁的属性，是否在创建时进行属性快照，否在激活时进行属性快照*/
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet,Armor,Target,false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet,ArmorPenetratinon,Target,false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet,blockChance,Target,false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet,CriticalHitChance,Source,false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet,CriticalHitDamage,Source,false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet,CriticalHitResistance,Target,false);
	}
};

static const SDamageStruct& DamageStruct()
{
	static SDamageStruct DStruct;
	return DStruct;
}


UExecCalc_Damage::UExecCalc_Damage()
{
	/*3*初始化时告诉GAS系统要捕获的属性*/
	RelevantAttributesToCapture.Add(DamageStruct().ArmorDef);
	RelevantAttributesToCapture.Add(DamageStruct().ArmorPenetratinonDef);
	RelevantAttributesToCapture.Add(DamageStruct().blockChanceDef);
	RelevantAttributesToCapture.Add(DamageStruct().CriticalHitChanceDef);
	RelevantAttributesToCapture.Add(DamageStruct().CriticalHitDamageDef);
	RelevantAttributesToCapture.Add(DamageStruct().CriticalHitResistanceDef);
}

void UExecCalc_Damage::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams,
                                              FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	/*先获取GAS和Actor*/
	const UAbilitySystemComponent* SourceASC = ExecutionParams.GetSourceAbilitySystemComponent();
 	const UAbilitySystemComponent* TagrgetASC = ExecutionParams.GetTargetAbilitySystemComponent();

	 AActor* SourceCharacter = SourceASC ? SourceASC->GetAvatarActor() : nullptr;
	 AActor* TargetCharacter = TagrgetASC ? TagrgetASC->GetAvatarActor() : nullptr;

	
	 ICombotInterface* SourceCombotInterface = Cast<ICombotInterface>(SourceCharacter);
	 ICombotInterface* TargetCombotInterface = Cast<ICombotInterface>(TargetCharacter);

	const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();
	
	const FGameplayTagContainer* OwnTag = Spec.CapturedSourceTags.GetAggregatedTags();
	const FGameplayTagContainer* TargetTag = Spec.CapturedTargetTags.GetAggregatedTags();

	/*4*自定义一个聚合器评估参数，他是用来收集所有属性修改并返回该属性终值，一般GAS会自己创建，如果要自己创建，那就要填充源和目标标签*/
	FAggregatorEvaluateParameters EvaluateParam;
	EvaluateParam.SourceTags = OwnTag;
	EvaluateParam.TargetTags = TargetTag;
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//float Armor = 0.f;
	/*5*从创建的DEFINE_ATTRIBUTE_CAPTUREDEF里面获取属性值，其中EvaluateParam是根据标签进行复杂运算的这里没用到，获取到的值输出给Armor,他可以叫任何名，只要类型合适就行*/
	//ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStruct().ArmorDef,EvaluateParam,Armor);
	//Armor = FMath::Max<float>(0.f,Armor);

	//Armor = Armor + 10.f;
	/*6*计算5与6之间所有的公式得出结果根据第二个参数是加的选择，相加并输出，第一个是要修改的属性，第二个是修改方式，第三个是输出修改的数值*/
	//FGameplayModifierEvaluatedData EvaluatedData(DamageStruct().ArmorProperty,EGameplayModOp::Additive,Armor);
	/*把修改的值应用到GAS里*/
	//OutExecutionOutput.AddOutputModifier(EvaluatedData);
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	/*格挡几率*/
	float blockChance = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStruct().blockChanceDef,EvaluateParam,blockChance);
	blockChance = FMath::Max<float>(blockChance,0.f);/*输出夹值*/

	/*护甲穿透*/
	float ArmorPenetratinon = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStruct().ArmorPenetratinonDef,EvaluateParam,ArmorPenetratinon);
	ArmorPenetratinon = FMath::Max<float>(ArmorPenetratinon,0.f);

	/*暴击率*/
	float CriticalHitChance = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStruct().CriticalHitChanceDef,EvaluateParam,CriticalHitChance);
	CriticalHitChance = FMath::Max<float>(CriticalHitChance,0.f);

	/*暴击伤害*/
	float CriticalHitDamage = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStruct().CriticalHitDamageDef,EvaluateParam,CriticalHitDamage);
	CriticalHitDamage = FMath::Max<float>(CriticalHitDamage,0.f);

	/*暴击抗性*/
	float CriticalHitResistance = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStruct().CriticalHitResistanceDef,EvaluateParam,CriticalHitResistance);
	CriticalHitResistance = FMath::Max<float>(CriticalHitResistance,0.f);

	/*在自定义函数库创建了一个从GameMode获取角色配置数据的函数，在这里直接从函数库调用函数获取角色配置数据*/
	const UCharacterClassInfo* SourceCharacterInfo = UMyFunctionLibrary::GetCharacterClassInfo(SourceCharacter);

	/*从自建曲线表格中获取值*/
	FRealCurve* ArmorPenetrationCurve = SourceCharacterInfo->DamageCulationCoefficicients->FindCurve(FName("ArmorPenetration"),FString());
	const float ArmorPenetrationCoefficient = ArmorPenetrationCurve->Eval(SourceCombotInterface->GetPlayerLevel());
	
	FRealCurve* EffectiveArmorCurve = SourceCharacterInfo->DamageCulationCoefficicients->FindCurve(FName("EffeciveArmor"),FString());
	const float EffectiveArmorCoefficient = EffectiveArmorCurve->Eval(SourceCombotInterface->GetPlayerLevel());

	FRealCurve* CriticalHitResistanceCurve = SourceCharacterInfo->DamageCulationCoefficicients->FindCurve(FName("CriticalHitResistance"),FString());
	const float CriticalHitResistanceCoefficient = CriticalHitResistanceCurve->Eval(SourceCombotInterface->GetPlayerLevel());

	/*自定义计算属性区域*/
	/*护甲属性计算*/
	const float EffectiveArmor = blockChance*(100.f - ArmorPenetratinon*ArmorPenetrationCoefficient)/100.f;
	float Damage = Spec.GetSetByCallerMagnitude(FMyGameplayTags::Get().Damage);
	Damage *= (100.f - EffectiveArmor*EffectiveArmorCoefficient)/100.f;
	
	int32 IsBlockDamage = FMath::RandRange(0,100);
	Damage = blockChance>=IsBlockDamage ? Damage*0.5f : Damage;
	/*暴击属性计算*/
	const float CriticalHitResistanceValue = CriticalHitChance - CriticalHitResistance*CriticalHitResistanceCoefficient;
	const bool IsCriticalHit = FMath::RandRange(0,100) < CriticalHitResistanceValue;

	if (IsCriticalHit) Damage = Damage*2.f + CriticalHitDamage;
	
	/*End自定义计算区域*/
	
	FGameplayModifierEvaluatedData EvaluatedData(UAuraAttributeSet::GetIncomingDamageAttribute(),EGameplayModOp::Additive,Damage);
	/*把修改的值应用到GAS里*/
	OutExecutionOutput.AddOutputModifier(EvaluatedData);
	
}
