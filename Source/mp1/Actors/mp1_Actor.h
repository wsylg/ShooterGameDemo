

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "mp1_Actor.generated.h"

UCLASS()
class MP1_API Amp1_Actor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	Amp1_Actor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	
	
};
