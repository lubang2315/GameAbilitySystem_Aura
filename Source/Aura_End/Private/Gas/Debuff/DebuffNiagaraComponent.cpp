// 由来时路褒贬不一制作


#include "Gas/Debuff/DebuffNiagaraComponent.h"


#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Interface/CombotInterface.h"

UDebuffNiagaraComponent::UDebuffNiagaraComponent()
{
	/*关闭Niagara自动激活*/
	bAutoActivate = false;
}

void UDebuffNiagaraComponent::BeginPlay()
{
	Super::BeginPlay();

	/*获取到战斗接口*/
	ICombotInterface* CombotInterface = Cast<ICombotInterface>(GetOuter());

	if (UAbilitySystemComponent* ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetOwner()))
	{
		ASC->RegisterGameplayTagEvent(DebuffTag,EGameplayTagEventType::NewOrRemoved).AddUObject(this,&UDebuffNiagaraComponent::DebuffTagChanged);
	}
	/*如果接口初始化完成而ASC系统未初始化完成*/
	else if (CombotInterface)
	{
		CombotInterface->GetASCRegistered().AddWeakLambda(this,[this](UAbilitySystemComponent* InASC)
		{
			InASC->RegisterGameplayTagEvent(DebuffTag,EGameplayTagEventType::NewOrRemoved).AddUObject(this,&UDebuffNiagaraComponent::DebuffTagChanged);
		});
	}
	/*绑定死亡后销毁*/
	if (CombotInterface)
	{
		CombotInterface->GetDeath().AddDynamic(this,&UDebuffNiagaraComponent::OnOwnerDeath);
	}
}

void UDebuffNiagaraComponent::DebuffTagChanged(const FGameplayTag CallbackTag, int32 Newcount)
{
	/*如果敌人死亡则不再激活Niagara*/
	const bool bOwnIsvalid = IsValid(GetOwner());
	const bool bOwnAlive = GetOwner()->Implements<UCombotInterface>() && !ICombotInterface::Execute_IsDead(GetOwner());
	
	/*负面标签数量大于0激活Niagara*/
	if (Newcount > 0 && bOwnIsvalid && bOwnAlive)
	{
		Activate();
	}
	else
	{
		Deactivate();
	}
}

void UDebuffNiagaraComponent::OnOwnerDeath(AActor* DeathActor)
{
	Deactivate();
}
