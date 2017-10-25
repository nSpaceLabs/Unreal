// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "nElement.h"
#include "nLight.generated.h"

/**
 * 
 */
UCLASS()
class UnLight : public UnElement
	{
	GENERATED_BODY()
	
	public:	

	// Sets default values for this actor's properties
	UnLight();

	// Run-time data
	adtString				strType;						// Type
	adtFloat					fIntense;					// Intensity
	bool						bIntense;					// Intensity update
	ULightComponent		*pcLight;					// Game engine component

	// Base class memebers
	virtual void InitializeComponent		( void ) override;
	virtual void UninitializeComponent	( void ) override;

	// Utilities
	virtual bool	mainTick		( float );
	virtual bool	onReceive	( nElement *, const WCHAR *, const WCHAR *, const ADTVALUE &);

	};
