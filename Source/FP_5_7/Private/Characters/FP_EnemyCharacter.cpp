// Copyright JG

#include "Characters/FP_EnemyCharacter.h"
#include "AbilitySystem/FP_AbilitySystemComponent.h"
#include "AbilitySystem/FP_AttributeSet.h"
#include "Components/WidgetComponent.h"
#include "FP_5_7/FP_5_7.h"
#include "Libraries/FP_AbilitySystemLibrary.h"
#include "FP_GameplayTags.h"
#include "AI/FP_AIController.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "UI/Widget/FP_UserWidget.h"

static void FP_DebugAttr(UWorld* World, const FString& Label, float OldValue, float NewValue, const FColor& Color, float Time = 2.0f)
{
	if (GEngine && World)
	{
		GEngine->AddOnScreenDebugMessage(
			-1,
			Time,
			Color,
			FString::Printf(TEXT("[Enemy ASC] %s: Old=%.2f  New=%.2f"), *Label, OldValue, NewValue)
		);
	}
}

// Sets default values
AFP_EnemyCharacter::AFP_EnemyCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	GetMesh()->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);

	AbilitySystemComponent = CreateDefaultSubobject<UFP_AbilitySystemComponent>("AbilitySystemComponent");
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Minimal);
	
	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationYaw = false;
	GetCharacterMovement()->bUseControllerDesiredRotation = true;

	AttributeSet = CreateDefaultSubobject<UFP_AttributeSet>("AttributeSet");

	HP_HeatBar = CreateDefaultSubobject<UWidgetComponent>("HP_HeatBar");
	HP_HeatBar->SetupAttachment(GetRootComponent());
}

void AFP_EnemyCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	
	if (!HasAuthority()) return;
	FP_AIController = Cast<AFP_AIController>(NewController);
	FP_AIController->GetBlackboardComponent()->InitializeBlackboard(*BehaviorTree->BlackboardAsset);
	FP_AIController->RunBehaviorTree(BehaviorTree);
	FP_AIController->GetBlackboardComponent()->SetValueAsBool(FName("HitReacting"), false);
	FP_AIController->GetBlackboardComponent()->SetValueAsBool(FName("RangedAttacker"), CharacterClass != ECharacterClass::Warrior);
	
}

void AFP_EnemyCharacter::HighlightActor()
{
	GetMesh()->SetRenderCustomDepth(true);
	GetMesh()->SetCustomDepthStencilValue(CUSTOM_DEPTH_RED);
	Weapon->SetRenderCustomDepth(true);
	Weapon->SetCustomDepthStencilValue(CUSTOM_DEPTH_RED);
}

void AFP_EnemyCharacter::UnHighlightActor()
{
	GetMesh()->SetRenderCustomDepth(false);
	Weapon->SetRenderCustomDepth(false);
}

int32 AFP_EnemyCharacter::GetPlayerLevel()
{
	return Level;
}

void AFP_EnemyCharacter::Die()
{
	const float Extra = GetDeathRagdollDelay(); // protected in base; if private, expose a protected getter
	SetLifeSpan(LifeSpan + Extra);
	Super::Die();
}

void AFP_EnemyCharacter::SetCombatTarget_Implementation(AActor* InCombatTarget)
{
	CombatTarget = InCombatTarget;
}

AActor* AFP_EnemyCharacter::GetCombatTarget_Implementation() const
{
	return CombatTarget;
}

void AFP_EnemyCharacter::HitReactTagChanged(const FGameplayTag CallbackTag, int32 NewCount)
{
	SetHitReactState(NewCount > 0);
}

void AFP_EnemyCharacter::BeginPlay()
{
	Super::BeginPlay();
	GetCharacterMovement()->MaxWalkSpeed = BaseWalkSpeed;

	// 1) Init ActorInfo (DO NOT apply defaults inside InitAbilityActorInfo anymore)
	InitAbilityActorInfo();
	UFP_AbilitySystemLibrary::GiveStartupAbilities(this, AbilitySystemComponent, CharacterClass);

	// 2) Set widget controller
	if (UFP_UserWidget* FP_UserWidget = Cast<UFP_UserWidget>(HP_HeatBar->GetUserWidgetObject()))
	{
		FP_UserWidget->SetWidgetController(this);
	}

	// 3) Bind delegates BEFORE we apply defaults
	BindAttributeDelegates();

	// 4) Apply default attributes on the SERVER (clients will receive via replication)
	if (HasAuthority())
	{
		InitializeDefaultAttributes();
	}

	if (const UFP_AttributeSet* FP_AS=Cast<UFP_AttributeSet>(AttributeSet))
	{
		AbilitySystemComponent->RegisterGameplayTagEvent(FFP_GameplayTags::Get().Effects_HitReact, EGameplayTagEventType::NewOrRemoved).AddUObject(
			this,
			&AFP_EnemyCharacter::HitReactTagChanged
		);
	}
	
	AbilitySystemComponent->AbilityActivatedCallbacks.AddUObject(this, &AFP_EnemyCharacter::OnAbilityActivated);
	AbilitySystemComponent->OnAbilityEnded.AddUObject(this, &AFP_EnemyCharacter::OnAbilityEnded);



	// 5) Push initial values so UI is correct immediately (even if initial changes were missed)
	BroadcastInitialAttributeValues();
}

void AFP_EnemyCharacter::InitAbilityActorInfo()
{
	AbilitySystemComponent->InitAbilityActorInfo(this, this);
	Cast<UFP_AbilitySystemComponent>(AbilitySystemComponent)->AbilityActorInfoSet();

	// IMPORTANT:
	// Do NOT call InitializeDefaultAttributes() here.
	// We bind delegates in BeginPlay(), then apply defaults, then broadcast initial values.
}

void AFP_EnemyCharacter::InitializeDefaultAttributes() const
{
	UFP_AbilitySystemLibrary::InitializeDefaultAttributes(this, CharacterClass, Level, AbilitySystemComponent);
}

void AFP_EnemyCharacter::BindAttributeDelegates()
{
	const UFP_AttributeSet* FP_AS = Cast<UFP_AttributeSet>(AttributeSet);
	if (!FP_AS || !AbilitySystemComponent) return;

	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(FP_AS->GetHitPointsAttribute()).AddLambda(
		[this](const FOnAttributeChangeData& Data)
		{
			//FP_DebugAttr(GetWorld(), TEXT("HitPoints"), Data.OldValue, Data.NewValue, FColor::Green);
			OnHitPointsChanged.Broadcast(Data.NewValue);
		});

	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(FP_AS->GetMaxHitPointsAttribute()).AddLambda(
		[this](const FOnAttributeChangeData& Data)
		{
			//FP_DebugAttr(GetWorld(), TEXT("MaxHitPoints"), Data.OldValue, Data.NewValue, FColor::Yellow);
			OnMaxHitPointsChanged.Broadcast(Data.NewValue);
		});

	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(FP_AS->GetHeatAttribute()).AddLambda(
		[this](const FOnAttributeChangeData& Data)
		{
			//FP_DebugAttr(GetWorld(), TEXT("Heat"), Data.OldValue, Data.NewValue, FColor::Cyan);
			OnHeatChanged.Broadcast(Data.NewValue);
		});

	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(FP_AS->GetMaxHeatThresholdAttribute()).AddLambda(
		[this](const FOnAttributeChangeData& Data)
		{
			//FP_DebugAttr(GetWorld(), TEXT("MaxHeatThreshold"), Data.OldValue, Data.NewValue, FColor(255, 165, 0)); // orange-ish
			OnMaxHeatThresholdChanged.Broadcast(Data.NewValue);
		});

	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(FP_AS->GetMinHeatThresholdAttribute()).AddLambda(
		[this](const FOnAttributeChangeData& Data)
		{
			//FP_DebugAttr(GetWorld(), TEXT("MinHeatThreshold"), Data.OldValue, Data.NewValue, FColor(128, 0, 128)); // purple-ish
			OnMinHeatThresholdChanged.Broadcast(Data.NewValue);
		});
}

void AFP_EnemyCharacter::BroadcastInitialAttributeValues()
{
	const UFP_AttributeSet* FP_AS = Cast<UFP_AttributeSet>(AttributeSet);
	if (!FP_AS || !AbilitySystemComponent) return;

	const float HP      = AbilitySystemComponent->GetNumericAttribute(FP_AS->GetHitPointsAttribute());
	const float MaxHP   = AbilitySystemComponent->GetNumericAttribute(FP_AS->GetMaxHitPointsAttribute());
	const float Heat    = AbilitySystemComponent->GetNumericAttribute(FP_AS->GetHeatAttribute());
	const float MinHeat = AbilitySystemComponent->GetNumericAttribute(FP_AS->GetMinHeatThresholdAttribute());
	const float MaxHeat = AbilitySystemComponent->GetNumericAttribute(FP_AS->GetMaxHeatThresholdAttribute());

	// Push to UI/controller once (solves "missed initial change" due to bind order)
	OnHitPointsChanged.Broadcast(HP);
	OnMaxHitPointsChanged.Broadcast(MaxHP);
	OnHeatChanged.Broadcast(Heat);
	OnMinHeatThresholdChanged.Broadcast(MinHeat);
	OnMaxHeatThresholdChanged.Broadcast(MaxHeat);

	// 10s summary debug 
	/*if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(
			-1,
			10.0f,
			FColor::White,
			FString::Printf(
				TEXT("[Enemy ASC INIT] HP=%.2f | MaxHP=%.2f | Heat=%.2f | MinHeat=%.2f | MaxHeat=%.2f"),
				HP, MaxHP, Heat, MinHeat, MaxHeat
			)
		);
	}*/
}

void AFP_EnemyCharacter::OnAbilityActivated(UGameplayAbility* ActivatedAbility)
{
	if (!ActivatedAbility)
	{
		return;
	}

	if (ActivatedAbility->AbilityTags.HasTagExact(FFP_GameplayTags::Get().Effects_HitReact))
	{
		++ActiveHitReactAbilities;
		SetHitReactState(true);
	}
}

void AFP_EnemyCharacter::OnAbilityEnded(const FAbilityEndedData& AbilityEndedData)
{
	if (!AbilityEndedData.AbilityThatEnded)
	{
		return;
	}

	if (AbilityEndedData.AbilityThatEnded->AbilityTags.HasTagExact(FFP_GameplayTags::Get().Effects_HitReact))
	{
		ActiveHitReactAbilities = FMath::Max(0, ActiveHitReactAbilities - 1);
		if (ActiveHitReactAbilities == 0)
		{
			SetHitReactState(false);
		}
	}
}

void AFP_EnemyCharacter::SetHitReactState(bool bInHitReact)
{
	bHitReacting = bInHitReact;
	GetCharacterMovement()->MaxWalkSpeed = bHitReacting ? 0.f : BaseWalkSpeed;
	FP_AIController->GetBlackboardComponent()->SetValueAsBool(FName("HitReacting"), bHitReacting);
}



void AFP_EnemyCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}
