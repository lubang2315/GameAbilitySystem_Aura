// 由来时路褒贬不一制作


#include "Gas/Ability/AuraBeamSpell.h"
#include "GameFramework/Character.h"
#include "Gas/FunctionLibrary/MyFunctionLibrary.h"
#include "Kismet/KismetSystemLibrary.h"

void UAuraBeamSpell::StoreMouseDataInfo(const FHitResult& hitResult)
{
	/*判断当前是否拾取到内容*/
	if (hitResult.bBlockingHit)
	{
		MouseHitLocation = hitResult.ImpactPoint;
		MouseHitActor = hitResult.GetActor();
	}
	else
	{
		/*如果没有命中取消技能*/
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
	}
}

void UAuraBeamSpell::StoreOwnerVariables()
{
	if (CurrentActorInfo)
	{
		OwnerPlayerController = CurrentActorInfo->PlayerController.Get();
		/*这里不存储AvatarActor的原因，他只是我们控制的在世界中存在的实例，而转化成character我们就可以获取到虚幻引擎为我们写好的一些函数，诸如运动组件，GetMesh，播放蒙太奇等*/
		OwnerCharacter = Cast<ACharacter>(CurrentActorInfo->AvatarActor);
	}
}

void UAuraBeamSpell::TraceFirstTarget(const FVector& BeamTargetLocation)
{
	/*判断是否实现接口*/
	if (OwnerCharacter && OwnerCharacter->Implements<UCombotInterface>())
	{
		/*获取武器插槽，作为光束发起点*/
		if (USkeletalMeshComponent* Weapon = ICombotInterface::Execute_GetWeapon(OwnerCharacter))
		{
			/*添加阻挡光束的忽略对象*/
			TArray<AActor*> ActorToIgnore;
			ActorToIgnore.Add(OwnerCharacter);

			/*获取武器插槽位置*/
			const FVector StartLocation = Weapon->GetSocketLocation(FName("TipSocket"));

			/*第一个阻挡光束的对象*/
			FHitResult HitResult;

			/*在武器插槽发射一个射线检测轮廓是球体，光束结束点是鼠标点击点，并返回第一个阻挡对象HitResult*/
			UKismetSystemLibrary::SphereTraceSingle(
				OwnerCharacter,
				StartLocation,
				BeamTargetLocation,
				10.f,
				TraceTypeQuery1,
				false,
				ActorToIgnore,
				EDrawDebugTrace::ForDuration,
				HitResult,
				true);

			/*把命中结果赋值给传进来的变量*/
			if (HitResult.bBlockingHit)
			{
				MouseHitLocation = HitResult.ImpactPoint;
				MouseHitActor = HitResult.GetActor();
			}
		}
	}
	/*绑定主要敌人死亡通知*/
	if (ICombotInterface* CombotInterface = Cast<ICombotInterface>(MouseHitActor))
	{
		/*判断是否已经绑定了委托回调*/
		if (!CombotInterface->GetDeath().IsAlreadyBound(this,&UAuraBeamSpell::PrimaryTargetDied))
		{
			/*如果未绑定动态绑定*/
			CombotInterface->GetDeath().AddDynamic(this,&UAuraBeamSpell::PrimaryTargetDied);
		}
	}
}

void UAuraBeamSpell::StoreAdditionalTarget(TArray<AActor*>& OutAdditionalTarget)
{
	/*首先添加要忽略的对象*/
	TArray<AActor*> ActorToIgnore;
	ActorToIgnore.Add(OwnerCharacter);
	ActorToIgnore.Add(MouseHitActor);

	/*暂存范围内所有对象*/
	TArray<AActor*> OverlappingActors;

	/*获取范围内的对象*/
	UMyFunctionLibrary::GetLivePlayersWithinRadius(
		GetOwningActorFromActorInfo(),
		850.f,
		OverlappingActors,
		ActorToIgnore,
		MouseHitActor->GetActorLocation());

	/*限定捕获最大敌人数量*/
	int32 NumActors = FMath::Min(GetAbilityLevel() - 1, MaxNumShockTargets);
	//int32 NumActors = 5;

	/*根据要求筛选距离最近的Actor*/
	UMyFunctionLibrary::GetClosestTargets(NumActors,MouseHitActor->GetActorLocation(),OverlappingActors,OutAdditionalTarget);

	/*动态绑定次要敌人死亡通知*/
	for (auto Actor : OutAdditionalTarget)
	{
		if (ICombotInterface* CombotInterface = Cast<ICombotInterface>(Actor))
		{
			/*判断是否已经绑定了委托回调*/
			if (!CombotInterface->GetDeath().IsAlreadyBound(this,&UAuraBeamSpell::AdditionalTargetDied))
			{
				/*如果未绑定动态绑定*/
				CombotInterface->GetDeath().RemoveDynamic(this,&UAuraBeamSpell::AdditionalTargetDied);
			}
		}
	}
	
}
void UAuraBeamSpell::OnEndAbility(TArray<AActor*> AdditionalTarget)
{
	/*取消绑定主要敌人死亡通知*/
	if (ICombotInterface* CombotInterface = Cast<ICombotInterface>(MouseHitActor))
	{
			/*如果未绑定动态绑定*/
			CombotInterface->GetDeath().AddDynamic(this,&UAuraBeamSpell::PrimaryTargetDied);
	}
	/*取消动态绑定次要敌人死亡通知*/
	for (auto Actor : AdditionalTarget)
	{
		if (ICombotInterface* CombotInterface = Cast<ICombotInterface>(Actor))
		{
				/*如果未绑定动态绑定*/
				CombotInterface->GetDeath().RemoveDynamic(this,&UAuraBeamSpell::AdditionalTargetDied);
			
		}
	}
}


