

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "mp1_MultiplayerUtils.generated.h"

/**
 * 
 */
UCLASS()
class MP1_API Ump1_MultiplayerUtils : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
	UFUNCTION(BlueprintCallable, Category = "Multiplayer Utilities")
	static void PrintLocalNetRole(AActor* Actor);
	
	UFUNCTION(BlueprintCallable, Category = "Multiplayer Utilities")
	static void PrintRomoteNetRole(AActor* Actor);
};
