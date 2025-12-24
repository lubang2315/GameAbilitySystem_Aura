// 由来时路褒贬不一制作


#include "Gas/Ability/AuraFireBolt.h"

#include "Gas/FunctionLibrary/MyFunctionLibrary.h"
#include "Actor/Auraprojectile.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Tags/AuraGameplayTags.h"

FString UAuraFireBolt::GetDescription(int32 Level)
{
	const int32 ScaledDamage = Damage.GetValueAtLevel(Level);
	const float ManaCost = FMath::Abs(GetManaCost(Level));
	const float Cooldown = GetCoolDown(Level);
	if (Level == 1)
	{
		return FString::Printf(TEXT(
			// Title
			"<Title>FIRE BOLT</>\n\n"

			// Level
			"<Small>Level: </><Level>%d</>\n"
			// ManaCost
			"<Small>ManaCost: </><ManaCost>%.1f</>\n"
			// Cooldown
			"<Small>Cooldown: </><Cooldown>%.1f</>\n\n"
			
			"<Default>Launches a bolt of fire, "
			"exploding on impact and dealing: </>"

			// Damage
			"<Damage>%d</><Default> fire damage with"
			" a chance to burn</>"),

			// Values
			Level,
			ManaCost,
			Cooldown,
			ScaledDamage);
	}
	else
	{
		return FString::Printf(TEXT(
			// Title
			"<Title>FIRE BOLT</>\n\n"

			// Level
			"<Small>Level: </><Level>%d</>\n"
			// ManaCost
			"<Small>ManaCost: </><ManaCost>%.1f</>\n"
			// Cooldown
			"<Small>Cooldown: </><Cooldown>%.1f</>\n\n"

			// Number of FireBolts
			"<Default>Launches %d bolts of fire, "
			"exploding on impact and dealing: </>"

			// Damage
			"<Damage>%d</><Default> fire damage with"
			" a chance to burn</>"),

			// Values
			Level,
			ManaCost,
			Cooldown,
			FMath::Min(Level, MaxProjectile),
			ScaledDamage);		
	}
}

FString UAuraFireBolt::GetNextLevelDescription(int32 Level)
{
	const int32 ScaledDamage = Damage.GetValueAtLevel(Level);
	const float ManaCost = FMath::Abs(GetManaCost(Level));
	const float Cooldown = GetCoolDown(Level);
	return FString::Printf(TEXT(
			// Title
			"<Title>NEXT LEVEL: </>\n\n"

			// Level
			"<Small>Level: </><Level>%d</>\n"
			// ManaCost
			"<Small>ManaCost: </><ManaCost>%.1f</>\n"
			// Cooldown
			"<Small>Cooldown: </><Cooldown>%.1f</>\n\n"

			// Number of FireBolts
			"<Default>Launches %d bolts of fire, "
			"exploding on impact and dealing: </>"

			// Damage
			"<Damage>%d</><Default> fire damage with"
			" a chance to burn</>"),

			// Values
			Level,
			ManaCost,
			Cooldown,
			FMath::Min(Level, MaxProjectile),
			ScaledDamage);
}

void UAuraFireBolt::AuraSpawnProjectile(const FVector& ProjectileTargetLocation, const FGameplayTag& SocketTag,const bool bOverriderPitch, const float PitchOverrider, AActor* HomingTag)
{
	/*判断此函数是否在服务器运行*/
	const bool bIsServer = GetAvatarActorFromActorInfo()->HasAuthority();
	if (!bIsServer) return;
	if (GetAvatarActorFromActorInfo()->Implements<UCombotInterface>())
	{
		int32 NumProjectiles = FMath::Min(MaxProjectile,GetAbilityLevel());
		/*获取火球释放位置*/
		const FVector SocketLocation = ICombotInterface::Execute_GetCombatSocketLocation(GetAvatarActorFromActorInfo(), SocketTag);
		FRotator Rotation = (ProjectileTargetLocation - SocketLocation).Rotation();
		if (bOverriderPitch) Rotation.Pitch = PitchOverrider;//覆写发射角度
		
		TArray<FRotator> Rotators = UMyFunctionLibrary::EvenlySpacedRotators(Rotation.Vector(),FVector::UpVector,ProjectileSpread,NumProjectiles);
		
		for (FRotator& Rotator : Rotators)
		{
			/*Set投射物角度*/
			FTransform SpawnTransform;
			SpawnTransform.SetLocation(SocketLocation);
			SpawnTransform.SetRotation(Rotator.Quaternion());
      
		
			AAuraprojectile* Projectile = GetWorld()->SpawnActorDeferred<AAuraprojectile>(
				ProjectileSpellClass,
				SpawnTransform,
				GetOwningActorFromActorInfo(),
				Cast<APawn>(GetAvatarActorFromActorInfo()),
					ESpawnActorCollisionHandlingMethod::AlwaysSpawn);

			Projectile->DamageEffectParams = MakeDamageEffectPramsFromClassDefault();

			/*设置飞弹朝向目标*/
			if (HomingTag && HomingTag->Implements<UCombotInterface>())
			{
				/*瞄准敌人根骨骼*/
				Projectile->ProjectileMovement->HomingTargetComponent = HomingTag->GetRootComponent();
			}
			else
			{
				/*因为目标不再是敌人而是环江中物体，而HomingTargetComponent该函数只接受场景组件类型的坐标，因为追踪一个物体需要知道相对场景的坐标，所以这里创建一个ProjectileTargetLocation的场景坐标*/
				Projectile->HomingTargetSceneCompoment = NewObject<USceneComponent>(USceneComponent::StaticClass());
				Projectile->HomingTargetSceneCompoment->SetWorldLocation(ProjectileTargetLocation);
				Projectile->ProjectileMovement->HomingTargetComponent = Projectile->HomingTargetSceneCompoment;
			}

			/*设置飞弹朝向目标加速度为自定义范围内随机数*/
			Projectile->ProjectileMovement->HomingAccelerationMagnitude = FMath::RandRange(HomingAccelerationMin,HomingAccelectionMax);
			/*是否开启追踪*/
			Projectile->ProjectileMovement->bIsHomingProjectile = bLunchHomingProjectiles;
			
			Projectile->FinishSpawning(SpawnTransform);
		}
	}
}
