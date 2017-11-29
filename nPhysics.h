// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "nElement.h"
#include "nPhysics.generated.h"

/**
 * 
 */
UCLASS()
class AnPhysics : 
	public AnElement
	{
	GENERATED_BODY()
	
	public:	

	// Sets default values for this actor's properties
	AnPhysics();

	// Run-time data
	adtBool		bSimulate;								// Simulate physics/gravity
	adtBool		bGravity;								// Enable/disable gravity
	UPrimitiveComponent
					*pcCmp;									// Target component
	FVector		vForce;									// Current external force vector
	float			fUpdate,fAccum;						// Accumulated time
	FTransform	tPrev;									// Previous transform of component
	bool			bLimitV;									// Limit velocity ?
	FVector		vMaxV;									// Maximum velocity
	float			fMass;									// Desired mass (scale)

	// 'AnElement' class memebers
	virtual void	BeginPlay() override;
	virtual void	EndPlay	( const EEndPlayReason::Type ) override;
	virtual void	onValue	( const WCHAR *, const WCHAR *, const ADTVALUE & );
	virtual void	Tick		( float DeltaSeconds) override;

	private :

	// Internal utilities
	void updateValue ( const WCHAR *, float );

	};
