#include <algorithm>
#include <eepp_support/core.hpp>
#include <eepp_support/scene/action.hpp>
#include <eepp_support/scene/actionmanager.hpp>
#include <engine/sync.h>

namespace EE { namespace Scene {

ActionManager* ActionManager::New() {
	return eeNew( ActionManager, () );
}

ActionManager::ActionManager() : mUpdating( false ) {}

ActionManager::~ActionManager() {
	clear();
}

void ActionManager::addAction( Action* action ) {
	Lumix::MutexGuard l( mMutex );

	bool found = ( std::find( mActions.begin(), mActions.end(), action ) != mActions.end() );

	if ( !found )
		mActions.emplace_back( action );
}

Action* ActionManager::getActionByTag( const Uint32& tag ) {
	Lumix::MutexGuard l( mMutex );

	for ( auto it = mActions.begin(); it != mActions.end(); ++it ) {
		Action* action = *it;

		if ( action->getTag() == tag )
			return action;
	}

	return NULL;
}

std::vector<Action*> ActionManager::getActionsFromTarget( Node* target ) {
	Lumix::MutexGuard l( mMutex );

	std::vector<Action*> actions;

	for ( auto it = mActions.begin(); it != mActions.end(); ++it ) {
		Action* action = *it;

		if ( action->getTarget() == target )
			actions.emplace_back( action );
	}

	return actions;
}

std::vector<Action*> ActionManager::getActionsByTagFromTarget( Node* target,
															   const String::HashType& tag ) {
	Lumix::MutexGuard l( mMutex );
	std::vector<Action*> actions;

	for ( auto it = mActions.begin(); it != mActions.end(); ++it ) {
		Action* action = *it;

		if ( action->getTarget() == target && action->getTag() == tag )
			actions.emplace_back( action );
	}

	return actions;
}

void ActionManager::removeActionByTag( const Uint32& tag ) {
	removeAction( getActionByTag( tag ) );
}

void ActionManager::removeActionsByTagFromTarget( Node* target, const String::HashType& tag ) {
	std::vector<Action*> removeList;

	{
		Lumix::MutexGuard l( mMutex );
		for ( auto it = mActions.begin(); it != mActions.end(); ++it ) {
			Action* action = *it;

			if ( action->getTarget() == target && action->getTag() == tag ) {
				removeList.emplace_back( *it );
			}
		}
	}

	for ( auto it = removeList.begin(); it != removeList.end(); ++it )
		removeAction( *it );
}

void ActionManager::update( const Time& time ) {
	if ( isEmpty() )
		return;

	std::vector<Action*> removeList;

	{
		mUpdating = true;

		Lumix::MutexGuard l( mMutex );

		// Actions can be added during action updates, we need to only iterate the current actions
		auto actions = mActions;

		for ( auto it = actions.begin(); it != actions.end(); ++it ) {
			Action* action = *it;

			action->update( time );

			if ( action->isDone() ) {
				action->sendEvent( Action::ActionType::OnDone );

				removeList.emplace_back( action );
			}
		}

		mUpdating = false;
	}

	for ( auto it = mActionsRemoveList.begin(); it != mActionsRemoveList.end(); ++it )
		removeAction( *it );

	mActionsRemoveList.clear();

	for ( auto it = removeList.begin(); it != removeList.end(); ++it )
		removeAction( *it );
}

std::size_t ActionManager::count() const {
	Lumix::MutexGuard l( mMutex );
	return mActions.size();
}

bool ActionManager::isEmpty() const {
	Lumix::MutexGuard l( mMutex );
	return mActions.empty();
}

void ActionManager::clear() {
	Lumix::MutexGuard l( mMutex );

	for ( auto it = mActions.begin(); it != mActions.end(); ++it ) {
		Action* action = *it;

		eeSAFE_DELETE( action );
	}

	mActions.clear();
}

void ActionManager::removeAction( Action* action ) {
	if ( NULL != action ) {
		if ( !mUpdating ) {
			Lumix::MutexGuard l( mMutex );

			auto actionIt = std::find( mActions.begin(), mActions.end(), action );

			if ( actionIt != mActions.end() ) {
				mActions.erase( actionIt );

				eeSAFE_DELETE( action );
			}
		} else {
			mActionsRemoveList.emplace_back( action );
		}
	}
}

void ActionManager::removeActions( const std::vector<Action*>& actions ) {
	for ( auto& action : actions ) {
		removeAction( action );
	}
}

void ActionManager::removeAllActionsFromTarget( Node* target ) {
	std::vector<Action*> removeList;

	{
		Lumix::MutexGuard l( mMutex );
		for ( auto it = mActions.begin(); it != mActions.end(); ++it ) {
			Action* action = *it;

			if ( action->getTarget() == target ) {
				removeList.emplace_back( *it );
			}
		}
	}

	for ( auto it = removeList.begin(); it != removeList.end(); ++it )
		removeAction( *it );
}

}} // namespace EE::Scene
