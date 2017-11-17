// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "nElement.h"
#include "nProjectile.generated.h"

/**
 * 
 */
UCLASS()
class AnProjectile : 
	public AnElement
	{
	GENERATED_BODY()
	
	public:	

	// Sets default values for this actor's properties
	AnProjectile();

	// Run-time data
	adtFloat		fX,fY,fZ;								// Velocity
	UProjectileMovementComponent
					*pcProj;									// Game engine component

	// 'AnElement' class memebers
	virtual void	BeginPlay() override;
	virtual void	EndPlay	( const EEndPlayReason::Type ) override;
	virtual void	onValue	( const WCHAR *, const WCHAR *, const ADTVALUE & );
	};
