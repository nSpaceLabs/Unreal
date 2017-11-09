// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/PlayerController.h"
#include "nSpace.h"
//#include "nElement.h"
//#include "nGroup.h"
#include "nPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class AnPlayerController : public APlayerController
	{
	GENERATED_BODY()

	public :

	AnPlayerController ( const FObjectInitializer & );

	// Run-time data
	IDictionary		*pDctRy,*pDctBt;					// Input dictionaries

	// 'APlayerController' members
	virtual void BeginPlay				( void )									override;
	virtual void EndPlay					( const EEndPlayReason::Type )	override;
	virtual bool InputKey				( FKey, EInputEvent, float,
													bool )								override;
	virtual void SetupInputComponent	( void )									override;
	virtual void Tick						( float )								override;

/*
	nElement			*pElemCap;							// Captured input element
	bool				bElemCap;							// Caputre enabled
	float				fElemCap;							// Caputre distance
	FTransform		tElemCap;							// Initial capture transform
	FVector			vElemAt,vElemDr;					// Latest location/direction

	// Utilities
	virtual void onButton( nElement *, const WCHAR *, const WCHAR * );
	virtual void onClick ( nElement * );
	virtual void onRay	( nElement *, const FVector &, const FVector & );

*/
	};
