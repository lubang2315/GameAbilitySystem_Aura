// 由来时路褒贬不一制作


#include "Input/AuraInputConfig.h"

const UInputAction* UAuraInputConfig::FindAbilityInputActionForTag(const FGameplayTag& InputTag, bool bLogNotFond) const
{
	for (auto& InputInfo : AbilityInputActions)
	{
		if ( InputInfo.InputAction && InputInfo.InputTag == InputTag)
		{
			return InputInfo.InputAction;
		}
	}
	if (bLogNotFond)
	{
		UE_LOG(LogTemp, Error, TEXT("无法从InputConfig[%s]中找到InputTag[%s]对应的技能InputAction"),*GetNameSafe(this),*InputTag.ToString());
	}
	return nullptr;
}
