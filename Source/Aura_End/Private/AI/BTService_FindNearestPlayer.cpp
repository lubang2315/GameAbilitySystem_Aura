// 由来时路褒贬不一制作


#include "AI/BTService_FindNearestPlayer.h"

#include "AIController.h"
#include "BehaviorTree/BTFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"


void UBTService_FindNearestPlayer::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	APawn* OwningPawn = AIOwner->GetPawn();

	FName TArgetTag = OwningPawn->ActorHasTag("Player") ? FName("Enemy") : FName("Player");

	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsWithTag(OwningPawn,TArgetTag,FoundActors);

	/*设置float默认值为无限大*/
	float ClosestDistance = TNumericLimits<float>::Max();
	AActor* TargetActor = nullptr;

	for (AActor* FoundActor : FoundActors)
	{

		const float MinDistance = OwningPawn->GetDistanceTo(FoundActor);
		if (IsValid(FoundActor) && IsValid(OwningPawn))
		{
			if (MinDistance < ClosestDistance)
            		{
            			ClosestDistance = MinDistance;
            			TargetActor = FoundActor;
            		}
		}
		
		
	}

	UBTFunctionLibrary::SetBlackboardValueAsObject(this,TargetToFollowSelector,TargetActor);
	UBTFunctionLibrary::SetBlackboardValueAsFloat(this,DistanceToTargetSelector,ClosestDistance);
	
}
