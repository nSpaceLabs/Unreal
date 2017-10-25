// Fill out your copyright notice in the Description page of Project Settings.

#include "nGameMode.h"
#include "nPlayerController.h"

AnGameMode::AnGameMode(const FObjectInitializer &oi) :
	AGameMode(oi)
	{
	////////////////////////////////////////////////////////////////////////
	//
	//	PURPOSE
	//		-	Constructor for the object.
	//
	//	PARAMETERS
	//		-	oi is a reference to the object initializer
	//
	//	RETURN VALUE
	//		S_OK if successful
	//
	////////////////////////////////////////////////////////////////////////

	// Debug
	UE_LOG(LogTemp, Warning, TEXT("Hello from AnGameMode constructor"));

	// Own player controller
	PlayerControllerClass = AnPlayerController::StaticClass();
	}	// AnSpaceRender



