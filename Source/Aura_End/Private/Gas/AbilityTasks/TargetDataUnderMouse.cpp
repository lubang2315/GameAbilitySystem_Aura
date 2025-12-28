 // 由来时路褒贬不一制作


#include "Gas/AbilityTasks/TargetDataUnderMouse.h"

#include "AbilitySystemComponent.h"
#include "Aura_End/Aura_End.h"


 UTargetDataUnderMouse* UTargetDataUnderMouse::CreateTargetDataUnderMouse(UGameplayAbility* OwningAbility)
 {
  UTargetDataUnderMouse* Myobj = NewAbilityTask<UTargetDataUnderMouse>(OwningAbility);
  return Myobj;
 }

 void UTargetDataUnderMouse::Activate()/*技能激活入口*/
 {
  const bool bIsLocallyControlled = Ability->GetCurrentActorInfo()->IsLocallyControlled();
  if (bIsLocallyControlled)
  {
   SendMouseCursorData();/*如果是客户端触发就调用他*/
  }
  else
  {
   /*不是话，就是服务器端，服务器端要做的就是接受数据，写法如下*/
   const FGameplayAbilitySpecHandle SpecHandle = GetAbilitySpecHandle();
   const FPredictionKey ActivationPredictionKey = GetActivationPredictionKey();
   AbilitySystemComponent.Get()->AbilityTargetDataSetDelegate(SpecHandle, ActivationPredictionKey).AddUObject(this, &UTargetDataUnderMouse::OnTargetDataReplicatedCallback);

   /*判断在服务器端，上面的委托是否已经广播过*/
   const bool bCalledDelegate = AbilitySystemComponent.Get()->CallReplicatedTargetDataDelegatesIfSet(SpecHandle,ActivationPredictionKey);
   if (!bCalledDelegate)
   {
    /**/
    SetWaitingOnRemotePlayerData();
   }

  }
 }

 void UTargetDataUnderMouse::SendMouseCursorData()
 {
  /*创建一个预测窗口允许客户端预测操作*/
  FScopedPredictionWindow ScopedPrediction(AbilitySystemComponent.Get());
  /*获取鼠标下的数据*/
  APlayerController* AuraPC = Ability->GetCurrentActorInfo()->PlayerController.Get();
  FHitResult CurssortHit;
  AuraPC->GetHitResultUnderCursor(ECC_Target,false,CurssortHit);
  DrawDebugSphere(GetWorld(), CurssortHit.Location, 300, 100, FColor::Red);
 

  
  /*创建需要上传服务器端的TargetData*/
  FGameplayAbilityTargetData_SingleTargetHit* Data = new FGameplayAbilityTargetData_SingleTargetHit();
  Data->HitResult = CurssortHit;

  /*创建TargetData句柄，因为只支持句柄所以在打包成句斌上传*/
  FGameplayAbilityTargetDataHandle DataHandle;
  DataHandle.Add(Data);
  /*上传到服务器*/
  AbilitySystemComponent->ServerSetReplicatedTargetData(
  GetAbilitySpecHandle(),
  GetActivationPredictionKey(),
  DataHandle,
  FGameplayTag(),
  AbilitySystemComponent->ScopedPredictionKey
  );

  /*判断服务器是否通过验证，验证通过本地可以直接显示预测结果*/
  if (ShouldBroadcastAbilityTaskDelegates())
  {
   ValidData.Broadcast(DataHandle);
  }
 }

 void UTargetDataUnderMouse::OnTargetDataReplicatedCallback(const FGameplayAbilityTargetDataHandle& DataHandel,
  FGameplayTag ActivationTag)
 {
  /*通知客户端，服务器已经接受并处理了从客户端复制的目标数据（将服务器的Target应用到客户端，并清除缓存）*/
AbilitySystemComponent->ConsumeClientReplicatedTargetData(GetAbilitySpecHandle(),GetActivationPredictionKey());

  /*判断服务器端是否通过验证，通过广播给其他客户端和服务器显示*/
  if (ShouldBroadcastAbilityTaskDelegates())
  {
   ValidData.Broadcast(DataHandel);
  }
 }
