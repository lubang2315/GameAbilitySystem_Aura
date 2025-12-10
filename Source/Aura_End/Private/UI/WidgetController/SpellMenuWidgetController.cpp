// 由来时路褒贬不一制作


#include "UI/WidgetController/SpellMenuWidgetController.h"

#include "Gas/Player/AbilitySystemComponent/AuraAbilitySystemComponent.h"
#include "Tags/AuraGameplayTags.h"

void USpellMenuWidgetController::BroadcastInitialValues()
{
	BroadcastAbilityInfo();
	/*广播技能点*/
	SpellPointsChangedDelegate.Broadcast(GetAuraPlayerState()->GetPlayerSpellPoints());
}

void USpellMenuWidgetController::BindCallbacksToDependencies()
{
	GetAuraAbilitySystemComponent()->AbilityStatusChangedDelegate.AddLambda([this](const FGameplayTag& AbilityTag,const FGameplayTag& StatusTag,int32 NewLevel)
	{
		if (SelectedAbility.AbilityTag.MatchesTagExact(AbilityTag))
		{
			SelectedAbility.StateTag = StatusTag;
			
			bool bShpuldEnableSpellPoints = false;
			bool bShouldEnableEquip = false;
			
			ShouldEnableButtons(StatusTag,CurrentSpellPoints > 0,bShpuldEnableSpellPoints,bShouldEnableEquip);

			FString Description;
			FString NextLevelDescription;

			GetAuraAbilitySystemComponent()->GetDescrptionByAbilityTag(AbilityTag,Description,NextLevelDescription);
			SpellGlobeSelectionSignature.Broadcast(bShpuldEnableSpellPoints,bShouldEnableEquip,Description,NextLevelDescription);
		}
		
		if (AbilityInfo)
		{
			/*获取技能数据，并更新技能解锁状态*/
			FAuraAbilityInfo AuraAbilityInfo = AbilityInfo->FindAbilityInfoForTag(AbilityTag);
			AuraAbilityInfo.StatusTag = StatusTag;
			AbilityInfoDelegate.Broadcast(AuraAbilityInfo);
		}
	});

	/*向UI广播属性点*/
	GetAuraPlayerState()->OnSpellPointsChangedDelegate.AddLambda([this](const int32 Points)
	{
		SpellPointsChangedDelegate.Broadcast(Points);

		CurrentSpellPoints = Points;

		bool bShpuldEnableSpellPoints = false;
		bool bShouldEnableEquip = false;
			
		ShouldEnableButtons(SelectedAbility.StateTag,CurrentSpellPoints > 0,bShpuldEnableSpellPoints,bShouldEnableEquip);
		
		FString Description;
		FString NextLevelDescription;

		GetAuraAbilitySystemComponent()->GetDescrptionByAbilityTag(SelectedAbility.AbilityTag,Description,NextLevelDescription);
		SpellGlobeSelectionSignature.Broadcast(bShpuldEnableSpellPoints,bShouldEnableEquip,Description,NextLevelDescription);
	
	});

	/*绑定技能装备后的回调*/
	GetAuraAbilitySystemComponent()->AbilityEquipDelegate.AddUObject(this,&USpellMenuWidgetController::OnAbilityEquipped);
	
}

void USpellMenuWidgetController::SpellGlobeSelected(const FGameplayTag& AbilityTag)
{
	if (bWaitForEquipSelection)
	{
		/*从创建的技能信息中获取技能所属类型,并广播告诉装备按钮停止播放动画*/
		const FGameplayTag AbilityTypeTag = AbilityInfo->FindAbilityInfoForTag(AbilityTag).AbilityTypeTag;
		DeselectionForEquipSelectionDelegate.Broadcast(AbilityTypeTag);

		bWaitForEquipSelection = false;
	}
	
	FMyGameplayTags AuraGameplayTag = FMyGameplayTags::Get();
	const int32 SpellPoints = GetAuraPlayerState()->GetPlayerSpellPoints(); /*获取技能点*/
	FGameplayTag StatusTag;

	const bool bTagValid = AbilityTag.IsValid();/*判断传入标签是否有效*/
	const bool bTagNone = AbilityTag.MatchesTag(AuraGameplayTag.Abilities_None); /*判断传入的技能是否为空技能标签*/
	FGameplayAbilitySpec* AbilitySpec = GetAuraAbilitySystemComponent()->GetSpecFromAbilityTag(AbilityTag); /*根据标签获取技能规格*/
	const bool bSpecValid = AbilitySpec != nullptr; /*判断技能实例是否存在*/

	if (!bTagValid || bTagNone || !bSpecValid)
	{
		StatusTag = AuraGameplayTag.Abilities_Status_Locked; /*标签不存在，传入空技能标签，没有实例都把技能标签设置为锁定状态*/
	}
	else
	{
		/*从技能实例获取技能解锁状态标签*/
		StatusTag = GetAuraAbilitySystemComponent()->GetStatusTagFromSpec(*AbilitySpec);
	}
	/*更新一下全局变量*/
	SelectedAbility.AbilityTag = AbilityTag;
	SelectedAbility.StateTag = StatusTag;
	
	/*事先声明默认值，方便简化下面代码*/
	bool bShpuldEnableSpellPoints = false;
	bool bShouldEnableEquip = false;

	/*该函数会根据传入数据，直接修改上面三个bool值*/
	ShouldEnableButtons(StatusTag,SpellPoints > 0,bShpuldEnableSpellPoints,bShouldEnableEquip);
	
	/*根据技能解锁状态广播是否启用技能点按钮和装备按钮*/
	FString Description;
	FString NextLevelDescription;
	GetAuraAbilitySystemComponent()->GetDescrptionByAbilityTag(AbilityTag,Description,NextLevelDescription);
	SpellGlobeSelectionSignature.Broadcast(bShpuldEnableSpellPoints,bShouldEnableEquip,Description,NextLevelDescription);
	
}

void USpellMenuWidgetController::SpendSpellPointsOnButtonPressed()
{
	if (GetAuraAbilitySystemComponent())
	{
		/*调用ASC技能等级提升函数*/
		GetAuraAbilitySystemComponent()->ServerSpendSpellPoints(SelectedAbility.AbilityTag);
	}
}

void USpellMenuWidgetController::GlobeDeselected()
{
	if (bWaitForEquipSelection)
	{
		/*从创建的技能信息中获取技能所属类型,并广播告诉装备按钮停止播放动画*/
    	const FGameplayTag AbilityTypeTag = AbilityInfo->FindAbilityInfoForTag(SelectedAbility.AbilityTag).AbilityTypeTag;
    	DeselectionForEquipSelectionDelegate.Broadcast(AbilityTypeTag);

		bWaitForEquipSelection = false;
	}
	
	
	FMyGameplayTags AbilityGameplayTag = FMyGameplayTags::Get();
	SelectedAbility.AbilityTag = AbilityGameplayTag.Abilities_None;
	SelectedAbility.StateTag = AbilityGameplayTag.Abilities_Status_Locked;

	SpellGlobeSelectionSignature.Broadcast(false,false,FString(),FString());
}

void USpellMenuWidgetController::EquipButtonPressed()
{
	/*从创建的技能信息中获取技能所属类型*/
	const FGameplayTag AbilityTypeTag = AbilityInfo->FindAbilityInfoForTag(SelectedAbility.AbilityTag).AbilityTypeTag;
	WaitForEquipSelectionDelegate.Broadcast(AbilityTypeTag);

	bWaitForEquipSelection = true;

	/*首先当装备技能按钮被按下时先检测此技能是否已经装备，然后记下装备在技能栏的标签，因此可以通过标签清除在技能栏装备从而可以从新装备在其他技能球*/
	/*先检测选择技能解锁状态是不是已经装备*/
	const FGameplayTag SelectedStatusTag = AuraAbilitySystemComponent->GetStatusFromAbilityTag(SelectedAbility.AbilityTag);
	if (SelectedStatusTag.MatchesTagExact(FMyGameplayTags::Get().Abilities_Status_Equipped))
	{
		/*如果是已装备记下Input标签*/
		SelectionSlot = AuraAbilitySystemComponent->GetInputTagFromAbilityTag(SelectedAbility.AbilityTag);
	}
}

void USpellMenuWidgetController::SpellRowGlobePressed(const FGameplayTag& SlotTag, const FGameplayTag& AbilityTypeTag)
{
	/*如果是取消装备技能和装备的技能类型不符合：如主动技能你想要装进被动就能栏这两种情况直接返回*/
	if (!bWaitForEquipSelection) return;
	const FGameplayTag& SelectedAbilityType = AbilityInfo->FindAbilityInfoForTag(SelectedAbility.AbilityTag).AbilityTypeTag;
	if (!SelectedAbilityType.MatchesTagExact(AbilityTypeTag)) return;

	GetAuraAbilitySystemComponent()->ServerEquipAbility(SlotTag,SelectedAbility.AbilityTag);
}

void USpellMenuWidgetController::OnAbilityEquipped(const FGameplayTag& AbilityTag,const FGameplayTag& StatusTag,const FGameplayTag& SlotTag,const FGameplayTag& PreviousTag)
{
	/*主要是广播两个数据，一个是清除已经装备的技能数据，一个是广播新要装在的数据*/

	const FMyGameplayTags GameplayTags = FMyGameplayTags::Get();

	FAuraAbilityInfo LastSlotInfo;
	LastSlotInfo.StatusTag = GameplayTags.Abilities_Status_Unlocked;
	LastSlotInfo.InputTag = PreviousTag;
	LastSlotInfo.AbilityTag = GameplayTags.Abilities_None;
	AbilityInfoDelegate.Broadcast(LastSlotInfo);

	FAuraAbilityInfo NewAbilityInfo = AbilityInfo->FindAbilityInfoForTag(AbilityTag);
	NewAbilityInfo.StatusTag = StatusTag;
	NewAbilityInfo.InputTag = SlotTag;
	AbilityInfoDelegate.Broadcast(NewAbilityInfo);

	SpellGlobeReesignedDelegate.Broadcast(AbilityTag);
	GlobeDeselected();
}

void USpellMenuWidgetController::ShouldEnableButtons(const FGameplayTag& StatusTag, bool bHasSpellPoints,bool& bShpuldEnableSpellPoints, bool& bShouldEnableEquip)
{
	FMyGameplayTags AbilityGameplayTag = FMyGameplayTags::Get();

	if (StatusTag.MatchesTagExact(AbilityGameplayTag.Abilities_Status_Locked))
	{
		bShpuldEnableSpellPoints = false;
		bShouldEnableEquip = false;
	}
	else if (StatusTag.MatchesTagExact(AbilityGameplayTag.Abilities_Status_Unlocked))
	{
		bShpuldEnableSpellPoints = bHasSpellPoints;
		bShouldEnableEquip = true;
	}
	else if (StatusTag.MatchesTagExact(AbilityGameplayTag.Abilities_Status_Eligible))
	{
		bShpuldEnableSpellPoints = bHasSpellPoints;
	}
	else if (StatusTag.MatchesTagExact(AbilityGameplayTag.Abilities_Status_Equipped))
	{
		bShpuldEnableSpellPoints = bHasSpellPoints;
		bShouldEnableEquip = true;
	}
	
}
