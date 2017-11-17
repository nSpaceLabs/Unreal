////////////////////////////////////////////////////////////////////
//
//									nSpace.h
//
//					Main include file for project
//
////////////////////////////////////////////////////////////////////

#pragma once

// Unreal
#include "Engine.h"

// When building Unreal 32-bit, the build system seems to change the
// default packing alignment from the default of 8 bytes. Ensure proper 
// packing in case building inside other environment that do
// not default to the same alignment.
#pragma	pack(push)
#if		defined(_WIN64)
#pragma	pack(16)
#else
#pragma	pack(8)
#endif

// nSpace client
#include "AllowWindowsPlatformTypes.h"
#include <nshxl.h>
#include "HideWindowsPlatformTypes.h"

// Restore packing
#pragma	pack(pop)

// Unreal
#include "GameFramework/Actor.h"

// nSpace engine
#include "nSpace.generated.h"

// Forward decs.
class AnSpacet;

// Operations for async client
#define	OP_LOAD						1
#define	OP_STORE						2
#define	OP_LISTEN					3
#define	OP_UNLISTEN					4

//
// Class - nValues.  Base class to handle reception of values on one thread (worker)
//		and distribution of values on another (game loop).
//

class nValues :
	public nSpaceClientCB								// Callback function
	{
	public :
	nValues ( void );										// Constructor
	virtual ~nValues ( void );							// Destructor


	// Utilities
	HRESULT dequeue ( void );
	HRESULT enqueue ( const WCHAR *, const WCHAR *, const ADTVALUE & );

	// 'nValues' members
	virtual void onValue	( const WCHAR *, const WCHAR *, const ADTVALUE & );

	// 'nSpaceClientCB' members
	STDMETHOD(onReceive)	(const WCHAR *, const WCHAR *, const ADTVALUE &);

	private :

	// Run-time data
	IList	*pQv;												// Value queue
	IIt	*pQvIt;											// Value queue iterator

	};

//
// Class - AnSpace.  Primary nSpace actor for game engine.
//		Handles link to nSpace ActiveX client and manages render locations.
//

UCLASS()
class AnSpace : 
	public AActor,
	public nValues
	{
	GENERATED_BODY()
	
	public:	

	// Sets default values for this actor's properties
	AnSpace();												// Constructor
	virtual ~AnSpace();									// Destructor

	// Run-time data
	nSpaceClient		*pCli;							// nSpace client
	IDictionary			*pDctLoc;						// Location handler dictionary
	adtString			strRefActor;					// References
	adtString			strRenLoc;						// Render state location
	sysCS					csRen;							// Global render mutex
	bool					bCamera[4];						// Camera coordinate change
	float					fCamera[4];						// Camera coordinates

	// Worker thread
	AnSpacet				*pTick;							// Worker thread
	IThread				*pThrd;							// Worker thread
	IList					*pWrkQ;							// Worker queue
	IIt					*pWrkIt;							// Worker queue iterator
	sysEvent				evWork;							// Worker thread event
	bool					bWork;							// True to keep working

	// Utilities
	HRESULT addListen	( const WCHAR *, nSpaceClientCB *, const WCHAR * = NULL );
	HRESULT addStore	( const WCHAR *, const ADTVALUE &, const WCHAR * = NULL );

	// 'AActor' memebers
	virtual void BeginPlay	( ) override;
	virtual void EndPlay		( const EEndPlayReason::Type) override;
	virtual void Tick			( float DeltaSeconds) override;

	// 'nValues' members
	virtual void onValue	( const WCHAR *, const WCHAR *, const ADTVALUE & );
	};

//
// Class - AnSpacet.  Worker thread class to handle nSpace link
//		and perform work on behalf of the engine.
//

class AnSpacet : 
	public CCLObject,										// Base class
	public ITickable										// Interface
	{
	public :
	AnSpacet ( AnSpace * );								// Constructor

	// Run-time datas
	AnSpace			*pThis;								// Parent object

	// 'ITickable' members
	STDMETHOD(tick)		( void );
	STDMETHOD(tickAbort)	( void );
	STDMETHOD(tickBegin)	( void );
	STDMETHOD(tickEnd)	( void );

	// CCL
	CCL_OBJECT_BEGIN_INT(AnSpacet)
	CCL_OBJECT_END()
	};
