// 由来时路褒贬不一制作


#include "Gas/Ability/AuraSummonAbility.h"

#include "Kismet/KismetSystemLibrary.h"

TArray<FVector> UAuraSummonAbility::GetSpawnLocations()
{
	/*获取召唤师的朝向和位置，并创建根据玩家数量均分角度*/
	const FVector Forward = GetAvatarActorFromActorInfo()->GetActorForwardVector();
	const FVector Location = GetAvatarActorFromActorInfo()->GetActorLocation();
	const float DelatSpread = SpawnSpread / NumMinions;

	/*获取召唤敌人射线起始角度并绘制测试射线*/
	const FVector LeftOfSpread = Forward.RotateAngleAxis(-SpawnSpread / 2.f,FVector::UpVector);
	UKismetSystemLibrary::DrawDebugArrow(GetAvatarActorFromActorInfo(),Location,Location + MaxSpawnDistance * LeftOfSpread,4.f,FLinearColor::Red,4.f);

	/*创建一个循环根据生成玩家数量进行均分，并随机生成位置*/
	TArray<FVector> SpawnActorLocation;
	for (int32 i = 0; i < NumMinions; i++)
	{
		const FVector Direction = LeftOfSpread.RotateAngleAxis(DelatSpread * i,FVector::UpVector);
		FVector SpawnLocation = Location + Direction * FMath::RandRange(MinSpawnDistance,MaxSpawnDistance);
		UKismetSystemLibrary::DrawDebugArrow(GetAvatarActorFromActorInfo(),Location,Location + MaxSpawnDistance * Direction,4.f,FLinearColor::Red,4.f);

		/*召唤时扇形区域可能在斜坡上，在随机生成点加一个z轴的射线检测，与地面重合的点设置为召唤点*/
		FHitResult Hit;
		GetWorld()->LineTraceSingleByChannel(Hit,SpawnLocation + FVector(0.f,0.f,200.f),SpawnLocation - FVector(0.f,0.f,200.f),ECC_Visibility);
		if (Hit.bBlockingHit)
		{
			SpawnLocation = Hit.ImpactPoint;
			SpawnActorLocation.Add(SpawnLocation);
		}
	}
	
	return SpawnActorLocation;
}

TSubclassOf<APawn> UAuraSummonAbility::GetRandomMinionsClass()
{
	const int32 Selection = FMath::RandRange(0,MinionsClasses.Num()-1);
	return MinionsClasses[Selection];
}
