// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "nElement.h"
#include "nProjectile.generated.h"

/**
 * 
 */
UCLASS()
class UnProjectile : public UnElement
	{
	GENERATED_BODY()
	
	public:	

	// Sets default values for this actor's properties
	UnProjectile();

	// Run-time data
	adtFloat		fX,fY,fZ;								// Velocity
	bool			bVel;										// Update velocity
	adtString	strLocAt;								// Location of target component 
	bool			bLoc;										// Update assigned location ?
	UProjectileMovementComponent
					*pcProj;									// Game engine component

	// Base class memebers
	virtual void InitializeComponent		( void ) override;
	virtual void UninitializeComponent	( void ) override;
//	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction);

	// Utilities
	virtual bool	mainTick		( float );
	virtual bool	onReceive	( nElement *, const WCHAR *, const WCHAR *, const ADTVALUE &);

	};
