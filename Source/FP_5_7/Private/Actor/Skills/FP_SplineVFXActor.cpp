// Copyright JG

#include "Actor/Skills/FP_SplineVFXActor.h"
#include "Components/SplineComponent.h"
#include "NiagaraComponent.h"

AFP_SplineVFXActor::AFP_SplineVFXActor()
{
	PrimaryActorTick.bCanEverTick = false;

	Spline = CreateDefaultSubobject<USplineComponent>(TEXT("Spline"));
	SetRootComponent(Spline);

	Niagara = CreateDefaultSubobject<UNiagaraComponent>(TEXT("Niagara"));
	Niagara->SetupAttachment(RootComponent);
}

void AFP_SplineVFXActor::BeginPlay()
{
	Super::BeginPlay();
	SetLifeSpan(Lifespan);
}

void AFP_SplineVFXActor::SetEndpoints(const FVector& WorldStart, const FVector& WorldEnd,
                                       float MidpointOffset, int32 NumMidPoints)
{
	Spline->ClearSplinePoints(false);
	Spline->AddSplineWorldPoint(WorldStart);

	if (MidpointOffset > 0.f && NumMidPoints > 0)
	{
		const FVector Dir   = (WorldEnd - WorldStart).GetSafeNormal();
		const FVector Ref   = FMath::Abs(Dir.Z) < 0.9f ? FVector::UpVector : FVector::RightVector;
		const FVector Perp  = FVector::CrossProduct(Dir, Ref).GetSafeNormal();
		const FVector Perp2 = FVector::CrossProduct(Dir, Perp).GetSafeNormal();

		for (int32 i = 1; i <= NumMidPoints; ++i)
		{
			const float   T     = static_cast<float>(i) / static_cast<float>(NumMidPoints + 1);
			const float   R     = FMath::FRandRange(0.f, MidpointOffset);
			const float   Theta = FMath::FRandRange(0.f, 2.f * PI);
			const FVector Mid   = FMath::Lerp(WorldStart, WorldEnd, T)
			                    + Perp  * (R * FMath::Cos(Theta))
			                    + Perp2 * (R * FMath::Sin(Theta));

			Spline->AddSplineWorldPoint(Mid);
		}
	}

	Spline->AddSplineWorldPoint(WorldEnd);

	const ESplinePointType::Type PointType = (MidpointOffset > 0.f && NumMidPoints > 0)
		? ESplinePointType::Curve
		: ESplinePointType::Linear;

	for (int32 i = 0; i < Spline->GetNumberOfSplinePoints(); ++i)
		Spline->SetSplinePointType(i, PointType, false);

	Spline->UpdateSpline();
}
