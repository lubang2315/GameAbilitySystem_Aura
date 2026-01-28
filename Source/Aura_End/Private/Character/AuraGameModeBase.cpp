// 由来时路褒贬不一制作


#include "Character/AuraGameModeBase.h"

#include "EngineUtils.h"
#include "Aura_End/AuraAbilityInfoLogChannels.h"
#include "GameFramework/PlayerStart.h"
#include "Interface/SaveInterface.h"
#include "Kismet/GameplayStatics.h"
#include "SaveGame/AuraGameInstance.h"
#include "SaveGame/LoadScreenSaveGame.h"
#include "GameFramework/Character.h"
#include "Serialization/ObjectAndNameAsStringProxyArchive.h"
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
	LoadScreenSaveGame->SlotName = LoadSlot->GetLoadSlotName();
	LoadScreenSaveGame->SlotStatus = Taken;
	LoadScreenSaveGame->LoadMap = LoadSlot->GetLoadMap();
	LoadScreenSaveGame->PlayerStartTag = LoadSlot->PlayerStartTag;
	LoadScreenSaveGame->MapAssetName = LoadSlot->MapAssetName;

	/*设置完数据后存档*/
	UGameplayStatics::SaveGameToSlot(LoadScreenSaveGame,LoadSlot->GetLoadSlotName(),SlotIndex);
	
}

ULoadScreenSaveGame* AAuraGameModeBase::GetLoadScreenSaveGame(const FString& SaveSlotName, int32 SlotIndex) const
{

	USaveGame* SaveGameObject;
	//检查是否有对应名称的存档
	if(UGameplayStatics::DoesSaveGameExist(SaveSlotName, SlotIndex))
	{
		//获取存档
		SaveGameObject = UGameplayStatics::LoadGameFromSlot(SaveSlotName, SlotIndex);
	}
	else
	{
		//创建新存档
		SaveGameObject = UGameplayStatics::CreateSaveGameObject(LoadScreenSaveGameClass);
	}

	//转换类型
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

FString AAuraGameModeBase::GatMapNameFromMapAssetName(const FString& MapAssetName) const
{
	for (auto& Map : Maps)
	{
		if (Map.Value.ToSoftObjectPath().GetAssetName() == MapAssetName)
		{
			return Map.Key;
		}
	}
	return FString();
}

void AAuraGameModeBase::TravelToMap(UMVVM_LoadSlot* Slot)
{
	/*获取此存档相关信息，目前还没有使用*/
	const FString SlotName = Slot->GetLoadSlotName();
	const int32 SlotIndex = Slot->SlotIndex;
	
	/*切换关卡*/
	UGameplayStatics::OpenLevelBySoftObjectPtr(Slot,Maps.FindChecked(Slot->GetLoadMap()));
}

AActor* AAuraGameModeBase::ChoosePlayerStart_Implementation(AController* Player)
{
	/*获取游戏实例，存档全局游戏状态和数据管理，这里我们在里面设置了玩家出生点标签*/
	UAuraGameInstance* AuraGameInstance = Cast<UAuraGameInstance>(GetGameInstance());
	
	/*获取关卡里面所有出生点实例*/
	TArray<AActor*> AllPlayerStarts;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(),APlayerStart::StaticClass(),AllPlayerStarts);

	/*当关卡中有好几个出生点的时候我们默认选第一个符合要求的*/
	if (AllPlayerStarts.Num() > 0)
	{
		AActor* SelectPlayerStart = AllPlayerStarts[0];
		for (AActor* Actor : AllPlayerStarts)
		{
			if (APlayerStart* PlayerStart = Cast<APlayerStart>(Actor))
			{
				/*判断出生点是否具有指定出生标签*/
				if (PlayerStart->PlayerStartTag == AuraGameInstance->PlayerStartTag)
				{
					SelectPlayerStart = PlayerStart;
					
				}
			}
		}
		return SelectPlayerStart;
	}
	return nullptr;
}

ULoadScreenSaveGame* AAuraGameModeBase::RetrieveInGameSaveData() const
{
	UAuraGameInstance* AuraGameInstance = Cast<UAuraGameInstance>(GetGameInstance());
	/*从游戏实例中获取游戏存档的插槽和插槽索引*/
	const FString SlotName = AuraGameInstance->LoadSlotName;
	const int32 SlotIndex = AuraGameInstance->LoadSlotIndex;
	/*根据插槽名字索引返回保存的数据*/
	return GetLoadScreenSaveGame(SlotName,SlotIndex);
}

void AAuraGameModeBase::SaveGameProgress(ULoadScreenSaveGame* SaveGameData) const
{
	UAuraGameInstance* AuraGameInstance = Cast<UAuraGameInstance>(GetGameInstance());

	/*修改下一次的复活点*/
	AuraGameInstance->PlayerStartTag = SaveGameData->PlayerStartTag;

	/*获取保存插槽名称和索引*/
	const FString SlotName = AuraGameInstance->LoadSlotName;
	const int32 SlotIndex = AuraGameInstance->LoadSlotIndex;
	
	/*保存游戏实例*/
	UGameplayStatics::SaveGameToSlot(SaveGameData,SlotName,SlotIndex);
}

void AAuraGameModeBase::SaveWorldState(UWorld* World,const FString& DestinationMapAssetName) const
{
	//获取关卡名称
	FString WorldName = World->GetMapName();
	WorldName.RemoveFromStart(World->StreamingLevelsPrefix); //从关卡名称这里移除指定前缀，当前为移除通常用于标识流式加载的关卡文件前缀

	//获取到游戏实例
	UAuraGameInstance* RPGGI = Cast<UAuraGameInstance>(GetGameInstance());
	check(RPGGI);

	//获取存档
	if(ULoadScreenSaveGame* SaveGame = GetLoadScreenSaveGame(RPGGI->LoadSlotName, RPGGI->LoadSlotIndex))
	{
		/*修改地图存档名称个和地图资源名称*/
		if (DestinationMapAssetName != FString(""))
		{
			SaveGame->MapAssetName = DestinationMapAssetName;
			SaveGame->LoadMap = GatMapNameFromMapAssetName(DestinationMapAssetName);
		}
		
		if(!SaveGame->HasMap(WorldName))
		{
			//如果存档不包含对应关卡内容，将创建一个对应的数据结构体存储
			FSaveMap NewSavedMap;
			NewSavedMap.MapAssetName = WorldName;
			SaveGame->SaveMap.Add(NewSavedMap);
		}

		//获取对应的存档关卡数据结构体
		FSaveMap SavedMap = SaveGame->GetSavedMapWithMapName(WorldName);
		SavedMap.Actors.Empty(); //存储的内容

		//使用迭代起，便利场景里的每一个Actor，将需要保存Actor数据保存到结构体内
		for(FActorIterator It(World); It; ++It)
		{
			AActor* Actor = *It;

			//判断Actor是否存在，并判断Actor是否需要存储
			if(!IsValid(Actor) || !Actor->Implements<USaveInterface>()) continue;

			//创建存储结构体
			FSaveActor SavedActor;
			SavedActor.ActorName = Actor->GetFName();
			SavedActor.ActorTransform = Actor->GetTransform();

			//创建一个 FMemoryWriter，用于将数据写入SavedActor.Bytes
			FMemoryWriter MemoryWriter(SavedActor.Bytes);

			//创建一个序列化器，将对象的成员以名称和值的形式保存到 MemoryWriter。
			FObjectAndNameAsStringProxyArchive Archive(MemoryWriter, true);
			Archive.ArIsSaveGame = true; //设置序列化方式为保存到存档的模式

			//将Actor所需要保存的数据写入到Archive，Archive将把数据存储到SavedActor.Bytes
			Actor->Serialize(Archive);

			SavedMap.Actors.AddUnique(SavedActor);
		}

		//找到对应的名称的结构体，将数据存储到存档对象内
		for(FSaveMap& MapToReplace : SaveGame->SaveMap)
		{
			if(MapToReplace.MapAssetName == WorldName)
			{
				MapToReplace = SavedMap;
			}
		}

		//保存存档
		UGameplayStatics::SaveGameToSlot(SaveGame, RPGGI->LoadSlotName, RPGGI->LoadSlotIndex);
	}
}

void AAuraGameModeBase::LoadWorldState(UWorld* World) const
{
	//获取关卡名称
	FString WorldName = World->GetMapName();
	WorldName.RemoveFromStart(World->StreamingLevelsPrefix); //从关卡名称这里移除指定前缀，当前为移除通常用于标识流式加载的关卡文件前缀
	
	//获取到游戏实例
	UAuraGameInstance* RPGGI = Cast<UAuraGameInstance>(GetGameInstance());
	check(RPGGI);

	//判断获取的存档是否存在
	if(UGameplayStatics::DoesSaveGameExist(RPGGI->LoadSlotName, RPGGI->LoadSlotIndex))
	{
		//获取存档
		ULoadScreenSaveGame* SaveGame = Cast<ULoadScreenSaveGame>(UGameplayStatics::LoadGameFromSlot(RPGGI->LoadSlotName, RPGGI->LoadSlotIndex));
		if(SaveGame == nullptr)
		{
			UE_LOG(LogAura, Error, TEXT("加载对应存档失败"));
		}

		//判断存档是否含有对应关卡的数据
		if(SaveGame->HasMap(WorldName))
		{
			//遍历场景内的所有Actor，寻找存档内对应的数据并应用到场景
			for(FActorIterator It(World); It; ++It)
			{
				AActor* Actor = *It;

				if(!Actor->Implements<USaveInterface>()) continue;

				//遍历存档里对应关卡的所有actor数据
				for(FSaveActor SavedActor : SaveGame->GetSavedMapWithMapName(WorldName).Actors)
				{
					//查找到对应的actor的存档数据
					if(SavedActor.ActorName == Actor->GetFName())
					{
						//判断当前Actor是否需要设置位置变换
						if(ISaveInterface::Execute_ShouldLoadTransform(Actor))
						{
							Actor->SetActorTransform(SavedActor.ActorTransform);
						}

						//反序列化，创建一个FMemoryReader实例用于从二进制数据中读取内容
						FMemoryReader MemoryReader(SavedActor.Bytes);

						//FObjectAndNameAsStringProxyArchive 代理类，用于序列化和反序列化对象的属性 true：表示允许使用字符串形式的对象和属性名称（便于调试和可读性）。
						FObjectAndNameAsStringProxyArchive Archive(MemoryReader, true);
						Archive.ArIsSaveGame = true; //指定反序列化是用于加载存档数据。
						Actor->Serialize(Archive); //执行反序列化，将二进制数据设置到actor属性上

						//修改Actor上的属性后，调用函数更新Actor的显示
						ISaveInterface::Execute_LoadActor(Actor);
					}
				}
			}			
		}
	}
}

void AAuraGameModeBase::PlayerDied(const ACharacter* DeadCharater) const
{
	//获取存档数据
	const ULoadScreenSaveGame* SaveGame = RetrieveInGameSaveData();
	if(!IsValid(SaveGame)) return;

	//通过地图命名打开地图
	UGameplayStatics::OpenLevelBySoftObjectPtr(DeadCharater, Maps.FindChecked(SaveGame->MapAssetName));
}

void AAuraGameModeBase::BeginPlay()
{
	Super::BeginPlay();

	/*添加一个默认地图到映射中*/
	Maps.Add(DefaultMapName,DefaultMap);
}
