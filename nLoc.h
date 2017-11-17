// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"

// nSpace engine
#include "nSpace.h"

// nSpace actor
#include "nLoc.generated.h"

//
// Class - AnLoc.  This class handles a specific render location in the namespace.
//

UCLASS()
class AnLoc : 
	public AActor,
	public nValues
	{
	GENERATED_BODY()
	
	public:	
	AnLoc();													// Constructor
	virtual ~AnLoc();										// Destructor

	// Run-time data
	AnSpace		*pSpc;									// nSpace actor
	int			iIdx;										// Render index
	adtString	strLocRen;								// Render location
	IDictionary	*pDctRen;								// Render dictionary
	adtString	strRefActor;							// References

	// Utilities
	HRESULT	addListen	( const WCHAR *, nSpaceClientCB * );
	HRESULT	addStore		( const WCHAR *, const ADTVALUE & );
//	HRESULT	getParent	( const WCHAR *, nElement ** );
	void		init			( AnSpace *, const WCHAR *, int );
//	HRESULT	setRoot		( nElement * );
 
	// 'AActor' members
	virtual void BeginPlay	() override;
	virtual void EndPlay		( const EEndPlayReason::Type ) override;
	virtual void Tick			( float DeltaSeconds) override;
	
	// 'nValues' members
	virtual void onValue	( const WCHAR *, const WCHAR *, const ADTVALUE & );
	};

