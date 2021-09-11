#pragma once

#define OUT

	/*----------------------
			Lock
	----------------------*/

#define USE_MANY_LOCKS(count)	Lock _locks[count];
#define USE_LOCK				USE_MANY_LOCKS(1)
#define READ_LOCK_IDX(idx)		ReadLockGuard readLockGuard_##idx(_locks[idx], typeid(this).name());
#define READ_LOCK				READ_LOCK_IDX(0)
#define WRITE_LOCK_IDX(idx)		WriteLockGuard writeLockGuard_##idx(_locks[idx], typeid(this).name());
#define WRITE_LOCK				WRITE_LOCK_IDX(0)

	/*----------------------
			Memory
	----------------------*/
#ifdef _DEBUG
	#define x_alloc(size) StompAllocator::Alloc(size)
	#define x_release(ptr) StompAllocator::Release(ptr)
#else
	#define x_alloc(size) BaseAllocator::Alloc(size)
	#define x_release(ptr) BaseAllocator::Release(ptr)
#endif

	/*----------------------
			CRASH
	----------------------*/
#define CRASH(cause)							\
{												\
	uint32* crash = nullptr;					\
	__analysis_assume(crash != nullptr);		\
	*crash = 0x1DEADEBB;						\
}

#define ASSERT_CRASH(expr)			\
{									\
	if (!(expr))					\
	{								\
		CRASH("ASSERT_CRASH")		\
		__analysis_assume(expr);	\
	}								\
}