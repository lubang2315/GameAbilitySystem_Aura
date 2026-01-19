// 由来时路褒贬不一制作


#include "Character/AuraGameModeBase.h"

#include "Kismet/GameplayStatics.h"
#include "SaveGame/LoadScreenSaveGame.h"
#include "UI/MVVM/MVVM_LoadSlot.h"

void AAuraGameModeBase::SaveSlotData(const UMVVM_LoadSlot* LoadSlot, int32 SlotIndex)
{
	/*首先检查是否有对应的文档存在，如果有先删除对应的文档，然后在创建新文档，如果没有直接创建新文档*/
	DelegateSlotData(LoadSlot->GetName(), SlotIndex);
	
	/*在该槽位创建新的存档*/
	USaveGame* SaveGameObject = UGameplayStatics::CreateSaveGameObject(LoadScreenSaveGameClass);
	ULoadScreenSaveGame* LoadScreenSaveGame = Cast<ULoadScreenSaveGame>(SaveGameObject);

	/*设置存档实例的参数*/
	LoadScreenSaveGame->PlayerName = LoadSlot->GetPlayerName();
	LoadScreenSaveGame->SlotIndex = SlotIndex;
	LoadScreenSaveGame->SlotName = LoadSlot->GetWidgetName();
	LoadScreenSaveGame->SlotStatus = Taken;
	LoadScreenSaveGame->LoadMap = LoadSlot->GetLoadMap();

	/*设置完数据后存档*/
	UGameplayStatics::SaveGameToSlot(LoadScreenSaveGame,LoadSlot->GetWidgetName(),SlotIndex);
	
}

ULoadScreenSaveGame* AAuraGameModeBase::GetLoadScreenSaveGame(const FString& SaveSlotName, int32 SlotIndex) const
{
	USaveGame* SaveGameObject = nullptr;
	/*检查是否有存档有的话直接返回，没有就创建一个默认的*/
	if (UGameplayStatics::DoesSaveGameExist(SaveSlotName,SlotIndex))
	{
		/*获取文档*/
		SaveGameObject = UGameplayStatics::LoadGameFromSlot(SaveSlotName,SlotIndex);
	}
	if (!SaveGameObject)
	{
		SaveGameObject = UGameplayStatics::CreateSaveGameObject(LoadScreenSaveGameClass);
	}
	
	/*转换类型*/
	ULoadScreenSaveGame* LoadScreenSaveGame = Cast<ULoadScreenSaveGame>(SaveGameObject);
	
	return LoadScreenSaveGame;
	
	
}

void AAuraGameModeBase::DelegateSlotData(const FString& SaveSlotName, int32 SlotIndex)
{
	/*首先检查是否有对应的文档存在，如果有先删除对应的文档，然后在创建新文档，如果没有直接创建新文档*/
	if (UGameplayStatics::DoesSaveGameExist(SaveSlotName,SlotIndex))
	{
		/*删除已经存在的存档*/
		UGameplayStatics::DeleteGameInSlot(SaveSlotName,SlotIndex);
	}
}

void AAuraGameModeBase::TravelToMap(UMVVM_LoadSlot* Slot)
{
	/*获取此存档相关信息，目前还没有使用*/
	const FString SlotName = Slot->GetWidgetName();
	const int32 SlotIndex = Slot->SlotIndex;
	
	/*切换关卡*/
	UGameplayStatics::OpenLevelBySoftObjectPtr(Slot,Maps.FindChecked(Slot->GetLoadMap()));
}

void AAuraGameModeBase::BeginPlay()
{
	Super::BeginPlay();

	/*添加一个默认地图到映射中*/
	Maps.Add(DefaultMapName,DefaultMap);
}
