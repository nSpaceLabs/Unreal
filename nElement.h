#pragma once

#include "GameFramework/Actor.h"
#include "nLoc.h"
#include "nElement.generated.h"

// Definitions
#define	SZ_TIME_MOVE			1.0					// Seconds to move

// Element initiallization states due to work being done in
// different threads
#define	ELEM_STATE_ERROR			-1					// Element in error state
#define	ELEM_STATE_INIT			0					// Initialize element
#define	ELEM_STATE_LISTEN			1					// Perform element listen
#define	ELEM_STATE_RUN				2					// Element running
#define	ELEM_STATE_STOP			3					// Perform element shutdown

// Forward decs.
class AnElement;

//
// Class - nElementRef.  Wrapper class to hold an 'nElement' reference.
//		This class is basically so that actors can be referenced counted
//		using the nSpace object model and stored in dictionaries.
//

class nElementRef :
	public CCLObject
	{
	public :
	nElementRef ( AnElement *_pElem )				// Constructor
		{ pElem = _pElem; AddRef(); }
	virtual ~nElementRef ( void ) {};				// Destructor

	// Run-time data
	AnElement	*pElem;									// Actor reference

	// CCL
	CCL_OBJECT_BEGIN_INT(nElementRef)
	CCL_OBJECT_END()
	};

//
// Class - AnElement.  An element actor handles a single visual element location.
//		It acts as the root actor for the components that make up the visual.
//

UCLASS()
class AnElement : 
	public AActor,											// Base class
	public nValues											// Callback
	{
	GENERATED_BODY()

	public:	
	AnElement();											// Constructor
	virtual ~AnElement();								// Destructor

	// Run-time data
	AnLoc			*pLoc;									// Parent render location actor
	adtString	strLoc;									// Location of element state
	adtString	strLstn;									// Listen location
	bool			bRun;										// Element running

	// State
	FVector		fT,fS,fR;								// Target transformations
	FVector		fTt;										// Time for translation
	FVector		fSclLcl;									// Local scaling (if needed)
	FVector		fRotNow;									// Current rotation
	int			iColor;									// 32-bit color
	bool			bVisible;								// Should be visible ?

	// Utilities
	void	init			( AnLoc *, const WCHAR * );// Initialize state
	void	rootUpdate	( void );						// Root component has changed

	// 'AActor' members
	virtual void BeginPlay	() override;
	virtual void EndPlay		( const EEndPlayReason::Type ) override;
	virtual void Tick			( float DeltaSeconds) override;

	// 'nValues' members
	virtual void onValue	( const WCHAR *, const WCHAR *, const ADTVALUE & );
	};

