// 由来时路褒贬不一制作

#include "Character/AuraPlayerController.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "NavigationPath.h"
#include "NavigationSystem.h"
#include "NiagaraFunctionLibrary.h"
#include "ShaderPrintParameters.h"
#include "Actor/MagicCircle.h"
#include "Aura_End/Aura_End.h"
#include "Components/DecalComponent.h"
#include "Gas/Player/AbilitySystemComponent/AuraAbilitySystemComponent.h"
#include "Input/AuraInputComponent.h"
#include "Interface/EnemyInterface.h"
#include "Tags/AuraGameplayTags.h"
#include "GameFramework/Character.h"
#include "UI/Widget/DamageTextComponent.h"


AAuraPlayerController::AAuraPlayerController()
{
	bReplicates = true;
    
	Spline = CreateDefaultSubobject<USplineComponent>("Spline");
}

void AAuraPlayerController::ShowMagicCircle(UMaterialInterface* DeaclMaterial)
{
	/*显示魔法圈在光标位置*/
	if (!IsValid(MagicCircle)) MagicCircle = GetWorld()->SpawnActor<AMagicCircle>(MagicCircleClass);

	/*设置贴画材质*/
	if (DeaclMaterial != nullptr)
	{
		MagicCircle->MagicCircle->SetMaterial(0, DeaclMaterial);
	}
}

void AAuraPlayerController::HideMagicCircle() const
{
	if (IsValid(MagicCircle))
	{
		MagicCircle->Destroy();
	}
}

void AAuraPlayerController::ShowDamageNumber_Implementation(float DamageAmount, ACharacter* TargerCharacter,bool IsBlockHit,bool IsCriticalHit)
{
	if (IsValid(TargerCharacter) && DamageAmount)
	{
		UDamageTextComponent* DamageText = NewObject<UDamageTextComponent>(TargerCharacter, DamageTextComponentClass);
		DamageText->RegisterComponent();
		DamageText->AttachToComponent(TargerCharacter->GetRootComponent(),FAttachmentTransformRules::KeepRelativeTransform);
		DamageText->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
		DamageText->SetDamageText(DamageAmount,IsBlockHit,IsCriticalHit);
	}
}

void AAuraPlayerController::BeginPlay()
{
	Super::BeginPlay();

	check(AuraContext)
	//使用ULocalPlayer里面的GetSubsystem节点获取UEnhancedInputLocalPlayerSubsystem类型的本地玩家控制器GetLocalPlayer()
	UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());
	if(Subsystem)
	{
		Subsystem->AddMappingContext(AuraContext,0);
	}

	//在该模式下设置光标，在游戏开始时显示光标，样式为默认。
	bShowMouseCursor = true;
	DefaultMouseCursor = EMouseCursor::Default;

	//设置光标不会锁定到视口，同时当光标在视口上方，显示光标。
	FInputModeGameAndUI InputModeData;
	InputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	InputModeData.SetHideCursorDuringCapture(false);
	SetInputMode(InputModeData);

	
}

void AAuraPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	//虚幻引擎默认启用的是输入组件，需要把输入组件转换成增强输入组件（增强输入组件是继承自输入组件的）
    UAuraInputComponent* AuraInputComponent = CastChecked<UAuraInputComponent>(InputComponent);
	//给增强输入绑定一个回调函数Move，当MoverAction有值时就会激发回调函数（ETriggerEvent::Triggered此参数是触发方式）
	AuraInputComponent->BindAction(MoveAction,ETriggerEvent::Triggered,this,&AAuraPlayerController::Move);
	/*Bind Shift Activate Firebolt*/
	AuraInputComponent->BindAction(ShiftAction,ETriggerEvent::Started,this,&AAuraPlayerController::ShiftPressed);
	AuraInputComponent->BindAction(ShiftAction,ETriggerEvent::Completed,this,&AAuraPlayerController::ShiftReleased);
    /*绑定自定义回调函数，当有输入操作触发回调函数 *ActivateGA**/
	AuraInputComponent->BindAbilityAction(AuraInputConfig,this,&ThisClass::AbilityInputTagPressed,&ThisClass::AbilityInputTagReleased,&ThisClass::AbilityInputTagHold);
	
}

void AAuraPlayerController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);
	CursorTrace();
	/**MouseMove*/
	AutoMove();
	/*更新魔法圈位置*/
	UpdataMagicCircleLocation();
}

void AAuraPlayerController::AutoMove()
{
	
	if(!bAutoRunning) return;
	if (APawn* AuraPawn = GetPawn())
	{
		const FVector LocationOnSpline = Spline->FindLocationClosestToWorldLocation(AuraPawn->GetActorLocation(),ESplineCoordinateSpace::World);
		const FVector DirectOnSpline = Spline->FindDirectionClosestToWorldLocation(LocationOnSpline,ESplineCoordinateSpace::World);
		AuraPawn->AddMovementInput(DirectOnSpline);
		
		const float DistanceToDestination = (LocationOnSpline-CachedDestination).Length();
		if (DistanceToDestination <= AutoRunAcceptanceRadius)
		{
			bAutoRunning = false;
		}
	}
	
}

void AAuraPlayerController::UpdataMagicCircleLocation() const
{
	if (IsValid(MagicCircle))
	{
		MagicCircle->SetActorLocation(HitResult.ImpactPoint);
	}
}

//通过cursor光标重叠，高亮显示敌人
void AAuraPlayerController::CursorTrace()
{
	if (GetASC() && GetASC()->HasMatchingGameplayTag(FMyGameplayTags::Get().Player_Block_CursorTrace))
	{
		/*如果有些技能触发，需要阻止光标射线监测，在此通过标签即可阻止,同时清除相关操作*/
		/*鼠标拾取比较费性能，在不需要时候阻止*/
		UnHighHlightActor(LastActor);
		UnHighHlightActor(ThisActor);
		ThisActor =nullptr;
		LastActor =nullptr;
		return;
	}

	/*应用范围攻击技能瞄准时不需要索敌，这里通过通道排除*/
	const ECollisionChannel TraceChannel = IsValid(MagicCircle) ? ECC_ExcludePlayer : ECC_Visibility;
	GetHitResultUnderCursor(TraceChannel, false, HitResult); 
	if (!HitResult.bBlockingHit) return;
	
    LastActor = ThisActor;
	//获取拾取的Actor，判断Actor是否继承高亮接口
	if(IsValid(HitResult.GetActor()) && HitResult.GetActor()->Implements<UHighLightInterface>())
	{
		ThisActor = HitResult.GetActor();
	}
	else
	{
		ThisActor = nullptr;
	}
    //以下是几种情况
	if (LastActor != ThisActor)
	{
		HighHlightActor(ThisActor);
		UnHighHlightActor(LastActor);
	}
}

void AAuraPlayerController::HighHlightActor(AActor* InActor)
{
	if(IsValid(InActor) && InActor->Implements<UHighLightInterface>())
	{
		IHighLightInterface::Execute_HighLightActor(InActor);
	}
}

void AAuraPlayerController::UnHighHlightActor(AActor* InActor)
{
	if(IsValid(InActor) && InActor->Implements<UHighLightInterface>())
	{
		IHighLightInterface::Execute_UnHighLightActor(InActor);
	}
}

void AAuraPlayerController::AbilityInputTagPressed(FGameplayTag InputTag)/*按下*/
{
	if (GetASC() && GetASC()->HasMatchingGameplayTag(FMyGameplayTags::Get().Player_Block_InputPresseed))
	{
		/*如果有些技能触发，需要阻止按键按下动作，在此通过标签即可阻止*/
		return;
	}
	
	//GEngine->AddOnScreenDebugMessage(1,2,FColor::Red,*InputTag.ToString());
	if (InputTag.MatchesTagExact(FMyGameplayTags::Get().InputTag_LMB))
	{
		if (IsValid(ThisActor))
		{
			TargetingStatus = ThisActor->Implements<UEnemyInterface>() ? ETargetingStatus::TargetingEnemy : ETargetingStatus::TargetingNonEnemy;
		}
		else
		{
			TargetingStatus = ETargetingStatus::NotTargeting;
		}
		bAutoRunning = false;
		FollowTime = 0.f;
	}
	/*如果按钮按下调用ASC中，专注于雷电技能，前置瞄准阶段触发*/
	if (GetASC()) GetASC()->AbilityInputTagPressed(InputTag);
}

void AAuraPlayerController::AbilityInputTagReleased(FGameplayTag InputTag)/*释放*/
{

	if (GetASC() && GetASC()->HasMatchingGameplayTag(FMyGameplayTags::Get().Player_Block_InputReleased))
	{
		/*如果有些技能触发，需要阻止按键释放动作，在此通过标签即可阻止*/
		return;
	}
	
	if(!InputTag.MatchesTagExact(FMyGameplayTags::Get().InputTag_LMB))
	{
		/*我们通过ASC里面的函数激活GA所以要先获取ASC，但是不能确定是否为空指针，所以先检查*/
		if (GetASC()) GetASC()->AbilityAssetTagReleased(InputTag);
		return;
	}
	if (GetASC()) GetASC()->AbilityAssetTagReleased(InputTag);
	if (TargetingStatus != ETargetingStatus::TargetingEnemy && !bShiftKeyDown)
	{
		const APawn* ControllerPawn = GetPawn();
			if (FollowTime <= shortpressThreshold && ControllerPawn)
			{

				/*当点击的是检查点时把目标位置设置成检查点*/
				if (IsValid(ThisActor) && ThisActor->Implements<UHighLightInterface>())
				{
					IHighLightInterface::Execute_SetMoveToLocation(ThisActor,CachedDestination);
				}
				else
				{
					UNiagaraFunctionLibrary::SpawnSystemAtLocation(this,ClickNiagaraComponent,CachedDestination);
				}
				
				/*虚幻引擎内置自动寻路避障插件，返回路径*/
				if (UNavigationPath* NavPat = UNavigationSystemV1::FindPathToLocationSynchronously(this,ControllerPawn->GetActorLocation(),CachedDestination))
				{
					Spline->ClearSplinePoints();
					for (const FVector& PathPoint : NavPat->PathPoints)
					{
						Spline->AddSplinePoint(PathPoint,ESplineCoordinateSpace::World);
						DrawDebugSphere(GetWorld(),PathPoint,10,10,FColor::White,false,5.f);
					}
					if (NavPat->PathPoints.Num() > 0)
					{
						CachedDestination = NavPat->PathPoints[NavPat->PathPoints.Num()-1];/*把导航点减1是为了解决有的地方被遮挡无法到达导致人物无限移动*/
						bAutoRunning = true;
					}
        				FollowTime = 0.f;
						TargetingStatus = ETargetingStatus::NotTargeting;
				}
				if (GetASC() && !GetASC()->HasMatchingGameplayTag(FMyGameplayTags::Get().Player_Block_CursorTrace))
				{
					/*如果有些技能触发，需要阻止光标特效，这里通过标签阻止，当有标签阻止执行光标特效*/
					/*在鼠标点击地点生成特效*/
					UNiagaraFunctionLibrary::SpawnSystemAtLocation(this,ClickNiagaraComponent,CachedDestination);
				}
			}
	}
}

void AAuraPlayerController::AbilityInputTagHold(FGameplayTag InputTag)/*长按*/
{
	if (GetASC() && GetASC()->HasMatchingGameplayTag(FMyGameplayTags::Get().Player_Block_InputHold))
	{
		/*如果有些技能触发，需要阻止按键长按动作，在此通过标签即可阻止*/
		return;
	}
	
	if (!InputTag.MatchesTagExact(FMyGameplayTags::Get().InputTag_LMB))
	{
		if (GetASC()) GetASC()->AbilityInputTagHold(InputTag);
		return;
	}
	if (TargetingStatus == ETargetingStatus::TargetingEnemy || bShiftKeyDown)
	{
		if (GetASC()) GetASC()->AbilityInputTagHold(InputTag);
	
	}
	else
	{
	    FollowTime += GetWorld()->GetDeltaSeconds();
    	if (HitResult.bBlockingHit)
    		CachedDestination = HitResult.ImpactPoint;
    	
    	if (APawn* ControlledPawn = GetPawn())
    	{
    		const FVector WordDirection = (CachedDestination - ControlledPawn->GetActorLocation()).GetSafeNormal();
            ControlledPawn->AddMovementInput(WordDirection);
    	}	
	}
	
}

UAuraAbilitySystemComponent* AAuraPlayerController::GetASC()
{
	if(AuraASC == nullptr)
	{
		AuraASC = Cast<UAuraAbilitySystemComponent>(UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetPawn()));
	}
	return AuraASC;
}



void AAuraPlayerController::Move(const struct FInputActionValue& InputActionValue)
{
	if (GetASC() && GetASC()->HasMatchingGameplayTag(FMyGameplayTags::Get().Player_Block_InputPresseed))
	{
		/*如果有些技能触发，需要阻止人物移动，在此通过标签即可阻止*/
		return;
	}
	
	//分别获取二维值和旋转，然后新建一个旋转参数（俯视角镜头）
	const FVector2D InputAxisVector = InputActionValue.Get<FVector2D>();
	const FRotator Rotation = GetControlRotation();
	const FRotator YawRotation(0.f,Rotation.Yaw,0.f);

	//从旋转中分离出来x和Y轴5方向
	const FVector ForWardDirecction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

	//如果成功获得人物，把值返回给人物移动
	if (APawn* controlledPawn = GetPawn<APawn>())
	{
		controlledPawn->AddMovementInput(ForWardDirecction,InputAxisVector.Y);
		controlledPawn->AddMovementInput(RightDirection,InputAxisVector.X);
	}
	
}

