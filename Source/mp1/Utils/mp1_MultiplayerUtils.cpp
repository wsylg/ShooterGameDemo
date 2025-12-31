


#include "mp1_MultiplayerUtils.h"

void Ump1_MultiplayerUtils::PrintLocalNetRole(AActor* Actor)
{
	if(Actor->HasAuthority())
	{	
		UE_LOG(LogTemp, Warning, TEXT("Actor [%s] Has Authority."), *Actor->GetName());
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Actor [%s] Does Not Has Authority."), *Actor->GetName())
	}
	const ENetRole LocalRole = Actor->GetLocalRole();

	switch(Actor->GetLocalRole())
	{
		case ROLE_None:
			UE_LOG(LogTemp, Warning, TEXT("Actor Role : None - No networking "));
			break;

		case ROLE_SimulatedProxy:
			UE_LOG(LogTemp, Warning, TEXT("Actor Role : Simulated Proxy -Network client receiving updates from server"));
			break;

		case ROLE_AutonomousProxy:
			UE_LOG(LogTemp, Warning, TEXT("Actor Role : Autonomous Proxy -Network client, controlled locally"));
			break;

		case ROLE_Authority:
			UE_LOG(LogTemp, Warning, TEXT("Actor Role : Authority -Server or single player"));
			break;

		default:
			UE_LOG(LogTemp, Warning, TEXT("Actor Role : Unknown role value"));
			break;

	}
}

void Ump1_MultiplayerUtils::PrintRomoteNetRole(AActor* Actor)
{
	if (Actor->HasAuthority())
	{
		UE_LOG(LogTemp, Warning, TEXT("Actor [%s] Has Authority."), *Actor->GetName());
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Actor [%s] Does Not Has Authority."), *Actor->GetName())
	}
	const ENetRole LocalRole = Actor->GetLocalRole();

	switch (Actor->GetRemoteRole())
	{
	case ROLE_None:
		UE_LOG(LogTemp, Warning, TEXT("Actor Remote Role : None - No networking "));
		break;

	case ROLE_SimulatedProxy:
		UE_LOG(LogTemp, Warning, TEXT("Actor Remote Role : Simulated Proxy -Network client receiving updates from server"));
		break;

	case ROLE_AutonomousProxy:
		UE_LOG(LogTemp, Warning, TEXT("Actor Remote Role : Autonomous Proxy -Network client, controlled locally"));
		break;

	case ROLE_Authority:
		UE_LOG(LogTemp, Warning, TEXT("Actor Remote Role : Authority -Server or single player"));
		break;

	default:
		UE_LOG(LogTemp, Warning, TEXT("Actor Remote Role : Unknown role value"));
		break;

	}
}
