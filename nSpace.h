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

//
// Interface - InTick.  "Tickable" callback for processing from different threads.
//

interface InTick 
	{
	virtual bool tickMain ( float )	= 0;			// Tick from main game loop
	virtual bool tickWork ( void )	= 0;			// Tick from worker thread
	};

//
// Class - AnSpace.  Primary nSpace actor for game engine.
//		Handles link to nSpace ActiveX client and manages render locations.
//

UCLASS()
class AnSpace : public AActor
	{
	GENERATED_BODY()
	
	public:	

	// Sets default values for this actor's properties
	AnSpace();												// Constructor
	virtual ~AnSpace();									// Destructor

	// Run-time data
	nSpaceClient		*pCli;							// nSpace client
	IDictionary			*pDctLoc;						// Location handler dictionary
	bool					bNewLoc;							// A new render location has been added
	adtString			strRefActor;					// References
	adtString			strRenLoc;						// Render state location
	sysCS					csRen;							// Global render mutex
	adtString			strLevel;						// Level requests
	bool					bLevel;							// Level change ?
	bool					bCamera[4];						// Camera coordinate change
	float					fCamera[4];						// Camera coordinates

	// Worker thread
	AnSpacet				*pTick;							// Worker thread
	IThread				*pThrd;							// Worker thread
	IList					*pWrkQ;							// Worker queue
	IIt					*pWrkIt;							// Worker queue iterator
	sysEvent				evWork;							// Worker thread event
	bool					bWork;							// True to keep working
	IList					*pStQ;							// Store queue
	IIt					*pStIt;							// Store queue iterator

	// Game loop thread
	IList					*pMnQ;							// Main queue
	IIt					*pMnIt;							// Main queue iterator

	// Utilities
	HRESULT addMain	( InTick * );
	HRESULT addWork	( InTick * );
	HRESULT addStore	( const WCHAR *, const ADTVALUE &, const WCHAR * = NULL );

	// 'AActor' memebers
	virtual void BeginPlay	( ) override;
	virtual void EndPlay		( const EEndPlayReason::Type) override;
	virtual void Tick			( float DeltaSeconds) override;

	// Internal utilities
	HRESULT onValue (const WCHAR *, const WCHAR *, const ADTVALUE &);

	};

//
// Class - AnSpacet.  Worker thread class to handle nSpace link
//		and perform work on behalf of the engine.
//

class AnSpacet : 
	public CCLObject,										// Base class
	public ITickable,										// Interface
	public nSpaceClientCB								// Callback function
	{
	public :
	AnSpacet ( AnSpace * );								// Constructor

	// Run-time datas
	AnSpace			*pThis;								// Parent object

	// 'nSpaceClientCB' members
	STDMETHOD(onReceive)	( const WCHAR *, const WCHAR *, const ADTVALUE & );

	// 'ITickable' members
	STDMETHOD(tick)		( void );
	STDMETHOD(tickAbort)	( void );
	STDMETHOD(tickBegin)	( void );
	STDMETHOD(tickEnd)	( void );

	// CCL
	CCL_OBJECT_BEGIN_INT(AnSpacet)
	CCL_OBJECT_END()
	};
