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
	public nSpaceClientCB,								// Callback
	public InTick											// Callback
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
	int			iState;									// Element state

	// State
	FVector		fTi,fTf,fS,fR;							// Target transformations
	bool			bTrans;									// Translation updated
	FVector		fTt;										// Time for translation
	FVector		fSclLcl;									// Local scaling (if needed)
	bool			bScl;										// Scaling update
	FVector		fRotNow;									// Current rotation
	bool			bRot[3];									// Rotation update
	int			iVisible;								// Visible update
	int			iColor;									// 32-bit color
	bool			bColor;									// Color update required

	// Utilities
	void	init			( AnLoc *, const WCHAR * );// Initialize state
	void	rootUpdate	( void );						// Root component has changed

	// 'AnElement' members
	virtual bool onValue	( const WCHAR *, const WCHAR *, const ADTVALUE & );

	// 'AActor' members
	virtual void BeginPlay	() override;
	virtual void EndPlay		( const EEndPlayReason::Type ) override;

	// 'InTick' memebers
	virtual bool tickMain	( float );				// Main thread ticking
	virtual bool tickWork	( void );				// Worker thread ticking

	// 'nSpaceClientCB' members
	STDMETHOD(onReceive)	( const WCHAR *, const WCHAR *, const ADTVALUE & );

	};

/*
// Definitions
#define	SZ_TIME_MOVE			1.0					// Seconds to move


// Forward decs.
class AnActor;

//
// Class - nElement.  Base class for nSpace visual elements.
//

class nElement : 
	public CCLObject,										// Base class
	public nSpaceClientCB,								// Callback
	public InTick											// Callback
	{
	public:	
	nElement	( void );									// Constructor

	// Run-time data
	AnActor		*pRen;									// Master render object
	AnLoc			*pRenLoc;								// Render location
	adtString	strLoc,strDef;							// Namespace location and definition
	bool			bRun;										// Element running
	int			iRoot;									// Index for root elements
	int			iState;									// Element state

	// Component
	AActor		*pOuter;									// Outer actor dervied from this element
	FVector		fTi,fTf,fS,fR;							// Target transformations
	FVector		fTt;										// Time for translation
	FVector		fSclLcl;									// Local scaling (if needed)
	FVector		fRotNow;									// Current rotation
	bool			bRot[3];									// Rotation update
	int			iVisible;								// Visible update
	int			iColor;									// 32-bit color
	bool			bColor;									// Color update required

	// Utilities
	void init		( AnLoc *, const ADTVALUE & );// Initialize state



	// Custom events
	virtual void	onButton		( IDictionary *, const WCHAR *, const WCHAR * );
	virtual void	onRay			( IDictionary *, const FVector &, const FVector & );


	// CCL
	CCL_OBJECT_BEGIN_INT(nElement)
	CCL_OBJECT_END()
	virtual HRESULT	construct	( void );		// Construct object
	virtual void		destruct		( void );		// Destruct object
	
	};
*/
