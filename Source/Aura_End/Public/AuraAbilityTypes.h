#pragma once

#include "GameplayEffectTypes.h"
#include "AuraAbilityTypes.generated.h"

USTRUCT(BlueprintType)
struct FAuraGameplayEffectContext : public FGameplayEffectContext
{
	/*宏，自动生成构造函数，析构函数，拷贝函数等*/
	GENERATED_BODY()

public:
	/*返回用于序列化的实际结构体，子类必须覆盖他，所以这里覆盖*/
	virtual UScriptStruct* GetScriptStruct() const
	{
		return FAuraGameplayEffectContext::StaticStruct();
	}

	/*自定义序列化，同样子类必须override他*/
	virtual bool NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess);

	/** Creates a copy of this context, used to duplicate for later modifications */
	virtual FAuraGameplayEffectContext* Duplicate() const
	{
		FAuraGameplayEffectContext* NewContext = new FAuraGameplayEffectContext();
		*NewContext = *this;
		if (GetHitResult())
		{
			// Does a deep copy of the hit result
			NewContext->AddHitResult(*GetHitResult(), true);
		}
		return NewContext;
	}
	
	/*在其他地方可以获取格挡和暴击的bool*/
	bool IsblockedHit() const { return bIsBlockedHit; }
	bool IsCriticalHit() const { return bIsCriticalHit; }

	/*在其他地方可以设置格挡暴击的bool*/
	void SetBlockedHit(bool BlockedHit) { bIsBlockedHit = BlockedHit; }
	void SetCriticalHit(bool CriticalHit) { bIsCriticalHit = CriticalHit; }
	
protected:

	UPROPERTY()/*格挡*/
	bool bIsBlockedHit = false;

	UPROPERTY()/*暴击*/
	bool bIsCriticalHit = false;
	
private:
	
};

template<>
struct TStructOpsTypeTraits<FAuraGameplayEffectContext> : public TStructOpsTypeTraitsBase2<FAuraGameplayEffectContext>
{
	enum
	{
		WithCopy = true,		
		WithNetSerializer = true,
	};
};
