// 由来时路褒贬不一制作

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "Interface/CombotInterface.h"
#include "TargetDataUnderMouse.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMouseTargetDataSignature,const FGameplayAbilityTargetDataHandle&, DataHandle);
/**
 * 
 */
UCLASS()
class AURA_END_API UTargetDataUnderMouse : public UAbilityTask,public ICombotInterface
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Abilities|Tasks",meta=(DisplayName = "TargetDataUnderMouse",HidePin = "OwningAbility",DefaultToSelf = "OwningAbility",BlueprintInternalUseOnly = "true"))
	static  UTargetDataUnderMouse* CreateTargetDataUnderMouse(UGameplayAbility* OwningAbility);

	UPROPERTY(BlueprintAssignable)
	FMouseTargetDataSignature ValidData;

	private:
	virtual void Activate() override;

	/*向客户端提交数据*/
	void SendMouseCursorData();
	
	/*当数据提交到服务器端后的委托回调*/
	void OnTargetDataReplicatedCallback(const FGameplayAbilityTargetDataHandle& DataHandel,FGameplayTag ActivationTag);


};
