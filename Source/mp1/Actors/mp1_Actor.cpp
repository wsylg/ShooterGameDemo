


#include "mp1_Actor.h"


// Sets default values
Amp1_Actor::Amp1_Actor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	bNetLoadOnClient = true;
	bReplicates = true;
	SetReplicatingMovement(true);
}

// Called when the game starts or when spawned
void Amp1_Actor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void Amp1_Actor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


