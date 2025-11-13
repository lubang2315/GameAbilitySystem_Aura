// 由来时路褒贬不一制作


#include "Gas/AsyncAction/WaitCoolDownChange.h"



UWaitCoolDownChange* UWaitCoolDownChange::CreateWaitCoolDownChange(UAbilitySystemComponent* AbilitySystemComponent,
                                                                   const FGameplayTag& CoolDownTag)
{
	/*创建一个实例*/
	UWaitCoolDownChange* WaitCoolDownChange = NewObject<UWaitCoolDownChange>();
	WaitCoolDownChange->ASC = AbilitySystemComponent;
	WaitCoolDownChange->CoolDownTag = CoolDownTag;

	/*判断是否所有参数都初始化了*/
	if (!IsValid(AbilitySystemComponent) || !CoolDownTag.IsValid())
	{
		WaitCoolDownChange->EndTask();
		return nullptr;
	}
	/*监听冷却标签变动，并绑定回调*/
	AbilitySystemComponent->RegisterGameplayTagEvent(
		CoolDownTag,EGameplayTagEventType::NewOrRemoved).AddUObject(
			WaitCoolDownChange,&UWaitCoolDownChange::CoolDownTagChanged);

	/*监听GE，获取冷却激活，用于获取技能冷却结束*/
	AbilitySystemComponent->OnActiveGameplayEffectAddedDelegateToSelf.AddUObject(
		WaitCoolDownChange,&UWaitCoolDownChange::OnActiveEffectAdded);
	
	/*返回监听实例*/
	return WaitCoolDownChange;
}

void UWaitCoolDownChange::EndTask()
{
	if (!IsValid(ASC)) return;

	/*取消对冷却标签变动的监听*/
	ASC->RegisterGameplayTagEvent(CoolDownTag,EGameplayTagEventType::NewOrRemoved).RemoveAll(this);

	/*设置此对象可以被销毁，如果对象不被引用则可以销毁*/
	SetReadyToDestroy();

	/*标记此实例为垃圾资源，可以被回收*/
	MarkAsGarbage();
	
}

void UWaitCoolDownChange::CoolDownTagChanged(const FGameplayTag InCoolDownTag, int32 NewCount) const
{
	/*如果计数为0，表明没有冷却标签，即不在冷却时间*/
	if (NewCount == 0)
	{
		CoolDownEnd.Broadcast(0.f);
	}
}

void UWaitCoolDownChange::OnActiveEffectAdded(UAbilitySystemComponent* TargetASC, const FGameplayEffectSpec& Spec,
	FActiveGameplayEffectHandle ActiveEffectHandel) const
{
	/*获取设置到自身的所有标签*/
	FGameplayTagContainer AssetTags;
	Spec.GetAllAssetTags(AssetTags);

	/*获取设置到目标的所有标签*/
	FGameplayTagContainer GrantedTags;
	Spec.GetAllGrantedTags(GrantedTags);

	/*判断所有标签是否有冷却标签*/
	if (AssetTags.HasTagExact(CoolDownTag) || GrantedTags.HasTagExact(CoolDownTag))
	{
		/*通过标签倒查所有有这个标签并激活的GE，我个人认为这里写的不太合适直接使用ActiveEffectHandel查询GE时间就行，但是受限经验不确定现不做*/
		FGameplayEffectQuery GemeplayEffectQuery = FGameplayEffectQuery::MakeQuery_MatchAnyOwningTags(CoolDownTag.GetSingleTagContainer());
		TArray<float> TimesRemaining = ASC->GetActiveEffectsTimeRemaining(GemeplayEffectQuery);
		/*注意数组中()是检测所含元素个数，而[]是元素索引这个是从0开始而()不是有几个元素显示几*/
		if (TimesRemaining.Num() > 0)
		{
			float TimeRemaining = TimesRemaining[0];
			for (int32 i = 0; i < TimesRemaining.Num(); i++)
			{
				if (TimeRemaining < TimesRemaining[i])
				{
					TimeRemaining = TimesRemaining[i];
				}
			}
			
			CoolDownStart.Broadcast(TimeRemaining);
		}

	}
}
