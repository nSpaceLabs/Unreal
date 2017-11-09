// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"

// nSpace engine
#include "nSpace.h"

// nSpace actor
#include "nLoc.generated.h"

// Forward decs.
class nLocCB;
//class nElement;

//
// Class - AnLoc.  This class handles a specific render location in the namespace.
//

UCLASS()
class AnLoc : 
	public AActor,
	public InTick
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
	nLocCB		*pCB;										// Callback

	// Pending actors
	IList			*pActQ;									// Actor queue
	IIt			*pActIt;									// Actor queue iterator

	// Utilities
	HRESULT	addMain		( InTick * );
	HRESULT	addWork		( InTick * );
	HRESULT	addStore		( const WCHAR *, const ADTVALUE & );
//	HRESULT	getParent	( const WCHAR *, nElement ** );
	void		init			( AnSpace *, const WCHAR *, int );
//	HRESULT	setRoot		( nElement * );
 
	// 'AActor' members
	virtual void BeginPlay	() override;
	virtual void EndPlay		( const EEndPlayReason::Type ) override;
	
	// 'InTick' memebers
	virtual bool tickMain	( float );				// Main thread ticking
	virtual bool tickWork	( void );				// Worker thread ticking

	// Internal utilities
	HRESULT onValue	( const WCHAR *, const WCHAR *, const ADTVALUE & );
	};

//
// Class - nLocCB.  Callback object.
//

class nLocCB : 
	public CCLObject,										// Base class
	public nSpaceClientCB								// Callback function
	{
	public :
	nLocCB ( AnLoc * );									// Constructor

	// Run-time datas
	AnLoc	*pParent;										// Parent object

	// 'nSpaceClientCB' members
	STDMETHOD(onReceive)	(const WCHAR *, const WCHAR *, const ADTVALUE &);

	// CCL
	CCL_OBJECT_BEGIN_INT(nLocCB)
	CCL_OBJECT_END()
	};
