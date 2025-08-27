// 由来时路褒贬不一制作

#pragma once

#include "CoreMinimal.h"
#include "AuraInputConfig.h"
#include "EnhancedInputComponent.h"
#include "AuraInputComponent.generated.h"

/**
 * 
 */
UCLASS()
class AURA_END_API UAuraInputComponent : public UEnhancedInputComponent
{
	GENERATED_BODY()

public:
	/**利用增强输入里面提供的模版函数，就是BindAction用法差不多，特点可以自定义传入参数类型并绑定函数*/
	template<class UserCalss,typename pressedFuncType,typename ReleasedFuncType,typename HoldFuncType>
	void BindAbilityAction(const UAuraInputConfig* InputConfig,UserCalss* Object,pressedFuncType PressedFunc,ReleasedFuncType ReleasedFunc,HoldFuncType HoldFunc);
};
/**在这里自定义回调函数当有输入操作时会读取AuraGameplayTags里面设置的对应键位的标签，并激活三种键位状态的回调函数同时传递相应的Tags *ActivateGA**/
template <class UserCalss, typename pressedFuncType, typename ReleasedFuncType, typename HoldFuncType>
void UAuraInputComponent::BindAbilityAction(const UAuraInputConfig* InputConfig, UserCalss* Object,pressedFuncType PressedFunc, ReleasedFuncType ReleasedFunc, HoldFuncType HoldFunc)
{
	check(InputConfig);

	for (auto& Action : InputConfig->AbilityInputActions)
	{
		if (Action.InputAction && Action.InputTag.IsValid())
		{
			if (PressedFunc)
			{
				BindAction(Action.InputAction,ETriggerEvent::Started, Object, PressedFunc,Action.InputTag);
			}
			if (ReleasedFunc)
			{
				BindAction(Action.InputAction,ETriggerEvent::Completed, Object, ReleasedFunc,Action.InputTag);
			}
			if (HoldFunc)
			{
				BindAction(Action.InputAction,ETriggerEvent::Triggered, Object, HoldFunc,Action.InputTag);
			}
		}
	}
}