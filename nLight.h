// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "nElement.h"
#include "nLight.generated.h"

/**
 * 
 */
UCLASS()
class AnLight : 
	public AnElement
	{
	GENERATED_BODY()
	
	public:	

	// Sets default values for this actor's properties
	AnLight();

	// Run-time data
	ULightComponent		*pcLight;					// Light component
	adtString				strType;						// Type
	bool						bType;						// Type update
	adtFloat					fIntense;					// Intensity
	bool						bIntense;					// Intensity update

	// 'AnElement' class memebers
	virtual void	BeginPlay() override;
	virtual void	EndPlay	( const EEndPlayReason::Type ) override;
	virtual bool	tickMain	( float ) override;
	virtual bool	onValue	( const WCHAR *, const WCHAR *, const ADTVALUE & ) override;


	};
